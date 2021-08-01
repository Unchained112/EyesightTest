// TODO: receive one byte -> check if it is head
// distance range: 0.2m~14m; 200=0xc8; 14000=0x36b0;
// Normally, no two bytes will be 0x8981 except the head

#include <plib.h>
#include "util.h"
#include "lidar.h"
#include "servo.h"

const cmd_t li_start = {0x56500006}; // byte[0]: 06; response:895000D9
const cmd_t li_stop = {0x56060050}; // response:8906008F
// const cmd_t li_hardware = {0x560E0058};
// const cmd_t li_outmode = {0x56700026}; // hex:56700026, ascii:56710027
// const cmd_t li_measure_freq = {0x56330065}; // 50hz
// const cmd_t li_baud_rate = {0x56120044}; // 115200; 9600:56110047

const char data_byte_0 = 0x89;
const char data_byte_1 = 0x81;
const char data_byte_9 = 0x00; // status

int data_byte_num = 0; // the number of the next byte of data frame
int start_response = 0; // check whether response of start command is received
char data_frame[11]; // store received bytes
int distance = 0;

void LIDAR_init() {
    asm("di");
    TRISF = 0;

    // UART1 
    IPC6SET = 0x40000; //Set priority level = 1;
    IFS1CLR = 0x0001;
    IEC1SET = 0x0001;

    U1AMODE = 0x0;
    U1AMODEbits.PDSEL = 0x00; // 8-bit data & No parity
    U1AMODEbits.STSEL = 0x0; // 1 stop bit
    U1AMODEbits.BRGH = 1;
    U1ABRG = 16; // Configure baud rate to be 115200
    // 8MHz/(4*115200)-1=16.36 -> 16 ; error=2.1%
    U1AMODESET = 0x8000; //Enable UART1A

    U1ASTA = 0;
    U1ASTAbits.UTXEN = 1; //Transmit is enabled  
    U1ASTAbits.URXEN = 1; //Receive is enabled
    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1; //Enable receive interrupt
    IPC6bits.U1IP = 7;

    U1ASTAbits.URXISEL = 0;
  
    U1MODEbits.ON = 1;  // Enable UART1

    asm("ei");
}

void _send_cmd(cmd_t cmd) {
    // TODO
    U1ATXREG = cmd.bytes[3];
    U1ATXREG = cmd.bytes[2];
    U1ATXREG = cmd.bytes[1];
    U1ATXREG = cmd.bytes[0];
    /*
    for (int i = 3; i >= 0; --i) {
        U1ATXREG = cmd.bytes[i];
    }
     */
}

void start_lidar() {
    _send_cmd(li_start);
}

void stop_lidar() {
    _send_cmd(li_stop);
}

// TODO: check response of start

int temp = 0;

void _update_byte_num(char byte_i) {
    if ((data_byte_num == 0 && byte_i != data_byte_0) ||
            (data_byte_num == 1 && byte_i != data_byte_1) ||
            (data_byte_num == 10)) {
        // continues waiting for the first byte;
        // or, fails to detect the head; back to waiting for the first byte
        // or, receives the last byte
        data_byte_num = 0;
    } else {
        data_byte_num++;
    }
}

int check_verity() {
    char result = data_frame[0];
    int i = 1;
    while (i < 10) {
        result = result ^ data_frame[i];
        ++i;
    }
    return (result == data_frame[10]);
}

int dir_detect_num[4] = {0}; // r, d, l, u

void clear_result() {
    int i;
    for (i = 0; i < 4; ++i) {
        dir_detect_num[i] = 0;
    }
}

int* read_result() {
    return dir_detect_num;
}

// Receiving ISR
#pragma interrupt RX_ISR ipl7 vector 24
void RX_ISR(void) {
    data_frame[data_byte_num] = U1RXREG;
    if (data_byte_num == 2) {
        distance = (int)(data_frame[2]) & 0x000000ff;
    }
    else if (data_byte_num == 3) {
        distance = distance | ((int)data_frame[3] << 8 & 0x0000ff00);
    }
    else if (data_byte_num == 10) {
        // receive the last byte, i.e. the whole data frame
        if (data_frame[9] == data_byte_9) {
            // 900 1300 1700 2100
            if (distance > 0 && distance < 800) {
                if (read_pwm() < 1200) {
                    dir_detect_num[0]++;
                }
                else if (read_pwm() > 1800) {
                    dir_detect_num[2]++;
                }
                else {
                    if (distance < 400) {
                        dir_detect_num[1]++;
                    }
                    else {
                        dir_detect_num[3]++;
                    }
                }
            }
            /*
            if (distance > 0 && distance < 300) {
                LATDSET = OUTPUT_1;
                LATDCLR = OUTPUT_2;
            }
            else if (distance >= 300 && distance < 600) {
                LATDSET = OUTPUT_2;
                LATDCLR = OUTPUT_1;
            }
            else {
                LATDCLR = OUTPUT_1;
                LATDCLR = OUTPUT_2;
            }
             */
        }
    }
    _update_byte_num(data_frame[data_byte_num]);
    
    IFS0bits.U1RXIF = 0;
}

