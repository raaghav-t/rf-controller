/* 
 * This file is part of the rf-controller (https://github.com/bnahill/rf-controller).
 * Copyright (c) 2022 Ben Nahill.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Adafruit_NeoPixel.h>


static constexpr uint8_t PIN_NRF_CE = 0;
static constexpr uint8_t PIN_NRF_NCS = 1;
static constexpr uint8_t PIN_NRF_SCK = 2;
static constexpr uint8_t PIN_NRF_COPI = 3;
static constexpr uint8_t PIN_NRF_CIPO = 4;

static constexpr uint8_t PIN_SW_UP = 9;
static constexpr uint8_t PIN_SW_DOWN = 10;
static constexpr uint8_t PIN_SW_LEFT = 11;
static constexpr uint8_t PIN_SW_RIGHT = 8;

static constexpr uint8_t PIN_SW_A = 7;
static constexpr uint8_t PIN_SW_B = 6;
static constexpr uint8_t PIN_SW_C = 5;

static constexpr uint8_t PIN_JOYX = 28;
static constexpr uint8_t PIN_JOYY = 29;
static constexpr uint8_t PIN_JOY_PUSH = 27;

static constexpr uint8_t PIN_CONF5 = 26;
static constexpr uint8_t PIN_CONF4 = 15;
static constexpr uint8_t PIN_CONF3 = 14;
static constexpr uint8_t PIN_CONF2 = 13;
static constexpr uint8_t PIN_CONF1 = 12;

static constexpr uint8_t PIN_RGBLED = 16;

// Single RGB LED
Adafruit_NeoPixel strip(1, PIN_RGBLED, NEO_GRB + NEO_KHZ800);

// Compact structure to convey the current input state
typedef struct {
  uint16_t joy_x; //!< Joystick X-axis range 0 (left) - 4095 (right)
  uint16_t joy_y; //!< Joystick Y-axis range 0 (down) - 4095 (up)
  uint8_t up: 1;
  uint8_t down: 1;
  uint8_t left: 1;
  uint8_t right: 1;
  uint8_t a: 1;
  uint8_t b: 1;
  uint8_t c: 1;
  uint8_t joy_push: 1;
} __attribute__((packed)) button_status_t;

static_assert(sizeof(button_status_t) == 5, "button_status_t should be 5B");

// This address can be anything as long as it matches on the receiver side
uint8_t const NRF_ADDRESS[6] = "00001";
RF24 radio(PIN_NRF_CE, PIN_NRF_NCS);

/**
 * @brief Read button input
 * 
 * @return button_status_t 
 */
static button_status_t read_buttons(){
  button_status_t stat = {0};
  
  stat.joy_x = analogRead(PIN_JOYX);
  stat.joy_y = 4096 - analogRead(PIN_JOYY);
  stat.up = 1 - digitalRead(PIN_SW_UP);
  stat.down = 1 - digitalRead(PIN_SW_DOWN);
  stat.left = 1 - digitalRead(PIN_SW_LEFT);
  stat.right = 1 - digitalRead(PIN_SW_RIGHT);
  stat.a = 1 - digitalRead(PIN_SW_A);
  stat.b = 1 - digitalRead(PIN_SW_B);
  stat.c = 1 - digitalRead(PIN_SW_C);
  stat.joy_push = 1 - digitalRead(PIN_JOY_PUSH);
  return stat;
}

static constexpr bool SILENT = true;

/**
 * @brief Compare two inputs of type button_status_t
 * 
 * @param a 
 * @param b 
 * @return true If equal (with analog inputs within some threshold)
 * @return false If not equal
 */
bool status_equal(button_status_t const &a, button_status_t const &b){
  if(a.up != b.up || a.down != b.down || a.left != b.left || a.right != b.right ||
     a.a != b.a || a.b != b.b || a.c != b.c || a.joy_push != b.joy_push){
    return false;
  }
  int32_t const THRESHOLD = 15;
  if(abs((int32_t)a.joy_x - (int32_t)b.joy_x) > THRESHOLD){
    return false;
  }

  if(abs((int32_t)a.joy_y - (int32_t)b.joy_y) > THRESHOLD){
    return false;
  }
  return true;
}

button_status_t last_status = {0};

