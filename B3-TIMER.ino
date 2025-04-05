void initTimer(uint64_t alarmTimeUs) {
  // timer0 setup - sampling for values display
  batch_timer = timerBegin(0, 80, true);  // Timer 0, prescaler 80 → 1 tick = 1µs
  timerAttachInterrupt(batch_timer, &batchTimer, true);
  timerAlarmWrite(batch_timer, alarmTimeUs, true);  // 500ms (500,000 µs)
  timerAlarmEnable(batch_timer);

  // timer1 setup - sampling for log
  log_timer = timerBegin(1, 80, true);  // Timer 1, prescaler 80 → 1 tick = 1µs
  timerAttachInterrupt(log_timer, &batchTimer, true);
  timerAlarmWrite(log_timer, 1000000, true);  // 1s (1,000,000 µs)
  timerAlarmEnable(log_timer);
}