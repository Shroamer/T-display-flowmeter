void loop() {
  processLogStore();  // check for new data to update log with
  checkVBAT();        // update voltage if needed

  //check for BUTTON1 press
  switch (b1_pressStatus) {
    case 0:  // button1 isn't pressed
      break;
    case 1:  // short button1 press
      ESP_LOGD("BTN1", "short: %d", b1_pressStatus);
      break;
    case 2:  // long button1 press - reset user counter
      countPulses = totalPulses;
      newSensorData = true;  // Set flag for new data
      ESP_LOGD("BTN1", "long: %d", b1_pressStatus);
      break;
  }
  b1_pressStatus = 0;

  if (newBufferData) {      //We have new buffer calculated, let's process and display it
    newBufferData = false;  // Reset flag
    pageMain();
    printCanvas();
  }
}
