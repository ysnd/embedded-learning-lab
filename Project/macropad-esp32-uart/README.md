# ESP32 Media Controller with Keypad & Rotary Encoder

## Description
This project using **ESP32** as input device:
- **Keypad** works as Numpad or shortcut buttons
- **Rotary Encoder** control volume & media play/pause 
- **OLED SSD1306 I2C** display information such as CPU usage, RAM usage, and network usage upload/download.

Communication between ESP32 and the host via **UART**

## Feature
- Keypad -> Numpad / custom shortcut
- Rotary Encoder -> Volume control, media play/pause
- OLED SSD1306 display CPU, RAM, Network usage via vnstat

## Hardware
- ESP32
- Keypad matrix
- Rotary Encoder
- OLED SSD1306
- Breadboard & jumper

## Software
- ESP-IDF 5.x
- Python 3.x
- Library: 
    - `psutil` : CPU & RAM monitoring
    - `pynput` : Media key control
- System tools :
    - `vnstat` : Network usage monitoring (Linux only)

## Pin Wiring
|---|---|
| PIN | ESP32 |
|---|---|
| `CLK_PIN` | 32 |
| `DT_PIN` | 33 |
| `SW_PIN` | 25 |
| `R1` | 17 |
| `R2` | 19 |
| `R3` | 18 |
| `R4` | 5 |
| `C1` | 15 |
| `C2` | 4 |
| `C3` | 2 |
| `C4` | 0 |
| `SDA` | 21 |
| `SCL` | 22 |

## How to Run
1. Flash ESP32
2. Run python script