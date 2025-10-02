# GuitarBOT — Documentación Técnica (Consolidada) -- (Auto generado) 

GuitarBOT es una base robótica con respaldo inclinado para sujetar y transportar una guitarra (~hasta 5 kg), con electrónica visible sobre acrílico y estructura en perfiles de aluminio. Incorpora tracción con motores paso a paso NEMA17 y señalización/UX con LCD y LEDs direccionables. Este README consolida la semidocumentación y define pendientes para llegar a un build reproducible.

---

## 1) Especificaciones clave
- **Peso a soportar:** hasta **5 kg**
- **Autonomía objetivo:** ~**2 h** de operación continua
- **Ruedas:** Ø **10 cm** (motrices), 1 × **castor** (giro 360°)
- **Estructura:** perfiles **20×40** aluminio + base de **acrílico 5 mm**
- **Dimensiones aproximadas (prototipo):**
  - Base: **30 cm** ancho × **19 cm** largo
  - Altura suelo→base: **6–7 cm** (ruedas Ø 8–10 cm + perfiles 20×40)
  - Respaldo principal: **55 cm**; secundario (castor): **40 cm**

## 2) Electrónica principal
- **MCU:** Arduino **UNO R4 WiFi Type‑C** (RA4M1 + ESP32‑S3)
- **Motores:** 2 × **NEMA 17** (OK42STH22‑0804AC, 0.8 A/fase, 1.8°)
- **Drivers:** 2 × **TB6600** (hasta 4 A, microstep 1/16)
- **Interfaz:** **LCD 1602 I²C**, 2 × **WS2812B** (segmentos/zonas)
- **Sensores:** 3 × **HC‑SR04** (TRIG/ECHO)
- **Audio:** **DFPlayer Mini** → **PAM8403** → 2 × bocinas 8 Ω / 0.5 W
- **Alimentación:** **LiPo 3S 11.1 V 4400 mAh 70C** → **Buck 6–32 V → 5 V** (DROK)

## 3) Definición de pines (final acordado)
> [!IMPORTANT]
> Corresponde al UNO R4 (5 V). Si usas D13 para WS2812B, considera el LED onboard.

| Subsistema    | Señal              | Arduino                   | Notas                                               |
| ------------- | ------------------ | ------------------------- | --------------------------------------------------- |
| LCD 1602 I²C  | SDA                | A4 (SDA)                  | Bus I²C                                             |
|               | SCL                | A5 (SCL)                  | Bus I²C                                             |
| HC-SR04 #1    | TRIG               | D10                       | 5V y GND comunes                                    |
|               | ECHO               | D11                       |                                                     |
| HC-SR04 #2    | TRIG               | D8                        |                                                     |
|               | ECHO               | D9                        |                                                     |
| HC-SR04 #3    | TRIG               | D6                        |                                                     |
|               | ECHO               | D7                        |                                                     |
| WS2812B #1    | DIN                | D12                       |                                                     |
| WS2812B #2    | DIN                | D13                       | LED onboard puede producir flicker                  |
| DFPlayer      | RX (módulo)        | RX (D0 Arduino)           | **DF TX → D0** (ocupa UART HW al programar)        |
|               | TX (módulo)        | TX (D1 Arduino)           | **DF RX ← D1**                                      |
| PAM8403       | IN.L / IN.R        | —                         | Tomar señal de **DAC_L/DAC_R** del DFPlayer        |
| TB6600 (izq.) | PUL− / DIR− / ENA− | Puentes GND               | Entradas opto con cátodo común                      |
|               | PUL+ / DIR+ / ENA+ | D2 / D3 / —               | ENA no usado (habilitado fijo)                      |
| TB6600 (der.) | PUL− / DIR− / ENA− | Puentes GND               |                                                     |
|               | PUL+ / DIR+ / ENA+ | D4 / D5 / —               | ENA no usado                                        |

