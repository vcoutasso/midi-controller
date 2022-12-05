#include "bluetooth.hpp"
#include <Arduino.h>

void setupSerialBluetooth() {
	HC05.begin(31250);
}