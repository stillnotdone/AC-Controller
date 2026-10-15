#include "ac_controller.h"
#include "web_controller.h"
#include "midea.h"

#include <Arduino.h>

//#define RECIEVER_PIN 15 // gpio 1


void setup() {
  Serial.begin(115200);
  ac_setup();
  ac_sync_state(false);
  web_controller_setup();

  //IrReceiver.begin(RECIEVER_PIN, ENABLE_LED_FEEDBACK);  
}

void loop() {
  web_controller_loop();
  ac_loop();
  /*if (IrReceiver.decode()) {
    IrReceiver.resume();
  }*/
}
