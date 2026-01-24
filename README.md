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

# Build it yourself
## PCB
Order the PCB from the gerberfiles in [this zip](pcb/production/Attention-Seeker.zip). I would recommend black and the CAD files are designed for a 1.6mm thick pcb.

## Parts
Get/Order all parts from the [BOM](BOM.csv). I ordered them from reichelt and aliexpress and have included links for them. Watch out when buying the esp32 devkit, as there are countless versions and some even claim to be something different than they are.

## Soldering
First solder all smd components on the back, then the devkit and buzzer. Now you can solder on the screen based on your screen you might have to remove the sd card slot or cover it in kapton tape to be sure. I didnt have to shorten any pins, but you might have to. At last you can solder the LED on, but be very careful it snaps easily.

## Case
Print both the [top](case/case-top.3mf) and [bottom](case/case-bottom.3mf). If you dont have a cover for the rotary encoder you can also print [this](case/knob.3mf).

Place the pcb in the bottom and press fit the top on.

# Companion App
## Setup
Download [this folder](software/non-embedded/). 
Install the dependencys (if you want in a virtual environment)

`pip install -r requirements.txt`

Then just start the app
`python3 main.py`

## Device selection
Press refresh and it should automatically select your esp32. If not go through the drop down and select it manually.

## Time syncing
Press Sync Time

## Notifications
It will automatically send notifications to the device, but you can test it yourself. Press on the arrow to the left to extend the menu. Enter all your test informations and press Send JSON Notification. It should show up on the device and play a tone.