## 4) Integración eléctrica
- **Topología:** GND común (MCU, sensores, audio y lógica de TB6600).
- **5 V lógicos:** Buck → 5 V para MCU/DFPlayer/PAM8403/WS2812B/LCD.
- **Potencia motores:** Entrada VM de cada TB6600 desde LiPo 3S (o fuente dedicada), **no** del rail 5 V.
- **WS2812B:** resistor serie 220–470 Ω en DIN + electrolítico ≥ 470 µF en inicio de tira.
- **DFPlayer:** usar UART HW (D0/D1) o migrar a SoftwareSerial para facilitar programación por USB.

## 5) Firmware — módulos sugeridos
- **Motores:** `AccelStepper` (modo DRIVER: STEP=PUL, DIR=DIR).
- **Ultrasonidos:** `NewPing` o medición con `pulseIn` con timeouts.
- **LEDs:** `Adafruit_NeoPixel` o `FastLED` (cuidar timing en D13).
- **DFPlayer:** `DFRobotDFPlayerMini`; volumen, EQ y playlist por carpetas.
- **LCD:** `LiquidCrystal_I2C` (dirección típica 0x27/0x3F).

## 6) Ensamble mecánico (resumen)
- Perfiles **20×40** para base y respaldo; agregar **castor** posterior para estabilidad.
- Base en **acrílico 5 mm**; organizar visibilidad de módulos y rutas limpias.
- Sujeción de guitarra: montura + pin + seguro superior; agregar cinta antideslizante en base.

## 7) Seguridad y pruebas
- **LiPo:** cargar sólo con cargador LiPo (B3V2), no dejar sin supervisión.
- **Corriente TB6600:** ajustar DIP a 0.8 A/fase nominal; verificar temperatura.
- **Smoke test:** probar rail de 5 V sin cargas → añadir subsistemas uno a uno.
- **Calibración HC‑SR04:** validar 2–400 cm con blancos reales (±3 mm teórico).

## 8) Pendientes / Inconsistencias a resolver
- **WS2812B en texto menciona “pin del ESP32”** pero la placa de control final es **UNO R4 (5 V)**. Mantener UNO R4 como MCU y remover referencias a ESP32 **o** justificar coexistencia (nivel lógico, librerías).
- **Alimentación en esquemático previo vs. doc:** el documento consolida **LiPo 3S 11.1 V**; el esquemático inicial mencionaba 24 V. Unificar a **3S + Buck 5 V** o documentar variante a 24 V.
- **Convertidor lógico bidireccional:** innecesario si todo es 5 V (UNO R4, HC‑SR04, WS2812B, TB6600 entradas). Mantenerlo sólo si entra un periférico **3.3 V**.
- **D13 para WS2812B:** posible flicker por LED onboard; considerar migrar a D6/D7/D8 si libera sensores.
- **ENA de TB6600:** si se deja fijo (puente), reflejarlo en diagramas y firmware (no controlar ENA por GPIO).
- **DFPlayer en D0/D1:** interfiere con USB durante carga de firmware; proponer jumper o SoftwareSerial.

## 9) Estructura sugerida del repo
```
/docs/  (PDFs, renders, planos)
/firmware/
  ├─ include/config.h
  └─ src/main.cpp
/hardware/
  ├─ wiring/ (esquemático, fotos de cableado)
  └─ mechanics/ (STL, DXF, perfiles)
/bom/BOM_GuitarBOT.csv
README.md  (este documento)
```

## 10) `config.h` de referencia (coincide con pines)
```cpp
#define PIN_I2C_SDA   A4
#define PIN_I2C_SCL   A5
#define US1_TRIG      10
#define US1_ECHO      11
#define US2_TRIG      8
#define US2_ECHO      9
#define US3_TRIG      6
#define US3_ECHO      7
#define LED1_DATA     12
#define LED2_DATA     13
#define DF_TO_ARDUINO_RX   0
#define DF_FROM_ARDUINO_TX 1
#define M1_PUL_PLUS   2
#define M1_DIR_PLUS   3
#define M2_PUL_PLUS   4
#define M2_DIR_PLUS   5
```

---

### Apéndice A — Referencias internas
- Fichas de partes, medidas, enlaces de compra y descripciones provienen de la semidocumentación adjunta (motores, TB6600, LiPo 3S, buck, LCD I²C, WS2812B, DFPlayer, PAM8403, ruedas).

