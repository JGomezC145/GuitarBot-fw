#pragma once

#include <Arduino.h>

/**
 * @file ble_commands.h
 * @brief Procesamiento de comandos recibidos por BLE desde la app móvil.
 *
 * Este módulo se encarga de:
 * - Definir el conjunto de comandos lógicos que entiende el robot.
 * - Traducir caracteres (ej. 'G', 'H', etc.) a comandos.
 * - Ejecutar acciones en función del comando (motores, modos, etc.).
 *
 * No accede directamente a BLE; eso lo hace el HAL (hal/ble.*).
 * Aquí solo se trabaja con caracteres / strings ya recibidos.
 */

namespace ble_cmd {

/**
 * @brief Conjunto de comandos que puede entender el robot.
 */
enum class Command : uint8_t {
    None = 0,   ///< Sin comando / desconocido.
    Forward,    ///< Avanzar.
    Backward,   ///< Retroceder.
    Left,       ///< Girar a la izquierda.
    Right,      ///< Girar a la derecha.
    Stop,       ///< Detener movimiento.
    AutoMode,   ///< Activar modo automático.
    ManualMode,  ///< Activar modo manual.
    VoiceMode,  ///< Activar modo voz.
    AutoModeStart, ///< Iniciar modo automático.
    AutoModeStop   ///< Detener modo automático.
};

/**
 * @brief Decodifica un carácter recibido en un comando lógico.
 *
 * Mapeo final:
 *  - 'G' → Forward
 *  - 'H' → Backward
 *  - 'I' → Left
 *  - 'J' → Right
 *  - 'K' → Stop
 *  - '2' → AutoMode
 *  - 'M' → ManualMode
 *  - '3' → VoiceMode
 *  - 'A' → Inicia AutoMode
 *  - 'O' → Detiene AutoMode
 * 
 *
 * @param c Carácter recibido desde BLE.
 * @return Comando lógico correspondiente o Command::None si no coincide.
 */
Command decodeChar(char c);

/**
 * @brief Procesa un comando lógico y ejecuta las acciones necesarias.
 *
 * Aquí es donde se conectan los comandos con:
 *  - Control de motores (HAL de TB6600).
 *  - Cambio de modo (manual/auto).
 *  - Otros subsistemas según sea necesario.
 *
 * @param cmd Comando a procesar.
 */
void processCommand(Command cmd);

/**
 * @brief Procesa directamente un string recibido (ej. desde BLE).
 *
 * Toma el primer carácter del string, lo decodifica y lo procesa.
 *
 * @param raw Cadena recibida (por ejemplo desde ble::readCommand()).
 */
void processRawCommand(const String &raw);

}  // namespace cmd
