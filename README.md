# NanoGotchi: A Tamagotchi emulator for ESP chips × Arduino IDE

![PXL_20221216_121018390_2](https://user-images.githubusercontent.com/13364928/208096173-751cd1a7-0d5d-4028-bfaf-60bac058f4a5.jpg)

## <img src="https://github.com/Almarch/tamaR/blob/main/inst/www/icon.png" alt="TaMaGoTcHi" width="35"/> Synopsis

[TamaLIB](https://github.com/jcrona/tamalib) is an hardware-agnostic Original P1 Tamagotchi emulator.

It has been optimized and restructured to fit the [Arduino Uno](https://github.com/GaryZ88/ArduinoGotchi) board. The Arduino environment comes with an [IDE](https://www.arduino.cc/en/software) designed for ease of use. However, the Arduino Uno board, powered by an ATmega328P chip, is not powerful enough to follow up the real-time emulation.

From the Arduino fork, TamaLIB has been adapted to [ESP8266](https://github.com/anabolyc/Tamagotchi) and [ESP32](https://github.com/RBEGamer/TamagotchiESP32), both chips being powerful enough for a real-time emulation. These two ESP forks rely on a different IDE.

The goal of this fork is to combine the beginner-friendliness of the Arduino IDE and the performance of the ESP chips. 

An extra functionality is added to the original gameplay: the game state can be saved by a simultaneous click on both 3 buttons (right, middle, left). EEPROM can only be written a limited number of times, this functionality should be used with parcimony.

## Hardware

### List of components

  - ESP board, e.g. ESP8266 WeMos D1-mini
  - 1K resistor x 3
  - Push button x 3
  - Buzzer 3V
  - SSD1306 I2C OLED 128x64
  - Breadboard and wire

### Assembling

The board PINs have to be connected to the components.

| Name           | Id   | ESP8266 Lolin (WeMos) D1-mini PIN |
|----------------|------|------|
| Screen SCK     | -    | D1   |
| Screen SDA     | -    | D2   |
| Buzzer         | 14   | D5   |
| Left Button    | 12   | D6   |
| Middle Button  | 13   | D7   |
| Right Button   | 15   | D8   |
| VDD            | -    | 3V3  |
| Ground         | -    | G    |

More info on the PIN mapping is available for common [ESP8266](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/) and [ESP32](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/) boards.

VDD is distributed to the push buttons and to the screen. The resistors are used as pull-down.

### Example schema

![esp32_schema](https://github.com/RBEGamer/TamagotchiESP32/blob/main/hardware/TamagotchiESP32_schematic_Steckplatine.png)

## Software

Clone this repository:
```
git clone https://github.com/Almarch/Nanogotchi.git
```

Arduino IDE must be available.

### Original P1 ROM

The emulation requires a ROM converted to 12 bits.

Put the `rom.bin` file in the project folder, i.e. `/Nanogotchi/`. Run the below commands, if success, it will generate a file named `rom_12bit.h` in the project folder:

```
cd Nanogotchi
java TamaRomConvert rom.bin
```

Then open the .ino file in Arduino IDE. You will then need to install specific libraries.

The code contains conditionally compiled parts. Depending on the target chip, comment the appropriate line between `#define ESP32` and `#define ESP8266` at the beginning of `Nanogotchi.ino`.

### ESP8266

Add the lines into `File/Preferences/Additional Boards Manager URLs`:

```
https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
Then look for `esp8266` in `Tools/Board/Boards manager` and install the library.

You also need to install `U8g2` library in `Tool/Manage Libraries`.

When the environment is ready, select the appropriate board, for instance: `Tools/Board/ESP8266 (X.X.X)/"LOLIN(WEMOS) D1 R2 & Mini`.
Then select the port: `Tools/port/`, the port to which the board is connected should appear.

![PXL_20221216_131808264_4](https://user-images.githubusercontent.com/13364928/208108606-a6d87cba-38c4-466b-8830-3f7be8aa5aea.jpg)

## Launch

Finally, use the upload command: ➡️ and let the mystery of life begin  👾
