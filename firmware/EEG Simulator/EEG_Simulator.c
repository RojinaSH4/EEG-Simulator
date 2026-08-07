/*******************************************************
Project : EEG Brainwave Simulator
Version : 0.1
Date    : 7/27/2026
Author  : Rojina Shahabi
Comments: Microcontroller side firmware for acquiring EEG 
          band signals via ADC and transmitting via UART. 

Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega32.h>
#include <stdio.h>
#include <delay.h>

// Alphanumeric LCD functions
#include <alcd.h>

// EEG Band Values
unsigned int delta_value = 0;
unsigned int theta_value = 0;
unsigned int alpha_value = 0;
unsigned int beta_value  = 0;
unsigned int max_value   = 0;

#define SIGNAL_THRESHOLD 15

typedef enum
{
    NO_SIGNAL = 0,
    DEEP_SLEEP,
    DROWSY,
    RELAXED,
    FOCUSED

} BrainState;

BrainState brain_state = NO_SIGNAL;

// Voltage Reference: AVCC pin
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))

// Read one ADC channel
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | ADC_VREF_TYPE;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=(1<<ADSC);
// Wait for the AD conversion to complete
while ((ADCSRA & (1<<ADIF))==0);
ADCSRA|=(1<<ADIF);
return ADCW;
}

unsigned int read_adc_average(unsigned char channel)
{
    unsigned char i;
    unsigned long sum = 0;

    for(i=0; i<16; i++)
    {
        sum += read_adc(channel);
    }

    return (unsigned int)(sum/16);
}

// Read all EEG band amplitudes
void ReadEEG(void)
{
    delta_value = read_adc_average(0);
    theta_value = read_adc_average(1);
    alpha_value = read_adc_average(2);
    beta_value  = read_adc_average(3);
}
 

unsigned long total_power;

unsigned char delta_percent;
unsigned char theta_percent;
unsigned char alpha_percent;
unsigned char beta_percent;

// Determine dominant EEG band 
void ProcessEEG(void)
{
    max_value = delta_value;
    brain_state = DEEP_SLEEP;

    if(theta_value > max_value)
    {
        max_value = theta_value;
        brain_state = DROWSY;
    }

    if(alpha_value > max_value)
    {
        max_value = alpha_value;
        brain_state = RELAXED;
    }

    if(beta_value > max_value)
    {
        max_value = beta_value;
        brain_state = FOCUSED;
    }

    if(max_value < SIGNAL_THRESHOLD)
    {
        brain_state = NO_SIGNAL;
    }
      
    total_power = delta_value + theta_value + alpha_value + beta_value;

    if(total_power > 0)
    {
        delta_percent = (delta_value * 100UL) / total_power;
        theta_percent = (theta_value * 100UL) / total_power;
        alpha_percent = (alpha_value * 100UL) / total_power;
        beta_percent  = (beta_value  * 100UL) / total_power;
    }
    else
    {
        delta_percent = 0;
        theta_percent = 0;
        alpha_percent = 0;
        beta_percent = 0;
    } 
    if(total_power == 0)
    {
        brain_state = NO_SIGNAL;
    }
    else if(delta_percent >= 50)
    {
        brain_state = DEEP_SLEEP;
    }
    else if(theta_percent >= 40)
    {
        brain_state = DROWSY;
    }
    else if(alpha_percent >= 40)
    {
        brain_state = RELAXED;
    }
    else if(beta_percent >= 45)
    {
        brain_state = FOCUSED;
    }
    else
    {
        brain_state = RELAXED;
    }
}


// Display current state on LCD
char lcd_buffer[17];
char uart_buffer[80];
void DisplayEEG(void)
{
    lcd_gotoxy(0,0);

    switch(brain_state)
    {   
        case DEEP_SLEEP:
            lcd_putsf("State:Sleep   ");
            break;

        case DROWSY:
            lcd_putsf("State:Drowsy  ");
            break;

        case RELAXED:
            lcd_putsf("State:Relaxed ");
            break;

        case FOCUSED:
            lcd_putsf("State:Focused ");
            break;

        default:
            lcd_putsf("State:NoSignal");
            break;
    }

    switch(brain_state)
    {
        case DEEP_SLEEP:
            sprintf(lcd_buffer,"Delta:%u%%",delta_percent);
            break;

        case DROWSY:
            sprintf(lcd_buffer,"Theta:%u%%",theta_percent);
            break;

        case RELAXED:
            sprintf(lcd_buffer,"Alpha:%u%%",alpha_percent);
            break;

        case FOCUSED:
            sprintf(lcd_buffer,"Beta :%u%%",beta_percent);
            break;

        default:
            sprintf(lcd_buffer,"No Signal");
            break;
    }

    lcd_gotoxy(0,1);
    lcd_puts(lcd_buffer);
}

void UART_SendChar(char c)
{
    while(!(UCSRA & (1<<UDRE)));
    UDR = c;
}

void UART_SendString(char *str)
{
    while(*str)
    {
        UART_SendChar(*str++);
    }
}

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=In Bit0=Out 
DDRC=(1<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (0<<DDC1) | (1<<DDC0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=T Bit0=0 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);

// USART initialization
// 9600 Baud @ 8MHz
UBRRH = 0;
UBRRL = 51;
UCSRA = 0;
UCSRB = (1<<TXEN);     // Enable Transmitter
UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0); // 8 data bits, 1 stop bit


// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);

// ADC initialization
// ADC Clock frequency: 125.000 kHz
// ADC Voltage Reference: AVCC pin
// ADC Auto Trigger Source: ADC Stopped
ADMUX=ADC_VREF_TYPE;
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);
SFIOR=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

// SPI initialization
// SPI disabled
SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 0
// RD - PORTC Bit 1
// EN - PORTC Bit 2
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 16
lcd_init(16);
lcd_clear();

lcd_gotoxy(0,0);
lcd_putsf(" EEG Simulator");

lcd_gotoxy(0,1);
lcd_putsf(" Initializing...");

delay_ms(1500);

lcd_clear();
while (1)
      {
         ReadEEG();

         ProcessEEG();

         DisplayEEG();
        
         sprintf(uart_buffer,
        "Delta=%u%% Theta=%u%% Alpha=%u%% Beta=%u%% State=%d\r\n",  
            delta_percent,
            theta_percent,
            alpha_percent,
            beta_percent,
            brain_state); 
            
         UART_SendString(uart_buffer);  
       
       delay_ms(500);
      }      
      
      
}
