// https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/TFT_eSPI/examples/FactoryTest/FactoryTest.ino

// run to periodically update voltage to vBatMV
void checkVBAT() {
  uint64_t nowTime = millis();
  if (nowTime - vBatTimeStamp > VBAT_PERIOD_MS) {
    vBatMV = getVoltageMV();
  }
}

// return integer voltage in milivolts
int16_t getVoltageMV() {
  //static uint64_t timeStamp = 0;
  //if (millis() - timeStamp > 1000) {
  vBatPower(1);  // powers up voltage divider (shouldn't we wait for it to ramp up?)
  //timeStamp = millis();
  uint16_t v = analogRead(ADC_PIN);
  vBatPower(0);                                                                                      // powers down voltage divider for battery saving
  int16_t battery_voltage_mv = (int16_t)(((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0) * 1000);  // find out voltage in integer milivolts (chatgpt)
                                                                                                     //float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0); // find out voltage in float volts (original)
                                                                                                     //String voltage = "Voltage :" + String(battery_voltage) + "V";
                                                                                                     //Serial.println(voltage);
  //}
  return (battery_voltage_mv);
}

// turns vbat voltage divider on:HIGHT or off:LOW
void vBatPower(bool turnOn) {
  digitalWrite(ADC_EN, turnOn);
}

void vBat_init() {
  pinMode(ADC_EN, OUTPUT);
  vBatPower(1);  //digitalWrite(ADC_EN, HIGH);  // powers up voltage divider
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);  //Check type of calibration value used to characterize ADC
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    //Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
    vref = adc_chars.vref;
  } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
    //Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
  } else {
    //Serial.println("Default Vref: 1100mV");
  }
  vBatPower(0);  //digitalWrite(ADC_EN, LOW);  // powers down voltage divider
}