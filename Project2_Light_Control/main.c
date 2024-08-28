// TEAM: Sahil D., Michael L.

#include <stdint.h>
#include <stdbool.h>

// clock gating definitions
#define SYSCTL_RCGCGPIO_R           (*((volatile uint32_t *)0x400FE608))          // 0x400F.E000 + 0x608
#define SYSCTL_RCGCUART             (*((volatile uint32_t *)0x400FE618))          // 0x400F.E000 + 0x618
#define CLOCK_GATE_PINS             0x05                                          // enables clock gate for port C AND A(000101)
#define CLOCK_GATE_UART             0x01                                          // enable clock for UART0

// UART Config
#define UART0                       (*((volatile uint32_t *)0x4000C000))          // OFFSET: 0
#define UART0CTL                    (*((volatile uint32_t *)0x4000C030))          // OFFSET: 0x30
#define UART0LCRH                   (*((volatile uint32_t *)0x4000C02C))          // OFFSET: 0x24
#define UART0FR                     (*((volatile uint32_t *)0x4000C018))          // OFFSET: 0x18
#define UART0IBRD                   (*((volatile uint32_t *)0x4000C024))          // OFFSET: 0x24
#define UART0FBRD                   (*((volatile uint32_t *)0x4000C028))          // OFFSET: 0x28
#define UART0_CC                    (*((volatile uint32_t *)0x4000CFC8))          // OFFSET: 0xFC8

#define UARTCTLEN                   0x201;                                        // UART Receive

// UART GPIO PINS (PA0, PA1)
#define GPIO_PORTA                  ((volatile uint32_t *)0x40004000)             // port A base address
#define GPIO_PORTA_DEN              (*((volatile uint32_t *)0x4000451C))          // OFFSET: 0x51C
#define GPIO_PORTA_AFSEL            (*((volatile uint32_t *)0x40004420))          // OFFSET: 0x420
#define GPIO_PORTA_PCTL             (*((volatile uint32_t *)0x4000452C))          // OFFSET: 0x52C
#define GPIO_PORTA_AMSEL            (*((volatile uint32_t *)0x40004528))          // OFFSET: 0x528
#define PCTL_PIN0  0x1

// I/O GPIO PINS
#define GPIO_PORTC_DATA_BITS_R      ((volatile uint32_t *)0x40006000)             // Port C base address
#define GPIO_PORTC_DEN_R            (*((volatile uint32_t *)0x4000651C))          // OFFSET: 51C
#define GPIO_PORTC_DIR_R            (*((volatile uint32_t *)0x40006400))          // OFFSET: 400
#define GPIO_PORTC_DATA_BUTTON      (*((volatile uint32_t *)0x40006100))          // OFFSET: 100 -> 100000000 (pin 6)
#define GPIO_PORTC_DATA_LED         (*((volatile uint32_t *)0x40006080))          // OFFSET: 80  -> 010000000 (pin 5)

#define GPIO_PIN5                   0x20                                          // Pin 5 mask
#define GPIO_PIN6                   0x40                                          // Pin 6 mask
#define GPIO_PIN56                  0x60                                          // Pin 5+6 mask
#define GPIO_PIN0                   0x01                                          // Pin 0 mask

//Reads Character from receive buffer
char UART_Rx () {
    char data;
    while((UART0FR & 0x10));
    data = UART0;
    return ((unsigned char) data);
}

/*
 * Calc Baud-Rate:
 * BRD = 16,000,000/(16*9600) = 104.1667
 * IBRD = 104
 * FBRD = .1667*64+.5 = 11.1688 = 11
 */

int main(void)
{
    volatile uint32_t state = 1;                    // 1 = Positive; 0 = Negative
    char data;
    volatile uint32_t i;


    SYSCTL_RCGCGPIO_R |= CLOCK_GATE_PINS;           // enable clock for port C "100" AND A
    SYSCTL_RCGCUART |= CLOCK_GATE_UART;

    for(i = 0; i < 10000; i++);

    GPIO_PORTC_DEN_R  |= GPIO_PIN56;                // enable digital for pin 5 and 6
    GPIO_PORTC_DIR_R |= GPIO_PIN5;                  // set direction pin 5 to output and 6 to input (5 is 1, 6 is 0: 0100000)

    // uart config

    UART0CTL = 0x0;                                 // disable uart0
    GPIO_PORTA_DEN |= GPIO_PIN0;                    // Enable port A
    GPIO_PORTA_AFSEL |= GPIO_PIN0;                  // Sets pin to UART
    GPIO_PORTA_AMSEL = 0;                           // Disables Analog Mode
    GPIO_PORTA_PCTL |= PCTL_PIN0;                   // Sets port to UART receive
    UART0_CC = 0;                                   // Sets UART to asynchronous
    UART0IBRD = 104;                                // 9600 BPS; B = F/16 * BRD; F = 16MHz, BRD = 104.16
    UART0FBRD = 11;                                 // 0.16 * 64 + 0.5
    UART0LCRH = 0x70;                               // 01100000 Config Line Control for 8 data bits, no parity, 1 stop bit
    UART0CTL |= UARTCTLEN;                          // enable uart to receive

    for(i = 0; i < 10000; i++);

    while(1){
        if((UART0FR & (1<<4)) == 0){

            data = UART_Rx();
            if(data == 112){            // p
                state = 1;
            }else if(data == 110){      // n
                state = 0;
            }
        }

        if(state){                                  // Positive State
            GPIO_PORTC_DATA_LED &= ~GPIO_PIN5;
            if(GPIO_PORTC_DATA_BUTTON == GPIO_PIN6){
                GPIO_PORTC_DATA_LED = GPIO_PIN5;
            }
        }else{                                      // Negative State
            GPIO_PORTC_DATA_LED |= GPIO_PIN5;
            if(GPIO_PORTC_DATA_BUTTON == GPIO_PIN6){
                GPIO_PORTC_DATA_LED = ~GPIO_PIN5;
            }
            for(i = 0; i < 2000; i++);
        }

    }

    return 0;
}



