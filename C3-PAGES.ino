void pageMain() {

  //      DRAWING analog meter arc (0 is down, increments CW)
  int valueAngle = map(pulsesPerMinuteBufAvg, 0, maxPulsePerMinute, 90, 180);  // mapping current value pul/min within min/max sensor values into min/max angle values
  uint32_t analogColor = TFT_CYAN;                                             // Store active analog line color for overflow case
  if (pulsesPerMinuteBufAvg > maxPulsePerMinute) {                             // pin-top value and change analog color if we have sensor overflow
    analogColor = TFT_RED;                                                     // change color if overflowed
    valueAngle = 180;                                                          // and set value to maimum possible
  }
  if (valueAngle < 180)                                                       // draw passive only if needed
    img.drawArc(135, 135, 135, 125, valueAngle, 180, TFT_NAVY, TFT_BLACK);    // draw passive part of the arc
  if (valueAngle > 0)                                                         // draw active only if needed
    img.drawArc(135, 135, 135, 125, 90, valueAngle, analogColor, TFT_BLACK);  // draw active part

  //      DRAWING current flow value
  float displayLitersPerMinute = (pulsesPerMinuteBufAvg / (pulPerMl * 1000));  // calculating LpM from PpM and PpMl of the sensor
  img.setTextFont(7);
  img.setTextSize(1);
  //img.setCursor(20, 80);
  img.setTextDatum(TR_DATUM);  // Right alignment
  if (pulsesPerMinuteBufAvg > 0) img.setTextColor(TFT_YELLOW);
  else img.setTextColor(TFT_LIGHTGREY);
  uint8_t valDecimals = 3;
  if (displayLitersPerMinute >= 1000) valDecimals = 0;
  else if (displayLitersPerMinute >= 100) valDecimals = 1;
  else if (displayLitersPerMinute >= 10) valDecimals = 2;
  img.drawFloat(displayLitersPerMinute, 3, 136, 85);  // (value, decimal places, x, y)

  //      DRAWING current flow title
  img.setTextSize(1);
  img.setTextColor(TFT_LIGHTGREY);
  img.drawRightString("liters/min", 135, 69, 2);

  //      DRAWING counted value
  img.setTextFont(4);
  img.setTextSize(1);
  img.setTextColor(TFT_CYAN);
  float countLiters = (float)(totalPulses - countPulses) / (pulPerMl * 1000.0);  // calculates how many liters we accounted for user counter
  valDecimals = 3;
  if (countLiters > 1000) valDecimals = 0;
  else if (countLiters > 100) valDecimals = 1;
  else if (countLiters > 10) valDecimals = 2;
  img.drawFloat(countLiters, valDecimals, 136, 46);  // (value, decimal places, x, y)

  //      DRAWING counted title
  img.setTextSize(1);
  img.setTextColor(TFT_LIGHTGREY);
  img.drawRightString("liters", 135, 30, 2);

  //plotLog(136, 0, 104, 135, 0xFFF4, 0, 0, 1, 0, &img); // plot bottom-to-top log
  plotLog(136, 0, 104, 135, 0xFFF4, 0, 0, 1, 1, &img);  // plot top-to-bottom log
  //plotLog(136, 0, 104, 135, 0xFFF4, 0, 0, 0, 0, &img);  // plot right-to-left log
  //plotLog(136, 0, 104, 135, 0xFFF4, 0, 0, 0, 1, &img); // plot left-to-right log

  drawBatGauge(0, 0, 30, 12, &img);  // put battery gauge in corner
}

// welcome screen
void TFT_welcome() {
  img.fillRect(0, 0, TFT_HEIGHT, TFT_WIDTH / 2, TFT_CYAN);
  img.fillRect(0, (TFT_WIDTH / 2), TFT_HEIGHT, TFT_WIDTH / 2, TFT_YELLOW);
  //drawWedgeLine
  //img.setTextFont(4);
  //img.setTextDatum(TC_DATUM);  // top-center alignment
  img.setTextSize(2);
  img.setTextColor(TFT_NAVY);
  img.drawCentreString("FlowMeter", 120, 3, 4);
  img.setTextSize(1);
  img.setTextColor(TFT_BROWN);
  img.drawCentreString("WARNING!", 121, 46, 4);
  img.setTextColor(TFT_MAROON);
  img.drawCentreString("WARNING!", 119, 44, 4);
  img.setTextColor(TFT_GOLD);
  img.drawCentreString("WARNING!", 120, 45, 4);
  img.setTextColor(TFT_RED);
  img.drawCentreString("this software", 120, 70, 2);
  img.drawCentreString("contains vibeware!", 120, 80, 2);
  img.drawCentreString("It was written by someone", 120, 90, 2);
  img.drawCentreString("who has no idea what they're doing", 120, 100, 2);
  img.drawCentreString("or how any of it works.", 120, 110, 2);
  printCanvas();
}

// function will plot a battery gauge and voltage value in it if will be big to fit
void drawBatGauge(int16_t x, int16_t y, int16_t w, int16_t h, TFT_eSprite *sprite) {
  if (vBatMV) {
    int availW = w - 2;
    int availH = h - 2;
    if (vBatMV >= 2500 && vBatMV <= 4200) {                  // normal battery voltage
      int voltageX = map(vBatMV, 2500, 4200, 0 + 1, w - 1);  // find coordinates of current voltage
      sprite->fillRect(x, y, voltageX, h, TFT_GREEN);
      sprite->fillRect(x + voltageX, y, w - voltageX, h, TFT_DARKGREEN);
    } else if (vBatMV > 4200) {  // charging
      sprite->fillRect(x, y, w, h, TFT_CYAN);
    } else if (vBatMV < 2500) {  // undervoltage
      sprite->fillRect(x, y, w, h, TFT_MAROON);
    }
    //    String voltageStr = String(vBatMV, 1);  // Convert to a String with the chosen precision
    float floatVolts = vBatMV / 1000.0;  // converting to float in volts
    int symbolsFit = availW / 6;         // how many symbols of font1 size2 we can place inside our box (font1:8x6px, 2:18x11, 4:26x16, 6:46x24, 7:58x32, 8:78x48)
    Serial.print("availW==");
    Serial.println(availW);
    if (symbolsFit >= 3) {             // if we have where to place 3+ letters (3.6)
      sprite->setTextDatum(CC_DATUM);  // center text
      sprite->setTextFont(1);
      sprite->setTextSize(1);
      sprite->setTextColor(TFT_BLACK);
      int decimalsPlot = symbolsFit - 2;       // substracting 2 symbols : "3." to get how many space left for decimals
      if (decimalsPlot > 3) decimalsPlot = 3;  // we don't need more than 3 decimals
      Serial.print("decimalsPlot==");
      Serial.println(decimalsPlot);
      sprite->drawFloat(floatVolts, decimalsPlot, x + (availW / 2), y + (availH / 2), 1);
    }
  }
  sprite->drawRect(x, y, w, h, TFT_LIGHTGREY);  // draw frame
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