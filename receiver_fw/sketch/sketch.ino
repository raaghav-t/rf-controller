#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";
boolean button_state = 0;
int led_pin = 3;

// Compact structure to convey the current input state
typedef struct {
  uint16_t joy_x;
  uint16_t joy_y;
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

void setup() {
  pinMode(6, OUTPUT);
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
  radio.setChannel(31);
  radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.startListening();              //This sets the module as receiver
}

void loop()
{
  button_status_t button_status = {0};
  
  if (radio.available())              //Looking for the data.
  {
    radio.read(&button_status, sizeof(button_status_t));    //Reading the data

    {
      // Print things out!
      char buffer[128] = {0}; // This is just a temporary place to format a string
      snprintf(buffer, sizeof(buffer), "Joy X %u, Y %u, PB %lu!!", button_status.joy_x, button_status.joy_y, (uint32_t)button_status.joy_push);
      Serial.println(buffer);

      snprintf(buffer, sizeof(buffer), "%u %u %u %u %u %u %u",
              button_status.up, button_status.down, button_status.left, button_status.right,
              button_status.a, button_status.b, button_status.c);
      Serial.println(buffer);
    }

    // Now here do things with your input in button_status
  }
}