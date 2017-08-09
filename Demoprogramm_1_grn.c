#include <avr/io.h>
#include <avr/interrupt.h>


volatile uint8_t  programm, x1, up, einzel, xx, flash_state, tick_speed;
uint16_t x, y, verz,z;
uint8_t ms, ms10, ms100, sec;
volatile uint16_t tick, tick_alt;



void flaeche(uint8_t fl);
void reihe(uint8_t reihenr);
void led(uint8_t led, uint8_t flaeche);
void ledaus(void);
uint8_t flash(uint8_t nummer, uint8_t x1);
#ifndef F_CPU
#define F_CPU    20000000
#endif		
			
// #define IRQS_PER_SECOND   1000					// So viele IRQs werden jede Sekunde ausgeloest.
#define IRQS_PER_SECOND    1000						// Fuer optimale Genauigkeit muss
													// IRQS_PER_SECOND ein Teiler von F_CPU sein
													// und IRQS_PER_SECOND ein Vielfaches von 100.
													// Ausserdem muss gelten F_CPU / IRQS_PER_SECOND <= 65536
													// 500 µs //

#define IRQS_PER_10MS     (IRQS_PER_SECOND / 1000)	// Anzahl IRQs pro 10 Millisekunden

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
       ms++;
    }
	if(ms == 5)
	{
		ms=0;
		ms10++;
		 xx++;
       if(xx==tick_speed)
		{
			xx=0;
			if(tick!=51)tick++;
		}
	}
	if(ms10==10)
	{
		ms10=0;
		ms100++;
		 	if(flash_state!=15)flash_state++;
		if(flash_state==14)flash_state=0;
	
	}
	if(ms100==10)
	{
		ms100=0;
		sec++;
		
	}
	
}//End of ISR

