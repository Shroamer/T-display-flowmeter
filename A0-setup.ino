void setup() {
  Serial.begin(115200);
  TFT_init();
  FS_init();
  TFT_welcome();
  delay(200);
  button_init();
}