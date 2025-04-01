/*
https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/image/pinmap.jpg
https://protosupplies.com/product/lilygo-esp32-t-display-development-board/
https://homeding.github.io/boards/esp32/ttgo-t-display.htm

          CONNECTED   FEATURE  GPIO PIN       PIN GPIO  FEATURE   CONNECTED
                                    _____________
                   |         |[GND]|1          24|[3v3]|         |
                   |         |[GND]|2          23|  36*| SVP     | 
                   | I2C_SDA | 21  |3          22|  37*|         |
                   | I2C_SCL | 22  |4          21|  38*|         | 
                   |         | 17  |5          20|  39*| SVN     | 
                   |         | 2   |6          19|  32 |         | 
              EN_A |         | 15  |7          18|  33 |         | 
              EN_B | INT     | 13  |8          17|  25 | DAC1    |  
             EN_SW |         | 12  |9          16|  26 | DAC2    |
                   |         |[GND]|10         15|  27 |         | FSENS 
                   |         |[GND]|11         14|[GND]|         | 
                   |         |[3v3]|12         13|[5v0]|         | 
                               io0 |BTN _____ BTN| io35
                                        |USB|
ADC_IN 34     // BATT ADC
BUTTON1 35
BUTTON2 0
ADC Power 14
* input only

    ST7789V     ESP32-VSPI
   TFT_MOSI --> 19 V-MOSI
   TFT_SCLK --> 18 V-SCLK
     TFT_CS --> 5  V-CS
     TFT_DC --> 16
    TFT_RST --> 23 V-RST
     TFT_BL --> 4         // backlight

DS18B20 
 | | |
 1 2 3
 G D V
 N Q D
 D   D

*/
