/* ESP Metronome with Display
Use an ESP32 Microcontroller as a metronome for music projects.
Author: Ravi Umadi, 2023
License: MIT
*/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Check your display's I2C address, usually 0x3C or 0x3D
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.display();
  delay(2000);  // Pause for 2 seconds
  display.clearDisplay();
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(2);               // Normal 1:1 pixel scale, here 2
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(10, 30);              // Adjust text position
  display.print("BPM: ");
  display.println(bpm);
  display.display(); 

void loop() {
  if (buttonUpPressed) {
    bpm = min(bpm + 10, 300);  // Increment BPM, max limit 300
    buttonUpPressed = false;   // Reset the flag
    updateDisplay();           // Update display with new BPM
  }

  // Check if button down was pressed
  if (buttonDownPressed) {
    bpm = max(bpm - 10, 40);    // Decrement BPM, min limit 40
    buttonDownPressed = false;  // Reset the flag
    updateDisplay();            // Update display with new BPM
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
