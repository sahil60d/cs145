/**
 * Project 3: Interrupts and Timers
 *
 * Group Members: Sahil D., Michael L.
 *
 * Config LED as output     Port C[5]
 * Set up interrupt timer
 *
 * Set up UART      Port A[0]
 *
 * Continuously detect keypad presses, display character        Port A[4-7] for Columns, Port D[0-3] for Rows
 *
 * Setup GPIO interrupt for pushbutton          Port C[6]
 *
 *
 * *********************************
 *
 * Compute Prescalar & InitCnt:
 *  1s = 1000ms
 *  1000ms/4.096ms = 244.14... = 245
 *  CP = 62.5ns * 245 = 15.3 mircoseconds
 *  InitCnt = 1/15.3 mircoseconds = 65360
 *
 */

/**************************************
                MACROS
 *************************************/

// Clock configs
#define SYSCTL_RCGCGPIO_R           (*((volatile unsigned long *)0x400FE608))          // 0x400F.E000 + 0x608
#define SYSCTL_RCGCTIMER_R          (*((volatile unsigned long *)0x400FE604))          // 0x400F.E000 + 0x604
#define SYSCTL_RCGCUART             (*((volatile unsigned long *)0x400FE618))          // 0x400F.E000 + 0x618
#define CLOCK_GATE                  0xD                                                // Port A, C, D Clock Pin


// Port A Config (UART0)
#define UART0                       (*((volatile unsigned long *)0x4000C000))          // OFFSET: 0
#define UART0CTL                    (*((volatile unsigned long *)0x4000C030))          // OFFSET: 0x30
#define UART0LCRH                   (*((volatile unsigned long *)0x4000C02C))          // OFFSET: 0x24
#define UART0FR                     (*((volatile unsigned long *)0x4000C018))          // OFFSET: 0x18
#define UART0DR                     (*((volatile unsigned long *)0x4000C000))          // OFFSET: 0xC00
#define UART0IBRD                   (*((volatile unsigned long *)0x4000C024))          // OFFSET: 0x24
#define UART0FBRD                   (*((volatile unsigned long *)0x4000C028))          // OFFSET: 0x28
#define UART0_CC                    (*((volatile unsigned long *)0x4000CFC8))          // OFFSET: 0xFC8
#define UARTCTLEN                   0x101;                                             // UART Trasmit


// UART GPIO PINS (PA0, PA1)
#define GPIO_PORTA_DEN              (*((volatile unsigned long *)0x4000451C))          // OFFSET: 0x51C
#define GPIO_PORTA_AFSEL            (*((volatile unsigned long *)0x40004420))          // OFFSET: 0x420
#define GPIO_PORTA_PCTL             (*((volatile unsigned long *)0x4000452C))          // OFFSET: 0x52C
#define GPIO_PORTA_AMSEL            (*((volatile unsigned long *)0x40004528))          // OFFSET: 0x528
//#define PCTL_PIN1  0x2

#define GPIO_PORTA_DATA_KEY         (*((volatile unsigned long *)0x400043C0))          // OFFSET: 0x3C0 -> 001111000000 (Pins 4-7)
#define GPIO_PORTA_DIR              (*((volatile unsigned long *)0x40004400))          // OFFSET: 0x400
#define GPIO_PORTA_PDR              (*((volatile unsigned long *)0x40004514))          // OFFSET: 0x514
#define GPIO_PORTA_CR               (*((volatile unsigned long *)0x40004524))          // OFFSET: 0x524 - GPIO Commit


// Port C Config (LED + Pushbutton)
#define GPIO_PORTC_DEN_R            (*((volatile unsigned long *)0x4000651C))          // OFFSET: 0x51C
#define GPIO_PORTC_DIR_R            (*((volatile unsigned long *)0x40006400))          // OFFSET: 0x400
#define GPIO_PORTC_DATA_LED         (*((volatile unsigned long *)0x40006080))          // OFFSET: 0x80  -> 010000000 (pin 5)
#define GPIO_PORTC_IS               (*((volatile unsigned long *)0x40006404))          // OFFSET: 0x404 - Interrupt Sense
#define GPIO_PORTC_IEV              (*((volatile unsigned long *)0x4000640C))          // OFFSET: 0x40C - Interrupt Event
#define GPIO_PORTC_IM               (*((volatile unsigned long *)0x40006410))          // OFFSET: 0x410 - Interrupt Mask
#define GPIO_PORTC_ICR              (*((volatile unsigned long *)0x4000641C))          // OFFSET: 0x41C - Interrupt Clear


