#include <BleKeyboard.h>

// Button pins
const int backButtonPin = 16;    // GPIO16 (D16) pin connected to the back button
const int forwardButtonPin = 27; // GPIO27 (D27) pin connected to the forward button

// Left RGB LED pins
const int bluePinRight = 19;      // GPIO19 connected to the blue pin of the left RGB LED
const int greenPinRight = 5;      // GPIO5 connected to the green pin of the left RGB LED
const int redPinRight = 2;        // GPIO2 connected to the red pin of the left RGB LED

// Right RGB LED pins
const int bluePinLeft = 25;     // GPIO25 connected to the blue pin of the right RGB LED
const int greenPinLeft = 32;    // GPIO32 connected to the green pin of the right RGB LED
const int redPinLeft = 34;      // GPIO34 connected to the red pin of the right RGB LED

int backButtonState = HIGH;      // Current state of the back button
int lastBackButtonState = HIGH;  // Previous state of the back button
int forwardButtonState = HIGH;   // Current state of the forward button
int lastForwardButtonState = HIGH; // Previous state of the forward button
bool ledOn = false;              // LED state
unsigned long lastBackDebounceTime = 0;  // The last time the back button was toggled
unsigned long lastForwardDebounceTime = 0;  // The last time the forward button was toggled
unsigned long debounceDelay = 50;    // The debounce time; increase if the output flickers
unsigned long ledTurnOffTime = 0;    // The time when the LED should turn off

BleKeyboard bleKeyboard("ESP32 Keyboard", "Espressif", 100);

// Function to set the color of the left RGB LED
void setRGBColorLeft(int red, int green, int blue) {
  digitalWrite(redPinLeft, red);
  digitalWrite(greenPinLeft, green);
  digitalWrite(bluePinLeft, blue);
}

// Function to set the color of the right RGB LED
void setRGBColorRight(int red, int green, int blue) {
  digitalWrite(redPinRight, red);
  digitalWrite(greenPinRight, green);
  digitalWrite(bluePinRight, blue);
}

void setup() {
  // Set button pins as input with internal pull-up resistor
  pinMode(backButtonPin, INPUT_PULLUP);
  pinMode(forwardButtonPin, INPUT_PULLUP);

  // Set RGB LED pins as output
  pinMode(bluePinLeft, OUTPUT);
  pinMode(greenPinLeft, OUTPUT);
  pinMode(redPinLeft, OUTPUT);

  pinMode(bluePinRight, OUTPUT);
  pinMode(greenPinRight, OUTPUT);
  pinMode(redPinRight, OUTPUT);

  // Initialize BLE Keyboard
  bleKeyboard.begin();

  // Show green LED on the right to indicate power on
  setRGBColorRight(LOW, HIGH, LOW);

  // Turn off the left LED initially
  setRGBColorLeft(LOW, LOW, LOW);
}

void loop() {
  // Check if BLE is connected
  if (bleKeyboard.isConnected()) {
    // Show blue LED on the left to indicate Bluetooth connection
    setRGBColorLeft(LOW, LOW, HIGH);
  } else {
    // Show orange LED on the left when searching for Bluetooth
    setRGBColorLeft(HIGH, HIGH, LOW);
  }

  int backReading = digitalRead(backButtonPin);
  int forwardReading = digitalRead(forwardButtonPin);

  // Debounce the back button
  if (backReading != lastBackButtonState) {
    lastBackDebounceTime = millis();
  }

  if ((millis() - lastBackDebounceTime) > debounceDelay) {
    if (backReading == LOW && backButtonState == HIGH) {
      ledOn = true;
      ledTurnOffTime = millis() + 1000;
      bleKeyboard.write(KEY_LEFT_ARROW);
      // Change the right LED to purple when the back button is pressed
      setRGBColorLeft(HIGH, LOW, LOW); // Red
    }
    backButtonState = backReading;
  }

  // Debounce the forward button
  if (forwardReading != lastForwardButtonState) {
    lastForwardDebounceTime = millis();
  }

  if ((millis() - lastForwardDebounceTime) > debounceDelay) {
    if (forwardReading == LOW && forwardButtonState == HIGH) {
      ledOn = true;
      ledTurnOffTime = millis() + 1000;
      bleKeyboard.write(KEY_RIGHT_ARROW);
      setRGBColorLeft(HIGH, LOW, LOW); // Red
    }
    forwardButtonState = forwardReading;
  }

  // Check if the LED should be turned off
  if (ledOn && (millis() > ledTurnOffTime)) {
    ledOn = false;
    // Reset the LED colors
    setRGBColorRight(LOW, HIGH, LOW); // Green on right
    if (bleKeyboard.isConnected()) {
      setRGBColorLeft(LOW, LOW, HIGH); // Blue on left if connected
    } else {
      setRGBColorLeft(HIGH, LOW, LOW); // Red
    }
  }

  // Save the readings. Next time through the loop, they'll be the last states
  lastBackButtonState = backReading;
  lastForwardButtonState = forwardReading;
}