int main(void)
{
	
	DDRD = 0xFF;
	PORTD = 0xFF;
	
	DDRA = 0xff;
	PORTA = 0x01;	
	
	
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
	

	ms=0;
	ms10=0;
	ms100=0;
	sec=0;
	tick=0;
	tick_alt=0;
	einzel=1;
	tick_speed=8;
	
	y=0;
	z=0;
	x=0;
	verz=0;
	tick=0;
	up=1;//richtung aufwaertsa oder abwaerts     1=aufwaerts
	flash_state=0;
	while(1)
	{
		if(tick!=tick_alt)
		{
			tick_alt=tick;
			
			if((tick<=4) || ((tick>=16) && (tick<=18) && (up==0)))
			{
				x++;
			}else
			if((tick<=8) || ((tick >=19) && (tick<=20)&& (up==0)))
			{
				x+=5;
			}
			else 
			if((tick<=12) || ((tick>=21) && (tick <=22)&& (up==0)))
			{
				x--;
			}else
			if((tick<=15) || ((tick==23) && (up==0)))
			{
				x-=5;
			}else
			if(tick==16)
			{
				if(up==1)//aufwaerts
				{
					z++;
					if(z<=4)
					{
						tick_alt=0;
						tick=0;
						x=0;
					}else 
					{
						tick=16;
						z=4; 
						up=0;
						x=6;
					}
				}
			}
			if(tick==23) 
			{
				z--;
				if((z>=0) && (z<5))
				{
				x=6;
				tick=16;
				tick_alt=tick;
				}
				else 
				{
					tick=24;
					x=12;
				}
			}else 
			if((tick>23) && (tick<29))
			{
				z++;
			}else 
			if(tick>=29)
			{
				einzel=0;
				switch(tick)
				{
					case 29:led(4,12);
							tick_alt=tick-1;
							break;
					case 30:led(4,12);
							led(3,12);
							tick_alt=tick-1;
							break;		
					case 31:led(4,12);
							led(3,12);
							led(2,12);
							tick_alt=tick-1;
							break;
					case 32:led(4,12);
							led(3,12);
							led(2,12);
							led(1,12);
							tick_alt=tick-1;
							break;
					case 33: reihe(12);
							tick_alt=tick-1;
							break;		
							
					case 34:reihe(12);
							reihe(17);
							reihe(22);
							tick_alt=tick-1;
							break;		
					case 35:reihe(12);
							reihe(17);
							reihe(22);
							reihe(16);
							reihe(21);
							tick_alt=tick-1;
							break;		
					case 36:reihe(12);
							reihe(17);
							reihe(22);
							reihe(16);
							reihe(21);
							reihe(20);
							tick_alt=tick-1;
							break;		
					case 37:reihe(12);
							reihe(17);
							reihe(22);
							reihe(16);
							reihe(21);
							reihe(20);
							reihe(15);
							tick_alt=tick-1;
							break;		
					case 38:reihe(12);
							reihe(17);
							reihe(22);
							reihe(16);
							reihe(21);
							reihe(20);
							reihe(15);
							reihe(10);
							reihe(11);
							tick_alt=tick-1;
							break;		
					case 39:reihe(12);
							reihe(17);
							reihe(22);
							reihe(16);
							reihe(21);
							reihe(20);
							reihe(15);
							reihe(10);
							reihe(11);
							reihe(5);
							reihe(6);
							tick_alt=tick-1;
							break;		
					case 40:reihe(12);
							reihe(17);
							reihe(22);
							reihe(16);
							reihe(21);
							reihe(20);
							reihe(15);
							reihe(10);
							reihe(11);
							reihe(5);
							reihe(6);
							reihe(0);
							tick_alt=tick-1;
							tick_speed=24;
							break;		
					case 41:reihe(12);reihe(17);reihe(22);
							reihe(16);reihe(21);reihe(20);
							reihe(15);reihe(10);reihe(11);
							reihe(5);reihe(6);reihe(0);
							reihe(1);
							tick_alt=tick-1;
							break;		
					case 42:reihe(12);reihe(17);reihe(22);
							reihe(16);reihe(21);reihe(20);
							reihe(15);reihe(10);reihe(11);
							reihe(5);reihe(6);reihe(0);
							reihe(1);reihe(7);reihe(2);
							tick_alt=tick-1;
							break;
					case 43:reihe(12);reihe(17);reihe(22);
							reihe(16);reihe(21);reihe(20);
							reihe(15);reihe(10);reihe(11);
							reihe(5);reihe(6);reihe(0);
							reihe(1);reihe(7);reihe(2);
							reihe(3);reihe(8);
							tick_alt=tick-1;
							break;		
					case 44:reihe(12);reihe(17);reihe(22);
							reihe(16);reihe(21);reihe(20);
							reihe(15);reihe(10);reihe(11);
							reihe(5);reihe(6);reihe(0);
							reihe(1);reihe(7);reihe(2);
							reihe(3);reihe(8);reihe(4);
							tick_alt=tick-1;
							break;	
					case 45:reihe(12);reihe(17);reihe(22);
							reihe(16);reihe(21);reihe(20);
							reihe(15);reihe(10);reihe(11);
							reihe(5);reihe(6);reihe(0);
							reihe(1);reihe(7);reihe(2);
							reihe(3);reihe(8);reihe(4);
							reihe(9);
							tick_alt=tick-1;
							break;		
					case 46:reihe(12);reihe(17);reihe(22);
							reihe(16);reihe(21);reihe(20);
							reihe(15);reihe(10);reihe(11);
							reihe(5);reihe(6);reihe(0);
							reihe(1);reihe(7);reihe(2);
							reihe(3);reihe(8);reihe(4);
							reihe(9);reihe(13);reihe(14);
							tick_alt=tick-1;
							break;		
					case 47:reihe(12);reihe(17);reihe(22);
							reihe(16);reihe(21);reihe(20);
							reihe(15);reihe(10);reihe(11);
							reihe(5);reihe(6);reihe(0);
							reihe(1);reihe(7);reihe(2);
							reihe(3);reihe(8);reihe(4);
							reihe(9);reihe(13);reihe(14);
							reihe(18);reihe(19);
							tick_alt=tick-1;
							break;		
					case 48:reihe(12);reihe(17);reihe(22);
							reihe(16);reihe(21);reihe(20);
							reihe(15);reihe(10);reihe(11);
							reihe(5);reihe(6);reihe(0);
							reihe(1);reihe(7);reihe(2);
							reihe(3);reihe(8);reihe(4);
							reihe(9);reihe(13);reihe(14);
							reihe(18);reihe(19);reihe(24);
							tick_alt=tick-1;
							break;		
					case 49:reihe(12);reihe(17);reihe(22);
							reihe(16);reihe(21);reihe(20);
							reihe(15);reihe(10);reihe(11);
							reihe(5);reihe(6);reihe(0);
							reihe(1);reihe(7);reihe(2);
							reihe(3);reihe(8);reihe(4);
							reihe(9);reihe(13);reihe(14);
							reihe(18);reihe(19);reihe(24);
							reihe(23);
							tick_alt=tick-1;
							break;	
					case 50:flaeche(0);flaeche(1);flaeche(2); flaeche(3);
							flaeche(4);
							tick_alt=tick-1;
							break;	
							
							
					case 51:
							tick_alt=tick-1;
							if(flash(flash_state, 2)==1)
							{
								ledaus();
								tick_alt=tick-1;
								ms=0;
								ms10=0;
								ms100=0;
								sec=0;
								tick=0;
								einzel=1;
								tick_speed=10;
								y=0;
								z=0;
								x=255;
								verz=0;
								tick=0;
								up=1;//richtung aufwaertsa oder abwaerts     1=aufwaerts
								flash_state=0;
								xx=0;
						}	break;
			
							
										
								
				}
				
			}
		}//End of tick_alt
		
			
		if(einzel==1)
		{
			led(z,x);
			for(y=0;y<200;y++);
			ledaus();
			for(y=0;y<200;y++);
		}else ledaus();
	
	
	
	
	}//end of while
	
	return 1;
}//end of main