// Port D Config (Keypad)
#define GPIO_PORTD_DATA_KEY         (*((volatile unsigned long *)0x4000703C))          // OFFSET: 0x03C -> 00111100 (pins 0-3)
#define GPIO_PORTD_DIR              (*((volatile unsigned long *)0x40007400))          // OFFSET: 0x400
#define GPIO_PORTD_DEN              (*((volatile unsigned long *)0x4000751C))          // OFFSET: 0x51C
#define GPIO_PORTD_CR               (*((volatile unsigned long *)0x40007524))          // OFFSET: 0x524
#define GPIO_PORTD_PDR              (*((volatile unsigned long *)0x40007514))          // OFFSET: 0x514


// Timer Configs
#define TIMER1_CTL                  (*((volatile unsigned long *)0x4003100C))          // OFFSET: 0xC
#define TIMER1_CFG                  (*((volatile unsigned long *)0x40031000))          // OFFSET: 0x0
#define TIMER1_TAMR                 (*((volatile unsigned long *)0x40031004))          // OFFSET: 0x4
#define TIMER1_TAPR                 (*((volatile unsigned long *)0x40031038))          // OFFSET: 0x38
#define TIMER1_TAILR                (*((volatile unsigned long *)0x40031028))          // OFFSET: 0x28
#define TIMER1_MICR                 (*((volatile unsigned long *)0x40031024))          // OFFSET: 0x24
#define TIMER1_MIMR                 (*((volatile unsigned long *)0x40031018))          // OFFSET: 0x18


// Vector Table
#define NVIC_EN0                    (*((volatile unsigned long *)0xE000E100))          // OFFSET: 0x100


// Pins
#define PIN0_3                      0xF
#define PIN4_7                      0xF0
#define GPIO_PIN5                   0x20                                               // Pin 5 mask
#define GPIO_PIN6                   0x40                                               // Pin 6 mask
#define GPIO_PIN56                  0x60                                               // Pin 5+6 mask


/**************************************
              End MACROS
 *************************************/



// UART Transmit
void UART_Tx (char data) {
    while((UART0FR & 0x20) == 1);
    UART0DR = data;
}


// Function to handle timer interrupt
void TIMER1A_INTERRUPT_HANDLER() {
    GPIO_PORTC_DATA_LED ^= (1<<5);                  // Flips the LED
    TIMER1_MICR |= (1<<0);                          // Clear timeout Flag
}

// Function to handle GPIO interrupt from port C
void PUSHBUTTON_INTERRUPT_HANDLER() {
    UART_Tx('$');
    volatile unsigned i;
    //for (i = 0; i < 10000; i++);
    GPIO_PORTC_ICR |= (1<<6);
}


// Init GPIO Helper
void initGPIO () {
    GPIO_PORTA_CR |= PIN4_7;
    GPIO_PORTA_DEN |= PIN4_7;
    GPIO_PORTA_DIR |= PIN4_7;                         // set to 1 for output
    //GPIO_PORTA_PDR |= PIN4_7;                       // Enable pull down

    GPIO_PORTC_DEN_R |= GPIO_PIN56;                 // enable pins 5 and 6 on PORTC
    GPIO_PORTC_DIR_R |= GPIO_PIN5;                  // set direction pin 5 to output
    GPIO_PORTC_DIR_R &= ~GPIO_PIN6;                 // set direction pin 6 to input

    GPIO_PORTD_CR |= PIN0_3;
    GPIO_PORTD_DEN |= PIN0_3;
    GPIO_PORTD_DIR |= 0x00;                         // set as input
    GPIO_PORTD_PDR |= PIN0_3;                       // Enable pull down
}

