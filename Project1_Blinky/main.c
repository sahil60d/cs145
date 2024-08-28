//#include "tm4c123gh6pm.h"

#define LED_BLUE 0x04        //pin2
#define LED_OFF 0x00
#define PORTF_CLK 0x20      //bit 5 offset
#define PORTF_PIN2 0x04

#define PAUSE_TIME 250000

#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x40025010))   //read and write data: base=5000, offset=10 for pin 2
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long *)0x400FE608))   //address of clock: base=E000, Offset=608
#define GPIO_PORTF_DEN_R (*((volatile unsigned long *)0x4002551C))    //sets pins as digital in/out: base=5000, offset=51C
#define GPIO_PORTF_DIR_R (*((volatile unsigned long *)0x40025400))    //sets pins as input or output: base=5000, offset=400

/**
 * main.c
 */
int main(void)
{
    SYSCTL_RCGCGPIO_R |= PORTF_CLK;     //enable clock for port f
    GPIO_PORTF_DEN_R |= PORTF_PIN2;     //enable pin2 on port f
    GPIO_PORTF_DIR_R |= PORTF_PIN2;      //set pin1 on port f as output

    volatile unsigned long i;
    //infinite loop
    while(1) {
        GPIO_PORTF_DATA_R = LED_BLUE;    //Turn on Blue
        for (i = 0; i < PAUSE_TIME; i++) {}
        GPIO_PORTF_DATA_R = LED_OFF;    //Turn off
        for (i = 0; i < PAUSE_TIME; i++) {}
        }
}