uint8_t flash(uint8_t nummer, uint8_t x1)
{ 
	/*
	 * Diagonale Flaeche läuft von oberer zur gegenüberliegenden unteren Ecke
	 * 
	 * state gibt flaeche an. im Abstand von ca. 100ms State 1-13 hochzaehlen

	 */
	 uint8_t l,h,ret_wert;
	 
	 if(x1==1)
	 {
		 l=0;
		 h=4;
	}
	
	if(x1==2)
	{
		l=24;
		h=4;
	}
		
			switch(flash_state)
			{
				case 1:	led(h,l);ret_wert=0;break;
				case 2:	switch(x1)
						{
							case 1:	led(h,l+1);led(h,l+5);led(h-1,l);break;
							case 2:	led(h,l-1);led(h,l-5);led(h-1,l);break;
						}break;
									
				case 3:	switch(x1)
						{
							case 1: led(h,l+2); led(h,l+6);led(h,l+10);led(h-1,l+1);led(h-1,l+5);led(h-2,l);break;
							case 2: led(h,l-2); led(h,l-6);led(h,l-10);led(h-1,l-1);led(h-1,l-5);led(h-2,l);break;
						}break;
						
				case 4:	switch(x1)
						{
							case 1:	led(h,l+3); led(h,l+7);led(h,l+11);led(h,l+15);led(h-1,l+2);led(h-1,l+6);led(h-1,l+10);led(h-2,l+1);
									led(h-2,l+5);led(h-3,l);break;
							case 2:	led(h,l-3); led(h,l-7);led(h,l-11);led(h,l-15);led(h-1,l-2);led(h-1,l-6);led(h-1,l-10);led(h-2,l-1);
									led(h-2,l-5);led(h-3,l);break;
						}break;
						
				case 5:	switch(x1)
						{
							case 1:	led(h,l+4); led(h,l+8);led(h,l+12);led(h,l+16);led(h,l+20);led(h-1,l+3);led(h-1,l+7);led(h-1,l+11);led(h-1,l+15);led(h-2,l+2);
									led(h-2,l+5);led(h-2,l+10);led(h-3,l+1);led(h-3,l+10);led(h-4,l);break;
							case 2:	led(h,l-4); led(h,l-8);led(h,l-12);led(h,l-16);led(h,l-20);led(h-1,l-3);led(h-1,l-7);led(h-1,l-11);led(h-1,l-15);led(h-2,l-2);
									led(h-2,l-5);led(h-2,l-10);led(h-3,l-1);led(h-3,l-10);led(h-4,l);break;
						}break;
						
				case 6:	switch(x1)
						{
							case 1:	led(h,l+9); led(h,l+13);led(h,l+17);led(h,l+21);led(h-1,l+4); led(h-1,l+8);led(h-1,l+12);led(h-1,l+16);led(h-1,l+20);
									led(h-2,l+3); led(h-2,l+7);led(h-2,l+11);led(h-2,l+15);led(h-3,l+2); led(h-3,l+6);led(h-3,l+10);led(h-4,l+1);led(h-4,l+5);break;
							case 2:	led(h,l-9); led(h,l-13);led(h,l-17);led(h,l-21);led(h-1,l-4); led(h-1,l-8);led(h-1,l-12);led(h-1,l-16);led(h-1,l-20);
									led(h-2,l-3); led(h-2,l-7);led(h-2,l-11);led(h-2,l-15);led(h-3,l-2); led(h-3,l-6);led(h-3,l-10);led(h-4,l-1);led(h-4,l-5);break;
						}break;
				case 7:	switch(x1)
						{
							case 1:	led(h,l+14); led(h,l+18);led(h,l+22);led(h-1,l+9);led(h-1,l+13); led(h-1,l+17);led(h-1,l+21);led(h-1,l+4);led(h-2,l+8);
									led(h-2,l+12); led(h-2,l+16);led(h-2,l+20);led(h-3,l+3);led(h-3,l+7); led(h-3,l+11);led(h-3,l+15);led(h-4,l+2);led(h-4,l+6);led(h-4,l+10);break;
							case 2:	led(h,l-14); led(h,l-18);led(h,l-22);led(h-1,l-9);led(h-1,l-13); led(h-1,l-17);led(h-1,l-21);led(h-1,l-4);led(h-2,l-8);
									led(h-2,l-12); led(h-2,l-16);led(h-2,l-20);led(h-3,l-3);led(h-3,l-7); led(h-3,l-11);led(h-3,l-15);led(h-4,l-2);led(h-4,l-6);led(h-4,l-10);break;
						}break;
						
				case 8:	switch(x1)
						{
							case 1:	led(h,l+19); led(h,l+23);led(h-1,l+14); led(h-1,l+18);led(h-1,l+22);led(h-2,l+9);led(h-2,l+13); led(h-2,l+17);led(h-2,l+21);
									led(h-3,l+4); led(h-3,l+8);led(h-3,l+12);led(h-3,l+16);led(h-3,l+20);led(h-4,l+3); led(h-4,l+7);led(h-4,l+11);led(h-4,l+15);break;
							case 2:	led(h,l-19); led(h,l-23);led(h-1,l-14); led(h-1,l-18);led(h-1,l-22);led(h-2,l-9);led(h-2,l-13); led(h-2,l-17);led(h-2,l-21);
									led(h-3,l-4); led(h-3,l-8);led(h-3,l-12);led(h-3,l-16);led(h-3,l-20);led(h-4,l-3); led(h-4,l-7);led(h-4,l-11);led(h-4,l-15);break;
						}break;
						
				case 9:	switch(x1)
						{
							case 1:	led(h,l+24);led(h-1,l+19); led(h-1,l+23);led(h-2,l+14); led(h-2,l+18);led(h-2,l+22);led(h-3,l+9);led(h-3,l+13); led(h-3,l+17);
									led(h-3,l+21);led(h-4,l+4); led(h-4,l+8);led(h-4,l+12);led(h-4,l+16);led(h-4,l+20);break;
							case 2:	led(h,l-24);led(h-1,l-19); led(h-1,l-23);led(h-2,l-14); led(h-2,l-18);led(h-2,l-22);led(h-3,l-9);led(h-3,l-13); led(h-3,l-17);
									led(h-3,l-21);led(h-4,l-4); led(h-4,l-8);led(h-4,l-12);led(h-4,l-16);led(h-4,l-20);break;
						}break;
						
				case 10:switch(x1)
						{
							case 1: led(h-1,l+24);led(h-2,l+19); led(h-2,l+23);led(h-3,l+14); led(h-3,l+18);led(h-3,l+22);led(h-4,l+9);led(h-4,l+13); led(h-4,l+17);
									led(h-4,l+21);break;
							case 2: led(h-1,l-24);led(h-2,l-19); led(h-2,l-23);led(h-3,l-14); led(h-3,l-18);led(h-3,l-22);led(h-4,l-9);led(h-4,l-13); led(h-4,l-17);
									led(h-4,l-21);break;
						}break;
						
				case 11:switch(x1)
						{
							case 1:	led(h-2,l+24);led(h-3,l+19); led(h-3,l+23);led(h-4,l+14); led(h-4,l+18);led(h-4,l+22);break;
							case 2:	led(h-2,l-24);led(h-3,l-19); led(h-3,l-23);led(h-4,l-14); led(h-4,l-18);led(h-4,l-22);break;
						}break;
						
				case 12:switch(x1)
						{
							case 1:	led(h-3,l+24);led(h-4,l+19); led(h-4,l+23);break;
							case 2:	led(h-3,l-24);led(h-4,l-19); led(h-4,l-23);break;
						}break;
				case 13:switch(x1)
						{
							case 1:	led(h-4,l+24);ret_wert=1;break;
							case 2:	led(h-4,l-24);ret_wert=1;break;
						}break;
				
			}//end of switch
	 ledaus();
	 return ret_wert;
}


