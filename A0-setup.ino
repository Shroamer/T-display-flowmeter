void setup() {
  Serial.begin(115200);
  TFT_init();
  FS_init();
  // Init BUTTON1 to reset counter
  pinMode(BUTTON1, INPUT);                       // External pull-up required
  attachInterrupt(BUTTON1, button1ISR, RISING);  // Trigger on release
  TFT_welcome();
  delay(200);
  initTimer(bufferLengthUs);     // timer set to uS value (500000 - 0.5sec)
  newSensorData = true;  // initialize display mode
  pulseInterval = 1;     // we need nonzero value to pass to calculation
}