# <img src="https://github.com/Almarch/tamaR/blob/main/inst/www/icon.png" alt="TaMaGoTcHi" width="35"/> A Tamagotchi emulator for ESP chips × Arduino IDE

<img src="https://user-images.githubusercontent.com/13364928/208096173-751cd1a7-0d5d-4028-bfaf-60bac058f4a5.jpg" width="800px" />

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
            <th>Component</th>
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
            <td>D22</td>
        </tr>
        <tr>
            <td>Screen SDA</td>
            <td>-</td>
            <td>D2</td>
            <td>-</td>
            <td>D21</td>
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

<p float="left">
  <img src="https://user-images.githubusercontent.com/13364928/208108606-a6d87cba-38c4-466b-8830-3f7be8aa5aea.jpg" width="400px" />
  <img src="https://github.com/Almarch/Nanogotchi/assets/13364928/e7c7467e-63ee-4f2b-943c-0d868cc7da14" width="400px" /> 
</p>

## Software

Clone this repository:
```
git clone https://github.com/Almarch/Nanogotchi.git
```

Arduino IDE must be available.

### Original P1 ROM

The emulation requires a ROM converted to 12 bits. Two options are available:

- If you have a working Java environment, you can use the original program from [ArduinoGotchi](https://github.com/GaryZ88/ArduinoGotchi). Place the `rom.bin` file in the project folder and execute:

```
cd Nanogotchi
java TamaRomConvert rom.bin
```

- If you have a working R environment and [tamaR](https://github.com/Almarch/tamaR) installed, you can export the 12 bits ROM into the project folder:

```
cd Nanogotchi
R -e "library(tamaR); o = Tama(); o\$GetROM() |> tamaR:::nb2hex(header = 'static const unsigned char g_program_b12[] PROGMEM = {') |> write('rom_12bit.h')"
```

Using tamaR it is possible to substitute P1 sprites by P2 ones adding `p2(o);`. Before switching sprites, you might have to erase the EEPROM. From the Arduino IDE, start a serial connection and type the command: `9` to erase a previously saved games state.

After the conversion step, a `rom_12bit.h` file should be available in the project folder.

### Conditionally compiled code

The code contains conditionally compiled parts. Depending on the target chip, comment the appropriate line between `#define ESP32` or `#define ESP8266` at the beginning of `Nanogotchi.ino`.

### Arduino IDE parameterization

Open the `.ino` file in Arduino IDE.

Add the lines into `File/Preferences/Additional Boards Manager URLs`:

```
https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
Then look for `esp8266` and/or `esp32` in `Tools/Board/Boards manager` and install the library.

Install `U8g2` library in `Tool/Manage Libraries`.

You might have to install the USB-to-serial drivers for the board chipset. For instance, for some ESP32 development boards:

- [CP2102 chipset](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads)
- [CH340 chipset](https://www.wch-ic.com/downloads/CH341SER_ZIP.html)
- [FT232 chipset](https://ftdichip.com/drivers/vcp-drivers/)

Restart the computer after drivers installation.

## Compilation & launch

When the environment is ready, select the appropriate board as for instance: `Tools/Board/esp32/ESP32 Dev Module`.

Select the port: `Tools/port/`, the port to which the board is connected should appear.

Finally, use the upload command: ➡️. Some ESP32 development boards require a reset from the board reset button during the "Connecting..." step.
