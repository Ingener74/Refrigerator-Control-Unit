#define F_CPU  9600000UL

#include <avr/io.h>
#include <avr/interrupt.h>

enum {IN, OUT};
enum {BUT1, BUT2, LOAD, DATA, CLK};
enum {FALSE, TRUE};

typedef unsigned int  word;
typedef unsigned char byte;

word tim0 = 0;
byte load_state = FALSE;
word time = 0;
byte load_time_array[2] = {5, 0};
byte load_time = 0;

//const time_max = 60;
word time_max = 60;

 //  e d   c dp   b a   f g
 /*
		--- a --- 
       |         |
       f         b
       |         |
        --- g --
       |         |
       e         c
       |         |
        --- d --
                 dp
       
 */
byte numbers[] = {0x11, // 0  00 01 00 01
                  0xD7, // 1  11 01 01 11
                  0x32, // 2  00 11 00 10
                  0x92, // 3  10 01 00 10
                  0xD4, // 4  11 01 01 00
                  0x98, // 5  10 01 10 00
                  0x18, // 6  00 01 10 00
                  0xD3, // 7  11 01 00 11
                  0x10, // 8  00 01 00 00
                  0x90, // 9  10 01 00 00
                  0
                 };
		 
byte disp_number[2];
void display(void)
{
    disp_number[0] = numbers[load_time_array[0]];
    disp_number[1] = numbers[load_time_array[1]];
    byte n, e;
    for(n = 0; n < 2; n++)
    {
	for(e = 0; e < 8; e++)
	{
	    if( (disp_number[n] & 1) == 0)
	    {
		PORTB &= ~(1 << DATA);
	    }
	    else
	    {
		PORTB |=  (1 << DATA);
	    }
	    disp_number[n] >>= 1;
	
	    PORTB &= ~(1 << CLK);
	    PORTB |=  (1 << CLK);
	}
    }    
}

ISR(TIM0_OVF_vect)
{
    // Down
    if(PINB & (1 << BUT1))
    {
    }
    else
    {
	if(load_time_array[1] != 0)
	{
	    load_time_array[1]--;
	}
	else
	{
	    if(load_time_array[0] != 0)
	    {
		load_time_array[0]--;
		load_time_array[1] = 9;
	    }
	}
	display();
	
    }
    
    // Up
    if(PINB & (1 << BUT2))
    {
    }
    else
    {
	if(load_time_array[1] != 9)
	{
	    load_time_array[1]++;
	}
	else
	{
	    if(load_time_array[0] != 9)
	    {
		load_time_array[0]++;
		load_time_array[1] = 0;
	    }
	}
	display();
    }
    
    if(tim0 >= 9375 / 256)
    {
	time++;
	tim0 = 0;
    }
    tim0++;
    
    load_time = load_time_array[0] * 10 + load_time_array[1];
    
    if(time >= ((load_time * time_max) / 100) )
    {	
	PORTB |= (1 << LOAD);
    }
    
    if(time >= (time_max - 1) )
    {
	time = 0;
	PORTB &= ~(1 << LOAD);
    }
}

int main(void)
{
    DDRB  = (IN << BUT1) | (IN << BUT2) | (OUT << LOAD) | (OUT << DATA) | (OUT << CLK);
    PORTB = (1  << BUT1) | (1  << BUT2) | (1   << LOAD) | (0   << DATA) | (1   << CLK);
    
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 = (1 << TOIE0);
    
    sei();
    
    display();
    
    while(1)
    {
    }
}