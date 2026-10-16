#include "web/handler.hpp"
#include "ac/controller.hpp"

void setup() {
  Serial.begin(115200);
  ac_setup();
  web_setup(); 
}

void loop() {
  web_loop();
  ac_loop();
}
