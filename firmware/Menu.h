#ifndef Menu_h
#define Menu_h
#include <string>

#include <TFT_eSPI.h>
#include <Arduino.h> // Required for Arduino-specific types like Serial or String

struct MenuDescriptor{
  int itemsCount;
  String items[10];
};

class Menu {
  public:
    Menu(TFT_eSPI& tft);
//    int currentItem = 0;
    void render(String label = "Password");
    void next();
    void open();
    void back();
    int getCurrentInd();
    void setLabels(String labels[3]);
    void setColor(uint16_t fontColor, uint16_t bgColor);
  private:
    TFT_eSPI& tft;
    uint16_t fontColor;
    uint16_t bgColor;
    int currentInd = 0;
    String labels[3];
};

#endif
