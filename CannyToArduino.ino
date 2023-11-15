#include <Wire.h>
#include <TimerOne.h>

const int baudRate = 9600;
const int interval = 50;
const int numReadings = 10;

unsigned long previousMillis = 0;
int readings[numReadings];
int index = 0;
int sum = 0;
int average = 0;

void setup() {
  Serial.begin(baudRate);
  pinMode(3, OUTPUT);
  Timer1.initialize();  // Initialize Timer1
  Timer1.attachInterrupt(timerISR);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    int reading = readSerialInt();
    if (reading > 0) {
      readings[index] = reading;
      index++;
      previousMillis = currentMillis;
      // Serial.print("Nr. Live: ");
      // Serial.println(reading);
      if (index >= 10) {
        int sum = 0;
        for (int i = 0; i < 10; i++) {
          sum += readings[i];
        }
        float averageRpm = (sum / 10) * 0.3;
        Serial.print("Mesatarja: ");
        Serial.println(averageRpm);
        // Convert RPM to Frequency (Hz)
        float frequencyHz = averageRpm / 60.0;
        // Serial.print("Freq: ");
        // Serial.println(frequencyHz);
        // Update Timer1 to match the new pulse rate
        long intervalMicroseconds = (long)(1000000.0 / (frequencyHz * 4.0));
        Timer1.setPeriod(intervalMicroseconds);
        memset(readings, 0, sizeof(readings));
        index = 0;
      } else {
        Timer1.stop();
      }
    }
  }
}
int readSerialInt() {
  while (Serial.available() < 2) {}  // wait for at least 2 bytes
  int lowByte = Serial.read();
  int highByte = Serial.read();
  return (highByte << 8) | lowByte;
}

// Timer Interrupt Service Routine (ISR)
void timerISR() {
  static bool outputState = false;

  outputState = !outputState;
  digitalWrite(3, outputState);  // Toggle PWM pin
}
