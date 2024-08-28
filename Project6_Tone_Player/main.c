/**
 * Project 6: Tone PLayer/Recorder
 Group: Sahil D., Michael L.

 Ports Used:
    Keypad Rows: Port A [4-7]
    Keypad Cols: Port C [4-7]


 */

#define GPIO_CLK            (*((volatile unsigned long *)0x400FE608)) // 0x400F.E000 + 0x608
#define I2C_CLK             (*((volatile unsigned long *)0x400FE620)) // 0x400F.E000 + 0x608
#define SYSCTL_RCGCPWM_R            (*((volatile unsigned long *)0x400FE640))          // 0x400F.E000 + 0x640
#define SYSCTL_RCC                  (*((volatile unsigned long *)0x400FE060))          // 0x400F.E000 + 0x060 | pwm [19:17]

#define PORTA_ROW_DATA      (*((volatile unsigned long *)0x400043C0)) // Data Register - 1111000000 -> Pins 4-7
#define PORTA_DEN           (*((volatile unsigned long *)0x4000451C)) // Digital Enable
#define PORTA_DIR           (*((volatile unsigned long *)0x40004400)) // Pin Direction
#define PORTA_PUR           (*((volatile unsigned long *)0x40004510)) // Pull-up/down

#define GPIO_PORTB_AFSEL    (*((volatile unsigned long *)0x40005420))          // OFFSET: 0x420
#define GPIO_PORTB_PCTL     (*((volatile unsigned long *)0x4000552C))          // OFFSET: 0x420
#define GPIO_PORTB_DEN      (*((volatile unsigned long *)0x4000551C))          // OFFSET: 0x420

#define PORTC_COL_DATA      (*((volatile unsigned long *)0x400063C0)) // Data Register - 1111000000 -> Pins 4-7
#define PORTC_DEN           (*((volatile unsigned long *)0x4000651C)) // Digital Enable
#define PORTC_DIR           (*((volatile unsigned long *)0x40006400)) // Pin Direction

#define PORTE_DEN           (*((volatile unsigned long *)0x4002451C))
#define PORTE_AFSEL         (*((volatile unsigned long *)0x40024420))
#define PORTE_PCTL          (*((volatile unsigned long *)0x4002452C))
#define PORTE_ODR           (*((volatile unsigned long *)0x4002450C))
#define PORTE_PUR           (*((volatile unsigned long *)0x40024510))
#define PORTE_PDR           (*((volatile unsigned long *)0x40024514))

#define PORTF_DATA          (*((volatile unsigned long *)0x40025038))
#define PORTF_DEN           (*((volatile unsigned long *)0x4002551C))
#define PORTF_DIR           (*((volatile unsigned long *)0x40025400))


#define PWMCTL                      (*((volatile unsigned long *)0x40028000))          // PWM0 base address
#define PWM0CTL                     (*((volatile unsigned long *)0x40028040))          // PWM0CTL (offset: 0x040)
#define PWM1CTL                     (*((volatile unsigned long *)0x40028080))          // PWM0CTL (offset: 0x040)
#define PWM0GENA                    (*((volatile unsigned long *)0x40028060))          // PWM0CTL (offset: 0x060)
#define PWM0GENB                    (*((volatile unsigned long *)0x40028064))          // PWM0CTL (offset: 0x064)
#define PWM0LOAD                    (*((volatile unsigned long *)0x40028050))          // PWM0CTL (offset: 0x064)
#define PWM0CMPA                    (*((volatile unsigned long *)0x40028058))          // PWM0CTL (offset: 0x064)
#define PWM0CMPB                    (*((volatile unsigned long *)0x4002805C))          // PWM0CTL (offset: 0x064)
#define PWMENABLE                   (*((volatile unsigned long *)0x40028008))          // PWM0CTL (offset: 0x064)

#define I2C2MSA             (*((volatile unsigned long *)0x40022000))
#define I2C2MDR             (*((volatile unsigned long *)0x40022008))
#define I2C2MTPR            (*((volatile unsigned long *)0x4002200C))
#define I2C2MCS             (*((volatile unsigned long *)0x40022004))
#define I2C2MCR             (*((volatile unsigned long *)0x40022020))



