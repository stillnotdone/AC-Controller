# AC Controller

ESP firmware that controls a AC over IR through a
web API.

## Setup

1. Copy `include/secrets.template.hpp` to `include/secrets.hpp` and fill
   in your values.
2. Edit `include/config.hpp` to desired values.
3. Build and upload

## Circuit

![Circuit](circuit/circuit.png)

Adjust R1/R2 to for current according to your LEDs specs, 
but the default values should be safe.