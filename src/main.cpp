#include <Arduino.h>
#include <USB-MIDI.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();

// Buttons

const int buttonCount = 4;
const int buttonPins[buttonCount] = {2, 3, 4, 5};

int currentButtonState[buttonCount] = {};
int previousButtonState[buttonCount] = {};

unsigned long lastDebounceTime[buttonCount] = {0};
const unsigned long debounceDelay = 50;

// Potentiometers

const int potentiometerCount = 2;
const int potentiometerPins[potentiometerCount] = {A0, A1};

int currentPotentiometerState[potentiometerCount] = {0};
int previousPotentiometerState[potentiometerCount] = {0};

int currentMidiState[potentiometerCount] = {0};
int previousMidiState[potentiometerCount] = {0};

// MIDI

byte midiChannel = 1;
byte lowestNote = 36;
byte lowestCC = 1;

void readButtons();
void readPotentiometers();

void setup() {
  for (auto pin: buttonPins) pinMode(pin, INPUT_PULLUP);
}

void loop() {
  readButtons();
  readPotentiometers();
}

void readButtons() {
  for (int i = 0; i < buttonCount; i++) {
    currentButtonState[i] = digitalRead(buttonPins[i]);

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (previousButtonState[i] != currentButtonState[i]) {
        lastDebounceTime[i] = millis();

        int signal = currentButtonState[i] == LOW ? 127 : 0;

        MIDI.sendNoteOn(lowestNote + i, signal, midiChannel);

        previousButtonState[i] = currentButtonState[i];
      }
    }
  }

}

void readPotentiometers() {

}