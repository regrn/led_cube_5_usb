#include <avr/io.h>
#include <avr/interrupt.h>


uint8_t  programm, x1, ledflaeche, led_zaehler;
uint16_t x, y, verz,z;





void led(uint8_t led, uint8_t flaeche);
void ledaus(void);	//alle led ausschalten
void ledein(void);	//Alle led einschalten
uint16_t mic(void);	//ADC Wert von Mikrophon abholen (10bit)
void flaeche(uint8_t flaeche);


uint8_t cube_leds[125];	//Zusataende aller leds im cube
void muster(uint8_t ledflaeche, uint8_t reihe, uint8_t led1, uint8_t led2, uint8_t led3, uint8_t led4, uint8_t led5);

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
	

	
	
	while(1)
	{
			muster(0,0,    0,0,1,0,0  );
			muster(0,1,    0,0,1,0,0  );
			muster(0,2,    1,1,1,1,1  );
			muster(0,3,    0,0,1,0,0  );
			muster(0,4,    0,0,1,0,0  );
			
			
			muster(1,0,    0,0,0,0,0  );
			muster(1,1,    0,0,0,0,0  );
			muster(1,2,    0,0,0,0,0  );
			muster(1,3,    0,0,0,0,0  );
			muster(1,4,    0,0,0,0,0  );
		
			muster(2,0,    0,0,0,0,0  );
			muster(2,1,    0,0,0,0,0  );
			muster(2,2,    0,0,0,0,0  );
			muster(2,3,    0,0,0,0,0  );
			muster(2,4,    0,0,0,0,0  );
			
			muster(3,0,    0,0,0,0,0  );
			muster(3,1,    0,0,0,0,0  );
			muster(3,2,    0,0,0,0,0  );
			muster(3,3,    0,0,0,0,0  );
			muster(3,4,    0,0,0,0,0  );
			
			muster(4,0,    1,0,0,0,0  );
			muster(4,1,    0,0,0,0,0  );
			muster(4,2,    0,0,0,0,0  );
			muster(4,3,    0,0,0,0,0  );
			muster(4,4,    0,0,0,0,0  );
			
			
			//Array Ausgabe ganzer Cube
	
	for(led_zaehler=0; led_zaehler<125; led_zaehler++)
	{
		if(led_zaehler<25)          //Flaeche 1
		{
			ledflaeche=0;
		}else
		if(led_zaehler<50)          //Flaeche 2
		{
			ledflaeche=1;
		}
		else
		if(led_zaehler<75)          //Flaeche3
		{
			ledflaeche=2;
		}else
		if(led_zaehler<100)         //Flaeche 4
		{
			ledflaeche=3;
		}else
		if(led_zaehler<125)         //Flaeche 5
		{
			ledflaeche=4;
		}
		
		
		if(cube_leds[led_zaehler])
		{
		//	if(led_zaehler==0)ledein();
			led(ledflaeche,led_zaehler); //Falls Arrayplatz = 1,  led einschalten
		}
	}//end of for
	ledaus();
	}//end of while
	
	return 1;
}//end of main

