all: move colour apa102 led

clean:
	rm -f move colour apa102 led

move: move.c
	gcc -o move move.c -lwiringPi -lc

colour: colour.c
	gcc -o colour colour.c -lwiringPi -lc

apa102: apa102.c
	gcc -o apa102 apa102.c -lwiringPi -lc

led: led.c
	gcc -o led led.c -I /usr/include/json -lwiringPi -lc -ljson

