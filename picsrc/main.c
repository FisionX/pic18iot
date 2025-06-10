#include <delay.h>
#include <pic18fregs.h>
#include <stdint.h>

// FUSES START
#pragma config XINST = OFF
#pragma config FOSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config MCLRE = OFF

#define NDIGITS 4
#define RATE 60
#define TICK_RATE_HZ 1000
#define CALIBRATE 180

volatile static uint32_t tick_count = 0;
volatile static uint8_t digit[NDIGITS] = { 0 };
volatile static uint8_t dsp_en = 0;
volatile static uint8_t servos[6];

void isr(void) __interrupt (1);
void tmr_isr(void);
uint32_t xGetTicks(void);
void displayIsr(void);
uint8_t number_to_7seg(uint8_t);
void display(uint16_t);
void start_adc(void);
void servo_write();
inline void setup(void);
int main(void);


void isr(void) __interrupt (1) {
    if (INTCONbits.TMR0IF){
        tmr_isr();
        TMR0 = RATE;
    }
    INTCONbits.TMR0IF = 0;
}

void tmr_isr(void){
    /* time tracker handler */
    //tick_count++;
    __asm
        SETF _PORTD
    __endasm;
    servo_write();
    /* should check tasks and suck but I dont
     * think I will */
}

uint32_t xGetTicks(void) {
    return tick_count;
}

void displayIsr(void){
    LATA = LATE = 0;
    LATD = number_to_7seg(digit[dsp_en]);

    switch (dsp_en) {
        case 0:
            LATA = 0;
            LATE = 0x4;
            break;
        case 1:
            LATE = 0x2;
            break;
        case 2:
            LATE = 0x1;
            break;
        case 3:
            LATE = 0;
            LATA = 0x10;
            break;
    }
    dsp_en++;
    if (dsp_en > 3) dsp_en = 0;
}

uint8_t number_to_7seg(uint8_t number) {
    switch (number) {
        case 0:
            return 0x3F;
        case 1:
            return 0x06;
        case 2:
            return 0x5B;
        case 3:
            return 0x4F;
        case 4:
            return 0x66;
        case 5:
            return 0x6D;
        case 6:
            return 0x7D;
        case 7:
            return 0x07;
        case 8:
            return 0x7F;
        case 9:
            return 0x6F;
        case 0xa:
            return 0x77;
        case 0xb:
            return 0x7c;
        case 0xC:
            return 0x39;
        case 0xd:
            return 0x5e;
        case 0xe:
            return 0x79;
        case 0xf:
            return 0x71;
        default:
            return 0;
    }
}
void display(uint16_t num){
    uint8_t i = 0;
    if (num > 9999) return;
    while(num) {
        digit[i] = num%10;
        num = num/10;
        i++;
    }
}
void start_adc(void) {
    ADCON0bits.CHS = 0;
    ADCON1bits.PCFG = 0xE;
    ADCON2bits.ADFM = 1; 
    ADCON2bits.ACQT = 0x5; 
    ADCON2bits.ADCS = 0x1; 
    ADCON0bits.ADON = 1;
}
void servo_write(){
    uint8_t i = 0;
    uint8_t temp = 0;
    for(i = 30; i < CALIBRATE; i++);
    for(i = 0; i < 200; i++){
        if( i == servos[0] )
            __asm__("bcf _LATD, 0");
        if( i == servos[1] )
            __asm__("bcf _LATD, 1");
        if( i == servos[2] )
            __asm__("bcf _LATD, 2");
        if( i == servos[3] )
            __asm__("bcf _LATD, 3");
        if( i == servos[4] )
            __asm__("bcf _LATD, 4");
        //if( i == angle[5] + CALIBRATE)
        //    PORTDbits.RD5 = 0;
    }
}
inline void setup(void){
    /* Port setup */
    TRISD = 0;
    PORTD = 0x00;
    
    TRISB = 0xff;
    LATB = 0x00;
    ADCON1 = 0xf;

    TRISE = 0;
    LATE = 0;

    TRISA = 0;
    LATA = 0;

    /* Interrupt setup */
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.RBIE = 0;
    INTCON2bits.RBPU = 0;
    INTCON2bits.RBIP = 1;
    RCONbits.IPEN = 1;

    /* timer interrupt setup */
    INTCONbits.TMR0IE = 1;
    INTCON2bits.TMR0IP = 1;

    T0CONbits.T08BIT = 1;
    T0CONbits.T0CS = 0; /* Source internal oscilator */
    T0CONbits.PSA = 0;
    T0CONbits.T0PS = 0x6;
    T0CONbits.TMR0ON = 1;
}

int main(void) {
    uint32_t disptick = 45;
    uint32_t prevdisptick = xGetTicks();
    setup();
    PORTD = 0xff;
    servos[0]= 00;
    servos[1]= 30;
    servos[2]= 60;
    servos[3]= 80;
    servos[4]= 50;
    servos[5]= 50;
    for (;;) {
        //display(count);
        //delay1ktcy(500);
        //count++;
        //if(xGetTicks() - prevdisptick >= disptick) {
        //    //PORTD = !PORTD;
        //    PORTD = 0xff;
        //    prevdisptick = xGetTicks();
        //    servo_write(servos);
        //}
    /* the rest of free cycles are for communication */
        
    }
    return 0;
}
