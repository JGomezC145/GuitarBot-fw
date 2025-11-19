#include <Arduino.h>
#include "config.h" // <- priority include
#include "hal.h"
#include "tasks.h"
#include "hal/ultrasonics.h"
#include "tasks/us_handle.h"


const unsigned long SAMPLE_INTERVAL_MS = 100;
unsigned long lastSample = 0;
bool isTriggered = false;

void prs() {
    Serial.println("Any triggered!");
}

void printStatus(const std::array<bool, static_cast<size_t>(ultrasonics::Sensor::Count)>& status) {
    Serial.print("Sensor status: ");
    for (size_t i = 0; i < status.size(); ++i) {
        Serial.print("Sensor ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(status[i] ? "OBSTACLE " : "clear ");
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200); 
    ultrasonics::init();
}

void loop() {
    unsigned long now = millis();
    if (now - lastSample >= SAMPLE_INTERVAL_MS) {
        lastSample = now;

        float d = ultrasonics::measureDistance(ultrasonics::Sensor::Front);
        // Serial.print("Front cm: ");
        // Serial.println(d);

        if (ultrasonics::hasObstacleAny(25.0f) && !isTriggered) { // si cualquier sensor detecta <25cm
            task_ultrasonics::checkObstacleAndExecute(25.0f, prs);
            task_ultrasonics::checkEachSensorObstacle(25.0f, printStatus);
            isTriggered = true;
        } else if (!ultrasonics::hasObstacleAny(25.0f) && isTriggered) {
            Serial.println("No hay obstaculos cercanos");
            isTriggered = false;
        }
    }

    // aquí puede ir otro trabajo no bloqueante (tareas, BLE, etc.)
}
