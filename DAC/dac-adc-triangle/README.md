# DAC Triangle Waveform

## Objective

Generate Triangle Waveform using DAC1 (GPIO25) and visualize in serialplotter(Arduino IDE)/pyserial with matplotlib.

## Hardware Used
- ESP32 Dev Board
- Bread Board
- Jumper

## Implementation

Connect DAC1(GPIO25) and ADC1_CHANNEL_6(GPIO34) in Bread Board with Jumper.

Approach: The DAC output a triangle waveform by increment/decrement values 0 - 255 and ADC read the voltage from DAC and show in serialplotter/pyserial.
