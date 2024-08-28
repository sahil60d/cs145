/*
 * Project 4: Calculator
 * Team Members: Sahil D., Michael L.
 *
 * Create Simple Calculator using 4x4 Keypad Matrix and LCD Display
 *
 * Ports Used:
 *      Keypad Rows: Port A [4-7]
 *      Keypad Cols: Port C [4-7]
 *      LCD Display: Port B [0-7]
 *      LCD Info:    Port E [0-2]
 *
 */

/********** Define MACROS **********/


// Clock Config
#define GPIO_CLK               (*((volatile unsigned long *)0x400FE608))          // 0x400F.E000 + 0x608
#define CLK_MASK               0x17;                                              // Ports A,B,C,E

// Port A Config
#define PORTA_ROW_DATA         (*((volatile unsigned long *)0x400043C0))          // Data Register - 1111000000 -> Pins 4-7
#define PORTA_DEN              (*((volatile unsigned long *)0x4000451C))          // Digital Enable
#define PORTA_DIR              (*((volatile unsigned long *)0x40004400))          // Pin Direction
#define PORTA_PUR              (*((volatile unsigned long *)0x40004510))          // Pull-up/down

// Port C Config
#define PORTC_COL_DATA         (*((volatile unsigned long *)0x400063C0))          // Data Register - 1111000000 -> Pins 4-7
#define PORTC_DEN              (*((volatile unsigned long *)0x4000651C))          // Digital Enable
#define PORTC_DIR              (*((volatile unsigned long *)0x40006400))          // Pin Direction

// Port B Config
#define PORTB_LCD_DATA         (*((volatile unsigned long *)0x400053FC))          // Data Register - 1111111100 -> Pins 0-7
#define PORTB_DEN              (*((volatile unsigned long *)0x4000551C))          // Digital Enable
#define PORTB_DIR              (*((volatile unsigned long *)0x40005400))          // Pin Direction

// Port E Config
#define PORTE_LCD_INFO_DATA    (*((volatile unsigned long *)0x4002401C))          // Data Register - 11100 -> Pins 0-2
#define PORTE_DEN              (*((volatile unsigned long *)0x4002451C))          // Digital Enable
#define PORTE_DIR              (*((volatile unsigned long *)0x40024400))          // Pin Direction

// Pins
#define PIN0_2                 0x7;
#define PIN4_7                 0xF0;
#define PIN0_7                 0xFF


/************ End MACROS ***********/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char A[9];
char B[9];

void DELAY(unsigned int n) {
    volatile unsigned i;
    for (i = 0; i < n; i++) {}
}

// Init GPIO Pins
void GPIO_INIT(void) {
    // GPIO CLOCK
    GPIO_CLK |= CLK_MASK;
    DELAY(50000);

    // Port A
    PORTA_DEN |= PIN4_7;        // Digital Enable
    PORTA_DIR = 0x00;           // Set input
    PORTA_PUR |= PIN4_7;        // set weak pull-up

    // Port C
    PORTC_DEN |= PIN4_7;        // Digital Enable
    PORTC_DIR |= PIN4_7;        // Set output

    // Port B
    PORTB_DEN |= PIN0_7;        // Digital Enable
    PORTB_DIR |= PIN0_7;        // Set output

    // Port E
    PORTE_DEN |= PIN0_2;        // Digital Enable
    PORTE_DIR |= PIN0_2;        // Set output
}

// Keypad Map
const char keypad[4][4] = {{'1','2','3','A'},
                                 {'4','5','6','B'},
                                 {'7','8','9','C'},
                                 {'*','0','#','D'}};

// Scan Keypad
char keypad_getkey(void) {

    volatile unsigned c = 0;
    volatile unsigned r = 0;

    while(1)
    {
        for(c = 0; c < 4; c++) {                                // Traverse cols
            PORTC_COL_DATA = ~(1 << c+4);                       // Push low (0)
            DELAY(50000);
            for(r = 0; r < 4; r++){                             // Traverse rows
                if((PORTA_ROW_DATA & (1 << r+4)) == 0) {        // Check which bit is set
                    while((PORTA_ROW_DATA & (1 << r+4)) == 0){} // Wait to read bit
                    return keypad[r][c];
                }
            }
        }
    }
}

// Set LCD Command
void Set_LCD_Cmd(char cmd) {
    PORTE_LCD_INFO_DATA = 0x00;                                 // RS=0, RW=0, E=0 - Command mode
    PORTB_LCD_DATA = 0x00;
    PORTB_LCD_DATA = cmd;                                       // Write command to LCD
    PORTE_LCD_INFO_DATA = 0x04;                                 // E=1 - set command
    DELAY(50000);
    PORTE_LCD_INFO_DATA = 0x00;                                 // RS=0, RW=0, E=0
    PORTB_LCD_DATA = 0x00;
}