void ledaus(void)
{
		DDRA &= ~((1<<PA0) | (1<<PA1) |(1<<PA2) |(1<<PA3));//pin 0-4 Ausgang
		PORTA&= ~((1<<PA0) | (1<<PA1) |(1<<PA2) |(1<<PA3));
		
		DDRB &= ~((1<<PB0) | (1<<PB1) |(1<<PB2) |(1<<PB3));//pin 0-4 Ausgang
		PORTB&= ~((1<<PB0) | (1<<PB1) |(1<<PB2) |(1<<PB3));
		
		DDRC &= ~((1<<PC4) | (1<<PC5) |(1<<PC6) |(1<<PC7));//pin 0-4 Ausgang
		PORTC&= ~((1<<PC4) | (1<<PC5) |(1<<PC6) |(1<<PC7));
		
		DDRD &= ~((1<<PD4) | (1<<PD5) |(1<<PD6) |(1<<PD7));//pin 0-4 Ausgang
		PORTD&= ~((1<<PD4) | (1<<PD5) |(1<<PD6) |(1<<PD7));
}
void ledein(void)
{
	flaeche(0); flaeche(1); flaeche(2); flaeche(3); flaeche(4);
}
void led(uint8_t flaeche, uint8_t nummer)
{
		//Flaeche 0-4
		//nummer 0-14
		//
	
		
		DDRA &= ~((1<<PA0) | (1<<PA1) |(1<<PA2) |(1<<PA3) |(1<<PA4));//pin 0-4 Ausgang
		PORTA&= ~((1<<PA0) | (1<<PA1) |(1<<PA2) |(1<<PA3) |(1<<PA4));
		
		DDRB &= ~((1<<PB0) | (1<<PB1) |(1<<PB2) |(1<<PB3) |(1<<PB4));//pin 0-4 Ausgang
		PORTB&= ~((1<<PB0) | (1<<PB1) |(1<<PB2) |(1<<PB3) |(1<<PB4));
		
		DDRC &= ~((1<<PC4) | (1<<PC5) |(1<<PC6) |(1<<PC7) |(1<<PC3));//pin 0-4 Ausgang
		PORTC&= ~((1<<PC4) | (1<<PC5) |(1<<PC6) |(1<<PC7) |(1<<PC3));
		
		DDRD &= ~((1<<PD4) | (1<<PD5) |(1<<PD6) |(1<<PD7) |(1<<PD3));//pin 0-4 Ausgang
		PORTD&= ~((1<<PD4) | (1<<PD5) |(1<<PD6) |(1<<PD7) |(1<<PD3));
	
	
	/*Mit den Pins an Port A werden die gemeinsamen Kathoden oder Anoden gesteuert.
	 * Dies ergibt zum Einen die Flaeche zum Anderen die Reihe. Ungerade und gerade Flaechen 
	 * haben jeweils den selben gemeinsamen Anschluss.
	 * Um eine led leuchten zu lassen, wird nur die zuständige Reihe als Ausgang konfiguriert, 
	 * alle anderen Reihen verbleiben als Eingang und somit im Tristate Zustand.
	 * */	
	uint8_t nummerA, wert;
	uint8_t f024A[]={0x01, 0x02, 04, 0x08, 0x10};	//DDRA alle Flaechen und PORTA Flaechen 0,2,4
	uint8_t f13A[]={0xFE, 0xFD, 0xFB, 0xF7, 0xEF};	//PORTA Flaechen 1,3
	
	wert=0x00;	
	nummerA=nummer; 	
	while(nummerA > 4)		//zahelt immer -5 bis innerhalb array f135
	{
		nummerA = nummerA-5;
	}
	
	if((flaeche == 0) || (flaeche == 2) || (flaeche==4))	//gemeinsame Kathode high
	{
		DDRA=f024A[nummerA];
		PORTA=f024A[nummerA];
	}else 													//gemeinsame Kathode low
	{
		DDRA=f024A[nummerA];
		PORTA=f13A[nummerA];
	}
	
	switch(flaeche)
	{
		case 0:		//tristate					
					if(nummer<5)
					{
						DDRD |= (1<<PD3);
						PORTD &= ~(1<<PD3);
					}else if(nummer<10)
					{
						DDRD |= (1<<PD4);
						PORTD &= ~(1<<PD4);
					}else if(nummer<15)
					{
						DDRD |= (1<<PD5);
						PORTD &= ~(1<<PD5);
					}else if(nummer<20)
					{
						DDRD |= (1<<PD6);
						PORTD &= ~(1<<PD6);
					}else
					{
						DDRD |= (1<<PD7);
						PORTD &= ~(1<<PD7);
					}break;
		case 1:			if(nummer<5)
					{
						DDRD |= (1<<PD3);
						PORTD |= (1<<PD3);
					}else if(nummer<10)
					{
						DDRD |= (1<<PD4);
						PORTD |= (1<<PD4);
					}else if(nummer<15)
					{
						DDRD |= (1<<PD5);
						PORTD |= (1<<PD5);
					}else if(nummer<20)
					{
						DDRD |= (1<<PD6);
						PORTD |= (1<<PD6);
					}else
					{
						DDRD |= (1<<PD7);
						PORTD |= (1<<PD7);
					}break;
					
		case 2:		if(nummer<5)
					{
						DDRC=0x20;
						PORTC=0xDF;
					}else if(nummer<10)
					{
						DDRC=0x10;
						PORTC=0xEF;
					}else if(nummer<15)
					{
						DDRC=0x08;
						PORTC=0xF7;
					}else if(nummer<20)
					{
						DDRC=0x40;
						PORTC=0xBF;
					}else
					{
						DDRC=0x80;
						PORTC=0x7F;
					}
					break;
					
		case 3:		if(nummer<5)
					{
						wert=0x20;
					}else if(nummer<10)
					{
						wert=0x10;
					}else if(nummer<15)
					{
						wert=0x08;
					}else if(nummer<20)
					{
						wert=0x40;
					}else
					{
						wert=0x80;
					}
					DDRC=wert;
					PORTC=wert;
					break;
					
		case 4:		if(nummer<5)
					{
						DDRB=0x01;
						PORTB=0xFE;
					}else if(nummer<10)
					{
						DDRB=0x02;
						PORTB=0xFD;
					}else if(nummer<15)
					{
						DDRB=0x04;
						PORTB=0xFB;
					}else if(nummer<20)
					{
						DDRB=0x08;
						PORTB=0xF7;
					}else
					{
						DDRB=0x10;
						PORTB=0x0F;
					}
					break;
	}//end of switch
}//end of led
void flaeche(uint8_t flaeche)
{
	
	switch(flaeche)
	{
			case 0:	DDRA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));		
					PORTA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));
		
					DDRD |= ((1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7));		
					PORTD &= ~((1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7));
					break;
			case 1:	DDRA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));		
					PORTA &= ~((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));
		
					DDRD |= ((1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7));		
					PORTD |= ((1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7));
					break;
			case 2:	DDRA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));		
					PORTA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));
		
					DDRC |= ((1<<PC3) | (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7));		
					PORTC &= ~((1<<PC3) | (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7));
					break;
			case 3:	DDRA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));		
					PORTA &= ~((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));
		
					DDRC |= ((1<<PC3) | (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7));		
					PORTC |= ((1<<PC3) | (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7));
					break;
					
			case 4:	DDRA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));		
					PORTA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4));
		
					DDRB |= ((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4));		
					PORTB &= ~((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4));
					break;
		
	}
	
	}

