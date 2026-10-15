#include "ac_controller.hpp"
#include "web_controller.hpp"

void setup() {
  Serial.begin(115200);
  ac_setup();
  web_controller_setup(); 
}

void loop() {
  web_controller_loop();
  ac_loop();
}
