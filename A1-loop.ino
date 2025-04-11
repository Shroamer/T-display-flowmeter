int16_t b1_statLoop = 0;  // keep imported button1 press info so functions inside loop() could acces button1 status

void loop() {
  processLogStore();  // check for new data to update log with
  checkVBAT();        // update voltage if needed

  //taskENTER_CRITICAL();
  b1_statLoop = b1_pressStatus;
  b1_pressStatus = 0;
  //taskEXIT_CRITICAL();

  //check for BUTTON1 press
  switch (b1_statLoop) {
    case 0:  // button1 isn't pressed
      break;
    case 1:  // short button1 press - switch TFT backlight
      setBacklight(!getBacklight());
      ESP_LOGD("BTN1", "short: %d", b1_statLoop);
      break;
    case 2:  // long button1 press - reset user counter
      countPulses = totalPulses;
      newSensorData = true;  // Set flag for new data
      ESP_LOGD("BTN1", "long: %d", b1_statLoop);
      break;
  }

  if (newBufferData) {      //We have new buffer calculated, let's process and display it
    newBufferData = false;  // Reset flag
    pageMain();
    printCanvas();
  }
  b1_statLoop = 0;
}
