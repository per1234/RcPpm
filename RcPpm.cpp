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

#include <Arduino.h>
#include "RcPpm.h"

bool RcPpm::isrInitialized = false;

// Array of RcPpm objects attached to external interrupts
RcPpm * RcPpm::ppmReceObj[EXTERNAL_INT_NUM];

// Array of interrupt service routines
void (* RcPpm::isr[EXTERNAL_INT_NUM])(void);


//================================================================================================
//                                        CONSTRUCTOR
//================================================================================================
RcPpm::RcPpm(uint16_t minPulse, uint16_t maxPulse) : 
    attachedInterrupt(EXTERNAL_INT_NUM),
    lastTime(0),
    currentPulse(0),
    minPulseWidth(minPulse),
    maxPulseWidth(maxPulse),
    currentFrame(0)
{
    if (!isrInitialized) {
        isr[0] = [](){ppmReceObj[0]->risingEdgeDetected();};
#if EXTERNAL_INT_NUM > 1
        isr[1] = [](){ppmReceObj[1]->risingEdgeDetected();};
#if EXTERNAL_INT_NUM > 2
        isr[2] = [](){ppmReceObj[2]->risingEdgeDetected();};
#if EXTERNAL_INT_NUM > 3
        isr[3] = [](){ppmReceObj[3]->risingEdgeDetected();};
#if EXTERNAL_INT_NUM > 4
        isr[4] = [](){ppmReceObj[4]->risingEdgeDetected();};
#if EXTERNAL_INT_NUM > 5
        isr[5] = [](){ppmReceObj[5]->risingEdgeDetected();};
#endif
#endif
#endif
#endif
#endif
    }

    memset((void *)pulse, 0, MAX_PULSE_NUM * sizeof(uint16_t));
}


//================================================================================================
//                                      INTERRUPT ATTACH
//================================================================================================
void RcPpm::attachInterrupt(uint8_t n) {
    if (n < EXTERNAL_INT_NUM) {
        ppmReceObj[n] = this;
        attachedInterrupt = n;
        ::attachInterrupt(n, isr[n], RISING);
    }
}


//================================================================================================
//                                      INTERRUPT DETACH
//================================================================================================
void RcPpm::detachInterrupt() {
    if (attachedInterrupt < EXTERNAL_INT_NUM) {
        ::detachInterrupt(attachedInterrupt);
        ppmReceObj[attachedInterrupt] = nullptr;
        attachedInterrupt = EXTERNAL_INT_NUM;
    }
}


//================================================================================================
//                                   INTERRUPT SERVICE ROUTINE
//================================================================================================
void RcPpm::risingEdgeDetected(void) {
    uint32_t currentTime = micros();
    uint32_t duration = currentTime - lastTime;
    lastTime = currentTime;

    if (duration <= (uint32_t)maxPulseWidth) {
        if (currentPulse < MAX_PULSE_NUM) {
            pulseInternal[currentPulse++] = duration;
        }
    }
    else {
        // store current frame
        memcpy((void *)(pulse[currentFrame]), (void *)pulseInternal, currentPulse * sizeof(uint16_t));

        // next frame
        currentPulse = 0;
        if (++currentFrame >= PPM_FILTER_WINDOW) {
            currentFrame = 0;
        }
    }
}


//================================================================================================
//                        GET WIDTH/DURATION OF ONE PULSE+GAP (FILTERED)
//================================================================================================
uint16_t RcPpm::operator[](uint8_t idx) {
    uint16_t res = 0;

    if (idx < MAX_PULSE_NUM) {
        uint8_t i;
        uint32_t s = 0;

        noInterrupts();
        for (i = 0; i < PPM_FILTER_WINDOW; i++) {
            s += pulse[i][idx];
        }
        interrupts();

        res = s / PPM_FILTER_WINDOW;
    }

    return res;
}
