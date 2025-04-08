// this function will map float within range into int within range, used to map PpM values to plot bars
int mapFloatToInt(float x, float in_min, float in_max, int out_min, int out_max) {
  return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

// ChatGPT-070425 unified log plotting function that handles both vertical and horizontal plotting based on `vertical` flag.
// It replaces two separate functions: plotLogH and plotLogV.
// Draws logArray into sprite at (x,y), with width w and height h, using given color.
// Uses minVal/maxVal for scaling (if equal, auto-scale is used).
// `lessToMore` controls direction: left->right (horizontal) or top->bottom (vertical).
void plotLog(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color, float minVal, float maxVal, bool vertical, bool lessToMore, TFT_eSprite *sprite) {
  int plotSize = vertical ? h : w;                 // how many points to plot
  int plotArray[plotSize] = {0};                   // array to store scaled values to be plotted
  int16_t logIndex = logArrayIndex;                // start from latest entry in the log

  // Auto-scale min and max if needed
  if (minVal == maxVal) {
    minVal = maxPulsePerMinute;
    maxVal = 0;
    int i = logIndex;
    for (int p = 0; p < plotSize && p < logArrayLength; p++) {
      if (logArray[i] < minVal) minVal = logArray[i];
      if (logArray[i] > maxVal) maxVal = logArray[i];
      if (i > 1) i--; else i = logArrayLength - 1;
    }
    float autoRange = (vertical ? w : h) * pulPerMl;
    if (maxVal < minVal + autoRange) maxVal = minVal + autoRange;
  }

  // Map log values into plotArray
  for (int i = 0; i < plotSize && i < logArrayLength; i++) {
    plotArray[i] = mapFloatToInt(logArray[logIndex], minVal, maxVal, 0, vertical ? w : h);
    if (logIndex > 1) logIndex--; else logIndex = logArrayLength - 1;
  }

  // Plotting the graph
  for (int i = 0; i < plotSize - 1; i++) {
    int idx0 = lessToMore ? i : (plotSize - 1 - i);
    int idx1 = lessToMore ? (i + 1) : (plotSize - 2 - i);

    if (vertical) {
      int x0 = (x + w) - plotArray[idx0];
      int x1 = (x + w) - plotArray[idx1];
      if (plotArray[idx1] > 1)
        sprite->drawFastHLine(x1, y + i + 1, plotArray[idx1], TFT_NAVY);  // Background line
      sprite->drawWideLine(x0, y + i, x1, y + i + 1, 2, color);           // Main plot line
    } else {
      int y0 = (y + h) - plotArray[idx0];
      int y1 = (y + h) - plotArray[idx1];
      if (plotArray[idx1] > 1)
        sprite->drawFastVLine(x + i + 1, (y + h) - plotArray[idx1], plotArray[idx1], TFT_NAVY);  // Background line
      sprite->drawWideLine(x + i, y0, x + i + 1, y1, 2, color);                                    // Main plot line
    }
  }

  // Draw min/max labels if space allows
  if ((vertical && w > (5 * 6 * 2)) || (!vertical && h > 20)) {
    sprite->setTextSize(1);
    sprite->setTextColor(TFT_LIGHTGREY);

    float maxScaled = maxVal / (pulPerMl * 1000);
    float minScaled = minVal / (pulPerMl * 1000);
    int8_t afterdotMax = (maxScaled == 0.0) ? 0 : 3;
    int8_t afterdotMin = (minScaled == 0.0) ? 0 : 3;

    if (vertical) {
      int yLabel = lessToMore ? (y + h) : y;
      int datumMod = lessToMore ? 0 : -6;
      sprite->setTextDatum(BL_DATUM + datumMod);
      sprite->drawFloat(maxScaled, afterdotMax, x, yLabel, 1);
      sprite->setTextDatum(BR_DATUM + datumMod);
      sprite->drawFloat(minScaled, afterdotMin, x + w, yLabel, 1);
    } else {
      int xLabel = lessToMore ? (x + w) : x;
      int datumMod = lessToMore ? 0 : -2;
      sprite->setTextDatum(TR_DATUM + datumMod);
      sprite->drawFloat(maxScaled, afterdotMax, xLabel, y, 1);
      sprite->setTextDatum(BR_DATUM + datumMod);
      sprite->drawFloat(minScaled, afterdotMin, xLabel, y + h, 1);
    }
  }
}

/*
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

    // prepare for direction change
    int xVal = x + w;    // plot values rightmost if leftToRight
    int datumMod = 0;    // datum modifier (substract 2 to convert right to left)
    if (!leftToRight) {  // if not leftToRight
      xVal = x;          // plot values leftmost
      datumMod = -2;     // converts left datum to rigth if added
    }

    // print maximum plot value
    int8_t afterdot = 3;  // how many numbers after dot to show
    float floatVal = maxVal / (pulPerMl * 1000);
    if (floatVal == 0.0) afterdot = 0;
    sprite->setTextDatum(TR_DATUM + datumMod);
    sprite->drawFloat(floatVal, afterdot, xVal, y, 1);

    //print minimum plot value
    floatVal = minVal / (pulPerMl * 1000);
    if (floatVal == 0.0) afterdot = 0;
    else afterdot = 3;
    sprite->setTextDatum(BR_DATUM + datumMod);
    sprite->drawFloat(floatVal, afterdot, xVal, y + h, 1);
  }
}
// vertically plots logArray contents into sprite, starting with X,Y, W,H width and height, and color color;
// call plotLog(0, 0, 240, 135, TFT_WHITE, &img);
// minVal/maxVal to scale (if they are same - autoScale)
void plotLogV(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color, float minVal, float maxVal, bool topToBottom, TFT_eSprite *sprite) {
  int plotArray[h] = { 0 };         // making array of ints of height size :) to store each plot data point
  int16_t inIndex = logArrayIndex;  // take index to latest filled log array item
  // auto scale
  if (minVal == maxVal) {
    int16_t i = inIndex;         // making our little temporary logArray index to play with
    minVal = maxPulsePerMinute;  // setting absurd values so they are out of scale
    maxVal = 0;
    for (int iy = 0; iy < h && iy < logArrayLength; iy++) {  // going back tru log to check each item is within scale
      if (logArray[i] < minVal) minVal = logArray[i];
      if (logArray[i] > maxVal) maxVal = logArray[i];
      if (i > 1) i--;               // deiterating index to parse thru log
      else i = logArrayLength - 1;  // or cycle it
    }
    if (maxVal < minVal + (w * pulPerMl)) {
      maxVal = minVal + (w * pulPerMl);
    }
  }
  // generating mapped plotArray
  for (int iy = 0; iy < h && iy < logArrayLength; iy++) {  // roll thru whole plot width, but stop if we exceed logArray length
    // mapping each logArray item into plotArray range for further plot
    plotArray[iy] = mapFloatToInt(logArray[inIndex], minVal, maxVal, 0, w);  // mapping log to plot
    if (inIndex > 1) inIndex--;                                              // deiterate log index
    else inIndex = logArrayLength - 1;                                       // or reset log index to last if we reached 0
  }

  // plotting in any direction (chatGPT)
  for (int iy = 0; iy < h - 1; iy++) {                                                      // starting at 0 and going up to (w - 2), because we're drawing lines between points (need i and i+1).
    int i0 = topToBottom ? iy : (h - 1 - iy);                                               // if topToBottom is true, i0 = ix (normal order); else i0 = (w - 1 - ix), which reverses the direction
    int i1 = topToBottom ? (iy + 1) : (h - 2 - iy);                                         // if topToBottom is true, i1 = ix + 1; else i1 = (w - 2 - ix), which pairs with i0 when going backwards.
    int x0 = (x + w) - plotArray[i0];                                                       // plotting bottom-up, take bottom edge(y + h), and subtract plotArray[i0]
    int x1 = (x + w) - plotArray[i1];                                                       // same for next point
    if (plotArray[i1] > 1) sprite->drawFastHLine(x1, y + iy + 1, plotArray[i1], TFT_NAVY);  // draw background line if there's space for it under value    //(x + ix + 1, (y + h) - plotArray[i1], plotArray[i1], TFT_NAVY);
    sprite->drawWideLine(x0, y + iy, x1, y + iy + 1, 2, color);                             // This connects data point i0 to i1 on the graph.    //(x + ix, y0, x + ix + 1, y1, 2, color);
  }

  // putting values on plot
  if (w > (5 * 6 * 2)) {  // put values only if width is suficient to fit them (5symb*6pix*2times) (6x8pix font1, size1)
    sprite->setTextSize(1);
    sprite->setTextColor(TFT_LIGHTGREY);

    // prepare for direction change
    int yVal = y + h;  // set to bottom if plot starts from top
    int datumMod = 0;  // datum modifier (substract 6 to convert bottom to top)
    if (!topToBottom) {
      yVal = y;
      datumMod = -6;  // converts bottom (6,7,8) datum to top (1,2,3) if added
    }
    // print maximum plot value
    int8_t afterdot = 3;  // how many numbers after dot to show
    float floatVal = maxVal / (pulPerMl * 1000);
    if (floatVal == 0.0) afterdot = 0;
    sprite->setTextDatum(BL_DATUM + datumMod);
    sprite->drawFloat(floatVal, afterdot, x, yVal, 1);

    //print minimum plot value
    floatVal = minVal / (pulPerMl * 1000);
    if (floatVal == 0.0) afterdot = 0;
    else afterdot = 3;
    sprite->setTextDatum(BR_DATUM + datumMod);
    sprite->drawFloat(floatVal, afterdot, x + w, yVal, 1);
  }

  /*
  // plotting cycle, bars mode
  for (int ix = 0; ix < w; ix++) {
    sprite->drawFastVLine(x + ix, (y + h) - plotArray[ix], plotArray[ix], color);  // plotting vertical bars item by item
   }  
   // plotting cycle, graph mode
   for (int ix = 0; ix < w - 1; ix++) {  // "w-1" - becaue we're drawing lines between nodes, not nodes
    if (ix == 0) {  // we can't ask for previous value to draw line
      sprite->drawLine(x + ix, (y + h) - plotArray[ix], x + ix + 1, (y + h) - plotArray[ix], color);
      //sprite->drawWideLine(x + ix, (y + h) - plotArray[ix], x + ix + 1, (y + h) - plotArray[ix], 2, color, color);
    } else {
      sprite->drawLine(x + ix, (y + h) - plotArray[ix - 1], x + ix + 1, (y + h) - plotArray[ix], color);
      //sprite->drawWideLine(x + ix, (y + h) - plotArray[ix - 1], x + ix + 1, (y + h) - plotArray[ix], 2, color, color);
    }
   */


