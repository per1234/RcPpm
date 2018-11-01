# RC PPM to USB Joystick Converter 

This application allows you to connect your favorite RC transitter to a PC as
a regular USB joystick and use it in various flight simulators.

To build it you need Arduino IDE 1.8.3 and the joystick library located at 
https://github.com/MHeironimus/ArduinoJoystickLibrary
Please use joystick library commit 10eda40beecf1b1b632551ba6378a169832aec17
from the master branch.

The RC PPM to USB joystick converter runs on Arduino Leonardo.
It receives pulse-position modulated control signal from the RC transmitter's
trainer port and converts it to the regular USB HIDD joystick output. No device
drivers are required on PC (Win or Linux). But it has to be calibrated before
the first use.

This application supports up to 10 channels. It was tested with RadioLink AT9.

The PPM port should be connected to port INT0/D3 on the Arduino Leonardo board
via the logical level converter.


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
