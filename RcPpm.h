/*
  Copyright (c) 2018 Mikhail Durnev <mdurnev@gmail.com>.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef RC_PPM_H
#define RC_PPM_H

// Max number of PPM sources is restricted by the number of interrupts we can handle
#define EXTERNAL_INT_NUM 6

// Max number of pulses in one frame (usually it does not exceed 10)
#define MAX_PULSE_NUM 12

// Window size for the FIR filter
#define PPM_FILTER_WINDOW 4

class RcPpm {
protected:
    // ppmReceObj should be initialized once for all objects
    static bool isrInitialized;

    // Array of RcPpm objects attached to external interrupts
    static RcPpm * ppmReceObj[EXTERNAL_INT_NUM];

    // Array of interrupt service routines
    static void (* isr[EXTERNAL_INT_NUM])(void);

    // Interrupt number
    uint8_t attachedInterrupt;

    // ISR internal variables
    uint32_t lastTime;
    uint16_t pulseInternal[MAX_PULSE_NUM];
    uint8_t currentPulse;

    // Min and max pulse width/duration in us (actually pulse+gap)
    uint16_t minPulseWidth;
    uint16_t maxPulseWidth;

    // Storage for unfiltered data
    volatile uint16_t pulse[PPM_FILTER_WINDOW][MAX_PULSE_NUM];
    uint8_t currentFrame;

public:
    RcPpm(uint16_t minPulse = 1000, uint16_t maxPulse = 2500);

    void attachInterrupt(uint8_t n);

    void detachInterrupt();

    // callback/ISR
    void risingEdgeDetected();

    // API to get decoded and filtered PPM signal (returns values in us)
    uint16_t operator[](uint8_t idx);
};

#endif // RC_PPM_H