// Set LCD Data
void Set_LCD_Data(char data) {
    PORTE_LCD_INFO_DATA = 0x01;                                 // RS=1, RW=0, E=0 - Data mode
    PORTB_LCD_DATA = 0x00;
    PORTB_LCD_DATA = data;                                      // Write data to LCD
    PORTE_LCD_INFO_DATA = 0x05;                                 // RS=1, RW=0, E-1 - set data
    DELAY(50000);
    PORTE_LCD_INFO_DATA = 0x00;                                 // RS=0, RW=0, E=0
    PORTB_LCD_DATA = 0x00;
}

// Init LCD Display
void LCD_INIT(void) {
    Set_LCD_Cmd(0x06);                                          // Entry Mode set: 110 -> increment cursor, display shift off
    Set_LCD_Cmd(0x38);                                          // Function set: 111000 -> 8-bit mode, 2 lines, 5x8
    Set_LCD_Cmd(0x0F);                                          // Display on: 1111 -> Display on, cursor on, blink on
    Set_LCD_Cmd(0x01);                                          // Clear Display
}


void Clear_Line1(void) {
    Set_LCD_Cmd(0x0C);                                          // Display on: 1100 -> Display on, cursor off, blink off
    Set_LCD_Cmd(0x02);                                          // Return Home
    volatile unsigned i;
    for (i = 0; i < 16; i++) {
        Set_LCD_Data(' ');
    }
    Set_LCD_Cmd(0x02);                                          // Return Home
    Set_LCD_Cmd(0x0F);                                          // Display on: 1111 -> Display on, cursor on, blink on
}

void Clear_Line2(void) {
    Set_LCD_Cmd(0x0C);                                          // Display on: 1100 -> Display on, cursor off, blink off
    Set_LCD_Cmd(0xC0);                                          // Return Home
    volatile unsigned i;
    for (i = 0; i < 16; i++) {
        Set_LCD_Data(' ');
    }
    Set_LCD_Cmd(0xC0);                                          // Return Home
    Set_LCD_Cmd(0x0F);                                          // Display on: 1111 -> Display on, cursor on, blink on
}


// Convert int to string for display
void displaynum(int n) {
    unsigned char num[16];
    short int count = 0;
    num[0] = '0';

    if(n == 0) {
        Set_LCD_Data('0');
        return;
    }

    while(n>0) {
       num[count++] = (unsigned char) ( n %10 + 48 );
       n /=10;
    }

    for(n=count-1;n>=0;n--) {
       Set_LCD_Data(num[n]);
    }
}

// Function Declarations
unsigned State_Init(void);
unsigned State_Start(void);
unsigned State_A(void);
unsigned State_B(void);
unsigned State_Display(void);

int main(void)
{
	GPIO_INIT();
	LCD_INIT();

	// States: 0=Initial, 1=Start, 2=A, 3=B, 4=Display
	volatile unsigned state = 0;

	while(1) {

	    switch(state) {
	    case 0:
	        state = State_Init();                               // Initial State
	        break;
	    case 1:                                                 // Start State
	        state = State_Start();
	        break;

	    case 2:                                                 // A
	        state = State_A();
	        break;
	    case 3:                                                 // B
	        state = State_B();
	        break;
	    case 4:                                                 // Display
	        state = State_Display();
	        break;
	    }
	}
}

unsigned State_Init(void) {
    Set_LCD_Cmd(0x01);                                          // Clear Display
    Set_LCD_Cmd(0x02);                                          // Return Home

    return 1;
}

unsigned State_Start(void) {
    Set_LCD_Cmd(0x02);                                          // Return Home
    Clear_Line1();
    A[0] = '\0';
    B[0] = '\0';

    return 2;
}

unsigned State_A(void) {
    char input;
    volatile unsigned count = 0;
    while(1) {
        input = keypad_getkey();

        switch(input) {
        case 'A':
            break;
        case 'B':
            break;
        case 'D':
            break;
        case 'C':
            return 0;
        case '*':
            return 3;
        case '#':
            return 4;
        default:
            Set_LCD_Data(input);
            strncat(A, &input, 1);
            count++;
            if (count == 8) {
                count = 0;
                return 3;
            }
            break;
        }
    }
}

unsigned State_B(void) {
    Clear_Line1();
    char input;
    volatile unsigned count = 0;
    while(1) {
        input = keypad_getkey();

        switch(input) {
        case 'A':
            break;
        case 'B':
            break;
        case 'D':
            break;
        case 'C':
            return 0;
        case '*':
            break;
        case '#':
            return 4;
        default:
            Set_LCD_Data(input);
            strncat(B, &input, 1);
            count++;
            if (count == 8) {
                count = 0;
                return 4;
            }
            break;
        }
    }
}

unsigned State_Display(void) {
    Clear_Line2();

    Set_LCD_Cmd(0xC0);                                          // move cursor to the second row

    if(A[0] == '\0' || B[0] == '\0'){
        Set_LCD_Data('0');
        return 1;
    }

    volatile unsigned a = atoi(A);
    volatile unsigned b = atoi(B);
    int ans = a*b;

    displaynum(ans);

    return 1;

}
