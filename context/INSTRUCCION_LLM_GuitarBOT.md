# Instrucción técnica — Configuración del proyecto **GuitarBOT** y de los agentes LLM

> **Propósito**: estandarizar cómo clonar, construir y extender el firmware de GuitarBOT, y cómo **agentes LLM** (como este) deben trabajar sobre el repo con **contexto completo**, generando código correcto, reproducible y documentado.

---

## 0) Alcance y supuestos
- **MCU**: Arduino **UNO R4 WiFi** (Renesas RA4M1, 5 V lógicos).
- **Drivers**: 2 × **TB6600** (STEP/DIR), ENA no cableado (habilitado fijo).
- **Sensores**: 3 × **HC-SR04**.
- **LEDs**: 2 × segmentos **WS2812B**.
- **Audio**: **DFPlayer Mini** → **PAM8403** → parlantes 8 Ω.
- **UI**: **LCD 1602 I²C**.
- **Alimentación**: LiPo 3S → **buck 5 V** (GND común).
- **Mapa de pines (verdad canónica)**: ver §2.3 (coincide con README_MASTER).

---

## 1) Estructura del repositorio (canon)
```
guitarbot/
├─ docs/
│   ├─ README_GuitarBOT_MASTER.md          # visión y pines (fuente de verdad)
│   ├─ Schematic_Conexiones-GuitarBot_2025-09-25.pdf
│   └─ Diseño de GuitarBOT (...).docx.pdf
├─ firmware/
│   ├─ include/
│   │   └─ config.h                        # pines & opciones (solo constantes)
│   ├─ src/
│   │   ├─ main.cpp                        # orquestación
│   │   ├─ hal/                            # capa de abstracción de HW
│   │   │   ├─ motors_tb6600.hpp/.cpp
│   │   │   ├─ ultrasonics.hpp/.cpp
│   │   │   ├─ leds_ws2812.hpp/.cpp
│   │   │   ├─ audio_dfplayer.hpp/.cpp
│   │   │   └─ lcd_i2c.hpp/.cpp
│   │   ├─ tasks/                          # lógica de alto nivel (FSM)
│   │   │   └─ navigator.hpp/.cpp
│   │   └─ util/                           # helpers (timing, filtros, log)
│   ├─ lib/                                # librerías locales (si aplica)
│   └─ platformio.ini
├─ bom/
│   └─ BOM_GuitarBOT.csv
└─ tools/
    ├─ format.sh                           # clang-format/arduino-lint
    └─ ci.sh                               # build + pruebas
```

> **Regla**: cualquier cambio de pines, energía o topología **debe** reflejarse en `docs/README_GuitarBOT_MASTER.md` y `firmware/include/config.h` en el mismo PR.

---

## 2) Configuración de firmware

### 2.1 Requisitos locales
- **PlatformIO Core** (CLI) y **VSCode + PlatformIO IDE** (opcional).
- **Toolchain** Arduino Renesas RA para UNO R4.
- **Python 3.10+** para scripts y herramientas.

### 2.2 `platformio.ini` (plantilla recomendada)
```ini
; firmware/platformio.ini
[env:uno_r4_wifi]
platform = renesas-ra
board    = uno_r4_wifi
framework = arduino

; Opciones de compilación
build_flags =
  -D CORE_DEBUG_LEVEL=0
  -D DISABLE_DFPLAYER_AT_BOOT=1    ; evita inicialización si interfiere con USB
  -D PIO_FRAMEWORK_ARDUINO_ENABLE_CDC ; monitor serie virtual
  -D SERIAL_TX_BUFFER_SIZE=128
  -D SERIAL_RX_BUFFER_SIZE=128

lib_deps =
  ; Motores
  mike-matera/AccelStepper
  ; Ultrasonidos
  NewPing
  ; LEDs
  adafruit/Adafruit NeoPixel
  ; DFPlayer
  DFRobotDFPlayerMini
  ; LCD I2C
  marcoschwartz/LiquidCrystal I2C

monitor_speed = 115200
upload_speed  = 921600
```

> Si la compilación falla por resolución de libs, fija versiones en `lib_deps` (`@ ^x.y.z`) o mueve la lib a `firmware/lib/`.

