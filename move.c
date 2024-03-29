#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

int start_frame()
{
	int i;
	uint8_t buf[1];

	for (i = 0; i < 4; i++) {
		buf[0] = 0x00;
		wiringPiSPIDataRW(0, buf, 1);
	}

	return 0;
}

int end_frame(int bytes)
{
	int i;
	uint8_t buf[1];

	for (i = 0; i < bytes; i++) {
		buf[0] = 0xFF;
		wiringPiSPIDataRW(0, buf, 1);
	}

	return 0;
}

int send_frame(uint8_t r, uint8_t g, uint8_t b, uint8_t br)
{
	uint8_t led_frame[4];

	led_frame[0] = 0b11100000 | (0b00011111 & br);
	led_frame[1] = b;
	led_frame[2] = g;
	led_frame[3] = r;

	wiringPiSPIDataRW(0, led_frame, 4);

	return 0;
}

int clear_display(int leds) {
	int j;

	start_frame();

	for (j = 0; j <= leds; j++) {
		send_frame(0, 0, 0, 16);
	}

	start_frame();  // One extra clock cycle per LED, but we can only send bytes

	return 0;
}

int next(int current, int min, int max) {
	static int delta = -1;

	if ((current == min) || (current == max)) {
		delta = -delta;
	}

	return current + delta;
}

int randomize(uint8_t *r, uint8_t *g, uint8_t *b) {
	*r = (uint8_t) (rand() & 0x01) * 0x20;
	*g = (uint8_t) (rand() & 0x01) * 0x20;
	*b = (uint8_t) (rand() & 0x01) * 0x20;

	return 0;
}

int main(int argc, char *argv[])
{
	uint32_t colour = 0;
	uint8_t red, green, blue;
	uint8_t brightness = 31;
	uint8_t leds = 10;
	uint16_t delay = 10;
	uint8_t iterations = 10;
	int i, j, c;
	int position = 0;
	int random = 0;

	while ((c = getopt (argc, argv, "X:b:l:t:i:rh")) != -1) {
		switch (c) {
		case 'X':
			colour = strtol(optarg, NULL, 16);
			red = (colour & 0xFF0000) >> 16;
			green = (colour & 0x00FF00) >> 8;
			blue = (colour & 0x0000FF) >> 0;
			break;
		case 'b':
			brightness = atoi(optarg);
			break;
		case 'l':
			leds = atoi(optarg);
			break;
		case 't':
			delay = atoi(optarg);
			break;
		case 'i':
			iterations = atoi(optarg);
			break;
		case 'r':
			random = 1;
			break;
		case '?':
		case 'h':
			printf("Usage: colour -X <000000 - FFFFFF> -b <0 - 31> -b <1 - 10>\n");
			return 1;
		default:
			abort ();
		}
	}

	srand(time(NULL));

	wiringPiSetup();
	if (wiringPiSPISetup(0, 6000000) < 0) {
		printf("wiringPiSPISetup failed\n");
		return -1;
	}

	int total = (2 * iterations * leds) - (2 * iterations) + 1;
	for (i = 0; i < total; i++) {
		start_frame();

		for (j = 0; j <= leds; j++) {
			if (position == j) {
				if (1 == random)
					randomize(&red, &green, &blue);
printf("Red = 0x%02x, green = 0x%02x, blue = 0x%02x\n", red, green, blue);
				send_frame(red, green, blue, brightness);
			} else
				send_frame(0, 0, 0, 16);
		}

		start_frame();  // One extra clock cycle per LED, but we can only send bytes
		usleep(delay * 1000);

		position = next(position, 0, 9);
//		printf("Next is %d\n", position);
	}
	start_frame();  // One extra clock cycle per LED, but we can only send bytes

	clear_display(leds);

	return 0;
}

