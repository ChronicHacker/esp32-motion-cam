# ESP32 Motion Camera 📸💥

Hacker-grade motion detection with the ESP32-CAM. When motion hits, this beast captures a snapshot, saves it to SD, and lets you review or delete it through a sleek web UI.

## Features
- PIR motion detection
- JPEG photo capture
- SD card logging with timestamps
- Web interface to view and delete photos
- NTP sync for filenames

## Hardware
- ESP32-CAM (OV2640)
- PIR Motion Sensor (e.g. HC-SR501)
- MicroSD Card (formatted FAT32)
- Optional: Flash LED on GPIO 4

## Setup
1. Wire the PIR to GPIO 13.
2. Insert microSD.
3. Flash the sketch.
4. Open the IP from Serial Monitor.
5. Boom. Surveillance mode: ON.

## License
MIT — Built by [Chr0nicHacker](https://chronicHacker.com)
