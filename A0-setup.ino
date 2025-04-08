void setup() {
  Serial.begin(115200);
  TFT_init();     // init ST-7789 display and stuff
  FS_init();      // Init FlowSensor stuff
  TFT_welcome();  // show welcome vibescreen
  delay(1000);    // and wait
  button_init();  // init buttons and interrupts
  vBat_init();    // init of battery voltage check circuitry
}