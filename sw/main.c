#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUD 2400

#include "usart.h"

#define AVG_SAMPLES 10	// Running average samle count

volatile uint16_t delay, real;
volatile uint32_t accumulator = 0;
volatile uint16_t samples[AVG_SAMPLES];
volatile uint8_t index = 0;
volatile uint8_t status = 0;

volatile uint32_t timestamp = 0;

ISR(TIMER1_CAPT_vect) {
	TCNT1 = 0;

	// The scale MCU makes two pulses, one of which has fixed length, shorter than 8000 cycles
	if (ICR1 > 8000) {
		real = ICR1;
		if (status < AVG_SAMPLES) {
			// Filling the running average buffer
			samples[status] = ICR1;
			accumulator += samples[status];
			status++;
		} else {
			// Calculate the running average
			accumulator -= samples[index];
			samples[index] = ICR1;
			accumulator += samples[index];
			delay = accumulator / AVG_SAMPLES;
			index++;
			if (index == AVG_SAMPLES) index = 0;
		}
	}
}

ISR(TIMER2_COMP_vect) {
	timestamp++;
}

int main(void) {
	
	usart_init();
	stdout = &usart_out;                                    // Redirect printf() to usart

	TCCR1B = (1 << CS11) | (1 << CS10) | (1 << ICNC1);	// Set prescaler to 64, activate input capture noise canceler

	TCCR2 = (1 << CS22) | (1 << WGM21);                     // Set prescaler to 64 on Timer 2, set Clear Timer on Compare Match
	OCR2 = 250;                                             // Output compare every 1 ms with 16MHz / 64 prescaler

	TIMSK = (1 << TICIE1) | (1 << OCIE2);                   // Enable Input Capture Interrupt for Timer1 and Timer2 Compare Match interrupt

	sei();

	uint32_t delay_local;
	uint32_t timestamp_local;

	while(1) {
		cli();
		delay_local = delay;
		timestamp_local = timestamp;                    // To prevent variables from being modified during a printf operation
		sei();
		uint32_t m = (delay_local * 10000) / 21178;     // Pulse width of 2.1178 cycles represents ~1 gram
		printf("%lu,%lu\r", timestamp_local, m);
	}

	while(1);

}