void muster(uint8_t ledflaeche, uint8_t reihe, uint8_t led1, uint8_t led2, uint8_t led3, uint8_t led4, uint8_t led5)
{
	switch(ledflaeche)
	{
		case 0:	switch(reihe)
				{
					case 0:	cube_leds[0]=led1;
							cube_leds[1]=led2;
							cube_leds[2]=led3;
							cube_leds[3]=led4;
							cube_leds[4]=led5;
							break;
					case 1:	cube_leds[5]=led1;
							cube_leds[6]=led2;
							cube_leds[7]=led3;
							cube_leds[8]=led4;
							cube_leds[9]=led5;
							break;
					case 2:	cube_leds[10]=led1;
							cube_leds[11]=led2;
							cube_leds[12]=led3;
							cube_leds[13]=led4;
							cube_leds[14]=led5;
							break;
					case 3:	cube_leds[15]=led1;
							cube_leds[16]=led2;
							cube_leds[17]=led3;
							cube_leds[18]=led4;
							cube_leds[19]=led5;
							break;
					case 4:	cube_leds[20]=led1;
							cube_leds[21]=led2;
							cube_leds[22]=led3;
							cube_leds[23]=led4;
							cube_leds[24]=led5;
							break;
				}	
				break;
		case 1:	switch(reihe)
				{
					case 0:	cube_leds[25]=led1;
							cube_leds[26]=led2;
							cube_leds[27]=led3;
							cube_leds[28]=led4;
							cube_leds[29]=led5;
							break;
					case 1:	cube_leds[30]=led1;
							cube_leds[31]=led2;
							cube_leds[32]=led3;
							cube_leds[33]=led4;
							cube_leds[34]=led5;
							break;
					case 2:	cube_leds[35]=led1;
							cube_leds[36]=led2;
							cube_leds[37]=led3;
							cube_leds[38]=led4;
							cube_leds[39]=led5;
							break;
					case 3:	cube_leds[40]=led1;
							cube_leds[41]=led2;
							cube_leds[42]=led3;
							cube_leds[43]=led4;
							cube_leds[44]=led5;
							break;
					case 4:	cube_leds[45]=led1;
							cube_leds[46]=led2;
							cube_leds[47]=led3;
							cube_leds[48]=led4;
							cube_leds[49]=led5;
							break;
				}//end of switch		
				break;
		case 2:	switch(reihe)
				{
					case 0:	cube_leds[50]=led1;
							cube_leds[51]=led2;
							cube_leds[52]=led3;
							cube_leds[53]=led4;
							cube_leds[54]=led5;
							break;
					case 1:	cube_leds[55]=led1;
							cube_leds[56]=led2;
							cube_leds[57]=led3;
							cube_leds[58]=led4;
							cube_leds[59]=led5;
							break;
					case 2:	cube_leds[60]=led1;
							cube_leds[61]=led2;
							cube_leds[62]=led3;
							cube_leds[63]=led4;
							cube_leds[64]=led5;
							break;
					case 3:	cube_leds[65]=led1;
							cube_leds[66]=led2;
							cube_leds[67]=led3;
							cube_leds[68]=led4;
							cube_leds[69]=led5;
							break;
					case 4:	cube_leds[70]=led1;
							cube_leds[71]=led2;
							cube_leds[72]=led3;
							cube_leds[73]=led4;
							cube_leds[74]=led5;
							break;
				}//end of switch			
				break;
		case 3:	switch(reihe)
				{
					case 0:	cube_leds[75]=led1;
							cube_leds[76]=led2;
							cube_leds[77]=led3;
							cube_leds[78]=led4;
							cube_leds[79]=led5;
							break;
					case 1:	cube_leds[80]=led1;
							cube_leds[81]=led2;
							cube_leds[82]=led3;
							cube_leds[83]=led4;
							cube_leds[84]=led5;
							break;
					case 2:	cube_leds[85]=led1;
							cube_leds[86]=led2;
							cube_leds[87]=led3;
							cube_leds[88]=led4;
							cube_leds[89]=led5;
							break;
					case 3:	cube_leds[90]=led1;
							cube_leds[91]=led2;
							cube_leds[92]=led3;
							cube_leds[93]=led4;
							cube_leds[94]=led5;
							break;
					case 4:	cube_leds[95]=led1;
							cube_leds[96]=led2;
							cube_leds[97]=led3;
							cube_leds[98]=led4;
							cube_leds[99]=led5;
							break;
				}//end of switch			
				break;
		case 4:	switch(reihe)
				{
					case 0:	cube_leds[100]=led1;
							cube_leds[101]=led2;
							cube_leds[102]=led3;
							cube_leds[103]=led4;
							cube_leds[104]=led5;
							break;
					case 1:	cube_leds[105]=led1;
							cube_leds[106]=led2;
							cube_leds[107]=led3;
							cube_leds[108]=led4;
							cube_leds[109]=led5;
							break;
					case 2:	cube_leds[110]=led1;
							cube_leds[111]=led2;
							cube_leds[112]=led3;
							cube_leds[113]=led4;
							cube_leds[114]=led5;
							break;
					case 3:	cube_leds[115]=led1;
							cube_leds[116]=led2;
							cube_leds[117]=led3;
							cube_leds[118]=led4;
							cube_leds[119]=led5;
							break;
					case 4:	cube_leds[120]=led1;
							cube_leds[121]=led2;
							cube_leds[122]=led3;
							cube_leds[123]=led4;
							cube_leds[124]=led5;
							break;
				}//end of switch			
				break;
	}
}//end of muster	
	
uint16_t mic(void)
{
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0) |(1<<ADPS1);    // Frequenzvorteiler 
                               
	ADMUX |= ((1<<MUX0) | (1<<MUX1) | (1<<MUX2));  // ADC7 Mikrophon
	ADMUX |= (1<<REFS1); 					// externe Referenzspannung nutzen 
	ADCSRA |= (1<<ADSC);            		// eine Wandlung "single conversion"
	while(ADCSRA & (1<<ADSC));   			// auf Abschluss der Konvertierung warten (ADIF-bit)
	//result = ADCW;		    				// Wandlungsergebnisse aufaddieren
	
	return ADCW;
}
