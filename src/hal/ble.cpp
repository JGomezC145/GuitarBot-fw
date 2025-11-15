#include "ble.h"

#include <ArduinoBLE.h>

namespace ble {

// ============================================================================
// Definición de servicio y características BLE
// ============================================================================

/**
 * @brief Servicio principal de GuitarBOT.
 */
static BLEService guitarService(BLE_SERVICE_UUID);

/**
 * @brief Característica para recibir comandos desde el central.
 *
 * Tipo: String, permisos de escritura.
 * Ejemplos de comandos: "STOP", "PLAY", "AUTO", "MANUAL", etc.
 */
static BLEStringCharacteristic commandCharacteristic(BLE_CHAR_COMMAND_UUID, BLERead | BLEWrite, 20);

/**
 * @brief Característica para enviar estado hacia el central.
 *
 * Tipo: String, solo notificación.
 * Formato por defecto: "M:<modo>,B:<bateria>,O:<obstaculo>"
 * Ejemplo: "M:2,B:85,O:0"
 */
static BLEStringCharacteristic statusChar(
    "19b10001-e8f2-537e-4f6c-d104768a1215", // any%
    BLENotify,
    64);

// ============================================================================
// Estado interno del módulo
// ============================================================================

/**
 * @brief Flag interno que indica si hay un central conectado.
 */
static bool g_connected = false;

/**
 * @brief Último comando recibido desde la característica de comandos.
 */
static String g_lastCommand = "";

/**
 * @brief Flag que indica si g_lastCommand contiene un comando nuevo sin leer.
 */
static bool g_hasNewCommand = false;

/**
 * @brief Último estado lógico enviado / preparado para enviar por BLE.
 */
static Status g_lastStatus = {};

/*
 * @brief Nombre del dispositivo para publicidad BLE.
 */
static const char *g_deviceName = "GuitarBOT";

// ============================================================================
// Funciones internas
// ============================================================================

/**
 * @brief Serializa un Status a una cadena compacta para enviar por BLE.
 *
 * Formato actual:
 *   "M:<modo>,B:<bateria>,O:<obstaculo>"
 *
 * donde:
 *   - <modo> es el valor numérico del enum RobotMode.
 *   - <bateria> es el porcentaje de batería (int).
 *   - <obstaculo> es 1 si hay obstáculo, 0 si no.
 *
 * @param s Estado a serializar.
 * @return Cadena resultante.
 */
static String encodeStatus(const Status &s) {
    String out = "M:";
    out += static_cast<uint8_t>(s.mode);
    out += ",B:";
    out += s.batteryPct;
    out += ",O:";
    out += s.obstacleFront ? "1" : "0";
    return out;
}

// ============================================================================
// API pública
// ============================================================================

bool begin(const char *deviceName) {
    // Inicialización del stack BLE
    Serial.println("[BLE] Initializing...");
    if (!BLE.begin()) {
        // DEBUG_PRINTLN("[BLE] Error inicializando BLE");
        return false;
    }
    Serial.println("[BLE] BLE initialized");
    g_deviceName = (deviceName);

    Serial.print("[BLE] Starting BLE config...");
    BLE.setLocalName(deviceName);
    BLE.setDeviceName(deviceName);

    // Configurar servicio y características
    BLE.setAdvertisedService(guitarService);

    guitarService.addCharacteristic(commandCharacteristic);
    guitarService.addCharacteristic(statusChar);
    BLE.addService(guitarService);

    // Valores iniciales por seguridad
    commandCharacteristic.writeValue("READY");
    statusChar.writeValue("");

    // Comenzar publicidad para que los centrals puedan descubrir el dispositivo
    Serial.println("done.");
    Serial.println("[BLE] Advertising service...");
    BLE.advertise();
    Serial.println("[BLE] Device is now advertising as: " + String(g_deviceName));


    g_connected     = false;
    g_hasNewCommand = false;
    g_lastCommand   = "";

    return true;
}

void update() {
    // Procesa eventos internos del stack BLE
    BLE.poll();

    // Gestionar conexión / desconexión de centrals
    BLEDevice central = BLE.central();
    if (central && central.connected()) {
        // Se ha establecido o se mantiene una conexión
        if (!g_connected) {
            // Transición: desconectado -> conectado
            Serial.println("[BLE] Central connected: " + central.address());
            g_connected = true;

            // Opcional: enviar estado inicial al conectar
            if (statusChar.subscribed()) {
                statusChar.writeValue(encodeStatus(g_lastStatus));
            }
        }
    } else {
        // No hay centrals conectados
        if (g_connected) {
            // Transición: conectado -> desconectado
            g_connected = false;
            Serial.println("[BLE] Central disconnected");

            // rehabilita el advertising para aceptar nuevas conexiones
            BLE.advertise();

        }
    }

    // Comprobar si llegó un nuevo comando
    if (commandCharacteristic.written()) {
        String value = commandCharacteristic.value();
        if (value.length() > 0) {
            g_lastCommand   = value;
            g_hasNewCommand = true;
        }
    }
}

void setStatus(const Status &status) {
    g_lastStatus = status;

    // No se hace nada si no hay central conectado
    if (!g_connected) {
        return;
    }

    // Solo se envia notificación si el central está suscrito a la característica
    if (!statusChar.subscribed()) {
        return;
    }

    String encoded = encodeStatus(status);
    statusChar.writeValue(encoded);
}

bool commandAvailable() {
    return g_hasNewCommand;
}

String readCommand() {
    if (!g_hasNewCommand) {
        return String();
    }

    g_hasNewCommand = false;
    return g_lastCommand;
}

bool isConnected() {
    return g_connected;
}

}  // namespace ble
