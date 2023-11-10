/* ESP Metronome
Use an ESP32 Microcontroller as a metronome for music projects.
Author: Ravi Umadi, 2023
License: MIT
*/

// Define the GPIO pins
const int buzzerPin = 25;      // Buzzer pin
const int ledPin = 4;          // LED pin
const int buttonUpPin = 34;    // Button to increase BPM
const int buttonDownPin = 35;  // Button to decrease BPM

volatile int bpm = 120;  // Initial BPM, volatile as it is changed in ISRs
unsigned long previousMillis = 0;

// Flags for button press, volatile as they are used in ISRs
volatile bool buttonUpPressed = false;
volatile bool buttonDownPressed = false;

// ISR for button up
void IRAM_ATTR increaseBpm() {
  buttonUpPressed = true;
}

// ISR for button down
void IRAM_ATTR decreaseBpm() {
  buttonDownPressed = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonUpPin, INPUT_PULLDOWN);
  pinMode(buttonDownPin, INPUT_PULLDOWN);

  // Attach interrupts to buttons
  attachInterrupt(digitalPinToInterrupt(buttonUpPin), increaseBpm, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonDownPin), decreaseBpm, RISING);
}

void loop() {
  // Check if button up was pressed
  if (buttonUpPressed) {
    bpm = min(bpm + 10, 300);  // Increment BPM, max limit 300
    buttonUpPressed = false;   // Reset the flag
    Serial.print("BPM: ");
    Serial.println(bpm);
  }

  // Check if button down was pressed
  if (buttonDownPressed) {
    bpm = max(bpm - 10, 40);    // Decrement BPM, min limit 40
    buttonDownPressed = false;  // Reset the flag
    Serial.print("BPM: ");
    Serial.println(bpm);
  }

  // Metronome logic
  unsigned long currentMillis = millis();
  int beatInterval = 60000 / bpm;  // Recalculate interval

  if (currentMillis - previousMillis >= beatInterval) {
    previousMillis = currentMillis;

    // Turn on the buzzer and LED for a short pulse
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    delay(10);  // Short pulse

    // Turn off the buzzer and LED
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
  }
}
