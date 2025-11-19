#include <Arduino.h>
#include <array>
#include "hal/ultrasonics.h"
#include <functional>

namespace task_ultrasonics {

    /**
     * @brief Verifica si algún sensor ultrasónico detecta un obstáculo dentro del umbral especificado.
     * 
     * @param thresholdCm Umbral de distancia en centímetros.
     * @return true Si algún sensor detecta un obstáculo dentro del umbral.
     * @return false Si ningún sensor detecta un obstáculo dentro del umbral.
     */
    bool hasObstacleAny(float thresholdCm);

    /**
     * @brief Verifica si algun sensor ha detectado un obstáculo dentro del umbral especificado, y ejecuta el callback si es así.
     * @param thresholdCm Umbral de distancia en centímetros.
     * @param callback Función a ejecutar si se detecta un obstáculo.
     */
    void checkObstacleAndExecute(float thresholdCm, const std::function<void()> &callback);

    /**
     * @brief Verifica si el cada sensor ultrasónico detecta un obstáculo dentro del umbral especificado, y ejecuta el callback con el resultado.
     * @param thresholdCm Umbral de distancia en centímetros.
     * @param callback Función a ejecutar con el resultado de la verificación. Recibe un array de booleanos indicando el estado de cada sensor.
     */
    void checkEachSensorObstacle(float thresholdCm, const std::function<void(const std::array<bool, static_cast<size_t>(ultrasonics::Sensor::Count)>&)>& callback);


    /**
     * @brief Revisa los sensores ultrasónicos y devuelve un array tipo [bool, bool, bool] indicando si cada sensor detecta un obstáculo dentro del umbral.
     * @param thresholdCm Umbral de distancia en centímetros.
     * @return std::array<bool, static_cast<size_t>(ultrasonics::Sensor::Count)> Array con el estado de cada sensor.
     */
    std::array<bool, static_cast<size_t>(ultrasonics::Sensor::Count)> getEachSensorObstacleStatus(float thresholdCm);
} // namespace task_ultrasonics