#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

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

int main(int argc, char *argv[])
{
	uint32_t colour = 0;
	uint8_t red, green, blue;
	uint8_t brightness = 31;
	uint8_t leds = 10;
	int i, c;

	while ((c = getopt (argc, argv, "X:b:l:h")) != -1) {
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
		case '?':
		case 'h':
			printf("Usage: colour -X <000000 - FFFFFF> -b <0 - 31> -b <1 - 10>\n");
			return 1;
		default:
			return 1;
		}
	}


	wiringPiSetup();
	if (wiringPiSPISetup(0, 6000000) < 0) {
		printf("wiringPiSPISetup failed\n");
		return -1;
	}

	start_frame();

	for (i = 0; i < leds; i++) {
		send_frame(red, green, blue, brightness);
	}

	end_frame(1);  // One extra clock cycle per LED, but we can only send bytes

	return 0;
}

