# 🩺 Smart Thermoregulation Heatpad with Heart Rate Monitor

![ESP32](https://img.shields.io/badge/Hardware-ESP32-blue?style=for-the-badge&logo=espressif)
![Firmware](https://img.shields.io/badge/Firmware-C%2B%2B%20%2F%20Arduino-00979D?style=for-the-badge&logo=arduino)
![Architecture](https://img.shields.io/badge/Logic-Non--Blocking-orange?style=for-the-badge)

### 📝 Project Overview
This repository contains the firmware and architecture design for an **IoT Smart Thermoregulation Heatpad** powered by an ESP32 microcontroller. The device seamlessly integrates real-time health telemetry (heart rate monitoring) with automated thermal regulation. 

Engineered with a **Non-blocking Firmware Architecture**, the system guarantees continuous, high-frequency biological data collection without suffering from execution stalls or telemetry gaps typically caused by power-switching latency. This project demonstrates practical hardware-level risk management, designed to eliminate thermal hazards in wearable medical devices or industrial hardware applications.

---

### 🚀 Key Features
* **Real-Time Health Telemetry:** Multi-threaded feel processing that captures object temperature and pseudo-BPM simultaneously via a shared I2C bus.
* **Non-Blocking Execution Loop:** Replaces restrictive `delay()` functions with internal hardware timer checks via `millis()`, completely preventing sensor data loss, buffer underflows, and Watchdog Timer (WDT) resets.
* **Automated Thermal Risk Mitigation:** Built-in fail-safe mechanism that acts as a hardware-level limiter, immediately cutting off heating elements if structural or object temperatures breach safety boundaries.

---

### 🛠️ Hardware Components
* **Microcontroller:** ESP32 (NodeMCU Development Board)
* **Temperature Sensor:** MLX90614 (Contactless Infrared Thermometer)
* **Heart Rate Sensor:** MAX30102 / MAX30105 (High-Sensitivity Pulse Oximeter)
* **Switching Actuator:** IRLZ44N MOSFET (Logic-Level Gate Control)
* **Heating Element:** Flexible Polyimide Heat Film
* **Power Delivery System:** 3.7V LiPo Battery + TP4056 Charging IC + Step-Up Boost Converter

---

### 📊 Pin Mapping & Wiring Layout

Both sensor nodes share a parallel I2C bus configuration to minimize GPIO utilization and maximize hardware expandability:

| Component | Component Pin | ESP32 Pin | Signal Type / Function |
| :--- | :--- | :--- | :--- |
| **MAX30102 / 105** | SDA | D21 | I2C Data Line (Shared Bus) |
| | SCL | D22 | I2C Clock Line (Shared Bus) |
| **MLX90614** | SDA | D21 | I2C Data Line (Shared Bus) |
| | SCL | D22 | I2C Clock Line (Shared Bus) |
| **IRLZ44N MOSFET** | Gate | D19 | Digital Output (Control Switch) |
| **Power Infrastructure**| VCC / GND | VIN / GND | System Voltage Distribution |

---

### ⚙️ Control Logic & Risk Thresholds

The firmware evaluates thermal data arrays every **1000 ms** to execute real-time defensive risk controls:
* **Safe State (Temperature <= 38°C):** Pemanas/Heater pin is driven `HIGH` (ON), maintaining a stable, regulated heating curve for the wearable pad.
* **Hazardous State (Temperature > 38°C):** Pemanas/Heater pin is instantly forced `LOW` (OFF). This eliminates the risk of thermal burns to the user and protects the surrounding enclosure from structural degradation.

---

### 💻 Core Firmware Implementation

```cpp
// Non-Blocking Logic & Automated Heater Control Execution
if (millis() - lastDisplayTime >= DISPLAY_INTERVAL) {
    lastDisplayTime = millis();

    // Ingest telemetry data points
    tempAmbient = mlx.readAmbientTempC();
    tempObject  = mlx.readObjectTempC();

    // Real-time risk management threshold evaluation
    if (tempObject > 38.0) {
      digitalWrite(HEATER_PIN, LOW);  // Overheat Mitigation Active
    } else {
      digitalWrite(HEATER_PIN, HIGH); // Normal Heating State
    }
}
