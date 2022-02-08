#ifndef EPSKLIB_H
#define EPSKLIB_H

#define _BV(bit) (1 << (bit))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)


//zero_state = bitRead(PIND, PD2);		// digitalRead(PD2)

#define PINA_UP(pin) PORTA |= _BV(pin);
#define PINB_UP(pin) PORTB |= _BV(pin);
#define PINC_UP(pin) PORTC |= _BV(pin);
#define PIND_UP(pin) PORTD |= _BV(pin);
#define PINE_UP(pin) PORTE |= _BV(pin);
#define PINF_UP(pin) PORTF |= _BV(pin);
#define PING_UP(pin) PORTG |= _BV(pin);
#define PINH_UP(pin) PORTH |= _BV(pin);
#define PINJ_UP(pin) PORTJ |= _BV(pin);
#define PINK_UP(pin) PORTK |= _BV(pin);
#define PINL_UP(pin) PORTL |= _BV(pin);

#define PINA_DOWN(pin) PORTA &= ~ _BV(pin);
#define PINB_DOWN(pin) PORTB &= ~ _BV(pin);
#define PINC_DOWN(pin) PORTC &= ~ _BV(pin);
#define PIND_DOWN(pin) PORTD &= ~ _BV(pin);
#define PINE_DOWN(pin) PORTE &= ~ _BV(pin);
#define PINF_DOWN(pin) PORTF &= ~ _BV(pin);
#define PING_DOWN(pin) PORTG &= ~ _BV(pin);
#define PINH_DOWN(pin) PORTH &= ~ _BV(pin);
#define PINJ_DOWN(pin) PORTJ &= ~ _BV(pin);
#define PINK_DOWN(pin) PORTK &= ~ _BV(pin);
#define PINL_DOWN(pin) PORTL &= ~ _BV(pin);

//A, B, C, D, E, F, G, H, J, K è L:

//PORT{letter} |= _BV(P{letter}{number}); // HIGH
//PORT{letter} &= ~ _BV(P{letter}{number}); // LOW
	
#endif