void setup() {
  if(!SILENT) Serial.begin(9600);

  // Configure the SPI pins to do SPI
  gpio_set_function(PIN_NRF_COPI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_NRF_CIPO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_NRF_SCK, GPIO_FUNC_SPI);

  pinMode(PIN_SW_UP, INPUT_PULLUP);
  pinMode(PIN_SW_DOWN, INPUT_PULLUP);
  pinMode(PIN_SW_LEFT, INPUT_PULLUP);
  pinMode(PIN_SW_RIGHT, INPUT_PULLUP);
  pinMode(PIN_SW_A, INPUT_PULLUP);
  pinMode(PIN_SW_B, INPUT_PULLUP);
  pinMode(PIN_SW_C, INPUT_PULLUP);
  pinMode(PIN_JOYX, INPUT);
  pinMode(PIN_JOYY, INPUT);
  pinMode(PIN_JOY_PUSH, INPUT_PULLUP);

  pinMode(PIN_CONF1, INPUT_PULLDOWN);
  pinMode(PIN_CONF2, INPUT_PULLDOWN);
  pinMode(PIN_CONF3, INPUT_PULLDOWN);
  pinMode(PIN_CONF4, INPUT_PULLDOWN);
  pinMode(PIN_CONF5, INPUT_PULLDOWN);

  analogReadResolution(12);

  delay(1000);

  if (!radio.begin()) {
    if(!SILENT) Serial.println(F("radio hardware is not responding!!"));

    strip.begin();
    strip.setBrightness(50);
    strip.show();
    // Flash red/green if the radio fails to initialize
    for(uint32_t i = 0; i < 40u; i++){
      strip.setPixelColor(0, strip.Color(0, 255, 0));
      strip.show();
      delay(50);
      strip.setPixelColor(0, strip.Color(255, 0, 0));
      strip.show();
      delay(50);
    }
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
    while (1) {}  // hold in infinite loop
  }
  char buffer[64] = {0};
  if(!SILENT){
    snprintf(buffer, sizeof(buffer), "radio hardware initialized %u!!", radio.getChannel());
    Serial.println(buffer);
  }

  uint32_t channel = (digitalRead(PIN_CONF5) << 4) | (digitalRead(PIN_CONF4) << 3) |
                     (digitalRead(PIN_CONF3) << 2) | (digitalRead(PIN_CONF2) << 1) |
                     (digitalRead(PIN_CONF1) << 0);
  
  radio.setChannel(channel);
  if(!SILENT){
    snprintf(buffer, sizeof(buffer), "New channel %u!!", radio.getChannel());
    Serial.println(buffer);
  }
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(NRF_ADDRESS);
  radio.stopListening();

  strip.begin();
  strip.setBrightness(50);
  strip.show();

  for(uint32_t i = 0; i < 5u; i++){
    strip.setPixelColor(0, strip.Color(255, 255, 0));
    strip.show();
    delay(50);
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
    delay(50);
  }

  last_status = read_buttons();
}

static constexpr uint32_t MIN_UPDATE_RATE_MS = 100u;
static constexpr uint32_t LED_TIMEOUT_MS = 1000u;
static constexpr uint32_t IDLE_TIMEOUT_MS = 5u * 60u * 1000u;

static uint32_t last_change_ms = 0u;
static uint32_t last_update_ms = 0u;

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  //Serial.println("Line");
  char buffer[128] = {0};
  button_status_t const status = read_buttons();
  if(!status_equal(status, last_status)){
    if(!SILENT){
      snprintf(buffer, sizeof(buffer), "Joy X %u, Y %u, PB %lu!!", status.joy_x, status.joy_y, (uint32_t)status.joy_push);
      Serial.println(buffer);

      snprintf(buffer, sizeof(buffer), "%u %u %u %u %u %u %u",
              status.up, status.down, status.left, status.right,
              status.a, status.b, status.c);
      Serial.println(buffer);
    }
    strip.setPixelColor(0, strip.Color(status.joy_x / 16, status.joy_y / 16, 0));
    strip.show();
    radio.write(&status, sizeof(button_status_t));

    last_change_ms = millis();
    last_update_ms = last_change_ms;
  } else if ( millis() - last_change_ms > MIN_UPDATE_RATE_MS ) {
    radio.write(&status, sizeof(button_status_t));
    last_update_ms = millis();
  }

  if ( millis() - last_change_ms > LED_TIMEOUT_MS ) {
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
  }

  if ( millis() - last_change_ms > IDLE_TIMEOUT_MS ) {
    //sleep_goto_dormant_until_edge_high(10);
  }

  last_status = status;

}

