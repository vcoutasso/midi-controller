#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#define OLED_CS 13
#define OLED_DS 12
#define OLED_RST -1

#include <Adafruit_SSD1331.h>

extern Adafruit_SSD1331 display;

void setupDisplay();
void displayText(int channel, int velocity, int baseNote);

#endif // DISPLAY_HPP