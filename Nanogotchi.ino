/*
 * ArduinoGotchi - A real Tamagotchi emulator for Arduino UNO
 *
 * Copyright (C) 2022 Gary Kwok
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <U8g2lib.h>
#include <Wire.h>
#include "tamalib.h"
#include "hw.h"
#include "bitmaps.h"
#include "hardcoded_state.h"
#include "savestate.h"

//#define ESP8266
#define ESP32

/***** U8g2 SSD1306 Library Setting *****/
#define DISPLAY_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
/****************************************/

/***** Tama Setting and Features *****/
#define TAMA_DISPLAY_FRAMERATE  6
#define ENABLE_TAMA_SOUND
#define ENABLE_REAL_TIME
#define ENABLE_SAVE_STATUS
//#define AUTO_SAVE_MINUTES 60    // Auto save for every hour (to preserve EEPROM lifespan)
#define ENABLE_LOAD_STATE_FROM_EEPROM 
//#define ENABLE_DUMP_STATE_TO_SERIAL_WHEN_START
#define ENABLE_SERIAL_DUMP
//#define ENABLE_SERIAL_DEBUG_INPUT
//#define ENABLE_LOAD_HARCODED_STATE_WHEN_START
/***************************/

/***** Set display orientation, U8G2_MIRROR_VERTICAL is not supported *****/
#define U8G2_LAYOUT_NORMAL
//#define U8G2_LAYOUT_ROTATE_180
//#define U8G2_LAYOUT_MIRROR
/**************************************************************************/

#ifdef U8G2_LAYOUT_NORMAL
U8G2_SSD1306_128X64_NONAME_2_HW_I2C display(U8G2_R0);
#endif

#ifdef U8G2_LAYOUT_ROTATE_180
U8G2_SSD1306_128X64_NONAME_2_HW_I2C display(U8G2_R2);
#endif

#ifdef U8G2_LAYOUT_MIRROR
U8G2_SSD1306_128X64_NONAME_2_HW_I2C display(U8G2_MIRROR);
#endif

#ifdef ESP32
#define PIN_BTN_L 18
#define PIN_BTN_M 19
#define PIN_BTN_R 23
#define PIN_BUZZER 15
#endif

#ifdef ESP8266
#define PIN_BTN_L 12 // D6
#define PIN_BTN_M 13 // D7
#define PIN_BTN_R 15 // D8
#define PIN_BUZZER 14 // D5
#endif

/**** TamaLib Specific Variables ****/
static uint16_t current_freq = 0; 
static bool_t matrix_buffer[LCD_HEIGHT][LCD_WIDTH/8] = {{0}};
static byte runOnceBool = 0;
static bool_t icon_buffer[ICON_NUM] = {0};
static cpu_state_t cpuState;
static unsigned long lastSaveTimestamp = 0;
/************************************/

static void hal_halt(void) {
  //Serial.println("Halt!");
}

static void hal_log(log_level_t level, char *buff, ...) {
  Serial.println(buff); 
}

static void hal_sleep_until(timestamp_t ts) {
  #ifdef ENABLE_REAL_TIME
  int32_t remaining = (int32_t) (ts - hal_get_timestamp());
  if (remaining > 0) {
    delayMicroseconds(1);
    delay(1);
  }
  #endif
}

static timestamp_t hal_get_timestamp(void) {
  return millis() * 1000;
}

static void hal_update_screen(void) {
  displayTama();
} 

static void hal_set_lcd_matrix(u8_t x, u8_t y, bool_t val) {
  uint8_t mask;
  if (val) {
    mask = 0b10000000 >> (x % 8);
    matrix_buffer[y][x/8] = matrix_buffer[y][x/8] | mask;   
  } else { 
    mask = 0b01111111;
    for(byte i=0;i<(x % 8);i++) {
      mask = (mask >> 1) | 0b10000000;
    }
    matrix_buffer[y][x/8] = matrix_buffer[y][x/8] & mask;  
  }
}

static void hal_set_lcd_icon(u8_t icon, bool_t val) {
  icon_buffer[icon] = val;
}

static void hal_set_frequency(u32_t freq) {
  current_freq = freq;
}

static void hal_play_frequency(bool_t en) {
#ifdef ENABLE_TAMA_SOUND 
  if (en) {
    tone(PIN_BUZZER, current_freq); 
  } else {
    noTone(PIN_BUZZER); 
  }
#endif  
}

static bool_t button4state = 0;

