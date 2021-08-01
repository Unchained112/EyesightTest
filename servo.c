#include <plib.h>

int is_change = 0;
int is_add = 1;
int acc = 100;
int pwm = 900; // 900 - 2100

void servo_init() {
    OC1CON = 0x0000; // Turn off the OC1 when performing the setup
    OC1R = 0; // Initialize primary Compare register
    OC1RS = pwm; // Initialize secondary Compare register
    OC1CON = 0x0006; // Configure for PWM mode without Fault pin enabled
    T2CONSET = 0x0008; // Enable 32-bit Timer mode
    T2CONbits.TCKPS = 3;
    PR2 = 19999; // Set period
    // Configure Timer3 interrupt. Note that in PWM mode, the corresponding source timer
    // interrupt flag is asserted. OC interrupt is not generated in PWM mode.
    IFS0CLR = 0x00001000; // Clear the T3 interrupt flag
    IEC0SET = 0x00001000; // Enable T3 interrupt
    IPC3SET = 0x0000001C; // Set T3 interrupt priority to 7
    T2CONSET = 0x8000; // Enable Timer2
    OC1CONSET = 0x8020; // Enable OC1 in 32-bit mode.
}

void change_pwm(int new_pwm) {
    pwm = new_pwm;
}

int read_pwm() {
    return pwm;
}

int COUNT_MAX=10;
int count = 0;
int COUNT_STOP_MAX = 24; // 2400/acc
int count_stop = 24; // initialized to COUNT_STOP_MAX

void change_count_stop(int new_count_stop) {
    // change count_stop to 0 to start a new cycle
    count_stop = new_count_stop;
}

void change_count_max(int new_count_max) {
    // use adc to control
    COUNT_MAX = new_count_max;
}

int is_unfinished() {
    if (count_stop < COUNT_STOP_MAX - 1) {
        return 1;
    }
    else {
        return 0;
    }
}

void __ISR(_TIMER_3_VECTOR, ipl7) T3_IntHandler(void) {
    if (count == COUNT_MAX) {
        if (count_stop < COUNT_STOP_MAX) {
            if (is_add) {
                pwm += acc;
                if (pwm >= 2100) {
                    is_add = 0;
                }
            }
            else {
                pwm -= acc;
                if (pwm <= 900) {
                    is_add = 1;
                }
            }  
            count_stop++;
        }
        count = 0;
    }
    count++;
    OC1RS = pwm;
    IFS0CLR = 0x1000; // Clearing Timer3 interrupt flag
}