#include <stdio.h>
#include <stdint.h>
#include <pigpio.h>
#include <stdbool.h>

// define buzzer and button
#define BUZZER 21
#define NUM_BUTTON 7
// button that will be used from 2 to 8
enum melody_buttons {BUT_DO = 2, BUT_RE, BUT_MI, BUT_FA, BUT_SO, BUT_LA, BUT_XI};
// button for changing level on melody from 14 to 15
#define BUT_LEVEL_1 0
#define BUT_LEVEL_2 1

// define melody
#define DO_1 262
#define RE_1 294
#define MI_1 330
#define FA_1 349
#define SO_1 392
#define LA_1 440
#define XI_1 494

#define DO_2 523
#define RE_2 587
#define MI_2 659
#define FA_2 698
#define SO_2 784
#define LA_2 880
#define XI_2 988

uint32_t melody[2][NUM_BUTTON] = {
    {DO_1, RE_1, MI_1, FA_1, SO_1, LA_1, XI_1},
    {DO_2, RE_2, MI_2, FA_2, SO_2, LA_2, XI_2}
};
// use to indicate current molody level
bool current = 1;

// use for prevent double press at a really short amount of time
#define MIN_DIFF 10
uint32_t last_tick;

void buzz(uint32_t freq, int button) {
    // calculate out frequency
    double half_wave_duration = 1.0 / (freq * 2);
    // continuously buzz according to frequency
    while (true) {
        gpioWrite(BUZZER, 1);
        gpioDelay(half_wave_duration * 1e6);
        gpioWrite(BUZZER, 0);
        gpioDelay(half_wave_duration * 1e6);
        // break when button is released
        if(gpioRead(button) == 1) break;
    }
}

// level is 1 when button is released
// 0 when button is pressed
void melody_button_pressed (int gpio, int level, uint32_t tick) {
    // if button is pressed and not double press
    if (level == 0 && tick - last_tick > MIN_DIFF) { 
        last_tick = tick;
        // give corresponding melody to buzz
        buzz(melody[current][gpio-BUT_DO], gpio);
    }
}

void level_button_pressed (int gpio, int level, uint32_t tick) {
    // if button is pressed and not double press
    if (level == 0 && tick - last_tick > MIN_DIFF) { 
        last_tick = tick;
        // change to different level
		current = gpio;
    }
}

void initialise() {
    last_tick = gpioTick();
	
    // when button pressed, it will call this function
    gpioSetAlertFunc(BUT_LEVEL_1, level_button_pressed);
    gpioSetMode(BUT_LEVEL_1, PI_INPUT);
    gpioSetAlertFunc(BUT_LEVEL_2, level_button_pressed);
    gpioSetMode(BUT_LEVEL_2, PI_INPUT);

    uint8_t button = BUT_DO;
	for (int i = 0; i < NUM_BUTTON; i++) {
		// when button pressed, it will call this function
		gpioSetAlertFunc(button, melody_button_pressed);
		// set button's gpio to input
		gpioSetMode(button, PI_INPUT);
        button++;
	}
    
	// set buzzer's gpio to output 
    gpioSetMode(BUZZER, PI_OUTPUT);
}



int main() {
	// initialise the program
    if (gpioInitialise()<0) return 1;
	initialise();
	// the program will never stop and always detect button
    while (true) gpioDelay(100);
	// terminates program
    gpioTerminate();
	return 0;
}

