# Firmware examples

The firmware to drive the board is provided here. Examples are more or less functional, some of them used in production environment. Firmware runs the board in the lowest power I was able to achieve. I hope the code is self explanatory. 

You have to change the keys hardcoded in `radio.cpp`. Examples run in APB mode. If you want to set up OTAA, have a look into the [MCCI LoRaWAN LMIC library](https://github.com/mcci-catena/arduino-lmic/) examples.

To send your sensor data set them up in `main.cpp` `setup()` function, have a look into method `read_sensors()`. 

## Programming

 * Connect the USB cable to the board
 * Start of Firmware mode is inditaced by all three leds lighting up.
 * Bootloader is indicated by SENS led fading. 
   * Slow fading indicates that USB is successfully connected. 
   * Fast fading indicates that USB is not connected. If USB is not connected for some time, it will time out and got into Firmware mode.
 * Board can't be programmed if it's in deep sleep in Firmware mode.
 * Press Reset button to swith the modes.

Once the board goes into bootloader mode it will show up as drive in your OS. After pressing reset again it will go into firmware mode and execute your firmware. If your firmware is using sleep mode, enter into bootloader mode to program the board.

Type `pio run -t upload` on the command line in the example directory to compile and upload the code. 

