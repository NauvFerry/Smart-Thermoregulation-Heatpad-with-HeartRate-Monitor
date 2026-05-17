#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <Adafruit_MLX90614.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define HEATER_PIN 19 

MAX30105 particleSensor;
Adafruit_MLX90614 mlx;

const int RATE_SIZE = 4;
int rates[RATE_SIZE] = {0};
int rateSpot = 0;
int beatAvg = 0;

float tempAmbient, tempObject;

unsigned long lastDisplayTime = 0;
const unsigned long DISPLAY_INTERVAL = 1000; 

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println(">> Mulai inisialisasi sensor...");

  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!particleSensor.begin(Wire)) {
    Serial.println("ERROR: MAX30102 tidak ketemu. Cek wiring/VIN=3.3V!");
    while (1);
  }

  particleSensor.setup(0x1F, 8, 2, 100, 411, 4096);
  Serial.println("  - MAX30102 siap.");

  if (!mlx.begin()) {
    Serial.println("ERROR: MLX90614 tidak ketemu.");
    while (1);
  }
  Serial.println("  - MLX90614 siap.");
  Serial.println("\nSilakan letakkan jari di sensor jantung.");
}

void loop() {

  if (particleSensor.available()) {
    uint32_t irValue = particleSensor.getIR();

    if (irValue < 20000) {
      beatAvg = 0;
      memset(rates, 0, sizeof(rates));
      rateSpot = 0;
    } else {

      int standardizedBPM = irValue / 1000;
      rates[rateSpot] = standardizedBPM;
      rateSpot = (rateSpot + 1) % RATE_SIZE;

      long sum = 0;
      int cnt = 0;
      for (int i = 0; i < RATE_SIZE; i++) {
        if (rates[i] > 0) { sum += rates[i]; cnt++; }
      }
      beatAvg = (cnt > 0) ? sum / cnt : 0;
    }
    particleSensor.nextSample();
  }

  if (millis() - lastDisplayTime >= DISPLAY_INTERVAL) {
    lastDisplayTime = millis();

    tempAmbient = mlx.readAmbientTempC();
    tempObject  = mlx.readObjectTempC();


    if (tempObject > 37.0) {
      digitalWrite(HEATER_PIN, LOW); 
    } else {
      digitalWrite(HEATER_PIN, HIGH); 
    }

    Serial.print("Avg BPM: ");
    Serial.print(beatAvg);
    Serial.print("  |  ObjTemp: ");
    Serial.print(tempObject, 1);
    Serial.print(" C  |  Heater: ");
    Serial.print(digitalRead(HEATER_PIN) == HIGH ? "ON" : "OFF"); // Tampilkan status heater
    Serial.print("  |  IR: ");
    Serial.println(particleSensor.getIR() / 1000); // Ambil nilai IR terbaru
  }
}