# DAC Sine Waveform

## Objective

Generate Sine Waveform using DAC1 (GPIO25) and visualize in serialplotter(Arduino IDE)/pyserial with matplotlib.

## Hardware Used
- ESP32 Dev Board
- Bread Board
- Jumper

## Implementation

Connect DAC1(GPIO25) and ADC1_CHANNEL_6(GPIO34) in Bread Board with Jumper.

Approach: The DAC output a Sine waveform generated from lookup table and ADC read the voltage from DAC and show in serialplotter/pyserial.
