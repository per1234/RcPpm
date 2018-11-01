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

#include <Joystick.h>
#include <RcPpm.h>

#define MAX_RC_CHANNELS 10
#if MAX_RC_CHANNELS > MAX_PULSE_NUM
#error "Max number of PPM channels exceeded"
#endif

// The output data range is the same as the input range
#define AXIS_RANGE_MIN 1000
#define AXIS_RANGE_MAX 2500

// Input: PPM signal from RC, e.g. RadioLink AT9
RcPpm ppm_src(AXIS_RANGE_MIN, AXIS_RANGE_MAX);

// Output: USB HID Joystick
// We need 10 axes to use all PPM channels for USB joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                   JOYSTICK_TYPE_JOYSTICK, 0, 0,
                   true, true, true, true, true, true,
                   true, true, false, true, true);

uint16_t ppm_data[MAX_RC_CHANNELS];
uint16_t new_ppm_data[MAX_RC_CHANNELS];

void setup() {
  //Serial.begin(9600);
  //while (!Serial) {;}

  pinMode(13, OUTPUT);  // Throttle indicator

  // All 10 axes can use the same range
  Joystick.setXAxisRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);
  Joystick.setYAxisRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);
  Joystick.setZAxisRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);
  Joystick.setRxAxisRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);
  Joystick.setRyAxisRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);
  Joystick.setRzAxisRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);
  Joystick.setThrottleRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);
  Joystick.setRudderRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);
  Joystick.setBrakeRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);
  Joystick.setSteeringRange(AXIS_RANGE_MIN, AXIS_RANGE_MAX);

  Joystick.begin(false);

  memset(ppm_data, 255, MAX_RC_CHANNELS * sizeof(int16_t));

  // Start PPM receiving
  ppm_src.attachInterrupt(0);
}

void loop() {
  // Read all PPM channels. Map the values to AXIS_RANGE_MIN..AXIS_RANGE_MAX
  uint8_t i;
  for (i = 0; i < MAX_RC_CHANNELS; i++) {
    new_ppm_data[i] = ppm_src[i];
  }

  // Typical channel assignments for Futaba / Hitec
  // 0 - ailerons
  // 1 - elevator
  // 2 - throttle
  // 3 - rudder
  
  // Typical channel assignments for Graupner / JR
  // 0 - throttle
  // 1 - ailerons
  // 2 - elevator
  // 3 - rudder

  // Typical channel assignments for MPX / robbe
  // 0 - ailerons
  // 1 - elevator
  // 2 - rudder
  // 3 - throttle

  // Typical channel assignments for Sanwa
  // 0 - elevator
  // 1 - ailerons
  // 2 - throttle
  // 3 - rudder

  // Typical channel assignments for RadioLink AT9:
  // 0 - ailerons / aileron1 / flaperon1
  // 1 - elevator
  // 2 - throttle
  // 3 - rudder
  // 4 - spare / landing gear / aileron2 / flaperon2
  // 5 - flaps / flap1
  // 6 - spare / aileron2 / flap2
  // 7 - spare / mixture control / spoiler
  // 8 - spare
  // 9 - spare

  // Typical PPM to USB converter mapping:
  // ailerons - X
  // elevator - Y
  // throttle - Z
  // rudder   - RY

  bool stateUpdated = false;

  // Map channel 0 to axis X which is usually used for roll control (ailerons)
  if (new_ppm_data[0] != ppm_data[0]) {
    Joystick.setXAxis(new_ppm_data[0]);
    stateUpdated = true;
  }

  // Map channel 1 to axis Y which is usually used for pitch control (elevator)
  if (new_ppm_data[1] != ppm_data[1]) {
    Joystick.setYAxis(new_ppm_data[1]);
    stateUpdated = true;
  }

  // Map channel 2 to axis Z which will be used for throttle
  if (new_ppm_data[2] != ppm_data[2]) {
    Joystick.setZAxis(new_ppm_data[2]);
    stateUpdated = true;
  }

  // Map channel 3 to axis RY which will be used for yaw control (rudder)
  // NOTE: on regular joysticks the rudder control channel is usually mapped to axis RZ.
  //       But some PPM to USB converters use RY instead.
  if (new_ppm_data[3] != ppm_data[3]) {
    Joystick.setRyAxis(new_ppm_data[3]);
    stateUpdated = true;
  }

  if (new_ppm_data[4] != ppm_data[4]) {
    Joystick.setRxAxis(new_ppm_data[4]);
    stateUpdated = true;
  }

  if (new_ppm_data[5] != ppm_data[5]) {
    Joystick.setRzAxis(new_ppm_data[5]);
    stateUpdated = true;
  }

  if (new_ppm_data[6] != ppm_data[6]) {
    Joystick.setThrottle(new_ppm_data[6]);
    stateUpdated = true;
  }

  if (new_ppm_data[7] != ppm_data[7]) {
    Joystick.setRudder(new_ppm_data[7]);
    stateUpdated = true;
  }

  if (new_ppm_data[8] != ppm_data[8]) {
    Joystick.setBrake(new_ppm_data[8]);
    stateUpdated = true;
  }

  if (new_ppm_data[9] != ppm_data[9]) {
    Joystick.setSteering(new_ppm_data[9]);
    stateUpdated = true;
  }

  if (stateUpdated) {
    Joystick.sendState();
  }

  memcpy(ppm_data, new_ppm_data, MAX_RC_CHANNELS * sizeof(int16_t));

  //Serial.println(ppm_data[0]);
  //Serial.println(ppm_data[1]);
  //Serial.println(ppm_data[2]);
  //Serial.println(ppm_data[3]);
  //Serial.println(ppm_data[4]);
  //Serial.println();

  // Throttle position indicator
  analogWrite(13, (ppm_data[2] - 1000) >> 4);
}
