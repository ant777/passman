#include "Menu.h"
#include "Icons.h"
#include "general_config.h"
#include <string>



Menu::Menu(TFT_eSPI& tft ) : tft(tft)  {};

void Menu::render(String label) {
      tft.fillScreen(bgColor);
      tft.setTextFont(1);
      tft.setTextColor(fontColor, fontColor);
      tft.drawString(FIRMWARE_VERSION, 118, 1); 
      tft.fillRoundRect(10 + currentInd * 50, 50, 40, 5, 2, TFT_WHITE);
      drawSVGKey_round(tft, 10, 17, TFT_WHITE);
      drawSVGCredit_card(tft, 62, 17);
      drawSVGNotepad_text(tft, 114, 17); 
      tft.setTextFont(2);
      tft.setTextColor(fontColor, fontColor);
      tft.drawString(labels[currentInd].c_str(), 10, 62); 
}

void Menu::setLabels(String labels[3]) {
  for (int i = 0; i < 3; ++i) {
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

  if (currentInd < 2) {
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
