#pragma once

#include <Arduino.h>
#include <array>

namespace ultrasonics {

    /**
     * @brief Inicializa el sensor ultrasónico.
     */
    void init();

    //void init(const std::array<SensorPins, static_cast<size_t>(Sensor::Count)>& pins);


    /**
     * @brief Tipos de sensores ultrasónicos disponibles.
     */
    enum class Sensor : uint8_t {
        Front = 0,
        Left,
        Right,
        Count
    };

    struct SensorPins {
        uint8_t trigPin;
        uint8_t echoPin;
    };

    class UltrasonicSensor {
    public:
        constexpr UltrasonicSensor() = default;
        constexpr UltrasonicSensor(uint8_t trig, uint8_t echo) : trigPin(trig), echoPin(echo) {}

        void begin() const {
            pinMode(trigPin, OUTPUT);
            pinMode(echoPin, INPUT);
            digitalWrite(trigPin, LOW);
        }

        float measureDistanceCm() const {
            // implementación simple (puedes moverla al cpp)
            digitalWrite(trigPin, LOW);
            delayMicroseconds(2);
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10);
            digitalWrite(trigPin, LOW);
            long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
            if (duration == 0) return -1.0f;
            return (duration / 2.0f) / 29.1f; // cm
        }

        bool hasObstacle(float thresholdCm) const {
            float d = measureDistanceCm();
            return (d > 0.0f && d <= thresholdCm);
        }

        uint8_t trigPin = 0;
        uint8_t echoPin = 0;
    };

    /**
     * @brief Mide la distancia actual en centímetros.
     * @return Distancia medida en cm. Retorna -1 si no hay lectura válida.
     */
    float measureDistance(Sensor sensor); // mide un sensor específico


    /**
     * @brief Verifica si hay un obstáculo detectado por el sensor especificado.
     * @param sensor Sensor a verificar.
     * @return true si hay un obstáculo, false en caso contrario.
     */
    bool hasObstacle(Sensor sensor, float thresholdCm = 20.0f);

    /**
     * @brief Verifica si hay un obstáculo detectado por cualquier sensor.
     * @return true si hay un obstáculo, false en caso contrario.
     */
    bool hasObstacleAny(float thresholdCm = 20.0f);


} // namespace ultrasonics