/* The MIT License (MIT)
 *
 * Copyright (c) 2019 Cynara Krewe
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
#include "flow/reactor.h"
#include "flow/utility.h"

#include "flow/tm4c/clock.h"
#include "flow/tm4c/digitalio.h"

using namespace Flow::TM4C;

// Create the components of the application.
SoftwareTimer timer{100 /*ms*/};
Toggle toggle;
Digital::Output led{Pin::Port::N, 1, Digital::Polarity::Normal};

int main(void)
{
	// Set up the clock circuit.
	Clock::instance().configure<Device::TM4C129>(8 MHz);

	// Set up the pin mux configuration.
	PinoutSet();

	// Connect the components of the application.
	Flow::connect(timer.outTick, toggle.tick);
    Flow::connect(toggle.out, led.inState);

	Flow::Reactor::start();

	// Run the application.
	while(true)
	{
	    Flow::Reactor::run();
	}
}

// SysTick related stuff.
extern "C" void isrSysTick(void)
{
	timer.isr();
}

// An assert will end up here.
extern "C" void __assert_func(const char* file, int line, const char* function, const char* expression)
{
    __asm__ __volatile__("bkpt");
    while(true);
}

