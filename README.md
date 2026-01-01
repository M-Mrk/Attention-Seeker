# Attention-Seeker
## Overview
A multifunctional desk peripheral designed to display and alert you to notifications.

Attention-Seeker receives notifications from a companion app and displays them on its 2.8" color screen, with optional visual and audible alerts. It also features a rotary encoder and three mechanical keyboard switches for user input (no touch!).

In addition to notification-related features, it functions as a **clock**, **desk light**, and **macropad**.

![Front View](pcb/pictures/front.png) ![Semi-Assembled](case/complete.png)

### Why?
I often missed important notifications while zoning out or concentrating and also needed a desk light, so I built this.

### How?
- 2.8" TFT screen
- 3 mechanical keyboard switches
- Rotary encoder
- Rigid LED filament
- Buzzer
- ESP32-S3 DevKitC N16R8 CP2102
- Python companion app to run on a PC

### Example Use Cases
- **Notification:** Attention-Seeker buzzes, blinks, and displays the message.
- **Desk Light:** Toggle and dim the built-in LED for illumination.



## Features

### Notifications
Displays incoming notifications sent by the companion app with optional blinking or buzzing.

<img src="images/notification.jpeg" width="512">

### Clock
- Displays time
- Synced via the companion app

<img src="images/time.jpeg" width="512">

### Macropad
- 6 hotkeys

<img src="images/hotkey.jpeg" width="512">

### Navigation
- Mechanical keyboard switches for on-page actions
- Rotary encoder for switching between pages

### Other Features
- Dimmable desk light
- Buzzer for alerts

## Hardware

### MCU
The ESP32-S3 might be overkill but is ideal for handling the screen's pixel density, enabling fast page switching. Almost all pins are utilized, as the SPI interface for the screen consumes the majority of them.

### Lighting

#### LED Filament
The 3V filament draws about 100mA, requiring a 5V-to-3.3V LDO for power. An N-channel MOSFET drives the negative side of the LED filament.

#### Backlight
The backlight's current draw was uncertain during PCB design, so a transistor was added between it and the MCU for safety. This transistor is pulled high and can only be pulled low by the MCU.

### Other Components
All remaining components (switches, encoder, buzzer) are directly connected to the MCU.
