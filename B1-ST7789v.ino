// Backlight control; 1 = on; 0 = off;
void setBacklight(bool state) {
  digitalWrite(TFT_BL, state ? HIGH : LOW);
}
// Backlight state; 1 = on; 0 = off;
bool getBacklight() {
  return (digitalRead(TFT_BL));
}

// turns st7789 into sleep mode about 10 µA comparing to 2-5mA in working mode.
void displaySleep() {
  tft.writecommand(ST7789_SLPIN);  // Sleep in
  delay(120);                      // Required delay after sleep command
}

// wakes up st7789 from sleep mode
void displayWake() {
  tft.writecommand(ST7789_SLPOUT);  // Sleep out
  delay(120);                       // Required delay after wake command
}


// display init routine
void TFT_init() {
  pinMode(TFT_BL, OUTPUT);  // TTGO T-Display enable Backlight pin 4
  setBacklight(1);          // T-Display turn on Backlight
  tft.init();               // eSPI variant
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  // Create an 8-bit sprite 135×240 pixels (uses ~32KB of RAM)
  img.setColorDepth(8);
  img.createSprite(TFT_HEIGHT, TFT_WIDTH);
  //img.setRotation(1); // rotates image, not the sprite
  // Fill Sprite with a "transparent" colour
  // TFT_TRANSPARENT is already defined for convenience
  // We could also fill with any colour as "transparent" and later specify that
  // same colour when we push the Sprite onto the screen.
  img.fillSprite(TFT_BLACK);  //TFT_TRANSPARENT
  //  img.deleteSprite();  // Delete it to free memory
}

// Transfer the canvas to the display
void printCanvas() {
  //img.pushSprite(0, 0, TFT_TRANSPARENT);  // Push sprite to TFT screen CGRAM at coordinate x,y (top left corner) Specify what colour is to be treated as transparent.
  img.pushSprite(0, 0);
  //img.fillSprite(TFT_TRANSPARENT);        // Fill Sprite with a TFT_TRANSPARENT colour
  img.fillSprite(TFT_BLACK);
  //img.deleteSprite();                     // Delete it to free memory
}




/*
📌 Basic Drawing Functions
Function	Description
img.fillScreen(color); // Fills the entire sprite with a color.
img.fillSprite(color); // Same as above (preferred for clearing).
img.drawPixel(x, y, color); // Draws a single pixel.
img.drawLine(x0, y0, x1, y1, color); // Draws a straight line.
img.drawFastHLine(x, y, width, color); // Draws a horizontal line.
img.drawFastVLine(x, y, height, color); // Draws a vertical line.
img.drawRect(x, y, w, h, color); // Draws an empty rectangle.
img.fillRect(x, y, w, h, color); // Draws a filled rectangle.
img.drawRoundRect(x, y, w, h, r, color); // Draws a rounded rectangle.
img.fillRoundRect(x, y, w, h, r, color); // Draws a filled rounded rectangle.
img.drawCircle(x, y, r, color); // Draws an empty circle.
img.fillCircle(x, y, r, color); // Draws a filled circle.
img.drawEllipse(x, y, rx, ry, color); // Draws an empty ellipse.
img.fillEllipse(x, y, rx, ry, color); // Draws a filled ellipse.
img.drawTriangle(x0, y0, x1, y1, x2, y2, color); // Draws an empty triangle.
img.fillTriangle(x0, y0, x1, y1, x2, y2, color); // Draws a filled triangle.
🖋️ Text Drawing Functions
Function	Description
img.setCursor(x, y); // Sets text position.
img.setTextColor(fg, bg); // Sets text foreground and background color.
img.setTextSize(size); // Sets text scaling (1 = default size).
img.setTextDatum(TC_DATUM); // Aligns text (e.g., TC_DATUM for top center).
img.print("Hello"); // Prints text to the img.
img.println("World"); // Prints text and moves to a new line.
img.drawString("Hello", x, y, font); // Draws a string at (x, y).
img.drawCentreString("Text", x, y, font); // Draws centered text at (x, y).
img.drawRightString("Text", x, y, font); // Draws right-aligned text at (x, y).
img.drawFloat(num, decimals, x, y, font); // Draws a floating-point number.
img.drawNumber(num, x, y, font); // Draws an integer.
🖼️ Image & Bitmap Functions
Function	Description
img.pushImage(x, y, w, h, data); // Draws a raw RGB565 image.
img.pushImage(x, y, w, h, data, transparent); // Draws an image with transparency.
img.pushRotated(sprite, angle, transparent); // Rotates and draws another img.
img.readRect(x, y, w, h, buffer); // Reads a portion of the sprite into a buffer.
🌟 Advanced Features
Function	Description
img.createSprite(width, height); // Creates a sprite (RAM buffer).
img.deleteSprite(); // Deletes the sprite (frees RAM).
img.pushSprite(x, y); // Uploads the buffer to the TFT.
img.pushSprite(x, y, transparent_color);
img.setRotation(0-3); // Rotates the sprite (0-3 for 0°, 90°, 180°, 270°).
img.setPivot(x, y); // Sets the pivot point for rotations.
*/