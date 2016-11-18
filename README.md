# APA102

A set of simple tools for controlling APA102 LEDs. The tools use various libraries and run on the Raspberry Pi B

## Libraries Used

* WiringPi
* libjson-c

## Build instructions

$ gcc -o apa102 apa102.c -lwiringPi -lc
$ gcc -o move move.c -lwiringPi -lc
$ gcc -o colour colour.c -lwiringPi -lc
$ gcc -o led led.c -I /usr/include/json -lwiringPi -lc -ljson

or just run the make, e.g.

$ make all

