#include <plib.h>
#include <stdio.h>
#include "util.h"

static unsigned int raw_value = 0;
// static float        cvt_value = 0.0;

void ADC_init()
{
    // ADC module
    IPC6bits.AD1IP = 3;
    IPC6bits.AD1IS = 1;
    IFS1bits.AD1IF = 0;
    IEC1bits.AD1IE = 1;
    AD1CON1 = 0;
    AD1CON1bits.FORM = 4; // 32-bit integer
    AD1CON1bits.SSRC = 7; // auto convert
    AD1CON1bits.ASAM = 1; // auto start
    AD1CON2 = 0;
    AD1CON2bits.VCFG = 0; // AV_DD/AV_SS
    AD1CON2bits.SMPI = 0; // interrupt for each sample
    AD1CON2bits.BUFM = 0; // one 16-word buffer
    AD1CON2bits.ALTS = 0; // always use sample A input mux
    AD1CON3 = 0;
    AD1CON3bits.ADRC = 0; // clock from PBCLK

    AD1CON3bits.SAMC = 4; // 4 T_AD
    // Sample Time = Acquisition Time (SAMC) + Conversion Time (12T_AD) = 1/500Hz = 2ms
    // 4 T_AD + 12 T_AD = 2ms => T_AD = 1/8ms
    // T_PB = 2.5*e-4 ms (1/4MHz)
    // TAD = 2*TPB*(ADCS + 1) (Reference manual Sec17)
    // ADCS = (T_AD/(2*T_PB))-1 = 249
    AD1CON3bits.ADCS = 249;
    AD1PCFG = 0;
    AD1PCFGbits.PCFG = 0xFFFE;
    AD1CHS = 0;
    AD1CHSbits.CH0SA = 0; // AN0 --> Channel 0 pos input

    AD1CON1bits.ON = 1;
}

int read_adc_value() {
    return raw_value;
}

#pragma interrupt ADC_ISR ipl3 vector 27
void ADC_ISR(void){
    raw_value = ADC1BUF0; // 0 - 1024
    // cvt_value = (double)raw_value / 1024 * 3.3;
    // LATDbits.LATD0 = 1;
    IFS1bits.AD1IF = 0;
}