// Init Timer helper
void initTIMER() {
    // Uses Timer1A
    SYSCTL_RCGCTIMER_R |= (1<<1);                   // Enable clock to Timer 1
    TIMER1_CTL &= (~(1<<0));                        // disable timer before config
    TIMER1_CFG = 0x4;                               // Set bandwith to 16bits
    TIMER1_TAMR = 0x2;                              // Set Periodic Timer Mode, Default set to Down Count
    TIMER1_TAPR = 245;                              // Set Prescale
    TIMER1_TAILR = 65360;                           // Set Delay
    NVIC_EN0 |= (1<<21);                            // Enable Timer 1A: 21 = interrupt number
    TIMER1_MIMR |= (1<<0);                          // Interrupt is Enabled
    TIMER1_MICR |= (1<<0);                          // Clear timeout Flag
    TIMER1_CTL |= (1<<0);                           // Enable timer
}

// Init Pushbutton helper
void initPushbuttonInterrput() {
    GPIO_PORTC_IS &= ~GPIO_PIN6;                     // Edge Sensitive
    GPIO_PORTC_IEV |= GPIO_PIN6;                     // Rising Edge Trigger
    GPIO_PORTC_IM |= GPIO_PIN6;                      // Set pin 6 to send interrupts to controller
    NVIC_EN0 |= (1<<2);                              // Enable interrupt for Port C, number 2
}

// Init UART
void initUART0() {
    UART0CTL = 0x0;                                 // disable uart0
    GPIO_PORTA_DEN |= (1<<1);                       // Enable port A, pin 1
    GPIO_PORTA_AFSEL |= (1<<1);                     // Sets pin to UART
    GPIO_PORTA_PCTL |= (1<<1);                      // Sets port to UART Transmit
    GPIO_PORTA_AMSEL = 0;
    UART0_CC = 0;                                   // Sets UART to asynchronous
    UART0IBRD = 104;                                // 9600 BPS; B = F/16 * BRD; F = 16MHz, BRD = 104.16
    UART0FBRD = 11;                                 // 0.16 * 64 + 0.5
    UART0LCRH = 0x60;                               // 01100000 Config Line Control for 8 data bits, no parity, 1 stop bit
    UART0CTL |= 0x101;                              // enable uart to transmit
}


// char array mapping keypad
const char keypad[4][4] = {{'1','2','3','A'},
                                 {'4','5','6','B'},
                                 {'7','8','9','C'},
                                 {'*','0','#','D'}};


void DELAY() {
    volatile unsigned i;
    for (i = 0; i < 1000000; i++) {

    }
}

int main(void)
{

    volatile unsigned i;
    volatile unsigned j;
    volatile unsigned k;
    volatile unsigned row;
    volatile unsigned col;

    // Enable Clocks
    SYSCTL_RCGCGPIO_R |= CLOCK_GATE;                // Enable Port Clocks
    SYSCTL_RCGCUART |= 0x1;                         // enable clock to UART0

    for(i = 0; i < 10000; i++);

    initUART0();

    initGPIO();

    initTIMER();

    initPushbuttonInterrput();

    while(1) {

        while(1) {
            row = 0;
            GPIO_PORTD_DATA_KEY = 0x0E;
            DELAY();
            col = GPIO_PORTA_DATA_KEY & 0xFF;
            if (col != 0xFF) {
                UART_Tx('A');
                break;
            }

            row = 1;
            GPIO_PORTD_DATA_KEY = 0x0D;
            DELAY();
            col = GPIO_PORTA_DATA_KEY & 0xFF;
            if (col != 0xFF) {
                UART_Tx('B');
                break;
            }


            row = 2;
            GPIO_PORTD_DATA_KEY = 0x0B;
            DELAY();
            col = GPIO_PORTA_DATA_KEY & 0xFF;
            if (col != 0xFF) {
                UART_Tx('C');
                break;
            }

            row = 3;
            GPIO_PORTD_DATA_KEY = 0x07;
            DELAY();
            col = GPIO_PORTA_DATA_KEY & 0xFF;
            if (col != 0xFF) {
                UART_Tx('D');
                break;
            }
        }

        if (col == 0x10) {
            UART_Tx(keypad[row][0]);
        }

        if (col == 0x20) {
            UART_Tx(keypad[row][1]);
        }

        if (col == 0x40) {
            UART_Tx(keypad[row][2]);
        }

        if (col == 0x80) {
            UART_Tx(keypad[row][3]);
        }
    }
}
