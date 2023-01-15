# A tamagotchi emulator for ESP8266 boards

![PXL_20221216_121018390_2](https://user-images.githubusercontent.com/13364928/208096173-751cd1a7-0d5d-4028-bfaf-60bac058f4a5.jpg)

## Synopsis

This fork of [ArduinoGotchi](https://github.com/GaryZ88/ArduinoGotchi) is adapted to ESP8266 boards. The emulator relies on [TamaLib](https://github.com/jcrona/tamalib).

Adaptation to ESP8266 is heavily inspired from [this other fork](https://github.com/anabolyc/Tamagotchi), however, it is designed to be compiled and uploaded in an [Arduino IDE](https://www.arduino.cc/en/software) framework.

The emulation is on real time. It allows saving the game (by pressing the 3 buttons simultaneously), afterward it will be automatically loaded.

/!\ EEPROM can only be written a limited number of times, use with parcimony.

## Hardware

You will need the following components:
  - An ESP8266 board, e.g. ESP8266 WeMos D1-mini
  - 1K resistor x 3
  - Push button x 3
  - Buzzer 3V
  - SSD1306 I2C OLED 128x64
  - Breadboard and wire

You may have to soldier the board to its pin grid.

Use the resistors as [pull-down resistors](https://en.wikipedia.org/wiki/Pull-up_resistor) for each of the push buttons (this is also well illustrated [here](https://github.com/GaryZ88/ArduinoGotchi)).

Connect the board as follows:

| Name           | Id   | PIN  |
|----------------|------|------|
| Screen SCK     | -    | D1   |
| Screen SDA     | -    | D2   |
| Buzzer         | 14   | D5   |
| Left Button    | 12   | D6   |
| Middle Button  | 13   | D7   |
| Right Button   | 15   | D8   |
| VDD            | -    | 3V3  |
| Ground         | -    | G    |

VDD is distributed to the push buttons and to the screen. More info about the setup can be accessed [here](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/).

ESP32 boards are not currently supported due to the lack of the tone function for ESP32.

## Software

The emulation requires a ROM converted to 12 bits as explained [here](https://github.com/GaryZ88/ArduinoGotchi).

Clone this repository:
```
git clone https://github.com/Almarch/ESPgotchi.git
```

Then open the .ino file in Arduino IDE. You will need to install the ESP8266 library as explained in [this tutorial](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ie/). It consists in adding the following line:
```
https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
into File/Preferences/Additional Boards Manager URLs ; then looking for "esp8266" in Tools/Board and install the library.

You will also need to install U8g2 library in Tool/Manage Libraries.

![PXL_20221216_131808264_4](https://user-images.githubusercontent.com/13364928/208108606-a6d87cba-38c4-466b-8830-3f7be8aa5aea.jpg)

When the environment is ready, select the appropriate board: Tools/Board/ESP8266 (X.X.X)/"LOLIN(WEMOS) D1 R2 & Mini"
And the port: Tools/port/. A single port should appear, corresponding to the one to which the board is connected via USB.

Finally, use the right arrow: "UPLOAD" and let the mystery of life begin  👾
