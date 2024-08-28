/*
 * Project 2: Light Control
 *
 */

#include <stdio.h>

//Clock Control for Port C
#define SYSCTL_RCGCGPIO_R       (*((volatile unsigned long *)0x400FE608))           //address of clock: base=E000, Offset=608
#define PORTAC_CLCK              0x5                                                 //bit offset for ports A&C

//Port C Config
#define GPIO_PORTC_DATA_R_LED   (*((volatile unsigned long *)0x40006080))           //base=6000, offset=0x20 for pin 5, LED
#define GPIO_PORTC_DATA_R_PB    (*((volatile unsigned long *)0x40006100))           //base=6000, offset=0x40 for pin 6, push button
#define GPIO_PORTC_DEN_R        (*((volatile unsigned long *)0x4000651C))           //base=6000, offset=51c
#define GPIO_PORTC_DIR_R        (*((volatile unsigned long *)0x40006400))           //base-6000, offset=400
#define PORTC_PIN56             0x60                                                //Pins 5+6
#define PORTC_PIN5              0x20                                                //Pin 5 only
#define PORTC_PIN6              0x40                                                //Pin 6

//Port A0 Config
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))


//UART Config
#define SYSCTL_RCGCUART_R       (*((volatile unsigned long *)0x400FE618))           //UART Clock: base=E000, Offset=618
#define UART0                   0x1                                                 //UART 0
#define UART0_DR_R              (*((volatile unsigned long *)0x4000C000))           //UART0 Data Register
#define UART0_FR_R              (*((volatile unsigned long *)0x4000C018))           //UART0 Flag Register
#define UART0_CTL_R             (*((volatile unsigned long *)0x4000C030))           //UART0 Control Register
#define UART0_IBRD_R            (*((volatile unsigned long *)0x4000C024))           //Integer Baud-Rate Divisor
#define UART0_FBRD_R            (*((volatile unsigned long *)0x4000C028))           //Fractional Baud-Rate Divisor
#define UART0_LCRH_R            (*((volatile unsigned long *)0x4000C02C))           //UART0 Line Control

/*
 * Calc Baud-Rate:
 * BRD = 16,000,000/(16*9600) = 104.1667
 * IBRD = 104
 * FBRD = .1667*64+.5 = 11.1688 = 11
 */

char UART_Rx();                                                                     //Declare function
char state = 'p';                                                                   //Default state is POSITIVE
unsigned volatile long i;

int main(void)
{
    //Clock Setup
    SYSCTL_RCGCGPIO_R |= PORTAC_CLCK;                                               //Enable clock for port C
    SYSCTL_RCGCUART_R |= UART0;                                                     //Enable clock for UART0
    for(i = 0; i<40000; i++);                                                       //Delay

    //PortC56 set up
    GPIO_PORTC_DEN_R |= PORTC_PIN56;                                                //Enable pins 5 and 6 on port C
    GPIO_PORTC_DIR_R |= PORTC_PIN5;                                                 //Set only pin 5 as output (LED)

    //PortA0 set up
    GPIO_PORTA_DEN_R |= UART0;
    GPIO_PORTA_AFSEL_R |= UART0;
    GPIO_PORTA_PCTL_R |= UART0;

    //UART set up
    UART0_CTL_R = 0x0;                                                              //Disable UART0
    UART0_IBRD_R = 104;                                                             //Set integer part of BRD
    UART0_FBRD_R = 11;                                                              //Fractional part of BRD
    UART0_LCRH_R = 0x70;                                                            //Config Line Control for 8 data bits, no parity, 1 stop bit
    UART0_CTL_R = 0x301;                                                            //Enables UART0 for reception

    while(1) {

        //Check UART
        if (!(UART0_FR_R & 0x10)) {                                                    //Check if transmit FIFO is full
            char data = UART_Rx();                                                  //Read input data

            if (data == 'p' || data == 'n') {                                       //make sure input is p or n
                state = data;                                                       //set state to input
            }
        }

        GPIO_PORTC_DATA_R_LED &= ~PORTC_PIN5;
        if(GPIO_PORTC_DATA_R_PB == PORTC_PIN6) {
            GPIO_PORTC_DATA_R_LED |= PORTC_PIN5;
            //for(i = 0; i<40000; i++);
        }
       /*
        //Positive State
        if (state == 'p') {

            GPIO_PORTC_DATA_R_LED &= ~PORTC_PIN5;
            if(GPIO_PORTC_DATA_R_PB == PORTC_PIN6) {
                GPIO_PORTC_DATA_R_LED |= PORTC_PIN5;
                for(i = 0; i<40000; i++);
            }
        }

        //Negative State
        if (state == 'n') {
            if(GPIO_PORTC_DATA_R_PB & 0x10) {
                GPIO_PORTC_DATA_R_LED |= 0x02;                                          //Turn on LED
            } else {
                GPIO_PORTC_DATA_R_LED |= ~0x02;                                         //Turn off LED, one's comp flip bits
            }
        }
        */
    }
}

//Reads Character
char UART_Rx() {
    char data;
    while (UART0_FR_R & 0x10);                                                            //Wait until buffer is not empty
    data = UART0_DR_R;
    return (unsigned char)data;
}

