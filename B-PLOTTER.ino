// this function will map float within range into int within range, used to map PpM values to plot bars
int mapFloatToInt(float x, float in_min, float in_max, int out_min, int out_max) {
  return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

// horizontally plots logArray contents into sprite, starting with X,Y, W,H width and height, and color color;
// call plotLog(0, 0, 240, 135, TFT_WHITE, &img);
// minVal/maxVal to scale (if they are same - autoScale)
// This way i can scale array vertically to focus within actual range of stored values
void plotLogH(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color, float minVal, float maxVal, bool leftToRight, TFT_eSprite *sprite) {
  int plotArray[w] = { 0 };         // making array of ints of width size :) to store each plot data point
  int16_t inIndex = logArrayIndex;  // take index to latest filled log array item
  // auto scale
  if (minVal == maxVal) {
    int16_t i = inIndex;         // making our little temporary logArray index to play with
    minVal = maxPulsePerMinute;  // setting absurd values so they are out of scale
    maxVal = 0;
    for (int ix = 0; ix < w && ix < logArrayLength; ix++) {  // going back tru log to check each item is within scale
      if (logArray[i] < minVal) minVal = logArray[i];
      if (logArray[i] > maxVal) maxVal = logArray[i];
      if (i > 1) i--;               // deiterating index to parse thru log
      else i = logArrayLength - 1;  // or cycle it
    }
    if (maxVal < minVal + (h * pulPerMl)) {
      maxVal = minVal + (h * pulPerMl);
    }
  }
  // generating mapped plotArray
  for (int ix = 0; ix < w && ix < logArrayLength; ix++) {  // roll thru whole plot width, but stop if we exceed logArray length
    // mapping each logArray item into plotArray range for further plot
    plotArray[ix] = mapFloatToInt(logArray[inIndex], minVal, maxVal, 0, h);  // mapping log to plot
    if (inIndex > 1) inIndex--;                                              // deiterate log index
    else inIndex = logArrayLength - 1;                                       // or reset log index to last if we reached 0
  }

  // plotting in any direction (chatGPT)
  for (int ix = 0; ix < w - 1; ix++) {                                                                           // starting at 0 and going up to (w - 2), because we're drawing lines between points (need i and i+1).
    int i0 = leftToRight ? ix : (w - 1 - ix);                                                                    // if leftToRight is true, i0 = ix (normal order); else i0 = (w - 1 - ix), which reverses the direction
    int i1 = leftToRight ? (ix + 1) : (w - 2 - ix);                                                              // if leftToRight is true, i1 = ix + 1; else i1 = (w - 2 - ix), which pairs with i0 when going backwards.
    int y0 = (y + h) - plotArray[i0];                                                                            // plotting bottom-up, take bottom edge(y + h), and subtract plotArray[i0]
    int y1 = (y + h) - plotArray[i1];                                                                            // same for next point
    if (plotArray[i1] > 1) sprite->drawFastVLine(x + ix + 1, (y + h) - plotArray[i1], plotArray[i1], TFT_NAVY);  // draw background line if there's space for it under value
    sprite->drawWideLine(x + ix, y0, x + ix + 1, y1, 2, color);                                                  // This connects data point i0 to i1 on the graph.
  }

  // putting values on plot
  if (h > 20) {  // put values only if height is enough to fit them
    sprite->setTextSize(1);
    sprite->setTextColor(TFT_LIGHTGREY);
    float floatVal = maxVal / (pulPerMl * 1000);
    int xVal = x + w;
    int datumMod = 0;  // datum modifier (substract 2 to convert right to left)
    if (!leftToRight) {
      xVal = x;
      datumMod = -2;  // converts left datum to rigth if added
    }
    sprite->setTextDatum(TR_DATUM + datumMod);
    sprite->drawFloat(floatVal, 3, xVal, y, 1);
    floatVal = minVal / (pulPerMl * 1000);
    sprite->setTextDatum(BR_DATUM + datumMod);
    sprite->drawFloat(floatVal, 3, xVal, y + h, 1);
  }
  /*
  // plotting cycle, bars mode
  for (int ix = 0; ix < w; ix++) {
    sprite->drawFastVLine(x + ix, (y + h) - plotArray[ix], plotArray[ix], color);  // plotting vertical bars item by item
  }  */

  /*
  // plotting cycle, graph mode
  for (int ix = 0; ix < w - 1; ix++) {  // "w-1" - becaue we're drawing lines between nodes, not nodes
    if (ix == 0) {  // we can't ask for previous value to draw line
      sprite->drawLine(x + ix, (y + h) - plotArray[ix], x + ix + 1, (y + h) - plotArray[ix], color);
      //sprite->drawWideLine(x + ix, (y + h) - plotArray[ix], x + ix + 1, (y + h) - plotArray[ix], 2, color, color);
    } else {
      sprite->drawLine(x + ix, (y + h) - plotArray[ix - 1], x + ix + 1, (y + h) - plotArray[ix], color);
      //sprite->drawWideLine(x + ix, (y + h) - plotArray[ix - 1], x + ix + 1, (y + h) - plotArray[ix], 2, color, color);
    }
  }*/
}