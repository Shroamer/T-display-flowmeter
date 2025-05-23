// TFT display pins
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4  // backlight

// Other T-Display pin
#define ADC_PIN 34    // BATT ADC connected to battery 1:2 voltage divider (100k/100k)
#define ADC_EN 14     // ADC_EN is the ADC detection enable port. If the USB port is used for power supply, it is turned on by default. If it is powered by battery, it needs to be set to high level
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
// === POWER MANAGMENT
// ================================================
// https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/TFT_eSPI/examples/FactoryTest/FactoryTest.ino
#include <esp_adc_cal.h>
int vref = 1100;
volatile int16_t vBatMV = 0;  // storing last battery voltage read
#define VBAT_PERIOD_MS 1000   // check for vbat voltage every 5000mS (5second)
uint64_t vBatTimeStamp = 0;   // store timestamp of last vBat check


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

// ⏰ batch_timer setup (how often to calculate and log sample)
unsigned long lastPulseCount = 0;            // last accounted pulse number. All later pulses are to be accounted. Increments inside batch_timer ISR
hw_timer_t *batch_timer = NULL;              // Timer handle
unsigned long bufferLengthUs = 250000;       // Alarm time in uS value (500000 - 0.5sec). Chunk of time we will calculate values for.
volatile unsigned long pulsesToAccount = 0;  // specify how many pulses were accounted. Basically, totalPulses index to determine how many pulses to account in buffer process
volatile float pulsesPerMinuteBufAvg = 0;    // Here we keep average flow speed in pul/min for last buffer processed
volatile bool newBufferData = false;         // flag rises in batch_timer ISR to indicate we have new buffer to account to (display and/or store)

// ⏰ batch_timer ISR   Returns pul/min of unaccounted pulses into pulsesPerMinuteBufAvg; updates lastPulseCount.
void IRAM_ATTR batchTimer() {
  unsigned long pulsesTemp;  // to store value of how many pulses we accounting for in case we have another pulse during this routine
  {
    noInterrupts();            // Critical: Disable interrupts while retrieving volatile data
    pulsesTemp = totalPulses;  // assign value of how many pulses we accounting for in case we have another pulse during this routine
    interrupts();              // Re-enable interrupts ASAP
  }
  unsigned long unaccountedPulses = pulsesTemp - lastPulseCount;  // lets determine how many pulses added since latest buffer processing
  float buffersInMinuteUs = 60000000.0 / bufferLengthUs;          // how many buffers fits in minute (60`000`000 uS). could be fraction.
  pulsesPerMinuteBufAvg = buffersInMinuteUs * unaccountedPulses;  // average pul/min of buffer
  lastPulseCount = pulsesTemp;                                    // setting up last unaccounted pulse to current, as we ccounted all of them right now.
  newBufferData = 1;                                              // rising flag to process data further
}

// logger variables
float logArray[240] = { NAN };                                      // here we'll store our logged values (PpM), initialize array with not-a-number values.
const int logArrayLength = sizeof(logArray) / sizeof(logArray[0]);  // how many items we have in log array
int16_t logArrayIndex = logArrayLength - 1;                         // index of last stored to log value. it is the last one, so we can start writing to 0. This way we can pull values circularly and know when the sequence starts. Just because i think shifting whole array each time will be extra resourse consuming. (i'm not a programmer)

// log_timer setup
unsigned long lastLogCount = 0;            // last accounted in log pulse number. All later pulses are to be accounted for next log entry. Increments inside log_timer ISR
hw_timer_t *log_timer = NULL;              // Timer handle
unsigned long logSampleLengthUs = 250000;  // log sample length in uS value (1000000 - 1sec). Also a timer alarm. Each log entry will be for that chunk length.
volatile float pulsesPerMinuteLogAvg = 0;  // Here we store average flow speed in pul/min for last log sample processed
volatile bool newLogData = false;          // flag rises in log_timer ISR to indicate we have new log item to account to

// ⏰ log_timer ISR here we about to calculate average pul/min of last log period and iterate lastLogCount index.
void IRAM_ATTR logTimer() {
  unsigned long pulsesTemp;  // to store value of how many pulses we accounting for in case we have another pulse during this routine
  {
    noInterrupts();            // Critical: Disable interrupts while retrieving volatile data
    pulsesTemp = totalPulses;  // assign value of how many pulses we accounting for in case we have another pulse during this routine
    interrupts();              // Re-enable interrupts ASAP
  }
  unsigned long unaccountedPulses = pulsesTemp - lastLogCount;  // lets determine how many pulses added since latest log sampling
  float logsInMinuteUs = 60000000.0 / logSampleLengthUs;        // how many log samples fits in minute (60`000`000 uS). could be fraction.
  pulsesPerMinuteLogAvg = unaccountedPulses * logsInMinuteUs;   // average pul/min of log sample
  lastLogCount = pulsesTemp;                                    // storing where to start next log count from
  newLogData = 1;                                               // rising flag to process data into log
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
#define DEBOUNCE_TIME_US 10000                 // 50 ms debounce threshold (in microseconds)
#define LONG_PRESS_THRESHOLD_US 500000         // 1 second threshold for a long press
volatile int16_t b1_pressStatus = 0;            // 0 - unpressed; 1 - short-pressed; 2 - long pressed
volatile uint64_t b1_pressStartTime = 0;        // store timestamp
volatile uint64_t b1_lastButtonChangeTime = 0;  // store how long button was pressed

void IRAM_ATTR button1ISR() {
  uint64_t now = esp_timer_get_time();                              // store isr call timestamp
  if ((now - b1_lastButtonChangeTime) < DEBOUNCE_TIME_US) return;  // Debounce: Ignore any changes that occur faster than DEBOUNCE_TIME_US.
  b1_lastButtonChangeTime = now;

  bool pressed = isButtonPressedRaw(BUTTON1);  // Use the raw port read to get the current button state, but inverse, as button wiring returns 1 when released and 0 when pressed
  if (pressed) {                               // Button has just been pressed: record the timestamp.
    b1_pressStartTime = now;
    b1_pressStatus = 0;
  } else {  // Button released: measure the duration of the press.
    uint64_t duration = now - b1_pressStartTime;
    if (duration >= LONG_PRESS_THRESHOLD_US) {
      b1_pressStatus = 2;  // Long press detected.
    } else {
      b1_pressStatus = 1;  // Short press detected.
    }
  }
}
