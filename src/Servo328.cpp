/*
 *  Copyright (C) 2023 Space Cubics, LLC.
 */


#include <Arduino.h>

#include "Servo328.h"

Pin::Pin(int num)
	: num(num)
{
}

static uint16_t to_ticks(uint16_t us)
{
	// Convert us to ticks
	// 1 tick is 62.5 ns (= 16 MHz)
	// 1 us = 16 ticks
	return us * 16;
}

void Pin::write_us(uint16_t value)
{
	uint8_t sreg;

	if (value < this->min_value) {
		value = this->min_value;
	}
	else if (value > this->max_value) {
		value = this->max_value;
	}

	value = to_ticks(value);

	sreg = SREG;
	cli();
	if (this->num == 9) {
		OCR1A = value;
	}
	else {
		OCR1B = value;
	}
	SREG = sreg;
}

void Pin::write(uint16_t value)
{
	if (value < this->min_value) {
		if (value > 180) {
			value = 180;
		}
		value = map(value, 0, 180, this->min_value, this->max_value);
	}
	write_us(value);
}

Servo328::Servo328()
{
	TCCR1A = 0;
	TCCR1B = 0;
}

uint8_t Servo328::attach(int pin_num, int min, int max)
{
	uint8_t ret = INVALID_SERVO;

	// We only support pin 9 or pin 10
	if (pin_num == 9) {
		this->pin9.active = true;
		this->pin9.min_value = min;
		this->pin9.max_value = max;
		ret = 0;
	}
	else if (pin_num == 10) {
		this->pin10.active = true;
		this->pin10.min_value = min;
		this->pin10.max_value = max;
		ret = 0;
	}
	else {
		return INVALID_SERVO;
	}

	pinMode(pin_num, OUTPUT);

	switch(pin_num) {
	case 9:
		// Clear OC1A on Compare Match, set OC1A at
		// BOTTOM (non-inverting mode)
		TCCR1A |= _BV(COM1A1);
		TCCR1A &= ~_BV(COM1A0);
		break;
	case 10:
		// Clear OC1B on Compare Match, set OC1B at
		// BOTTOM (non-inverting mode)
		TCCR1A |= _BV(COM1B1);
		TCCR1A &= ~_BV(COM1B0);

	default:
		break;
	}

	// Fast PWM, 16-bit
	TCCR1A &= ~_BV(WGM10);
	TCCR1A |= _BV(WGM11);
	TCCR1B |= _BV(WGM12);
	TCCR1B |= _BV(WGM13);

	// Set prescaler of 1
	TCCR1B |= _BV(CS10);
	TCCR1B &= ~_BV(CS11);
	TCCR1B &= ~_BV(CS12);

	ICR1 = 0xffff;

	return ret;
}

uint8_t Servo328::attach(int pin_num)
{
	return attach(pin_num, DEFAULT_MIN_PULSE_WIDTH, DEFAULT_MAX_PULSE_WIDTH);
}

void Servo328::write_us(uint16_t value, int pin_num)
{
	if (pin_num == 9) {
		this->pin9.write_us(value);
	}
	else {
		this->pin10.write_us(value);
	}
}

void Servo328::write(uint16_t value, int pin_num)
{
	if (pin_num == 9) {
		this->pin9.write(value);
	}
	else {
		this->pin10.write(value);
	}
}

void Servo328::write(uint16_t value)
{
	write(value, 9);
	write(value, 10);
}
