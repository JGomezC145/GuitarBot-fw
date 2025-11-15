#include "ble_commands.h"

#include "hal/ble.h"

/*
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
*/

namespace cmd {

// Velocidad base ejemplo para movimientos manuales

Command decodeChar(char c) {
    switch (c) {
        case 'G':  // Adelante
        case 'g':
            return Command::Forward;

        case 'H':  // Atrás
        case 'h':
            return Command::Backward;

        case 'I':  // Izquierda
        case 'i':
            return Command::Left;

        case 'J':  // Derecha
        case 'j':
            return Command::Right;

        case 'K':  // Stop
        case 'k':
            return Command::Stop;

        case '2':  // Modo automático
            return Command::AutoMode;

        case 'M':  // Modo manual
        case 'm':
            return Command::ManualMode;

        case '3':  // Modo voz
            return Command::VoiceMode;

        case 'A':  // Inicia modo automático
        case 'a':
            return Command::AutoModeStart;

        case 'O':  // Detiene modo automático
        case 'o':
            return Command::AutoModeStop;

        default:
            return Command::None;
    }
}

void processCommand(Command cmd) {
    Serial.print("Processing command: ");
    Serial.println(static_cast<uint8_t>(cmd));
    switch (cmd) {
        case Command::Forward:
            // __
            break;

        case Command::Backward:
            // __
            break;

        case Command::Left:
            // __
            break;

        case Command::Right:
            // __
            break;

        case Command::Stop:
            // __
            break;

        case Command::AutoMode:
            // __
            break;

        case Command::ManualMode:
            // __
            break;

        case Command::None:
        default:
            // ignore
            break;
    }

    // Opcional: actualizar el estado que se envía por BLE
    // ble::Status st;
    // // st.mode = (cmd == Command::AutoMode) ? ble::RobotMode::Auto : ble::RobotMode::Manual;
    // st.mode          = ble::RobotMode::Manual;  // placeholder
    // st.batteryPct    = -1;                      // posible API
    // st.obstacleFront = false;                   // leer de sensores

    // ble::setStatus(st);
}

void processRawCommand(const String &raw) {
    if (raw.length() == 0) {
        return;
    }

    const char c = raw[0];
    Command cmd  = decodeChar(c);
    processCommand(cmd);
}

}  // namespace cmd
