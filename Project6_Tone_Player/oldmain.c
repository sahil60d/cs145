///**
// * Project 6: Tone PLayer/Recorder
// Group: Sahil D., Michael L.
//
// Ports Used:
//    Keypad Rows: Port A [4-7]
//    Keypad Cols: Port C [4-7]
//    PWM: Port B
//    I2C3: Port D
//
// */
//
//#define GPIO_CLK            (*((volatile unsigned long *)0x400FE608)) // 0x400F.E000 + 0x608
//#define I2C_CLK             (*((volatile unsigned long *)0x400FE620))
//#define PORTA_ROW_DATA      (*((volatile unsigned long *)0x400043C0)) // Data Register - 1111000000 -> Pins 4-7
//#define PORTA_DEN           (*((volatile unsigned long *)0x4000451C)) // Digital Enable
//#define PORTA_DIR           (*((volatile unsigned long *)0x40004400)) // Pin Direction
//#define PORTA_PUR           (*((volatile unsigned long *)0x40004510)) // Pull-up/down
//#define PORTC_COL_DATA      (*((volatile unsigned long *)0x400063C0)) // Data Register - 1111000000 -> Pins 4-7
//#define PORTC_DEN           (*((volatile unsigned long *)0x4000651C)) // Digital Enable
//#define PORTC_DIR           (*((volatile unsigned long *)0x40006400)) // Pin Direction
//#define PORTD_DEN           (*((volatile unsigned long *)0x4000751C))
//#define PORTD_AFSEL         (*((volatile unsigned long *)0x40007420))
//#define PORTD_PCTL          (*((volatile unsigned long *)0x4000752C))
//#define PORTD_ODR           (*((volatile unsigned long *)0x4000750C))
//
//#define I2C3_MSA            (*((volatile unsigned long *)0x40023000))
//#define I2C3_MDR            (*((volatile unsigned long *)0x40023008))
//#define I2C3_MTPR           (*((volatile unsigned long *)0x4002300C))
//#define I2C3_MCS            (*((volatile unsigned long *)0x40023004))
//#define I2C3_MCR            (*((volatile unsigned long *)0x40023020))
//
//#define SYSCTL_RCGCPWM_R            (*((volatile unsigned long *)0x400FE640))          // 0x400F.E000 + 0x640
//#define SYSCTL_RCC                  (*((volatile unsigned long *)0x400FE060))          // 0x400F.E000 + 0x060 | pwm [19:17]
//#define SYSCTL_RCGC0                (*((volatile unsigned long *)0x400FE100))          // 0x400F.E000 + 0x060 | pwm [19:17]
//#define SYSCTL_RCGC2                (*((volatile unsigned long *)0x400FE108))          // 0x400F.E000 + 0x060 | pwm [19:17]
//
//#define GPIO_PORTB_AFSEL            (*((volatile unsigned long *)0x40005420))          // OFFSET: 0x420
//#define GPIO_PORTB_PCTL             (*((volatile unsigned long *)0x4000552C))          // OFFSET: 0x420
//#define GPIO_PORTB_DEN              (*((volatile unsigned long *)0x4000551C))          // OFFSET: 0x420
//
//
//#define PWMCTL                      (*((volatile unsigned long *)0x40028000))          // PWM0 base address
//#define PWM0CTL                     (*((volatile unsigned long *)0x40028040))          // PWM0CTL (offset: 0x040)
//#define PWM1CTL                     (*((volatile unsigned long *)0x40028080))          // PWM0CTL (offset: 0x040)
//#define PWM0GENA                    (*((volatile unsigned long *)0x40028060))          // PWM0CTL (offset: 0x060)
//#define PWM0GENB                    (*((volatile unsigned long *)0x40028064))          // PWM0CTL (offset: 0x064)
//#define PWM0LOAD                    (*((volatile unsigned long *)0x40028050))          // PWM0CTL (offset: 0x064)
//#define PWM0CMPA                    (*((volatile unsigned long *)0x40028058))          // PWM0CTL (offset: 0x064)
//#define PWM0CMPB                    (*((volatile unsigned long *)0x4002805C))          // PWM0CTL (offset: 0x064)
//#define PWMENABLE                   (*((volatile unsigned long *)0x40028008))          // PWM0CTL (offset: 0x064)
//
//
//void DELAY(unsigned int n) {
//    volatile unsigned i;
//    for (i = 0; i < n; i++) {}
//}
//
//// Keypad Map
//const char keypad[4][4] = {{1, 2, 3, 'A'},
//                           {4, 5, 6, 'B'},
//                           {7, 8, '9', 'C'},
//                           {'*', '0', '#', 'D'}};
//
///*
// * calc: (bus freq. / pwm clock divisor) / (note frequency)
// * bus freq = 16 mhz
// * pwm clock div = 2
// */
//
//// Notes
//const unsigned notes[] = {
//                        2093,  //C4
//                        2349,  //D4
//                        2637,  //E4
//                        2793,  //F4
//                        3136,  //G4
//                        3520,  //A4
//                        3951,  //B4
//                        4186   //C5
//};
//
//void PWMSet(char key);
//void PWMDisable();
//
//
//// Scan Keypad
//char keypad_getkey(void) {
//
//  volatile unsigned c = 0;
//  volatile unsigned r = 0;
//
//  while (1) {
//    for (c = 0; c < 4; c++) {         // Traverse cols
//      PORTC_COL_DATA = ~(1 << c + 4); // Push low (0)
//      DELAY(50000);
//      for (r = 0; r < 4; r++) {                     // Traverse rows
//        if ((PORTA_ROW_DATA & (1 << r + 4)) == 0) { // Check which bit is set
//          if (keypad[r][c] <= 8) {
//              PWMSet(keypad[r][c]);
//          }
//          while ((PORTA_ROW_DATA & (1 << r + 4)) == 0) {} // Wait to read bit
//          if (keypad[r][c] <= 8) {
//            PWMDisable();
//          }
//          return keypad[r][c];
//        }
//      }
//    }
//  }
//}
//
//void GPIO_INIT() {
//    GPIO_CLK |= 0x0F;           // Ports A, B, C, D
//    I2C_CLK |=0x08;             // I2C3 enable
//    DELAY(50000);
//
//    // Port A
//    PORTA_DEN |= 0xF0;        // Digital Enable
//    PORTA_DIR = 0x00;           // Set input
//    PORTA_PUR |= 0xF0;        // set weak pull-up
//
//    // Port C
//    PORTC_DEN |= 0xF0;        // Digital Enable
//    PORTC_DIR |= 0xF0;        // Set output
//
//}
//
//void I2C3_INIT() {
//    // Port D
//    PORTD_DEN |= 0x03;        // Enable D0, D1
//    PORTD_AFSEL |= 0x03;
//    PORTD_PCTL |= 0x33;       // Enable I2C3 on pins 0 and 1
//    PORTD_ODR |= 0x02;        // Open Drain PD1
//
//    /*
//     * SCL_PERIOD = 2×(1 + TPR)×(SCL_LP + SCL_HP)×CLK_PRD
//     *    100000 = 2×(1 + TPR)×(6 + 4)×1/16000000
//     *    TPR = 7
//     */
//
//    I2C3_MCR |= 0x10;   // Set master
//    I2C3_MTPR = 0x07;
//}
//
//void I2C3_Write(int address, char data) {
//    I2C3_MSA = 0xA0;          // Slave address + write
//    I2C3_MDR = 0x00;          // High address
//    I2C3_MCS = 0x03;          // Start -> Transmit
//    I2C3_MDR = address;       // Low address
//    I2C3_MCS = 0x01;          // Transmit
//    I2C3_MDR = data;          // Data
//    I2C3_MCS = 0x05;          // Transmit -> Stop
//}
//
//char I2C3_Read(int address) {
//    I2C3_MSA = 0xA0;          // Slave address + write
//    I2C3_MDR = 0x00;          // High Address
//    I2C3_MCS = 0x03;          // Start -> Transmit
//    I2C3_MDR = address;       // Low Address
//    I2C3_MCS = 0x01;          // Transmit
//
//    I2C3_MSA = 0xA1;          // Slave address + read
//    I2C3_MDR = 0x07;          // Start -> Receive -> Stop
//    char data = I2C3_MDR;     // Read single byte
//    return data;
//}
//
//void PWMInit(){
//    // M0PWM0 pin on PB6 digital fn 4 AND M0PWM1 PB7
//    // Well use PWM0 module
//    SYSCTL_RCGCPWM_R |= 0x01;               // enable PWM module 0
//    DELAY(100000);
//    PWMCTL =0x0;                            // disable PWM
//    GPIO_PORTB_DEN = 0xC0;                  // digi enable B
//    GPIO_PORTB_AFSEL |= 0xC0;               // alt function on pin 6 and 7
//    GPIO_PORTB_PCTL |= 0x44000000;          // pin 6 and 7 set to 4 (PWM alt fn)
//    PWM0CTL |= 0x0;                         // disable pwm 0
//    SYSCTL_RCC |= 0x100000;                 // DIV 2, PWM DIV ENABLE
//    SYSCTL_RCC &= ~0xE0000;                 // DIV 2, PWM DIV ENABLE
//    DELAY(100000);
//
//    //PWM0LOAD = 399;                         // Load = 399 (10Mhz = 400 per sec, 399 = 400 -1)
//    //PWM0CMPA = 260;                         // CMPA = 260 (changes with init)
//    PWM0GENA = 0x0000008C;                  //Drive pwmA Low on compA down; Drive pwmA high on counter=load
//    DELAY(10000);
//    PWM0CTL |= 0x1;                         // enable pwm0 count down mode
//    //PWMENABLE = 0x00000003;                 // enable output
//    PWMCTL =0x1;                            // enable pwm
//}
//
//void PWMSet(char key){
//    PWM0CTL = 0x0;
//    PWM0LOAD = key;
//    PWM0CMPA = key/2;
//    PWMENABLE = 0x00000003;
//    PWM0CTL |= 0x01;        // START timer
//}
//
//void PWMDisable() {
//    PWMENABLE = 0x00;
//}
//
//void playNote()
//
//unsigned State_Play();
//unsigned State_Record();
//unsigned State_Playback();
//
//unsigned volatile count = 0; // number of keys recorded
//
//
//int main(void) {
//
//    GPIO_INIT();
//    DELAY(50000);
//    I2C3_INIT();
//    DELAY(50000);
//
//    unsigned volatile state = 0;
//
//    while(1) {
//
//        // States: 0 = A, 1 = B, 2 = C
//        switch (state) {
//        case 0:             // Play
//            state = State_Play();
//            break;
//        case 1:             // Record
//            count = 0;
//            state = State_Record();
//            break;
//        case 2:             // Playback
//            state = State_Playback();
//            break;
//
//        }
//    }
//}
//
//unsigned State_Play() {
//    char input;
//
//    while(1) {
//        input = keypad_getkey();
//
//        switch (input) {
//        case 'A':
//            return 0;
//        case 'B':
//            return 1;
//        case 'C':
//            return 2;
//        }
//    }
//}
//
//unsigned State_Record() {
//    char input;
//
//    while(1) {
//        input = keypad_getkey();
//
//        switch (input) {
//        case 'A':
//            return 0;
//        case 'B':
//            return 1;
//        case 'C':
//            return 2;
//        case 'D':
//            break;
//        case '*':
//            break;
//        case '#':
//            break;
//        case '9':
//            break;
//        case '0':
//            break;
//        default:
//            // write note to memory
//            // Default address 0x00
//
//            I2C3_Write(0x00 + count, input);
//            count++;
//            break;
//        }
//    }
//}
//
//unsigned State_Playback() {
//    char input;
//
//    while(1) {
//        input = keypad_getkey();
//
//        switch (input) {
//        case 'A':
//            return 0;
//        case 'B':
//            return 1;
//        case 'C':
//            return 2;
//        case'D':
//            // Start Playback
//            // base address 0x00
//            // read byte, play note for a second -> 16000000 cycles (16mhz clock system)
//
//            volatile unsigned i;
//            for (i = 0; i < count; i++) {
//
//            }
//        }
//    }
//}
