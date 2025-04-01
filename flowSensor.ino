// Init flow sensor pin, interrupt
void FS_init() {
  pinMode(FSENS, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FSENS), pulseISR, RISING);
}