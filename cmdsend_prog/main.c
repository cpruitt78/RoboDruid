#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <bcm2835.h>

#define PIN_TX RPI_BPLUS_GPIO_J8_38
#define PIN_LED RPI_BPLUS_GPIO_J8_36
#define BIT_TIME 833

struct rs_map {
	const char *name;
	unsigned int cmd;
	const char key;
};

struct rs_map cmd_map[] = {
	{"turn_right",        0x80, 'd'},
	{"rarm_up",       0x81, 'e'},
	{"rarm_out",      0x82, '\0'},
	{"rbody_tilt",    0x83, '\0'},
	{"rarm_down",     0x84, 'c'},
	{"rarm_in",       0x85, '\0'},
	{"walk_forward",      0x86, 'w'},
	{"walk_back",     0x87, 's'},
	{"turn_left",         0x88, 'a'},
	{"larm_up",        0x89, 'q'},
	{"larm_out",       0x8A, '\0'},
	{"lbody_tilt",     0x8B, '\0'},
	{"larm_down",      0x8C, 'z'},
	{"larm_in",        0x8D, '\0'},
	{"shoot",          0x8E, 's'},
	{"stop",             0x8E, 'x'},
	{"burp",             0xC2, '\0'},
	{"all",              0xC2, '\0'},
	{"righthandstrike",  0xC0, '\0'},
	{"righthandsweep",   0xC1, '\0'},
	{"righthandstrike2", 0xC3, '\0'},
	{"high5",            0xC4, '5'},
	{"fart",             0xC7, '\0'},
	{"lefthandstrike",   0xC8, '\0'},
	{"lefthandsweep",    0xC9, '\0'},
	{"whistle",          0xCA, '\0'},
	{"roar",             0xCE, 'x'},
	{"dance",            0xD4, 'n'},
	{"highfive",         0xC4, '\0'},
	{"feetshuffle",      0xF6, '\0'},
	{"pickleft",         0xAC, '1'},
	{"pickright",        0xA4, '3'},
	{"bulldozer",        0xC6, '\0'},
	{0},
};

int pulse_cmd(unsigned int command, int PIN_SEND)
{
	// Preamble. Set IR signal line LOW
	bcm2835_gpio_write(PIN_SEND, LOW);
	bcm2835_delayMicroseconds(8*BIT_TIME);
	
	// Pulse the 8 bit command. Delay 4 slices for 1-bit and 1 slice for a 0-bit.
	for(int b = 0; b < 8; b++) {
		bcm2835_gpio_write(PIN_SEND, HIGH);
		bcm2835_delayMicroseconds(BIT_TIME);
		if((command & 128)!=0) bcm2835_delayMicroseconds(3*BIT_TIME);
		bcm2835_gpio_write(PIN_SEND, LOW);
		bcm2835_delayMicroseconds(BIT_TIME);
		command <<= 1;
	}
	
	bcm2835_gpio_write(PIN_SEND, HIGH);

	bcm2835_delay(250);	

	return 0;
}

int send_cmd_str(const char *cmd, unsigned int PIN_SEND) {
	if(!strcmp("shoot", cmd)) {
                printf("Shooter\n");
                for(int s = 0; s < 10; s++) {
			printf("Shoot!\n");
                        bcm2835_gpio_write(PIN_LED, HIGH);
                        bcm2835_delay(100);
                	bcm2835_gpio_write(PIN_LED, LOW);
			bcm2835_delay(100);
                }
        	return 0;
        }
	if(!strcmp("all", cmd)) {
		for(struct rs_map *i = cmd_map; i->name; i++) {
			bcm2835_delay(10000);
			pulse_cmd(0x8E, PIN_SEND);
			pulse_cmd(i->cmd, PIN_SEND);
		}
		return 0;
	}
	for(struct rs_map *i = cmd_map; i->name; i++) {
		if(!strcmp(i->name, cmd)) {
			printf("%s\n", i->name);
			pulse_cmd(i->cmd, PIN_SEND);
			return 0;
		}
	}
	return 1;
}

int send_cmd_key(const char key, unsigned int PIN_SEND) {
	for(struct rs_map *i = cmd_map; i->key; i++) {
		if(i->key == key) {
			pulse_cmd(i->cmd, PIN_SEND);
			return 0;
		}
	}
	return 1;
}

void on_quit_signal(int signo)
{
	bcm2835_gpio_write(PIN_LED, LOW);
	send_cmd_str("stop", HIGH);
	exit(1);
}

int main(int argc, char *argv[])
{
	setpriority(PRIO_PROCESS, 0, -20);

	if (!bcm2835_init())
		return 1;

	// Set the pin to be an output
	bcm2835_gpio_fsel(PIN_TX, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_LED, BCM2835_GPIO_FSEL_OUTP);

	if (signal(SIGINT, on_quit_signal) == SIG_ERR) {
		fputs("An error occurred while setting SIGINT signal handler.\n", stderr);
		bcm2835_gpio_write(PIN_TX, HIGH);
		return 1;
	}

	if(argc < 2) return 1;

	bcm2835_gpio_write(PIN_TX, HIGH);

	int retval = send_cmd_str(argv[1], PIN_TX);
	//sleep(5);
	//send_cmd_str("larm_down", PIN_TX);
	//bcm2835_gpio_write(PIN_TX, LOW);
	
	return retval;
}
