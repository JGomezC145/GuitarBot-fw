#pragma once

#include <Arduino.h>
#include "config.h"

namespace motors {

    /**
     * @brief Direcciones de movimiento para los motores. 
     */
    enum class Direction : uint8_t {
        Stop = 0,    ///< Detener motor.
        Forward,    ///< Mover en dirección positiva.
        Backward     ///< Mover en dirección negativa.
    };

    /**
     * @brief Inicializa los motores TB6600 con los pines definidos en config.h.
     */
    void init();

    /**
     * @brief Establece la velocidad del motor 1 en pasos por segundo.
     * @param speed Velocidad deseada.
     */
    void setMotor1Speed(float speed);

    /**
     * @brief Establece la velocidad del motor 2 en pasos por segundo.
     * @param speed Velocidad deseada.
     */
    void setMotor2Speed(float speed);

    /**
     * @brief Mueve el motor 1 a la posición absoluta especificada.
     * @param position Posición objetivo en pasos.
     */
    void moveMotor1To(long position);

    /**
     * @brief Mueve el motor 1 indefinidamente.
     * @param direction Dirección del movimiento: forward o backward.
     */
    void moveMotor1Infinitely(Direction direction);

    /**
     * @brief Mueve el motor 2 indefinidamente.
     * @param direction Dirección del movimiento: forward o backward.
     */
    void moveMotor2Infinitely(Direction direction);

    /**
     * @brief Mueve el motor 2 a la posición absoluta especificada.
     * @param position Posición objetivo en pasos.
     */
    void moveMotor2To(long position);

    /**
     * @brief Ejecuta el ciclo de control de los motores. Debe llamarse frecuentemente.
     */
    void run();

    /**
     * @brief Detiene ambos motores.
     */
    void stopAll();

    /**
     * @brief Detiene ambos motores instantáneamente. Uso de emergencia. 
     */
    void emergencyStop();

} // namespace motors