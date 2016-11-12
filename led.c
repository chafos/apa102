#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <json_object.h>
#include <json_util.h>
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

#define LEDS_MAX 10

int main(int argc, char *argv[])
{
	uint32_t colour = 0;
	uint8_t red, green, blue;
	uint8_t brightness = 31;
	uint8_t leds = LEDS_MAX;
	uint16_t delay = 10;
	int i, j, c;
	int position = 0;
	int clear = 0;
	json_object *state = NULL;

	while ((c = getopt (argc, argv, "X:b:l:p:Ch")) != -1) {
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
		case 'p':
			position = atoi(optarg);
			if ((position < 0) || (position > leds)) {
				printf("Incorrect LED position\n");
				return 1;
			}
			break;
		case 'l':
			leds = atoi(optarg);
			break;
		case 'C':
			clear = 1;
			break;
		case '?':
		case 'h':
			printf("Usage: led [-l <1 - 10>] -p <0 - %d> -X <000000 - FFFFFF> -b <0 - 31>\n", leds);
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

	if (clear) {
		clear_display(LEDS_MAX);
		return 0;
	}

	state = json_object_from_file("/tmp/leds.json");
	
	// Load led configuration - if doesn't exist, then create it
	if (NULL == state) {
		state = json_object_new_array();
	}

	json_object *led = json_object_new_object();
	json_object_object_add(led, "red", json_object_new_int((uint32_t) red));
	json_object_object_add(led, "green", json_object_new_int((uint32_t) green));
	json_object_object_add(led, "blue", json_object_new_int((uint32_t) blue));
	json_object_object_add(led, "brightness", json_object_new_int((uint32_t) brightness));

	// Replace or insert into array
	json_object_array_put_idx(state, position, led);
	
	start_frame();

	for (i = 0; i < LEDS_MAX; i++) {
		if (NULL == (led = json_object_array_get_idx(state, i))) {
			red = green = blue = brightness = 0;
		} else {
			red = (uint8_t) json_object_get_int(json_object_object_get(led, "red"));
			green = (uint8_t) json_object_get_int(json_object_object_get(led, "green"));
			blue = (uint8_t) json_object_get_int(json_object_object_get(led, "blue"));
			brightness = (uint8_t) json_object_get_int(json_object_object_get(led, "brightness"));
		}

		//printf("Red = 0x%02x, green = 0x%02x, blue = 0x%02x, brightness = 0x%02x\n", red, green, blue, brightness);
		send_frame(red, green, blue, brightness);
		usleep(delay * 1000);
	}

	start_frame();  // One extra clock cycle per LED, but we can only send bytes

	// Save the current state for another time
	json_object_to_file_ext("/tmp/leds.json", state, JSON_C_TO_STRING_PRETTY);

	json_object_put(state);

	return 0;
}

