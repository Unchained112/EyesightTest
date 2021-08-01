/* 
 * File:   newmain.c
 * Author: 12591
 *
 * Created on July 22, 2021, 4:47 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <plib.h>
#include "util.h"
#include "servo.h"
//#include "lidar.h"

/*
 * 
 */

void TIMER_init() {
    // PBCLK = 8MHz
    SYSKEY = 0x0; // write invalid key to force lock
    SYSKEY = 0xAA996655; // write Key1 to SYSKEY
    SYSKEY = 0x556699AA; // write Key2 to SYSKEY
    OSCCONbits.FRCDIV = 0b000; // SYSCLK = FRC = 8MHz
    OSCCONbits.PBDIV = 0b00; // configure PBDIV so PBCLK = SYSCLK
    OSCCONbits.COSC = 0b111; // select internal FRC divided by OSCCON<FRCDIV>
    SYSKEY = 0x0; // write invalid key to force lock
}

void MCU_init() {
    INTCONbits.MVEC = 1; // TODO
    INTEnableSystemMultiVectoredInt(); // Enable system wide interrupt to multivectored mode.

    TRISD = INPUT_2;
    LATDCLR = OUTPUT_0 | OUTPUT_1 | OUTPUT_2;

    TRISE = 0;
}

//int pwm = 1000; // TODO

int check_result(int result[4], int dir) {
    int i;
    
    int has_nonzero = 0;
    for (i = 0; i < 4; ++i) {
        if (result[i] > 0) {
            has_nonzero = 1;
            break;
        }
    }
    if (!has_nonzero) {
        return 0;
    }
    
    int max = 0;
    int result_max = result[0];
    for (i = 1; i < 4; ++i) {
        if (result[i] > result_max) {
            max = i;
            result_max = result[i];
        }
    }
    return (dir == max);
}

int main(int argc, char** argv) {
    //OSCSetPBDIV(OSC_PB_DIV_1);
    MCU_init();
    TIMER_init();

    LIDAR_init();
    servo_init();

    initial_lcd();
    clear_screen();
    
    ADC_init();

    /*
    start_lidar();
    while(1){}
     */

    /*
    
    while (1) {
        disp_rand(0x00);
        delay_sw(100000);
        disp_rand(0x01);
        delay_sw(100000);
        disp_rand(0x02);
        delay_sw(100000);
    }
     */

    int sw3_pushed = 0;
    char disp_level = 0x00;
    int disp_delay = 100000;
    
    while (1) {
        if ((PORTD & INPUT_2) == 0) {
            delay_sw(1600); // delay for de-bounce
            if ((PORTD & INPUT_2) == 0)
                sw3_pushed = 1;
        } else {
            delay_sw(1600); // delay for de-bounce
            if ((PORTD & INPUT_2) != 0) {
                if (sw3_pushed == 1) {
                    sw3_pushed = 0;
                    
                    disp_delay = 100000 + read_adc_value() * 400;
                    disp_level = 0x00;
                    LATDCLR = OUTPUT_1;
                    
                    // main logic
                    int matched = 1;
                    while (matched && disp_level < 0x03) {
                        disp_rand(disp_level);
                        delay_sw(disp_delay);
                        
                        change_pwm(900);
                        clear_result();
                        change_count_stop(0);
                        
                        while (is_unfinished()) {}
                        if (check_result(read_result(), read_lcd_dir())) {
                            disp_level++;
                        }
                        else {
                            matched = 0;                            
                        }
                    }
                    
                    if (disp_level >= 0x03) {
                        LATDSET = OUTPUT_1;
                    }
                    
                }
            }
        }
    }

    return (EXIT_SUCCESS);
}

