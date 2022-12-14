#include "display/display.hpp"
#include "bluetooth/bluetooth.hpp"
#include <Arduino.h>
#include <USB-MIDI.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, BLEMIDI);

// Buttons

const int buttonCount = 9;
const int buttonPins[buttonCount] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
// Workaround so we can have a full octave with 8 buttons by skipping the sharp notes
const int increments[buttonCount] = {0, 2, 4, 5, 7, 9, 11, 12, 13};

int currentButtonState[buttonCount] = {};
int previousButtonState[buttonCount] = {};

unsigned long lastDebounceTime[buttonCount] = {0};
const unsigned long debounceDelay = 50;

// Potentiometers

const int potentiometerCount = 4;
const int potentiometerPins[potentiometerCount] = {A0, A1, A2, A3};

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
byte lowestCC = 1;
byte lowestNote = 36;
byte velocity = 127;

void readButtons();
void readPotentiometers();

void setup() {
  for (auto pin: buttonPins) pinMode(pin, INPUT_PULLUP);
  MIDI.begin();
  BLEMIDI.begin();
  setupDisplay();
  setupSerialBluetooth();
}

uint32_t previousTime = 0;
uint32_t displayUpdateInterval = 250; // ms

void loop() {
  readButtons();
  readPotentiometers();
  uint32_t currentTime = millis();
  if (currentTime - previousTime >= displayUpdateInterval) {
    previousTime = currentTime;
    displayText(midiChannel, velocity, lowestNote);
  }
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
          BLEMIDI.sendNoteOn(note, velocity, midiChannel);
        } else {
          MIDI.sendNoteOff(note, velocity, midiChannel);
          BLEMIDI.sendNoteOff(note, velocity, midiChannel);
        }	

        previousButtonState[i] = currentButtonState[i];
      }
    }
  }
}

void readPotentiometers() {
  lowestNote = map(analogRead(A4), 0, 1023, 0, 127 - buttonCount);
  velocity = map(analogRead(A5), 0, 1023, 0, 127);

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
        BLEMIDI.sendControlChange(lowestCC + i, currentMidiState[i], midiChannel);

        previousPotentiometerState[i] = currentPotentiometerState[i];
        previousMidiState[i] = currentMidiState[i];
      }
    }
  }
}
