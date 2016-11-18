# APA102

A set of simple tools for controlling APA102 LEDs. The tools use various libraries and run on the Raspberry Pi B

## Libraries Used

The following libraries need to be installed on the development system used. I have only done
this on a Raspberry Pi Model B

```
$ sudo apt-get install wiringpi
$ sudo apt-get install libjson0-dev
```

## Build on Raspberry Pi

```
$ gcc -o apa102 apa102.c -lwiringPi -lc
$ gcc -o move move.c -lwiringPi -lc
$ gcc -o colour colour.c -lwiringPi -lc
$ gcc -o led led.c -I /usr/include/json -lwiringPi -lc -ljson
```

or just run the make, e.g.

```
$ make all
$ make led
$make clean
```

## Running on Raspberry Pi

These simple applications are mainly for fun and to prototype different behaviour for the API102
types of configurable LEDs

```
$ sudo led -p 4 -X 400000 -b 4
```

