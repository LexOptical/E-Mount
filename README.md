# E-Mount Protocol
## Purpose
This project is to implement the E-Mount communication protocol on an Arduino compatible board. You will need to have a device that can opereate a UART at 750kBaud. 

## Supported platforms
- Arduino Teensy 3.5 

## Current state
Can emulate a manual focus manual aperture lens well enough to convince a NEX7.

## Protocol documenation
We have documented the protocol [here](https://docs.google.com/document/d/1iw54nzrF0bzQgLINpcP9F8Odd0N5cd7LjlwCDPTNZK0/edit#).

## Sample data
See [LexOptical/E-Mount-Traffic-Samples](https://github.com/LexOptical/E-Mount-Traffic-Samples)

## Building
You need to setup the [Teensyduino add-ons](https://www.pjrc.com/teensy/td_download.html) for Arduino Studio. The project should then build in the Arduino Studio normally.

## Electrical interface
To interface with a camera you can construct a jig using a macro extension tube (use a 16mm one if possible as the wiring is difficult in a 10mm version). Meike brand is known to be workable.
