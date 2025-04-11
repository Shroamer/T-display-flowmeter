// https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/TFT_eSPI/examples/FactoryTest/FactoryTest.ino

// run in loop() to periodically update voltage to vBatMV
void checkVBAT() {
  uint64_t nowTime = millis();
  if (nowTime - vBatTimeStamp > VBAT_PERIOD_MS) {
    vBatMV = getVoltageMV();
    vBatTimeStamp = millis();
  }
}

// return integer voltage in milivolts
int16_t getVoltageMV() {
  vBatPower(1);  // powers up voltage divider (shouldn't we wait for it to ramp up?)
  delay(3); //weird, previously we have no need to wait here for normal value, but now i'm getting 1.2v on battery w/o this delay
  uint16_t v = analogRead(ADC_PIN);
  vBatPower(0);                                                  // powers down voltage divider for battery saving
  int16_t battery_voltage_mv = (int16_t)(((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0) * 1000);  // find out voltage in integer milivolts (chatgpt)
  //float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0); // find out voltage in float volts (original)
  //img.drawNumber(v, 30, 30, 1);
  //ESP_LOGD("Voltage/RAW:", "%d mV, %d", battery_voltage_mv, v);  //ESP_LOGD("Flow", "Pulse count: %d", pulseCount);
  return (battery_voltage_mv);
}

// turns vbat voltage divider on:HIGHT or off:LOW
void vBatPower(bool turnOn) {
  digitalWrite(ADC_EN, turnOn);
}

// run in setup() once
void vBat_init() {
  pinMode(ADC_EN, OUTPUT);
  vBatPower(1);  //digitalWrite(ADC_EN, HIGH);  // powers up voltage divider
  delay(100);
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);  //Check type of calibration value used to characterize ADC
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    ESP_LOGI("eFuse Vref:", "%u mV", adc_chars.vref);
    vref = adc_chars.vref;
  } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
    ESP_LOGI("Two Point --> coeff_a:", "%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
  } else {
    ESP_LOGI("Default Vref:", "1100mV");
  }
  //vBatMV = getVoltageMV();  // get initial voltage so not to wait 5 sec to update
  vBatPower(0);  //digitalWrite(ADC_EN, LOW);  // powers down voltage divider
}