static int hal_handler(void) {
#ifdef ENABLE_SERIAL_DUMP 
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    Serial.println(incomingByte, DEC);
    if (incomingByte==48) {          // 0
      dumpStateToSerial();
    } else if(incomingByte==57) {    // 9
      eraseStateFromEEPROM();
    } 
  }
#endif
#ifdef ENABLE_SERIAL_DEBUG_INPUT 
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    Serial.println(incomingByte, DEC);
    if (incomingByte==49) {  // 1
      hw_set_button(BTN_LEFT, BTN_STATE_PRESSED );
    } else if (incomingByte==52) {  // 4 which is above 1 on a pad
      hw_set_button(BTN_LEFT, BTN_STATE_RELEASED );
    } else if (incomingByte==50) {  // 2
      hw_set_button(BTN_MIDDLE, BTN_STATE_PRESSED );
    } else if (incomingByte==53) {  // 5 which is above 2 on a pad
      hw_set_button(BTN_MIDDLE, BTN_STATE_RELEASED );
    } else if (incomingByte==51) {  // 3
      hw_set_button(BTN_RIGHT, BTN_STATE_PRESSED );
    } else if (incomingByte==54) {  // 6 which is above 3 on a pad
      hw_set_button(BTN_RIGHT, BTN_STATE_RELEASED );
    }  
  } 
#else  
  if (digitalRead(PIN_BTN_L) == HIGH) {
    hw_set_button(BTN_LEFT, BTN_STATE_PRESSED );
  } else {
    hw_set_button(BTN_LEFT, BTN_STATE_RELEASED );
  }
  if (digitalRead(PIN_BTN_M) == HIGH) {
    hw_set_button(BTN_MIDDLE, BTN_STATE_PRESSED );
  } else {
    hw_set_button(BTN_MIDDLE, BTN_STATE_RELEASED );
  }
  if (digitalRead(PIN_BTN_R) == HIGH) {
    hw_set_button(BTN_RIGHT, BTN_STATE_PRESSED );
  } else {
    hw_set_button(BTN_RIGHT, BTN_STATE_RELEASED );
  }
  #ifdef ENABLE_SAVE_STATUS 
    if (digitalRead(PIN_BTN_L) == HIGH && digitalRead(PIN_BTN_M) == HIGH && digitalRead(PIN_BTN_R) == HIGH) {
      if (button4state==0) {

        saveStateToEEPROM(&cpuState);
        
        noTone(PIN_BUZZER);
        tone(PIN_BUZZER, 700,100);
        delay(120);
        noTone(PIN_BUZZER);
        tone(PIN_BUZZER, 880,100);
        delay(120);
        noTone(PIN_BUZZER);
        tone(PIN_BUZZER, 1175,100);
        delay(120);
        noTone(PIN_BUZZER);
      }
      button4state = 1;
    } else {
      button4state = 0;
    }
  #endif

#endif  
  return 0;
}

static hal_t hal = {
  .halt = &hal_halt,
  .log = &hal_log,
  .sleep_until = &hal_sleep_until,
  .get_timestamp = &hal_get_timestamp,
  .update_screen = &hal_update_screen,
  .set_lcd_matrix = &hal_set_lcd_matrix,
  .set_lcd_icon = &hal_set_lcd_icon,
  .set_frequency = &hal_set_frequency,
  .play_frequency = &hal_play_frequency,
  .handler = &hal_handler,
};

/*
void drawTriangle(uint8_t x, uint8_t y) {
  //display.drawLine(x,y,x+6,y);
  display.drawLine(x+1,y+1,x+5,y+1);
  display.drawLine(x+2,y+2,x+4,y+2); 
  display.drawLine(x+3,y+3,x+3,y+3); 
}
*/

void drawTamaRow(uint8_t tamaLCD_y, uint8_t ActualLCD_y, uint8_t thick) {
  uint8_t i;
  for (i = 0; i < LCD_WIDTH; i++) {
    uint8_t mask = 0b10000000;
    mask = mask >> (i % 8);
    if ( (matrix_buffer[tamaLCD_y][i/8] & mask) != 0) {
        display.drawBox(i+i+i+16,ActualLCD_y,2,thick);
    }
  }
}

void drawTamaSelection(uint8_t y) {
  uint8_t i;
  for(i=0;i<8;i++) {
    if (icon_buffer[i]) {
      // drawTriangle(i*16+5,y);
      display.drawXBMP(i*16+4,y+6,8,8,bitmaps+i*8);
    }
  }
}

