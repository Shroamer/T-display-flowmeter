void loop() {
  if (newBufferData) {      //We have new buffer calculated, let's process and display it
    newBufferData = false;  // Reset flag
    //noInterrupts();            // Critical: Disable interrupts during calculation
    //interval = pulseInterval;  // retrieve variable
    //pulses = totalPulses;
    //interrupts();           // Re-enable interrupts ASAP

    // Calculate pulses per minute based on *last* interval
    /*if (interval > 0) {
      pulsesPerMinute = 60000000.0 / interval;  // 60,000,000 us in a minute
      float mlPerMinute = pulsesPerMinute / pulPerMl;
    }
    */
    pageMain();
    printCanvas();
  }
}