### 2.3 `config.h` (pines **canónicos**)
```cpp
// I2C
#define PIN_I2C_SDA   A4
#define PIN_I2C_SCL   A5
// Ultrasonidos
#define US1_TRIG      10
#define US1_ECHO      11
#define US2_TRIG      8
#define US2_ECHO      9
#define US3_TRIG      6
#define US3_ECHO      7
// LEDs
#define LED1_DATA     12
#define LED2_DATA     13   // ojo: LED onboard; migrar si hay flicker
// DFPlayer (UART HW)
#define DF_TO_ARDUINO_RX   0  // DF TX → D0 (RX)
#define DF_FROM_ARDUINO_TX 1  // DF RX ← D1 (TX)
// TB6600 (+ a GPIO, − a GND)
#define M1_PUL_PLUS   2
#define M1_DIR_PLUS   3
#define M2_PUL_PLUS   4
#define M2_DIR_PLUS   5
```

### 2.4 Convenciones de código
- **C++17**, encabezados `.hpp`, implementación `.cpp`.
- **HAL** sin `delay()` bloqueante; usar **timers no bloqueantes** (millis/RA tick).
- **FSM** (máquina de estados) en `tasks/` con ticks a 100 Hz (configurable).
- **Logging** por `Serial` protegido por `#ifdef DEBUG`.
- **Style**: `clang-format` (`LLVM`), `clang-tidy` opcional (`modernize-*`).
- **Documentación**: comentarios Doxygen en funciones públicas.
- **namespaces** para organizar módulos (`hal`, `tasks`, `util`, `BLE`).

### 2.5 Scripts
```bash
# formateo
bash tools/format.sh
# build + upload + monitor
pio run -e uno_r4_wifi && pio upload -e uno_r4_wifi && pio device monitor -b 115200
```

---

## 3) Instrucción para **agentes LLM** (plantilla de *system prompt*)

> Copia y pega esto como *instrucción del sistema* para cualquier LLM que trabajará en este repo:

```
Eres el **Agente Firmware de GuitarBOT**. Reglas operativas:
1) Idioma de salida: **español técnico** y conciso. No inventes hardware.
2) **Fuente de verdad** de hardware y pines: `docs/README_GuitarBOT_MASTER.md` y `firmware/include/config.h`. Si hay conflicto, **deténte** y propone un PR que los sincronice.
3) MCU: Arduino UNO R4 WiFi (Renesas RA4M1, 5 V). No uses APIs específicas de AVR si no son compatibles. Evita `delay()`.
4) Drivers de motor: TB6600 en modo STEP/DIR. ENA no cableado (asumir habilitado).
5) UART D0/D1 ocupado por DFPlayer: durante programación por USB, condicionar la inicialización con `#ifdef DISABLE_DFPLAYER_AT_BOOT`.
6) LEDs WS2812B: respetar timing; si se usa D13 y hay flicker, sugerir migración de pin.
7) LCD 1602 por I2C: autodetectar 0x27/0x3F; exponer `lcd.begin(cols,rows)` y helpers.
8) Sensores HC-SR04: medir con timeouts, filtrar outliers (mediana o EMA).
9) Arquitectura: separar **HAL** (`src/hal/*`) de **tareas** (`src/tasks/*`). Proveer interfaces limpias.
10) **Nunca** asumas acceso a internet. Trabaja **solo** con los archivos del repo (RAG local).
11) Entregables de cada cambio: 
    - código compilable (PlatformIO env: `uno_r4_wifi`), 
    - pruebas mínimas (si aplican), 
    - actualización de docs (`CHANGELOG.md`, `docs/`),
    - comandos para reproducir (`pio ...`). 