float notes[8] = {
                  2093.04,  //C4
                  2349.28,  //D4
                  2637.04,  //E4
                  2793.84,  //F4
                  3136.9,   //G4
                  3520.0,   //A4
                  3951.04,  //B4
                  4186.0    //C5
};


void DELAY(unsigned int n) {
    volatile unsigned i;
    for (i = 0; i < n; i++) {}
}

// Keypad Map
const char keypad[4][4] = {{  0,   1,   2, 'A'},
                           {  3,   4,   5, 'B'},
                           {  6,   7, '9', 'C'},
                           {'*', '0', '#', 'D'}};

void NoteSet(float note);
void DisableNote();

// Scan Keypad
char keypad_getkey(unsigned isNote) {

  volatile unsigned c = 0;
  volatile unsigned r = 0;

  while (1) {
    for (c = 0; c < 4; c++) {         // Traverse cols
      PORTC_COL_DATA = ~(1 << c + 4); // Push low (0)
      DELAY(50000);
      for (r = 0; r < 4; r++) {                     // Traverse rows
        if ((PORTA_ROW_DATA & (1 << r + 4)) == 0) { // Check which bit is set
          if(isNote && keypad[r][c] <= 8){
            float note = notes[keypad[r][c]];
            NoteSet(note);
          }
          while ((PORTA_ROW_DATA & (1 << r + 4)) == 0) {
          } // Wait to read bit
          if(isNote && keypad[r][c] <= 8){
            DisableNote();
          }
          return keypad[r][c];
        }
      }
    }
  }
}

void GPIO_INIT() {
    GPIO_CLK |= 0x37;           // Ports A, B, C, E, F
    I2C_CLK |= 0x04;    // I2C2

    DELAY(50000);

    // Port A
    PORTA_DEN |= 0xF0;        // Digital Enable
    PORTA_DIR = 0x00;           // Set input
    PORTA_PUR |= 0xF0;        // set weak pull-up

    // Port B
    GPIO_PORTB_DEN = 0xC0;                  // digi enable B
    GPIO_PORTB_AFSEL |= 0xC0;               // alt function on pin 6 and 7
    GPIO_PORTB_PCTL |= 0x44000000;          // pin 6 and 7 set to 4 (PWM alt fn)

    // Port C
    PORTC_DEN |= 0xF0;        // Digital Enable
    PORTC_DIR |= 0xF0;        // Set output

    // Port E
    PORTE_DEN |= 0x30;        // Enable D0, D1
    PORTE_AFSEL |= 0x30;
    PORTE_PCTL |= 0x330000;       // Enable I2C3 on pins 0 and 1
    PORTE_ODR |= 0x20;        // Open Drain P
    //PORTE_PDR = 0x0;
    PORTE_PUR |= 0x30;

    // Port F
    PORTF_DEN |= 0x0E;
    PORTF_DIR |= 0x0E;
}

void PWMInit(){
    // M0PWM0 pin on PB6 digital fn 4 AND M0PWM1 PB7
    // Well use PWM0 module
    SYSCTL_RCGCPWM_R |= 0x01;               // enable PWM module 0
    DELAY(100000);
    PWMCTL =0x0;                            // disable PWM
    PWM0CTL |= 0x0;                         // disable pwm 0
    SYSCTL_RCC |= 0x100000;                 // DIV 2, PWM DIV ENABLE
    SYSCTL_RCC &= ~0xE0000;                 // DIV 2, PWM DIV ENABLE
    DELAY(100000);

    //PWM0LOAD = 399;                         // Load = 399 (10Mhz = 400 per sec, 399 = 400 -1)
    //PWM0CMPA = 260;                         // CMPA = 260 (changes with init)
    PWM0GENA = 0x0000008C;                  //Drive pwmA Low on compA down; Drive pwmA high on counter=load
    DELAY(10000);
    PWM0CTL |= 0x1;                         // enable pwm0 count down mode
    //PWMENABLE = 0x00000003;                 // enable output
    PWMCTL |=0x1;                            // enable pwm
}

