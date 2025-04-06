// Init flow sensor pin, interrupt
void FS_init() {
  pinMode(FSENS, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FSENS), pulseISR, RISING);
}

//converts impulses to various volume scales
unsigned long imp2vol(unsigned long impulses, float modifier) {
  return (impulses * modifier);
}