12) Si una instrucción del usuario contradice el mapa de pines o la energía, solicita confirmación y propone el parche mínimo seguro.
13) No ejecutes tareas en background; entrega todo en la respuesta actual. Si algo queda incompleto, explícitalo.
```

### 3.1 *Retrieval* (RAG) para el LLM
- **Context Pack** (carpeta `context/`): incluye en **texto** o **Markdown**:
  - `docs/README_GuitarBOT_MASTER.md` (prioridad **alta**).
  - Tablas de pines del README y `config.h`.
  - Partes relevantes del PDF de esquema y de la semidocumentación (extracción a `.md`).
  - `platformio.ini` y estructura del repo.
- **Chunking**: 800–1200 tokens por chunk, solapamiento 100–150 tokens.
- **Metadatos por chunk**: `{"file": "...", "section": "...", "version": "YYYY-MM-DD"}`.
- **Política de refresco**: reindexar si cambia `config.h` o README_MASTER.

### 3.2 Plantillas de *prompts* de trabajo
**A) Implementación HAL (motores):**
```
Objetivo: implementar TB6600 (STEP/DIR) para UNO R4 con AccelStepper.
Entradas: config.h, firma pública HAL deseada (proveer header), dependencias en platformio.ini.
Salidas:
  - src/hal/motors_tb6600.hpp/.cpp con init(), setSpeed(), moveTo(), run().
  - ejemplo en main.cpp que mueva ambos ejes 1000 pasos ida/vuelta.
Criterios:
  - sin delay(); usar run() en loop.
  - pinout según config.h.
  - logs #ifdef DEBUG.
  - compila en env: uno_r4_wifi.
```

**B) Lectura robusta HC-SR04:**
```
Implementa src/hal/ultrasonics.* con media de N lecturas y timeout.
API: begin(), read_cm(idx), read_cm_filtered(idx).
Pruebas: simular eco ausente (timeout) y devolver NAN/valor sentinel.
```

**C) LEDs & estados:**
```
Implementa leds_ws2812.* con setMode(IDLE|MOVING|ERROR), brillo limite 60.
Efectos sin bloqueos (millis) y ejemplo en main.cpp.
```

**D) DFPlayer & LCD:**
```
DFPlayer: init condicional #ifdef DISABLE_DFPLAYER_AT_BOOT; reproducir track 1.
LCD: helpers printLine(row,msg), progress(0..1).
```

**E) FSM de navegación:**
```
Estados: IDLE -> ARM -> MOVE -> STOP -> ERROR.
Condiciones: distancias de US, comandos serie 'g','s','e'.
```

---

## 4) Calidad y *gates* de CI (local o remota)
- **Compila**: `pio run -e uno_r4_wifi` sin warnings nuevos.
- **Formato**: `clang-format -i` (regla LLVM).
- **Revisión**: PR con diff legible y justificación (máx 200 líneas por PR si es posible).
- **Pruebas en hardware**: checklist simple en `docs/testcases.md` (mediciones US, LEDs, motores).

---

## 5) Seguridad y energía
- **GND común** obligatorio.
- **TB6600**: fija corriente a 0.8 A/fase (modelo de motor) antes de pruebas.
- **WS2812B**: resistor serie 220–470 Ω y electrolítico ≥ 470 µF al inicio.
- **DFPlayer**: si conectado a D0/D1, desconectar durante upload o activar `DISABLE_DFPLAYER_AT_BOOT`.

---

## 6) Convenciones de repo
- **Commits**: Conventional Commits (`feat:`, `fix:`, `docs:`, `refactor:`, `chore:`).
- **Versionado**: `CHANGELOG.md` + etiqueta semántica (x.y.z) por release.
- **Documentación viva**: cambios en pines/energía → actualizar README_MASTER y `config.h` **en el mismo commit**.

---

## 7) Primeros comandos (receta *copy-paste*)
```bash
# 1) Clonar
git clone <url> guitarbot && cd guitarbot

# 2) Abrir firmware y resolver deps
cd firmware
pio run -e uno_r4_wifi

# 3) Subir y monitorear
pio upload -e uno_r4_wifi
pio device monitor -b 115200

# 4) Formatear antes del PR
bash ../tools/format.sh
```

---

## 8) Checklist para abrir un PR asistido por LLM
- [ ] ¿El LLM incluyó referencias a `config.h` y a README_MASTER?
- [ ] ¿El código compila en `uno_r4_wifi`?
- [ ] ¿No hay `delay()` bloqueantes?
- [ ] ¿DFPlayer condicionado por `DISABLE_DFPLAYER_AT_BOOT`?
- [ ] ¿Se actualizaron docs si cambió pinout o energía?
- [ ] ¿Se incluyeron comandos para reproducir el resultado?

---

**Fin de la instrucción.** 
