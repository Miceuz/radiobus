# Firmware examples

Type `pio run -t upload` in the example directory to compile and upload the code. If the board is sleeping, double click the reset button so it goes into bootloader mode before programming.

The firmware to drive the board is provided here. Examples are more or less functional, some of them used in production environment. Firmware runs the board in the lowest power I was able to achieve. I hope the code is self explanatory. 

You have to change the keys hardcoded in `radio.cpp`. Examples run in APB mode. If you want to set up OTAA, have a look into the [MCCI LoRaWAN LMIC library](https://github.com/mcci-catena/arduino-lmic/) examples.

To send your sensor data set them up in `main.cpp` `setup()` function, have a look into method `read_sensors()`. 
