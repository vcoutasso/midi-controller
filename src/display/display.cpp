#include "display.hpp"
#include <SPI.h>

Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, OLED_CS, OLED_DS, OLED_RST);

void setupDisplay() {
	SPI.begin();
	display.begin();
	display.setFont();
	display.setTextColor(0xFFFF);
	display.setTextSize(1.5);
	display.fillScreen(0x001F);
	display.enableDisplay(true);
}

void displayText(int channel, int velocity, int baseNote) {
	display.fillScreen(0x001F);
	display.setCursor(0, 0);
	display.println();
	display.println();
	display.println("MIDI channel: " + String(channel));
	display.println();
	display.println("Velocity: " + String(velocity));
	display.println();
	display.println("Base note: " + String(baseNote));
}