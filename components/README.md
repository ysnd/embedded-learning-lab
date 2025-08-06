# Components
This folder is for learning how to create libraries for ESP-IDF using several sensor i have.

---

## Structure
- `../[lib]/` Contains the libraries I create for my own components/sensors.
- `../example/` Contains example ESP-IDF projects that demonstrate how to use the libraries.
- `../manual-coded/` Contains projects components/sensors I have, written manually without using library. Used as a comparsion with modular.

## Checklist

| Components | Manual Coded | Library Module | Example Project |
|------------|--------------|----------------|-----------------|
| IR-Obstacle| [x] | [x] | [x] |
| PIR Sensor | [x] | [x] | [x] |
| Ultrasonic | [x] | [x] | [x] |
| Servo | [] | [] | [] |
| DHT11 | [] | [] | [] |
| DS18B20 | [] | [] | [] |

## Purpose
- Practice designing reuseable drivers using c and ESP-IDF
- Compare manual code vs modular components
- Test each module through example usage
