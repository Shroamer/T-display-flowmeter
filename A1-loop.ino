void loop() {
  if (newDataAvailable) {
    noInterrupts();                          // Critical: Disable interrupts during calculation
    unsigned long interval = pulseInterval;  // retrieve variable
    unsigned long pulses = totalPulses;
    newDataAvailable = false;  // Reset flag immediately after copying
    interrupts();              // Re-enable interrupts ASAP

    // Calculate pulses per minute based on *last* interval

    if (interval > 0) {
      pulsesPerMinute = 60000000.0 / interval;  // 60,000,000 us in a minute
      float mlPerMinute = pulsesPerMinute / pulsesPerMl;
      img.setCursor(0, 0);
      img.setTextSize(1);
      img.setTextColor(TFT_WHITE);
      img.println("pulses/min: ");
      img.setTextSize(2);
      img.setTextColor(TFT_YELLOW);
      img.println(pulsesPerMinute);

      img.setTextSize(1);
      img.setTextColor(TFT_WHITE);
      img.print("ml/min (");
      img.print(String(pulsesPerMl, 3));
      img.println(" p/ml)");
      img.setTextSize(2);
      img.setTextColor(TFT_YELLOW);
      img.println(mlPerMinute);

      img.setTextSize(1);
      img.setTextColor(TFT_WHITE);
      img.println("interval uS: ");
      img.setTextSize(2);
      img.setTextColor(TFT_YELLOW);
      img.println(interval);

      img.setTextSize(1);
      img.setTextColor(TFT_WHITE);
      img.println("ml total/count: ");
      img.setTextSize(2);
      img.setTextColor(TFT_YELLOW);
      float mlTotal = pulses / pulsesPerMl;
      img.print(String(mlTotal, 2));
      img.setTextColor(TFT_WHITE);
      img.print(" / ");
      img.setTextColor(TFT_YELLOW);
      float mlCount = (pulses - countPulses) / pulsesPerMl;
      img.println(String(mlCount,2));

      img.setTextSize(1);
      img.setTextColor(TFT_WHITE);
      img.println("pulse total/count: ");
      img.setTextSize(2);
      img.setTextColor(TFT_YELLOW);
      img.print(pulses);
      img.setTextColor(TFT_WHITE);
      img.print(" / ");
      img.setTextColor(TFT_YELLOW);
      img.println(pulses - countPulses);
      printCanvas();
    }
  }
}