void I2C3_INIT() {
   /*
    * SCL_PERIOD = 2×(1 + TPR)×(SCL_LP + SCL_HP)×CLK_PRD
    *    100000 = 2×(1 + TPR)×(6 + 4)×1/16000000
    *    TPR = 7
    */
    I2C2MCR |= 0x10;
    I2C2MTPR |= 0x07;
    DELAY(10000);
}

char I2C_read(unsigned address){
    //PORTE_PUR = 0x0;
    //PORTE_PDR = 0x20;
    // reads a char at the given location in memory
    I2C2MSA = 0xA0;      // EEPROM ADDRESS + WRITE mode
    I2C2MDR = (address >> 8);      // High Address
    I2C2MCS = 0x03;      // start and tx
    I2C2MDR = (address & 0xFF);// low address
    I2C2MCS = 0x01;      // tx
    while(I2C2MCS & 1);

    // repeated start
    I2C2MSA = 0xA1;      // EEPROM ADDRESS + read mode
    I2C2MCS = 0x07;      // 0b0111 starts receives and stops
    while(I2C2MCS & 1);
    char data = I2C2MDR;
    return data;
}

void I2C_write(char data, unsigned address){
    //PORTE_PDR = 0x0;
    //PORTE_PUR = 0x20;
    I2C2MSA = 0xA0;      // EEPROM ADDRESS
    I2C2MDR = (address >> 8);      // High Address
    I2C2MCS = 0x3;       // start and tx
    while(I2C2MCS & 1);
    //I2C3MDR &= ~0xFF;
    I2C2MDR = address & 0xFF;   // low address
    I2C2MCS = 0x1;       // tx
    while(I2C2MCS & 1);
    //I2C3MDR &= ~0xFF;
    I2C2MDR = data;      // data
    I2C2MCS = 0x5;       // tx and stop
    while(I2C2MCS & 1);
    }

void NoteSet(float note){
    PWM0CTL = 0x0;
    PWM0LOAD = (unsigned) note;
    PWM0CMPA = note / 2;
    PWM0CTL |= 0x01;        // START timer
    PWMENABLE = 0x00000003;                 // enable output
}

void DisableNote(){
    PWMENABLE = 0x00000000;                 // enable output
}

void quickNote(float note){
    NoteSet(note);
    DELAY(1600000);    // delay for a sec
    DisableNote();
}

void playSong(unsigned n){
    char note_ind;
    unsigned i;
    for(i = 0; i < n; i++){
        note_ind = I2C_read(i);
        quickNote(notes[note_ind]);
    }
}

int main(void) {
    /* states:
     *  0: live
     *  1: record
     *  2: playback
     *  */
    GPIO_INIT();
    I2C3_INIT();
    PWMInit();
    unsigned state = 0;
    char input;
    unsigned n = 0;
    int cont;
    while(1){
        switch(state){
        PORTF_DATA = (1<<(state+1);
            case(0):
                cont = 1;
                while(cont){
                    input = keypad_getkey(1);
                    switch(input){
                    case('A'):
                        state = 0;
                        cont = 0;
                        break;
                    case('B'):
                        state = 1;
                        cont = 0;
                        break;
                    case('C'):
                        state = 2;
                        cont = 0;
                        break;
                    }
                }
                break;
            case(1):
                n = 0;
                cont = 1;
                while(cont){
                    input = keypad_getkey(1);
                    switch(input){
                    case('A'):
                        state = 0;
                        cont = 0;
                        break;
                    case('B'):
                        state = 1;
                        cont = 0;
                        break;
                    case('C'):
                        state = 2;
                        cont = 0;
                        break;
                    case('D'):
                        break;
                    case('*'):
                        break;
                    case('#'):
                        break;
                    case('0'):
                        break;
                    case('9'):
                        break;
                    default:
                        I2C_write(input, n);
                        n++;
                        break;
                    }
                }
                break;
            case(2):
                cont = 1;
                while(cont){
                    input = keypad_getkey(0);
                    switch(input){
                    case('A'):
                        state = 0;
                        cont = 0;
                        break;
                    case('B'):
                        state = 1;
                        cont = 0;
                        break;
                    case('C'):
                        state = 2;
                        cont = 0;
                        break;
                    case('D'):
                        playSong(n);
                        break;
                    }
                }
                break;
        }
    }

}
