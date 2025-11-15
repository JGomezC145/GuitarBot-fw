#include <Arduino.h>
#include "config.h" // <- priority include
#include "hal.h"
#include "tasks.h"

void setup() {
    Serial.begin(115200); 

    // Inicializar BLE
    if (!ble::begin(BLE_DEVICE_NAME)) {
        // TODO: manejar error (parpadear LED, etc.)
        Serial.println("Error initializing BLE!");
        bool ledState = false;
        pinMode(LED_BUILTIN, OUTPUT);
        while (1) {
            digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
            ledState = !ledState;
            delay(1000);
        }
    }

}

void loop() {
    ble::update();

    // Procesar comandos recibidos por BLE
    if (ble::commandAvailable()) cmd::processRawCommand(ble::readCommand());

}
