// this function will map float within range into int within range, used to map PpM values to plot bars
int mapFloatToInt(float x, float in_min, float in_max, int out_min, int out_max) {
  return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

// plots logArray contents into sprite, starting with X,Y, W,H width and height, and color color;
// call plotLog(0, 0, 240, 135, TFT_WHITE, &img);
// i think i have to add minVal/maxVal to properly window value scale.
// This way i can scale array vertically to focus within actual range of stored values
void plotLog(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color, float minVal, float maxVal, TFT_eSprite *sprite) {
  int plotArray[w] = { 0 };                            // making array of ints of width size :) to store each plot data point
  int16_t inIndex = logArrayIndex;                     // take index to latest filled log array item
  for (int x = 0; x < w && x < logArrayLength; x++) {  // roll thru whole plot width, but stop if we exceed logArray length
    // mapping each logArray item into plotArray range for further plot
    // (shouldn't i just plot it right here instead of extra array if i'd be a programmer?)
    plotArray[x] = mapFloatToInt(logArray[inIndex], 0.0, maxPulsePerMinute, 0, h);
    if (inIndex > 1) inIndex--;                // deiterate log index
    else logArrayIndex = logArrayLength - 1;  // or reset log index to last if we reached 0
  }



  // here i am about to put plotting cycle.
  // but now i concidering if i need to write separate vertical plotting function, or i can config this one



  sprite->fillRect(10, 10, 50, 50, TFT_RED);  //here is how to use sprite handler passed down
}