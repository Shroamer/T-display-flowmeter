

// Inline function to read the button state via raw port register to use inside ISR.
// Returns true if the button is pressed (active low).
inline bool isButtonPressedRaw(uint8_t pin) {
  // REG_READ(GPIO_IN_REG) returns a 32-bit word with all GPIO states.
  // If the bit corresponding to BUTTON1 is 0, then the button is pressed.
  //On ESP32, the GPIO register to read depends on the pin number:
  //For GPIOs 0–31, use GPIO_IN_REG
  //For GPIOs 32–39, use GPIO_IN1_REG
  //uint32_t gpioState = REG_READ(GPIO_IN1_REG);
  //return ((gpioState & BUTTON1_MASK) == 0);
  uint32_t gpioState = (pin < 32) ? REG_READ(GPIO_IN_REG) : REG_READ(GPIO_IN1_REG);
  uint32_t mask = 1 << (pin % 32);  // Use %32 to handle both ranges
  return ((gpioState & mask) == 0);
}

void button_init() {
  pinMode(BUTTON1, INPUT_PULLUP);                                       // PULLUP - External pull-up required
  attachInterrupt(digitalPinToInterrupt(BUTTON1), button1ISR, CHANGE);  // Trigger on release
  initTimer(bufferLengthUs);                                            // timer set to uS value (500000 - 0.5sec)
}
