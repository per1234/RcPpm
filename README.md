# RC PPM Library for Arduino

The RC PPM library decodes PPM signal from an RC receiver/transmitter.

This library provides APIs to read the PPM encoded signal supported by many RC systems. The PPM encoded
signal can be read from the RC transmitter's trainer port or from the RC receiver PPM port if available.

The radio control pulse-position modulation usually supports from 2 to 16 channels for propotional control.
Each PPM frame is at least 18 ms. It contains N+1 pulses, where N is the number of supported channels.
The pulses are all of the same width which is typically 0.1 .. 0.5 ms. Time between adjacent pulses i and i+1
encodes PWM pulse for the channel i servo. Servos usually support pulses of 1 .. 2 ms. The frame
length must be long enough to allow at least 2.5 ms pause after the last pulse in the frame for the worst
case scenario (i.e. all N PWM pulses are of maximal width). 8 channel transmiters may form frames of 20 ms,
while 16 channel transmitters require at least 34.5 ms.

This library works equally well with PPM pulses of high logical level and low logical level. It uses the 
same algorithm as the RcTrainer library by Mike McCauley (http://www.airspayce.com/mikem/arduino/RcTrainer).
It simply returns intervals between rising edges. The intervals include pulses and gaps. So it does not matter
whether pulses are high and gaps are low or vice versa.
![PPM and inverted PPM decoding](/doc/ppm.jpg)

This algorithm requires your system to be hard real time! Even if you use the highest priority INT0 for
servicing PPM, the interrupt may happen while another interrupt is being handled. In such cases calculation of
time intervals will be inaccurate. Please note that the timer interrupt happens roughly each millisecond (1024
times per second). It will sporadically delay our PPM decoder interrupt handler regardless of its priority.
So some measures should be taken to reduce the delay and improve accuracy.
![PPM interrupt delay](/doc/ppm_int.jpg)

If this library is used in a PPM to USB joystick converter, the USB ISRs will add more sporadic delays.
The system library should be modified to enable external interrupts while USB events are handled.
Fortunately, Leonardo has enough RAM to enable nested interrupts.

Since interference with other interrupts is inevitable, filtering is added to improve accuracy. The simple 
moving average filter with window of 4 samples shows good results and acceptable delay. You can increase 
the window to improve accuracy at the cost of higher delay.

If you modify the library code, please follow these rules:
* variables or class members which are used both in ISRs and other functions must be declared as **volatile**;
* use critical sections to access variables that are modified in ISRs;
* as an exception, 1 byte variables do not require critical sections.

The PPM signal from RC may not be electrically compatible with Arduino. Note that Arduino supports TTL
logical levels. Signals below 2.7 V are recognized as low. If you need a logical level converter, you
can refer to the following schematics:
![Logical Level Converter](/doc/logical_level_converter.jpg)

R1 1 .. 3 K

R2 1 .. 3 K

R3 1 .. 3 K (it protects T1 and the microcontroller if you by mistake configure the port for output)

This level converter inverts the signal. It accepts low input of 0 .. 0.5 V and high input of 1 .. 5 V.

# License

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
