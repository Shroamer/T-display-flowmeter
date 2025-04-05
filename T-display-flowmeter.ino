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

#define BUTTON1_MASK (1 << (BUTTON1 - 32))  // mask raw register for button1 read
#define BUTTON2_MASK (1 << BUTTON2)         // mask raw register for button1 read



// Encoder pins
//#define EN_A 15   // encoder A input
//#define EN_B 13   // encoder B input
//#define EN_SW 12  // encoder switch input

// Peripheral pins
#define FSENS 13  // water flow sensor input pin



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
TFT_eSprite img = TFT_eSprite(&tft);  // Create Sprite object "img" with pointer to "tft" object the pointer is used by pushSprite() to push it onto the TFT

//    Sensor parameters
const float pulPerMl = 8.254;                // how many pulse count in a ml of flow (1000ml/1000 in fact)
const float pulPerGal = pulPerMl * 3785.41;  // 3,785.41ml in galon
//  💧  Sensor volatile vars
//volatile unsigned long lastPulseTime = 0;    // Time of latest pulse read
//volatile unsigned long pulseInterval = 0;    // Calculated interval between last pulses
volatile unsigned long totalPulses = 0;     // total pulses counter (increments by sensor ISR)
volatile unsigned long countPulses = 0;     // User counter shift. How many total counter to skip to count user's counter.
volatile bool newSensorData = false;        // flag after each pulse read
float maxPulsePerMinute = 1500 * pulPerMl;  //1500 ml/min * pulPerMl to get max pulses per minute for 1.5L/min pump




//  🪣  buffered data
//unsigned long interval = 0;  // to store imported sample value
//unsigned long pulses = 0;    // to store imported sample value
//volatile unsigned long totalPulsesBuf = 0;
//volatile unsigned long countPulses = 0;

//volatile unsigned long pulsesPerMinute = 0.0;  // store calculated value

// ⏰ Buffering timer setup (how often to calculate and log sample)
unsigned long lastPulseCount = 0;            // last accounted pulse number. All later pulses are to be accounted. Increments inside timer ISR
hw_timer_t *timer = NULL;                    // Timer handle
unsigned long bufferLengthUs = 500000;       // Alarm time in uS value (500000 - 0.5sec). Chunk of time we will calculate values for.
volatile unsigned long pulsesToAccount = 0;  // specify how many pulses were accounted. Basically, totalPulses index to determine how many pulses to account in buffer process
volatile float pulsesPerMinuteBufAvg = 0;    // Here we keep average flow speed in pul/min for last buffer processed
volatile bool newBufferData = false;         // flag rises in buffer timer ISR to indicate we have new buffer to account to (display and/or store)

// ⏰ buffering ISR   Returns pul/min of unaccounted pulses into pulsesPerMinuteBufAvg; updates lastPulseCount.
void IRAM_ATTR onTimer() {
  unsigned long pulsesTemp;  // to store value of how many pulses we accounting for in case we have another pulse during this routine
  {
    noInterrupts();            // Critical: Disable interrupts while retrieving volatile data
    pulsesTemp = totalPulses;  // assign value of how many pulses we accounting for in case we have another pulse during this routine
    interrupts();              // Re-enable interrupts ASAP
  }
  unsigned long unaccountedPulses = pulsesTemp - lastPulseCount;  // lets determine how many pulses added since latest buffer processing
  float buffersInMinuteUs = 60000000.0 / bufferLengthUs;          // how many buffers fits in minute (60`000`000 uS). could be fraction.
  pulsesPerMinuteBufAvg = unaccountedPulses * buffersInMinuteUs;  // average pul/min of buffer
  lastPulseCount = pulsesTemp;                                    // setting up last unaccounted pulse to current, as we ccounted all of them right now.
  newBufferData = 1;                                              // rising flag to process data further
}

//  〽️  ISR for each sensor pulse
//do i still need unbuffered calculations now when i have a buffer?
//Why i need to know single pulse length while dealing with buffer?
//Maybe i could just increment totalPulses here?
void IRAM_ATTR pulseISR() {
  //unsigned long currentTime = micros();                         // take current time sample
  //unsigned long currentInterval = currentTime - lastPulseTime;  // calculate how many time passed since last pulse
  //if (currentInterval > 1000) {   // Debounce: Ignore very short intervals (e.g., < 1ms) to filter noise
  //pulseInterval = currentInterval;  // Store for calculation
  //lastPulseTime = currentTime;      // storing last pulse time to calculate length to nex one coming
  totalPulses++;         // increment total pulses with each pulse
  newSensorData = true;  // Set flag for new data
  //}
}

//  🔘  ISR for pressing BUTTON1 (GPIO35)
#define DEBOUNCE_TIME_US 50000                 // 50 ms debounce threshold (in microseconds)
#define LONG_PRESS_THRESHOLD_US 500000         // 0.5 second threshold for a long press
volatile int8_t b1_pressStatus = 0;            // 0 - unpressed; 1 - short-pressed; 2 - long pressed
volatile int64_t b1_pressStartTime = 0;        // store timestamp
volatile int64_t b1_lastButtonChangeTime = 0;  // store how long button was pressed

void IRAM_ATTR button1ISR() {
  int64_t now = esp_timer_get_time();                              // store isr call timestamp
  if ((now - b1_lastButtonChangeTime) < DEBOUNCE_TIME_US) return;  // Debounce: Ignore any changes that occur faster than DEBOUNCE_TIME_US.
  b1_lastButtonChangeTime = now;

  bool pressed = isButtonPressedRaw(BUTTON1);  // Use the raw port read to get the current button state, but inverse, as button wiring returns 1 when released and 0 when pressed
  if (pressed) {                               // Button has just been pressed: record the timestamp.
    b1_pressStartTime = now;
    b1_pressStatus = 0;
  } else {  // Button released: measure the duration of the press.
    int64_t duration = now - b1_pressStartTime;
    if (duration >= LONG_PRESS_THRESHOLD_US) {
      b1_pressStatus = 2;  // Long press detected.
    } else {
      b1_pressStatus = 1;  // Short press detected.
    }
  }
}
