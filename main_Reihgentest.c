#include <avr/io.h>
#include <avr/interrupt.h>


uint8_t  programm, x1;
uint16_t x;
#ifndef F_CPU
#define F_CPU    20000000
#endif		
			
// #define IRQS_PER_SECOND   1000					// So viele IRQs werden jede Sekunde ausgeloest.
#define IRQS_PER_SECOND    1000						// Fuer optimale Genauigkeit muss
													// IRQS_PER_SECOND ein Teiler von F_CPU sein
													// und IRQS_PER_SECOND ein Vielfaches von 100.
													// Ausserdem muss gelten F_CPU / IRQS_PER_SECOND <= 65536
													// 500 µs //

#define IRQS_PER_10MS     (IRQS_PER_SECOND / 100)	// Anzahl IRQs pro 10 Millisekunden

// Gültigkeitsprüfung.
// Bei ungeeigneten Werten gibt es einen Compilerfehler
#if (F_CPU/IRQS_PER_SECOND > 65536) || (IRQS_PER_10MS < 1) || (IRQS_PER_10MS > 255)
#   error Diese Werte fuer F_CPU und IRQS_PER_SECOND
#   error sind ausserhalb des gueltigen Bereichs!
#endif


ISR(TIMER1_COMPA_vect)
{
  static uint8_t interrupt_num_10ms;				// zaehlvariable deklarieren
      
    if (++interrupt_num_10ms == IRQS_PER_10MS)    	// interrupt_num_10ms erhöhen und mit Maximalwert vergleichen
    {
        interrupt_num_10ms = 0;						// interrupt_num_10ms zurücksetzen
      
       
	}
	
}//End of ISR

int main(void)
{
	
	DDRD = 0xFF;
	PORTD = 0xFF;
	
	DDRA = 0xff;
	PORTA = 0x01;	
	
	uint16_t x=0;
	
	
	
	
	
	
	
	
	TCCR1A = 0;					// Timer1: keine PWM

    // Timer1 ist Zähler: Clear Timer on Compare Match (CTC, Mode #4)
    // Timer1 läuft mit vollem MCU-Takt: Prescale = 1
#if defined (CTC1) && !defined (WGM12)
   TCCR1B = (1 << CTC1)  | (1 << CS10);
#elif !defined (CTC1) && defined (WGM12)
   TCCR1B = (1 << WGM12) | (1 << CS10);
#else
#error Keine Ahnung!
#endif

    // OutputCompare für gewünschte Timer1 Frequenz
    // TCNT1 zählt immer 0...OCR1A, 0...OCR1A, ... 
    // Beim überlauf OCR1A -> OCR1A+1 wird TCNT1=0 gesetzt und im nächsten
    // MCU-Takt eine IRQ erzeugt.
    OCR1A = (unsigned short) ((unsigned long) F_CPU / IRQS_PER_SECOND-1);

    // OutputCompareA-Interrupt für Timer1 aktivieren
#if defined (TIMSK1)
    TIMSK1 |= (1 << OCIE1A);
#elif defined (TIMSK)
    TIMSK  |= (1 << OCIE1A);
#else
#error Keine Ahnung!
#endif
	sei();
	
	PORTA = 0x00;
	PORTD =0x00;
	while(1)
	{
	
	
	PORTA = 0b00000001;
	PORTD = 0b00000000;
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	PORTA = 0b00000011;
	PORTD = 0b00000000;
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	PORTA = 0b00000111;
	PORTD = 0b00000000;
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	PORTA = 0b00001111;
	PORTD = 0b00000000;
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	PORTA = 0b00011111;
	PORTD = 0b00000000;
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	PORTA = 0b00000000;
	PORTD = 0b00000000;
	
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);

	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	for(x=0;x<20000;x++);
	
	
	}//end of while
	return 1;
}//end of main