void displayTama() {
  uint8_t j;
  display.firstPage();
#ifdef U8G2_LAYOUT_ROTATE_180
    drawTamaSelection(49);
    display.nextPage();

    for (j = 11; j < LCD_HEIGHT; j++) {
      drawTamaRow(j,j+j+j,2);
    }
    display.nextPage();

    for (j = 5; j <= 10; j++) {
      if (j==5) {
        drawTamaRow(j,j+j+j+1,1);
      } else {
        drawTamaRow(j,j+j+j,2);
      }
    }
    display.nextPage();

    for (j = 0; j <= 5; j++) {
      if (j==5) {
        drawTamaRow(j,j+j+j,1);
      } else {
        drawTamaRow(j,j+j+j,2);
      }    
    }
    display.nextPage();
#else
    for (j = 0; j < LCD_HEIGHT; j++) {
      if (j!=5) drawTamaRow(j,j+j+j,2);
      if (j==5) {
         drawTamaRow(j,j+j+j,1);
         display.nextPage();
         drawTamaRow(j,j+j+j+1,1);
      }
      if (j==10) display.nextPage();
    }
    display.nextPage();
    drawTamaSelection(49);
    display.nextPage();
#endif  
}

#if defined(ENABLE_DUMP_STATE_TO_SERIAL_WHEN_START) || defined(ENABLE_SERIAL_DUMP)
void dumpStateToSerial() {
  uint16_t i, count=0;
  char tmp[10];
  cpu_get_state(&cpuState);
  u4_t *memTemp = cpuState.memory;
  uint8_t *cpuS = (uint8_t *)&cpuState;

  Serial.println("");
  Serial.println("static const uint8_t hardcodedState[] PROGMEM = {");
  for(i=0;i<sizeof(cpu_state_t);i++,count++) {
    sprintf(tmp, "0x%02X,", cpuS[i]);
    Serial.print(tmp);
    if ((count % 16)==15) Serial.println("");
  }
  for (i = 0; i < MEMORY_SIZE; i++,count++) {
    sprintf(tmp, "0x%02X,",memTemp[i]);
    Serial.print(tmp);
    if ((count % 16)==15) Serial.println("");
  }  
  Serial.println("};");
/*
  Serial.println("");
  Serial.println("static const uint8_t bitmaps[] PROGMEM = {");
  for(i=0;i<144;i++) {
    sprintf(tmp, "0x%02X,", bitmaps_raw[i]);
    Serial.print(tmp);
    if ((i % 18)==17) Serial.println("");
  }
  Serial.println("};");  */
} 
#endif

uint8_t reverseBits(uint8_t num) {
    uint8_t reverse_num = 0;
    uint8_t i;
    for (i = 0; i < 8; i++) {
        if((num & (1 << i)))
           reverse_num |= 1 << ((8 - 1) - i);  
   }
   return reverse_num;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BTN_L, INPUT);
  pinMode(PIN_BTN_M, INPUT);
  pinMode(PIN_BTN_R, INPUT);
  //pinMode(PIN_BTN_SAVE, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  display.setI2CAddress(DISPLAY_I2C_ADDRESS * 2);  // required if display does not use default address of 0x3C
  display.begin();  // initialize U8g2 graphics library for selected display module

  tamalib_register_hal(&hal);
  tamalib_set_framerate(TAMA_DISPLAY_FRAMERATE);
  tamalib_init(1000000);

#if defined(ENABLE_SAVE_STATUS) || defined(AUTO_SAVE_MINUTES) || defined(ENABLE_LOAD_STATE_FROM_EEPROM)
 initEEPROM();
#endif

#ifdef ENABLE_LOAD_STATE_FROM_EEPROM
  if (validEEPROM())
  {
    loadStateFromEEPROM(&cpuState);
  } else {
    Serial.println(F("No magic number in state, skipping state restore"));
  }
#elif defined(ENABLE_LOAD_HARCODED_STATE_WHEN_START)
  loadHardcodedState(&cpuState);
#endif

/*
  int i;
  for(i=0;i<(18*8);i++) {
    bitmaps_raw[i]= reverseBits(bitmaps_raw[i]);
  }
*/

#ifdef ENABLE_DUMP_STATE_TO_SERIAL_WHEN_START
  dumpStateToSerial();
#endif 

}

void loop() {
  tamalib_mainloop_step_by_step();
#ifdef AUTO_SAVE_MINUTES    
  if ((millis() - lastSaveTimestamp) > (AUTO_SAVE_MINUTES * 60 * 1000)) {
    lastSaveTimestamp = millis();
    saveStateToEEPROM(&cpuState);
  }
#endif  
}
