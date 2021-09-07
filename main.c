#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define dir 				6 //PORTD
#define step 				5 //PORTD
#define clockwize 			0
#define counterclockwize 	1

#define SPSmin           	250	//min steps per second
#define SPSmax              10000	//max steps per second
#define SPSS				10	//steps per second per second(acceleration)
#define SPSchangingsteps    (SPSmax-SPSmin)/SPSS	//how many times the speed will change

volatile uint16_t sps = 0, accSTEPS = 0, regSTEPS = 0, decSTEPS = 0;

void moveSteps(uint8_t direction, uint16_t steps)
{
	if (direction)
	{
		PORTD |= 1 << dir;
	}
	else
	{
		PORTD &= ~(1 << dir);
	}

    if (steps <= SPSchangingsteps * 2)
	{
		accSTEPS = steps / 2;
		decSTEPS = steps / 2;
		regSTEPS = steps % 2;
	}
	else
	{
		accSTEPS = SPSchangingsteps;
		decSTEPS = SPSchangingsteps;
		regSTEPS = steps - (SPSchangingsteps * 2);
	}

    sps = SPSmin;
	sei();
	while (accSTEPS || regSTEPS || decSTEPS);
	cli();
}

ISR(TIMER1_COMPA_vect)
{
    OCR1A = F_CPU / sps;
    if(accSTEPS) 
    {
        PORTD |= 1 << step;
        sps += SPSS;
        accSTEPS--;
    }
    else if (regSTEPS)
    {
        PORTD |= 1 << step;
        regSTEPS--;
    }
    else if(decSTEPS)
    {
        PORTD |= 1 << step;
        sps -= SPSS;
        decSTEPS--;
    }
	PORTD &= ~(1 << step);
}

int main(void)
{
	DDRD = 1 << step | 1 << dir;
	PORTD = 1 << dir;

	DDRC = 0;
	PORTC = 1<<4|1<<5;

	TCCR1A = 0;
	TCCR1B = 1 << WGM12 | 1 << CS10;
	TIMSK1 = 1 << OCIE1A;

    _delay_ms(1000);
	while (1)
	{
		if(!(PINC&(1<<4)))
		{
			moveSteps(clockwize, 3200*10);
			_delay_ms(1000);
		}
		if(!(PINC&(1<<5)))
		{
			moveSteps(counterclockwize, 3200*10);
			_delay_ms(1000);
		}
	}
}
