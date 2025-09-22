import serial
import re
import subprocess
import time
import psutil
from pynput.keyboard import Key, Controller

keyboard = Controller()

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

keymap = {
        '0': '0', '1': '1', '2': '2', '3': '3',
    '4': '4', '5': '5', '6': '6', '7': '7',
    '8': '8', '9': '9',
    'B': '-', 'C': '+',
     
    # special
    'D': Key.enter,     
    'A': Key.delete,
    '#': '=',
    '*': '.',     
}

def get_net_stats():
	result = subprocess.check_output(["vnstat", "--oneline", "-i", "wlp3s0"]).decode()
	parts = result.split(";")
	rx=parts[3]
	tx=parts[4]
	return f"Download : {rx:<6} Upload : {tx:<6}"

def get_sys_stat():
	cpu = psutil.cpu_percent(interval=0.5)
	ram = psutil.virtual_memory().percent
	return f"CPU :{cpu:>3.0f}%   RAM :{ram:>3.0f}%"
	

last_vol = 50

def set_volume(percent):
    percent = max(0, min(100, percent))
    subprocess.run(["pactl", "set-sink-volume", "@DEFAULT_SINK@", f"{percent}%"])
    print(f"Volume System set ke {percent}%")

while True:
    line = ser.readline().decode(errors='ignore').strip()
    if not line:
        stats=get_sys_stat() +" "+ get_net_stats()
        ser.write((stats + "\n").encode())
        time.sleep(1)
        continue    
    print("Dari ESP:", line)

    if line.startswith("KEY:"):
        val=line.split(":")[1]
        if val in keymap:
            key=keymap[val]
            keyboard.press(key)
            keyboard.release(key)

    elif line.startswith("ENC:"):
        match=re.match(r"ENC:(\d+)", line)
        if match:
            vol = int(match.group(1))
            if vol != last_vol:
                set_volume(vol)
                last_vol=vol

    elif line.startswith("SW:PRESS"):
        keyboard.press(Key.media_play_pause)
        keyboard.release(Key.media_play_pause)

    time.sleep(0.01)
