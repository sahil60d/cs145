/**
 * Project 5: Dim an LED
 * main.c
 */

/* Define MACROS */

#define SYSCTL_RCGCPWM_R            (*((volatile unsigned long *)0x400FE640))          // 0x400F.E000 + 0x640
#define SYSCTL_RCC                  (*((volatile unsigned long *)0x400FE060))          // 0x400F.E000 + 0x060 | pwm [19:17]
#define SYSCTL_RCGC0                (*((volatile unsigned long *)0x400FE100))          // 0x400F.E000 + 0x060 | pwm [19:17]
#define SYSCTL_RCGC2                (*((volatile unsigned long *)0x400FE108))          // 0x400F.E000 + 0x060 | pwm [19:17]

#define GPIO_PORTB_AFSEL            (*((volatile unsigned long *)0x40005420))          // OFFSET: 0x420
#define GPIO_PORTB_PCTL             (*((volatile unsigned long *)0x4000552C))          // OFFSET: 0x420
#define GPIO_PORTB_DEN              (*((volatile unsigned long *)0x4000551C))          // OFFSET: 0x420


#define PWMCTL                      (*((volatile unsigned long *)0x40028000))          // PWM0 base address
#define PWM0CTL                     (*((volatile unsigned long *)0x40028040))          // PWM0CTL (offset: 0x040)
#define PWM1CTL                     (*((volatile unsigned long *)0x40028080))          // PWM0CTL (offset: 0x040)
#define PWM0GENA                    (*((volatile unsigned long *)0x40028060))          // PWM0CTL (offset: 0x060)
#define PWM0GENB                    (*((volatile unsigned long *)0x40028064))          // PWM0CTL (offset: 0x064)
#define PWM0LOAD                    (*((volatile unsigned long *)0x40028050))          // PWM0CTL (offset: 0x064)
#define PWM0CMPA                    (*((volatile unsigned long *)0x40028058))          // PWM0CTL (offset: 0x064)
#define PWM0CMPB                    (*((volatile unsigned long *)0x4002805C))          // PWM0CTL (offset: 0x064)
#define PWMENABLE                   (*((volatile unsigned long *)0x40028008))          // PWM0CTL (offset: 0x064)


// GPIO Config
#define RCGCGPIO (*((volatile unsigned long *)0x400FE608))
#define PE_AFSEL (*((volatile unsigned long *)0x40024420))
#define PE_AMSEL (*((volatile unsigned long *)0x40024528))
#define PE_DEN (*((volatile unsigned long *)0x4002451C))


// ADC Config
#define RCGCADC (*((volatile unsigned long *)0x400FE638))
#define ADC0_ACTSS (*((volatile unsigned long *)0x40038000))
#define ADC0_SSMUX0 (*((volatile unsigned long *)0x40038040))
#define ADC0_SSMUX3 (*((volatile unsigned long *)0x400380A0))
#define ADC0_SSCTL3 (*((volatile unsigned long *)0x400380A4))
#define ADC0_EMUX (*((volatile unsigned long *)0x40038014))
#define ADC0_PSSI (*((volatile unsigned long *)0x40038028))
#define ADC0_RIS (*((volatile unsigned long *)0x40038004))
#define ADC0_ISC (*((volatile unsigned long *)0x4003800C))
#define ADC0_SSFIFO3 (*((volatile unsigned long *)0x400380A8))


/* End MACROS */

void DELAY(unsigned int n) {
    volatile unsigned i;
    for (i = 0; i < n; i++) {}
}

// init GPIO
void GPIO_init() {
  RCGCGPIO |= (1 << 4); // Enable clock for port E, used for AN0
  DELAY(10000);
  PE_AFSEL |= (1 << 3); // Enable pin 3 afsel to use alternate function
  PE_DEN &= ~(1<<3);

  PE_AMSEL |= (1 << 3); // Enable pin 3 amsel to use AN0
}

// init ADC
void ADC_init() {
  RCGCADC |= (1 << 0);     // Enable clock ADC0
  DELAY(10000);
  ADC0_ACTSS &= ~(1 << 3); // disable SS3
  ADC0_EMUX &= ~0xF000;    // set SS3 trigger to processor
  ADC0_SSMUX3 = 0;         // selects AN0
  ADC0_SSCTL3 |= (1<<1)|(1<<2); // sets flag at first sample, takes 1 sample at a time
  DELAY(10000);
  ADC0_ACTSS |= (1 << 3);  // enable SS3
}

void PWMInit(){
    // M0PWM0 pin on PB6 digital fn 4 AND M0PWM1 PB7
    // Well use PWM0 module
    RCGCGPIO |=0x2;                         // enable port B
    SYSCTL_RCGCPWM_R |= 0x01;               // enable PWM module 0
    DELAY(100000);
    PWMCTL =0x0;                            // disable PWM
    GPIO_PORTB_DEN = 0xC0;                  // digi enable B
    GPIO_PORTB_AFSEL |= 0xC0;               // alt function on pin 6 and 7
    GPIO_PORTB_PCTL |= 0x44000000;          // pin 6 and 7 set to 4 (PWM alt fn)
    PWM0CTL |= 0x0;                         // disable pwm 0
    SYSCTL_RCC |= 0x100000;                 // DIV 2, PWM DIV ENABLE
    SYSCTL_RCC &= ~0xE0000;                 // DIV 2, PWM DIV ENABLE
    DELAY(100000);

    PWM0LOAD = 399;                         // Load = 399 (10Mhz = 400 per sec, 399 = 400 -1)
    PWM0CMPA = 260;                         // CMPA = 260 (changes with init)
    PWM0GENA = 0x0000008C;                  //Drive pwmA Low on compA down; Drive pwmA high on counter=load
    DELAY(10000);
    PWM0CTL |= 0x1;                         // enable pwm0 count down mode
    PWMENABLE = 0x00000003;                 // enable output
    PWMCTL =0x1;                            // enable pwm
}

void PWMSet(unsigned x){
    volatile unsigned cc = ((float) x / 4095.0)*350.0+1;
    PWM0CTL = 0x0;
    PWM0CMPA = cc;
    PWM0CTL |= 0x01;        // START timer
}


int main(void)
{
    GPIO_init();
    DELAY(10000);
    PWMInit();
    DELAY(10000);
    ADC_init();
    DELAY(10000);

    unsigned adc_val;

    while(1){
        ADC0_PSSI |= (1 << 3); // Initiate SS3
        while ((ADC0_RIS & 0x8) == 0);                     // wait until completion flag is set
        adc_val = ADC0_SSFIFO3; // read result of conversion
        ADC0_ISC |= 0x8;        // clear interrupt status
        PWMSet(adc_val);
    }

    return 0;
}
