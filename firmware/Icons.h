#ifndef Icons_h
#define Icons_h

    void drawSVGKey_round(TFT_eSPI &tft, int16_t x_offset, int16_t y_offset, uint16_t color) {
      // Element 1: path
      // Path drawing (simplified)
    //  tft.drawLine(x_offset + 11, y_offset + 10, x_offset + 2, y_offset + 19, color);
    //  tft.drawLine(x_offset + 12, y_offset + 11, x_offset + 3, y_offset + 20, color);
    //  tft.drawLine(x_offset + 2, y_offset + 19, x_offset + 5, y_offset + 23, color);
    //  tft.drawLine(x_offset + 3, y_offset + 19, x_offset + 6, y_offset + 22, color);
    //  tft.drawLine(x_offset + 4, y_offset + 19, x_offset + 7, y_offset + 21, color);
    //  tft.drawLine(x_offset + 6, y_offset + 21, x_offset + 6, y_offset + 20, TFT_WHITE);
    //  tft.drawLine(x_offset + 6, y_offset + 20, x_offset + 7, y_offset + 20, TFT_WHITE);
    //  tft.drawLine(x_offset + 7, y_offset + 20, x_offset + 7, y_offset + 19, TFT_WHITE);
    //  tft.drawLine(x_offset + 7, y_offset + 19, x_offset + 7, y_offset + 19, TFT_WHITE);
    
      // Element 1: rect
      tft.fillRect(x_offset + 2, y_offset + 15, 18, 3,  TFT_WHITE);
      tft.fillRect(x_offset + 4, y_offset + 15, 3, 8,  TFT_WHITE);
      // Element 2: circle
      tft.fillCircle(x_offset + 30, y_offset + 15, 2, color);
      tft.drawCircle(x_offset + 29, y_offset + 15  , 10, color);
    
    }
    void drawSVGCredit_card(TFT_eSPI &tft, int16_t x_offset, int16_t y_offset) {
      // Element 1: rect
      tft.drawRoundRect(x_offset + 2, y_offset + 5, 30, 21, 2, TFT_WHITE);
    
      // Element 2: line
      tft.drawLine(x_offset + 2, y_offset + 12, x_offset + 32, y_offset + 12, TFT_WHITE);
    
    }
    
    void drawSVGNotepad_text(TFT_eSPI &tft, int16_t x_offset, int16_t y_offset) {
      // Element 1: path
      // Path drawing (simplified)
      tft.drawLine(x_offset + 8, y_offset + 2, x_offset + 8, y_offset + 6, TFT_WHITE);
    
      // Element 2: path
      // Path drawing (simplified)
      tft.drawLine(x_offset + 12, y_offset + 2, x_offset + 12, y_offset + 6, TFT_WHITE);
    
      // Element 3: path
      // Path drawing (simplified)
      tft.drawLine(x_offset + 16, y_offset + 2, x_offset + 16, y_offset + 6, TFT_WHITE);
    
      // Element 4: rect
      tft.drawRoundRect(x_offset + 4, y_offset + 4, 25, 21, 2, TFT_WHITE);
    
      // Element 5: path
      // Path drawing (simplified)
      tft.drawLine(x_offset + 8, y_offset + 10, x_offset + 21, y_offset + 10, TFT_WHITE);
    
      // Element 6: path
      // Path drawing (simplified)
      tft.drawLine(x_offset + 8, y_offset + 14, x_offset + 21, y_offset + 14, TFT_WHITE);
    
      // Element 7: path
      // Path drawing (simplified)
      tft.drawLine(x_offset + 8, y_offset + 18, x_offset + 21, y_offset + 18, TFT_WHITE);
    
    }

    
#endif
