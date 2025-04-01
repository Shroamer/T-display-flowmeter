// TFT display pins
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4  // backlight

// Other T-Display pin
#define ADC_IN 34     // BATT ADC
#define BUTTON1 35    //
#define BUTTON2 0     //
#define ADC_POWER 14  // ???

// Encoder pins
#define EN_A 15   // encoder A input
#define EN_B 13   // encoder B input
#define EN_SW 12  // encoder switch input

// Peripheral pins
#define FSENS 27  // water flow sensor input pin



// ================================================
// === DISPLAY
// ================================================
//#include <Adafruit_GFX.h>     // Core graphics library
//#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789

//#include <SPI.h>
#define TFT_WIDTH 135
#define TFT_HEIGHT 240
//#define USE_DMA_TO_TFT  // Define to use DMA for Sprite transfer to SPI TFT, comment out to use no DMA
//#define COLOR_DEPTH 16  // Color depth has to be 16 bits if DMA is used to render image
//#define USER_SETUP_LOADED // defined in local User_Setup.h
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();            //TFT_eSPI tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
TFT_eSprite img = TFT_eSprite(&tft);  // Create Sprite object "img" with pointer to "tft" object
                                      // the pointer is used by pushSprite() to push it onto the TFT

// Sensor vars
const float pulsesPerMl = 8.0; // how many pulse count in a liter of flow
volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseInterval = 0;
volatile unsigned long totalPulses = 0;
volatile unsigned long countPulses = 0;
//volatile unsigned int lastPulseSpeed = 0; // pulses/min
volatile float pulsesPerMinute = 0.0;
volatile bool newDataAvailable = false;  // flag after each pulse read

// ISR for each sensor pulse
void IRAM_ATTR pulseISR() {
  unsigned long currentTime = micros();
  unsigned long currentInterval = currentTime - lastPulseTime;

  // Debounce: Ignore very short intervals (e.g., < 1ms) to filter noise
  //if (currentInterval > 1000) {
  pulseInterval = currentInterval;  // Store for calculation
  lastPulseTime = currentTime;
  totalPulses++;
  newDataAvailable = true;  // Set flag for new data
  //}
}

// ISR for pressing BUTTON1 (GPIO35)
void IRAM_ATTR button1ISR() {
  countPulses = totalPulses;  // Set flag when button is released
  newDataAvailable = true;  // Set flag for new data
}