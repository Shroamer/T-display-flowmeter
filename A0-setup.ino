void setup() {
  Serial.begin(115200);
  TFT_init();
  FS_init();
  // Init BUTTON1 to reset counter
  pinMode(BUTTON1, INPUT);                      // External pull-up required
  attachInterrupt(BUTTON1, button1ISR, RISING);  // Trigger on release
  TFT_welcome();
}