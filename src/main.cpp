#include <Arduino.h>
#include <USB-MIDI.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();

// Buttons

const int buttonCount = 8;
const int buttonPins[buttonCount] = {2, 3, 4, 5, 6, 7, 8, 9};
// Workaround so we can have a full octave with 8 buttons by skipping the sharp notes
const int increments[buttonCount] = {0, 2, 4, 5, 7, 9, 11, 12};

int currentButtonState[buttonCount] = {};
int previousButtonState[buttonCount] = {};

unsigned long lastDebounceTime[buttonCount] = {0};
const unsigned long debounceDelay = 50;

// Potentiometers

const int potentiometerCount = 2;
const int potentiometerPins[potentiometerCount] = {A0, A1};

int currentPotentiometerState[potentiometerCount] = {0};
int previousPotentiometerState[potentiometerCount] = {0};
bool isPotentiometerMoving = false;
int potentiometerVariationThreshold = 10;
unsigned long potentiometerReadTimeout = 300;

int currentMidiState[potentiometerCount] = {0};
int previousMidiState[potentiometerCount] = {0};

unsigned long previousReadTimestamp[potentiometerCount] = {0};
unsigned long timeElapsedSinceLastRead[potentiometerCount] = {0};

// MIDI

byte midiChannel = 1;
byte lowestNote = 36;
byte velocity = 127;

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
  for (int i = 0; i < buttonCount; ++i) {
    currentButtonState[i] = digitalRead(buttonPins[i]);

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (previousButtonState[i] != currentButtonState[i]) {
        lastDebounceTime[i] = millis();

        int isOn = currentButtonState[i] == LOW;
        int note = lowestNote + increments[i];

        if (isOn) {
          MIDI.sendNoteOn(note, velocity, midiChannel);
        } else {
          MIDI.sendNoteOff(note, velocity, midiChannel);
        }	

        previousButtonState[i] = currentButtonState[i];
      }
    }
  }

}

void readPotentiometers() {
  for (int i = 0; i < potentiometerCount; ++i) {
    currentPotentiometerState[i] = analogRead(potentiometerPins[i]);

    currentMidiState[i] = map(currentPotentiometerState[i], 0, 1023, 0, 127);

    int variation = abs(currentPotentiometerState[i] - previousPotentiometerState[i]);

    long timestamp = millis();

    if (variation > potentiometerVariationThreshold) {
      previousReadTimestamp[i] = timestamp;
    }

    timeElapsedSinceLastRead[i] = timestamp - previousReadTimestamp[i];

    isPotentiometerMoving = timeElapsedSinceLastRead[i] < potentiometerReadTimeout;

    if (isPotentiometerMoving) {
      if (currentMidiState[i] != previousMidiState[i]) {
        MIDI.sendControlChange(lowestCC + i, currentMidiState[i], midiChannel);

        previousPotentiometerState[i] = currentPotentiometerState[i];
        previousMidiState[i] = currentMidiState[i];
      }
    }
  }
}