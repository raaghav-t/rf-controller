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

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
Servo leftServo;
Servo rightServo;

// !!! Set this to the channel on your controller!
static constexpr uint8_t NRF_CHANNEL = 13;

static constexpr uint8_t PIN_NRF_CE = 9;
static constexpr uint8_t PIN_NRF_NCS = 10;

RF24 radio(PIN_NRF_CE, PIN_NRF_NCS); // CE, CSN
uint8_t const NRF_ADDRESS[6] = "00001";

// Setup for Left and Right Wheels
const int leftWheel = 7;
const int rightWheel = 3;

int leftState = 0;
int rightState = 0;
int mapped_joyyLeft = 0;
int mapped_joyyRight = 0;
int mapped_joyxLeft = 0;
int mapped_joyxRight = 0;

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

// Force a check to make sure it got packed to the right size
static_assert(sizeof(button_status_t) == 5, "button_status_t should be 5B");

/**
 * @brief Compare two inputs of type button_status_t
 * 
 * @param a 
 * @param b 
 * @return true If equal (with analog inputs within some threshold)
 * @return false If not equal
 */
bool status_equal(button_status_t const &a, button_status_t const &b) {
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

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  radio.openReadingPipe(0, NRF_ADDRESS);
  radio.setChannel(NRF_CHANNEL);
  // Since we never transmit this doesn't really matter. Adjust as needed if you do transmit!
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // Left and Right Wheels
  leftServo.attach(7);
  rightServo.attach(3);
  
}

void loop() {
  button_status_t button_status = {0};
  
  // Periodically poll for data. If anything is available, run this block:
  if (radio.available()) {
    // Receive whatever data there is
    radio.read(&button_status, sizeof(button_status_t));

    {
      // Print things out!
      // !!! Remove this once you know you receive data from the controller
      char buffer[128] = {0}; // This is just a temporary place to format a string
      snprintf(buffer, sizeof(buffer), "Joy X %u, Y %u, PB %lu!!", button_status.joy_x, button_status.joy_y, (uint32_t)button_status.joy_push);
      Serial.println(buffer);

      snprintf(buffer, sizeof(buffer), "%u %u %u %u %u %u %u",
              button_status.up, button_status.down, button_status.left, button_status.right,
              button_status.a, button_status.b, button_status.c);
      Serial.println(buffer);
    }

    // Now here do things with your input in button_status
    
    //Left Wheel
    mapped_joyyLeft = map(button_status.joy_y, 0, 4095, 0, 90);
    mapped_joyxLeft = map(button_status.joy_y, 0, 4095, 0, 90);
    leftState = mapped_joyyLeft + mapped_joyxLeft;
    leftServo.write(leftState); 

    //Right Wheel
    mapped_joyyRight = map(button_status.joy_y, 0, 4095, 90, 0);
    mapped_joyxRight = map(button_status.joy_x, 0, 4095, 90, 0);
    rightState = mapped_joyyRight + mapped_joyxRight;
    rightServo.write(rightState);

    
  }
}
