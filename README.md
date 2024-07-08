# <img src="https://github.com/Almarch/tamaR/blob/main/inst/www/icon.png" alt="TaMaGoTcHi" width="35"/> A Tamagotchi emulator for ESP chips × Arduino IDE

![PXL_20221216_121018390_2](https://user-images.githubusercontent.com/13364928/208096173-751cd1a7-0d5d-4028-bfaf-60bac058f4a5.jpg)

[TamaLIB](https://github.com/jcrona/tamalib) is a hardware-agnostic original P1 Tamagotchi emulator.

It has been optimized and restructured to fit the [Arduino Uno](https://github.com/GaryZ88/ArduinoGotchi) board. Arduino environment comes with an [IDE](https://www.arduino.cc/en/software) designed for ease of use. However, the Arduino Uno board, powered by an ATmega328P chip, is not fast enough to follow up the real-time emulation.

From the Arduino fork, TamaLIB has been adapted to [ESP8266](https://github.com/anabolyc/Tamagotchi) and [ESP32](https://github.com/RBEGamer/TamagotchiESP32), both chips being fast enough for a real-time emulation. These two ESP forks are set on a different IDE.

The goal of the present fork is to bring together the beginner-friendliness of the Arduino IDE and the performance of the ESP chips. It relies on the developments from all previously listed projects.

The game icons have been copied from [MCUgotchi](https://github.com/jcrona/mcugotchi), a Tamalib implementation targetting STM32FO/STM32LO boards. This choice of aesthetics is motivated by there resemblance to the original toy ones.

An extra functionality is added to the original gameplay: the game state can be saved by a simultaneous click on both 3 buttons (right, middle, left). EEPROM can only be written a limited number of times, this functionality should be used with parcimony.

## Hardware

### List of components

  - An ESP8266 or ESP32 board
  - 1K resistor x 3
  - Push button x 3
  - Buzzer 3V
  - SSD1306 I2C OLED 128x64
  - Breadboard and wire

### Assembling

The board PINs have to be connected to the components.

<table>
    <thead>
        <tr>
            <th></th>
            <th colspan=2>ESP8266</th>
            <th colspan=2>ESP32</th>
        </tr>
        <tr>
            <th>Name</th>
            <th>GPIO</th>
            <th>pinout</th>
            <th>GPIO</th>
            <th>pinout</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Screen SCK</td>
            <td>-</td>
            <td>D1</td>
            <td>-</td>
            <td>D21</td>
        </tr>
        <tr>
            <td>Screen SDA</td>
            <td>-</td>
            <td>D2</td>
            <td>-</td>
            <td>D22</td>
        </tr>
        <tr>
            <td>Buzzer</td>
            <td>14</td>
            <td>D5</td>
            <td>15</td>
            <td>D15</td>
        </tr>
        <tr>
            <td>Left button</td>
            <td>12</td>
            <td>D6</td>
            <td>18</td>
            <td>D18</td>
        </tr>
        <tr>
            <td>Middle button</td>
            <td>13</td>
            <td>D7</td>
            <td>19</td>
            <td>D19</td>
        </tr>
        <tr>
            <td>Right button</td>
            <td>15</td>
            <td>D8</td>
            <td>23</td>
            <td>D23</td>
        </tr>
        <tr>
            <td>VDD</td>
            <td>-</td>
            <td>3V3</td>
            <td>-</td>
            <td>3V3</td>
        </tr>
        <tr>
            <td>Ground</td>
            <td>-</td>
            <td>G</td>
            <td>-</td>
            <td>G</td>
        </tr>
    </tbody>
</table>

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

### Conditionally compiled code

The code contains conditionally compiled parts. Depending on the target chip, comment the appropriate line between `#define ESP32` or `#define ESP8266` at the beginning of `Nanogotchi.ino`.

### Arduino IDE parameterization

Open the .ino file in Arduino IDE.

Add the lines into `File/Preferences/Additional Boards Manager URLs`:

```
https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
Then look for `esp8266` and/or `esp32` in `Tools/Board/Boards manager`, then install the library.

Install `U8g2` library in `Tool/Manage Libraries`.

You might have to install the USB-to-serial drivers for the board chipset. For instance, for some ESP32 development boards:

- [CP210x chipset](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads)

- [CH340 chipset](https://www.wch-ic.com/downloads/CH341SER_ZIP.html)

## Compilation & launch

When the environment is ready, select the appropriate board as for instance: `Tools/Board/ESP8266 (X.X.X)/"LOLIN(WEMOS) D1 R2 & Mini`.

Select the port: `Tools/port/`, the port to which the board is connected should appear.

![PXL_20221216_131808264_4](https://user-images.githubusercontent.com/13364928/208108606-a6d87cba-38c4-466b-8830-3f7be8aa5aea.jpg)

Finally, use the upload command: ➡️. Some ESP32 development boards require a reset (with the board reset button) during the "Connecting..." step.
