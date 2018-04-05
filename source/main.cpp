/* The MIT License (MIT)
 *
 * Copyright (c) 2018 Cynara Krewe
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software, hardware and associated documentation files (the "Solution"), to deal
 * in the Solution without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Solution, and to permit persons to whom the Solution is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Solution.
 *
 * THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
 * SOLUTION.
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#include "driverlib/debug.h"

#include "pinmux/pinout.h"

#include "flow/components.h"
#include "flow/utility.h"

#include "flow/tm4c/clock.h"
#include "flow/tm4c/gpio.h"

static Flow::Component** _sysTickComponents = nullptr;
static unsigned int _sysTickComponentsCount = 0;

int main(void)
{
	// Set up the clock circuit.
	Clock::instance()->configure(120 MHz);

	// Set up the pin mux configuration.
	PinoutSet();

	Timer* timer = new Timer(100 /*ms*/);
	Toggle* toggle = new Toggle();
	DigitalOutput* led = new DigitalOutput(Gpio::Name{ Gpio::Port::N, 1 });

	// Connect the components of the application.
	Flow::Connection* connections[] =
	{
		Flow::connect(timer->outTick, toggle->tick),
		Flow::connect(toggle->out, led->inValue)
	};

	// Define the deployment of the components.
	Flow::Component* mainComponents[] =
	{
		toggle,
		led
	};

	Flow::Component* sysTickComponents[] =
	{
		timer
	};

	_sysTickComponents = sysTickComponents;
	_sysTickComponentsCount = ArraySizeOf(sysTickComponents);

	// Run the application.
	while(true)
	{
		for(unsigned int c = 0; c < ArraySizeOf(mainComponents); c++)
		{
			mainComponents[c]->run();
		}
	}

	// Disconnect the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(connections); i++)
	{
		Flow::disconnect(connections[i]);
	}

	// Destruct the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(mainComponents); i++)
	{
		delete mainComponents[i];
	}

	_sysTickComponentsCount = 0;
	_sysTickComponents = nullptr;

	for(unsigned int i = 0; i < ArraySizeOf(sysTickComponents); i++)
	{
		delete sysTickComponents[i];
	}
}

// SysTick related stuff.
extern "C" void SysTickIntHandler(void)
{
	for(unsigned int c = 0; c < _sysTickComponentsCount; c++)
	{
		_sysTickComponents[c]->run();
	}
}

// An assert will end up here.
extern "C" void __error__(const char *pcFilename, uint32_t ui32Line)
{
	__asm__ __volatile__("bkpt");
	while(true);
}

