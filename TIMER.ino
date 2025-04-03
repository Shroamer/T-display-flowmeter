void initTimer(uint64_t alarmTimeUs) {
  timer = timerBegin(0, 80, true);  // Timer 0, prescaler 80 → 1 tick = 1µs
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, alarmTimeUs, true);  // 500ms (500,000 µs)
  timerAlarmEnable(timer);
}