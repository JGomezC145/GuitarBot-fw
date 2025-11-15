#pragma once

#include <Arduino.h>
#include "config.h"

/**
 * @file ble.h
 * @brief Capa HAL para BLE en GuitarBOT sobre Arduino UNO R4 WiFi.
 *
 * Este módulo expone una API sencilla para:
 * - Inicializar BLE y publicar un servicio "GuitarBOT".
 * - Enviar estado del robot mediante notificaciones.
 * - Recibir comandos desde un dispositivo central (teléfono, tablet, etc.).
 *
 * Implementación no bloqueante: todas las funciones están pensadas
 * para ser llamadas desde el bucle principal sin usar delay().
 *
 * Requiere:
 * - Librería ArduinoBLE declarada en platformio.ini.
 */

namespace ble {

/**
 * @brief Modo de operación del robot expuesto por BLE.
 */
enum class RobotMode : uint8_t {
    Idle = 0,  ///< Robot en espera / parado.
    Manual,    ///< Control manual.
    Auto       ///< Modo automático.
};

/**
 * @brief Estado lógico del robot que se expone por BLE.
 */
struct Status {
    /**
     * @brief Modo de operación actual.
     */
    RobotMode mode = RobotMode::Idle;

    /**
     * @brief Porcentaje estimado de batería (0-100).
     *
     * Usar -1 cuando el valor sea desconocido o no medido.
     */
    int batteryPct = -1;

    /**
     * @brief Indica si hay obstáculo frontal detectado.
     *
     * true si se ha detectado un obstáculo, false en caso contrario.
     */
    bool obstacleFront = false;
};

/**
 * @brief Inicializa el stack BLE y configura el servicio GuitarBOT.
 *
 * Debe llamarse una sola vez desde setup(). Si la inicialización falla,
 * devuelve false y no se publicará el servicio.
 *
 * @param deviceName Nombre que verán los clientes al escanear dispositivos BLE.
 *                   Ejemplo: "GuitarBOT".
 * @return true si la inicialización fue correcta, false si hubo error.
 */
bool begin(const char *deviceName = "GuitarBOT");

/**
 * @brief Atiende eventos del stack BLE de forma no bloqueante.
 *
 * Debe llamarse con frecuencia dentro del loop() principal. Se encarga de:
 * - Procesar eventos internos de ArduinoBLE.
 * - Detectar conexiones / desconexiones de centrals.
 * - Capturar comandos escritos en la característica de comandos.
 */
void update();

/**
 * @brief Actualiza el estado expuesto por BLE y envía notificación si aplica.
 *
 * Si existe un central conectado y suscrito a notificaciones de la
 * característica de estado, la función serializa el estado y lo envía.
 *
 * @param status Estructura con el estado lógico actual del robot.
 */
void setStatus(const Status &status);

/**
 * @brief Indica si hay un nuevo comando recibido desde BLE pendiente de lectura.
 *
 * @return true si hay un comando nuevo pendiente; false en caso contrario.
 */
bool commandAvailable();

/**
 * @brief Lee el último comando recibido y limpia el flag interno.
 *
 * Si no había un comando pendiente, devuelve una cadena vacía.
 *
 * @note La llamada consume el comando pendiente; posteriores llamadas
 *       devolverán cadena vacía hasta que llegue un nuevo comando.
 *
 * @return Cadena con el comando recibido o cadena vacía si no había.
 */
String readCommand();

/**
 * @brief Indica si hay al menos un dispositivo central conectado por BLE.
 *
 * @return true si existe una conexión activa; false si no hay centrals conectados.
 */
bool isConnected();

}  // namespace ble
