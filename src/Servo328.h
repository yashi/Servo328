/*
 *  Copyright (C) 2023 Space Cubics, LLC.
 */

#pragma once

#include <inttypes.h>

#define INVALID_SERVO (-1)
#define DEFAULT_MIN_PULSE_WIDTH (500)
#define DEFAULT_MAX_PULSE_WIDTH (2500)

class Pin
{
public:
	Pin(int num);

	int num;
	bool active { false };
	uint16_t min_value { DEFAULT_MIN_PULSE_WIDTH };
	uint16_t max_value { DEFAULT_MAX_PULSE_WIDTH };

	void write_us(uint16_t value);
	void write(uint16_t value);
};

class Servo328
{
public:
	Servo328();
	uint8_t attach(int pin, int min, int max);
	uint8_t attach(int pin);
	void write(uint16_t value, int pin);
	void write(uint16_t value);

private:
	Pin pin9 { 9 };
	Pin pin10 { 10 };

	void write_us(uint16_t value, int pin);
};
