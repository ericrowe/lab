Code for a simple temperature/humidity logger.  Reports the readings up to Adafruit.io for logging and displays the current readings using the onboard screen.  I use this as freezer/refrigerator monitors.

Hardware:
Processor/Display: [ESP32-ST TFT Feather](https://www.adafruit.com/product/5300)
Sensor: [SHT40 Temperature and Humidity Sensor](https://www.adafruit.com/product/4885)

Uses the MAC address to generate an ID to report to Adafruit.io, so you don't need to hard code anything there.  Will automatically scan and find the first SHT4x sensor and report those readings up.

Rename the config-sample.h to config.h, then edit that file to add in your wifi credentials and adafruit.io credentials.  You can also set min and max temp/humidity values, which are used to color code the display (outside the range, and the values will be drawn in red to highlight the issue).

You will need to rebuild/reload the processor every so often as the SSL certs go out of date.  With the adafruit.io library, the only way to get new certs is to update your Arduino toolchain and rebuild/reinstall the code to your processor board.  Adafruit seems to update this around July of even numbered years.