void pageMain() {
  //      DRAWING analog meter arc (0 is down, increments CW)
  int valueAngle = map(pulsesPerMinuteBufAvg, 0, maxPulsePerMinute, 90, 180);  //mapping current value pul/min within min/max sensor values into min/max angle values
  if (valueAngle < 180)
    img.drawArc(135, 135, 135, 125, valueAngle, 180, TFT_NAVY, TFT_BLACK);  // draw passive part of the arc
  if (valueAngle > 0)
    img.drawArc(135, 135, 135, 125, 90, valueAngle, TFT_YELLOW, TFT_BLACK);  //draw active part

  img.drawFloat(pulsesPerMinuteBufAvg, 3, 138, 5);  //debug purpose
  img.setCursor(138, 15);
  img.print("PpM");
  img.drawFloat(maxPulsePerMinute, 3, 138, 25);
  img.setCursor(138, 35);
  img.print("PpM max");
  img.setCursor(138, 45);
  img.print(totalPulses);
  img.setCursor(138, 55);
  img.print("total");
  //      DRAWING current flow value
  float displayLitersPerMinute = (pulsesPerMinuteBufAvg / (pulPerMl*1000));  // calculating LpM from PpM and PpMl of the sensor
  if (displayLitersPerMinute >= 10) img.setTextFont(4);
  else img.setTextFont(7);  //7
  img.setTextSize(1);
  //img.setCursor(20, 80);
  img.setTextDatum(TR_DATUM);  // Right alignment
  img.setTextColor(TFT_YELLOW);
  img.drawFloat(displayLitersPerMinute, 3, 136, 85);  // (value, decimal places, x, y)

  //      DRAWING current flow title
  //img.setTextFont(2);
  img.setTextSize(1);
  //img.setCursor(105, 64);
  img.setTextColor(TFT_LIGHTGREY);
  img.drawRightString("liters/min", 135, 69, 2);

  //      DRAWING counted value
  img.setTextFont(4);
  img.setTextSize(1);
  img.setTextColor(TFT_CYAN);
  float countLiters = (float)(totalPulses - countPulses) / (pulPerMl * 1000.0);  // calculates how many liters we accounted for user counter
  uint8_t valDecimals = 3;
  if (countLiters > 1000) valDecimals = 0;
  else if (countLiters > 100) valDecimals = 1;
  else if (countLiters > 10) valDecimals = 2;
  img.drawFloat(countLiters, valDecimals, 136, 46);  // (value, decimal places, x, y)

  //      DRAWING counted title
  img.setTextSize(1);
  img.setTextColor(TFT_LIGHTGREY);
  img.drawRightString("liters", 135, 30, 2);


  img.setTextDatum(TL_DATUM);  // Left alignment
  img.setTextFont(1);
  img.drawRect(136, 0, 240 - 136, 135, TFT_DARKGREY);
}

/*void pageOverview() {
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
  img.print(String(pulPerMl, 3));
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
  float mlTotal = pulses / pulPerMl;
  img.print(String(mlTotal, 2));
  img.setTextColor(TFT_WHITE);
  img.print(" / ");
  img.setTextColor(TFT_YELLOW);
  float mlCount = (pulses - countPulses) / pulPerMl;
  img.println(String(mlCount, 2));

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
}*/