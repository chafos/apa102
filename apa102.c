#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
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

printf("%s: sending %d extra bytes\n", __FUNCTION__, bytes);
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
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	uint8_t brightness = 15;
	uint8_t leds = 10;
	uint8_t extra = 0;
	uint8_t cycle = 0;
	int i, c;

	while ((c = getopt (argc, argv, "r:g:b:l:n:x:c:h")) != -1) {
		switch (c) {
		case 'r':
			red = atoi(optarg);
			break;
		case 'g':
			green = atoi(optarg);
			break;
		case 'b':
			blue = atoi(optarg);
			break;
		case 'n':
			leds = atoi(optarg);
			break;
		case 'l':
			brightness = atoi(optarg);
			break;
		case 'x':
			extra = atoi(optarg);
			break;
		case 'c':
			cycle = atoi(optarg);
			break;
		case '?':
		case 'h':
			printf("Usage: apa102 -r <0 - 255> -g <0 - 255> -b <0 - 255> -l <0 - 31>\n");
			return 1;
		default:
			abort ();
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

	//end_frame(1 + (leds / 8));  // One extra clock cycle per LED, but we can only send bytes
	end_frame(extra);  // One extra clock cycle per LED, but we can only send bytes

	if (cycle) {
//		brightness = 0;
		red = green = blue = 0;

		for (cycle = 0; cycle < (256 * 4); cycle++) {

			start_frame();
//printf("brightness = %d\n", brightness);
			for (i = 0; i < leds; i++)
				send_frame(red, green, blue, brightness);
			end_frame(extra);

			usleep(50000);
//			brightness = (brightness + 1) % 16;
			red++; green++; blue++;
		}
	}

	return 0;
}

