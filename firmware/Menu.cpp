#include "Menu.h"
#include "Icons.h"
#include "general_config.h"
#include <string>

String locale = "ru";

Menu::Menu(TFT_eSPI& tft ) : tft(tft)  {};
  // '7a32a96c-7e4e-4b01-92b0-c4a543a7ada7', 160x20px
const unsigned char epd_password_ru [] PROGMEM = {
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x82, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x20, 0x89, 0x19, 0x8d, 0x84, 0x88, 0xc0, 0x00, 0x00, 0x00, 0x20, 0x80, 0x90, 0x98, 
  0x84, 0x89, 0xc0, 0x00, 0x00, 0x00, 0x20, 0x83, 0x90, 0xd0, 0x48, 0x89, 0x40, 0x00, 0x00, 0x00, 
  0x20, 0x88, 0x90, 0xd0, 0x48, 0x8a, 0x40, 0x00, 0x00, 0x00, 0x20, 0x98, 0x90, 0x98, 0xc8, 0x8c, 
  0x40, 0x00, 0x00, 0x00, 0x20, 0x89, 0x99, 0x88, 0x88, 0x8c, 0x40, 0x00, 0x00, 0x00, 0x20, 0x8c, 
  0x9b, 0x07, 0x10, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void Menu::render(String label) {
      tft.fillScreen(bgColor);
      tft.setTextFont(1);
      tft.setTextColor(fontColor, fontColor);
      tft.drawString(FIRMWARE_VERSION, 118, 1); 
      tft.fillRoundRect(5 + currentInd * 40, 50, 30, 5, 2, TFT_WHITE);
      drawSVGKey_round(tft, 5, 20, TFT_WHITE);
      drawSVGRecents(tft, 36, 17);
      drawSVGCredit_card(tft, 80, 20);
      drawSVGNotepad_text(tft, 114, 20); 
//      drawSVGRecent_svgrepo_com(tft, 0 ,10);
      tft.setTextFont(2);
      tft.setTextColor(fontColor, fontColor);
      if (locale == "ru" && currentInd == 0) {
        tft.drawBitmap(10, 62, epd_password_ru, 80, 15, fontColor);
        
      } else {
        tft.drawString(labels[currentInd].c_str(), 10, 62);  
      }
}

void Menu::setLabels(String labels[4]) {
  for (int i = 0; i < 4; ++i) {
    this->labels[i] = labels[i];
  }
}
void Menu::setColor(uint16_t fontColor, uint16_t bgColor) {
  this->fontColor = fontColor;
  this->bgColor = bgColor;
}

int Menu::getCurrentInd() {
  return currentInd;
}

void Menu::next() {

  if (currentInd < 3) {
    currentInd += 1;
  } else {
    currentInd = 0;
  }

  this->render();
 
}

void Menu::open() {

 
}

void Menu::back() {

 
}
//

//void MyClass::drawSensorData(float value) {
//  _tft.fillScreen(TFT_BLACK);
//  _tft.setCursor(0, 0);
//  _tft.setTextColor(TFT_WHITE);
//  _tft.setTextSize(2);
//  _tft.print("Sensor: ");
//  _tft.print(value);
//}
//class Menu {
//  public:
//    Menu::Menu(TFT_eSPI& display) : _tft(display) {}
//    Menu(TFT_eSPI& display, uint16_t fontColor) {
//      this->tft = tft;
//      this->fontColor = fontColor;
//    }
//    int currentItem; 
//
//    void next() {
//      
//    }
//
//    void render(String label = "Password") {
//      
//      tft.setTextFont(1);
//      tft.setTextColor(fontColor, fontColor);
//      tft.drawString(FIRMWARE_VERSION, 118, 1); 
//      tft.fillRoundRect(2, 52, 30, 5, 2, TFT_WHITE);
//      drawSVGKey_round(tft, 0, 20, TFT_WHITE);
//      drawSVGCredit_card(tft, 50, 20);
//      drawSVGNotepad_text(tft, 100, 20); 
//      tft.setTextFont(2);
//      tft.setTextColor(fontColor, fontColor);
//      tft.drawString(label, 10, 62); 
//    }
//  private:
//    TFT_eSPI& tft;
//    uint16_t fontColor;
//    
//};
