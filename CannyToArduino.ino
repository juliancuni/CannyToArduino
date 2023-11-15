#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// SH110X_WHITE
#define i2c_Address 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int baudRate = 9600;
const int interval = 50;     // Change to 100 milliseconds
const int numReadings = 10;  // Change to 10

unsigned long previousMillis = 0;
int readings[numReadings];
int index = 0;
int sum = 0;
int average = 0;

void setup() {
  Serial.begin(baudRate);

  // initialize with the I2C address specified above
  display.begin(i2c_Address, OLED_RESET);
  display.display();
  delay(20);
  display.clearDisplay();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the reading and increment the index
    int reading = readSerialInt();
    readings[index] = reading;
    index++;
    previousMillis = currentMillis;
    if (index >= 10) {
      int sum = 0;
      for (int i = 0; i < 10; i++) {
        sum += readings[i];
      }
      float averageRpm = sum / 10;
      Serial.println(averageRpm);
      // Convert RPM to Frequency (Hz)
      float frequencyHz = averageRpm / 60.0;
      // Display the sum and average on the OLED display
      // Update Timer1 to match the new pulse rate
      long intervalMicroseconds = (long)(1000000.0 / (frequencyHz * 4.0));
      // Timer1.setPeriod(intervalMicroseconds);
      display.clearDisplay();
      display.print("Sum: ");
      display.println(sum);
      display.print("Average Rpm: ");
      display.println(averageRpm);
      display.print("Freq: ");
      display.println(frequencyHz);
      // display.print("Free Memory: ");
      // display.println(getFreeHeap());
      display.display();

      // for (int i = 0; i < 10; i++) {
      //   incomingData[i] = 0;
      // }
      memset(readings, 0, sizeof(readings));
      index = 0;
    }
  }
}
int readSerialInt() {
  while (Serial.available() < 2) {}  // wait for at least 2 bytes
  int lowByte = Serial.read();
  int highByte = Serial.read();
  return (highByte << 8) | lowByte;
}