void flaeche(uint8_t fl)
{
	
	DDRA=0x00;
	DDRB =0x00;
	DDRC=0x00;
	DDRD=0x00;
	switch(fl)
	{
		case 0:	DDRA=0b00011111;
				DDRD=0b11111000;
				PORTA=0b00000000;
				PORTD=0b11111000;
				break;
		case 1:	DDRA=0b00011111;
				DDRD=0b11111000;
				PORTA=0b00011111;
				PORTD=0b00000000;
				break;		
		case 2:	DDRA=0b00011111;
				DDRC=0b11111000;
				PORTA=0b00000000;
				PORTC=0b11111000;
				break;
		case 3:	DDRA=0b00011111;
				DDRC=0b11111000;
				PORTA=0b00011111;
				PORTC=0b00000000;
				break;
		case 4:	DDRA=0b00011111;
				DDRB=0b00011111;
				PORTA=0b00011111;
				PORTB=0b00000000;
				break;
	}//End of switch
}
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
void reihe(uint8_t reihenr)
{
	uint8_t xxx;
	uint16_t yyy;

	for(xxx=0; xxx<5;xxx++)
	{
		led(xxx,reihenr);
		for(yyy=0; yyy<100;yyy++);
	}	
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
						DDRD |= (1<<PD7);
						PORTD &= ~(1<<PD7);
					}else if(nummer<10)
					{
						DDRD |= (1<<PD6);
						PORTD &= ~(1<<PD6);
					}else if(nummer<15)
					{
						DDRD |= (1<<PD5);
						PORTD &= ~(1<<PD5);
					}else if(nummer<20)
					{
						DDRD |= (1<<PD4);
						PORTD &= ~(1<<PD4);
					}else
					{
						DDRD |= (1<<PD3);
						PORTD &= ~(1<<PD3);
					}break;
		case 1:		if(nummer<5)
					{
						DDRD |= (1<<PD7);
						PORTD |= (1<<PD7);
					}else if(nummer<10)
					{
						DDRD |= (1<<PD6);
						PORTD |= (1<<PD6);
					}else if(nummer<15)
					{
						DDRD |= (1<<PD5);
						PORTD |= (1<<PD5);
					}else if(nummer<20)
					{
						DDRD |= (1<<PD4);
						PORTD |= (1<<PD4);
					}else
					{
						DDRD |= (1<<PD3);
						PORTD |= (1<<PD3);
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
