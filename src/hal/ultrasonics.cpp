#include <Arduino.h>
#include "config.h"
#include "ultrasonics.h"

namespace ultrasonics {

static std::array<UltrasonicSensor, static_cast<size_t>(Sensor::Count)> sensors = {
    UltrasonicSensor{US1_TRIG, US1_ECHO},
    UltrasonicSensor{US2_TRIG, US2_ECHO},
    UltrasonicSensor{US3_TRIG, US3_ECHO}
};

void init() {
    for (auto &s : sensors) s.begin();
}

// void init(const std::array<SensorPins, static_cast<size_t>(Sensor::Count)>& pins) {
//     // si prefieres pasar pines desde platformio/config, puedes inicializarlos aquí.
//     for (auto &s : sensors) s.begin();
// }

float measureDistance(Sensor sensor) {
    size_t idx = static_cast<size_t>(sensor);
    return sensors[idx].measureDistanceCm();
}

bool hasObstacle(Sensor sensor, float thresholdCm) {
    size_t idx = static_cast<size_t>(sensor);
    return sensors[idx].hasObstacle(thresholdCm);
}

bool hasObstacleAny(float thresholdCm) {
    for (auto &s : sensors) if (s.hasObstacle(thresholdCm)) return true;
    return false;
}

} // namespace ultrasonics