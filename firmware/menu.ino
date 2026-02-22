
#include "menu_config.h"
const int menuItemsLength = 5;
String menuItems[menuItemsLength] = {/*MENU_ITEM_NEW*/MENU_ITEM_ENABLE_BT, /*MENU_ITEM_EDIT,*/MENU_ITEM_EDIT_PWD_REGENERATE,  MENU_ITEM_PREV_PWD, /*MENU_ITEM_DUMP_ITEM,*/ MENU_ITEM_DUMP_ALL, "Version: " + FIRMWARE_VERSION};

const int subMenuItemsLength = 5;
String subMenuItems[subMenuItemsLength] = {MENU_ITEM_EDIT_SERVICE, MENU_ITEM_EDIT_LOGIN, MENU_ITEM_EDIT_PWD_GENERATE, MENU_ITEM_EDIT_PWD_RULES, MENU_ITEM_EDIT_PWD_REGENERATE};

int currentMenuItem = 0;
bool useSubmenu = false;

String getCurrentMenuItem() {
  if (useSubmenu) {    
    return subMenuItems[currentMenuItem];
  }
  return menuItems[currentMenuItem];
}

void renderMainMenu() {
  currentMenuItem = 0;
  useSubmenu = false;
  renderMenu();
}
void redrawMainMenu() {
  renderMenu();
}

void renderSubMenu() {
  currentMenuItem = 0;
  useSubmenu = true;
  renderMenu();
}




void renderMenu() {
  tft.fillScreen(THEME_BG_COLOR);
  tft.setTextFont(2);
  
  static int16_t bx, by; static uint16_t bw, bh;
//  char* pch = strtok(sourceStr, ",");
  bh = tft.fontHeight();
  bx = 0;
  by = 2;
  tft.setTextDatum(TL_DATUM);
  
  if (useSubmenu) {
    
    for (int i = 0; i < subMenuItemsLength; i ++) {
      const String npp = String(i) + String(". ");
  
      if(i == currentMenuItem) {
        
        tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_HIGHLIGHT_COLOR);
      }
      if(subMenuItems[i] == MENU_ITEM_EDIT_PWD_RULES) {
        tft.drawString(npp + subMenuItems[i] + " " + currentPwdRule, bx, by + i*bh);  
      }else{
        tft.drawString(npp + subMenuItems[i], bx, by + i*bh);
      }
      tft.setTextColor(THEME_FONT_COLOR, THEME_FONT_COLOR);
    }
  } else {
    
    for (int i = 0; i < menuItemsLength; i ++) {
      const String npp = String(i) + String(". ");
  
      if(i == currentMenuItem) {
        
        tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_HIGHLIGHT_COLOR);
      }

        tft.drawString(npp + menuItems[i], bx, by + i*bh);
      tft.setTextColor(THEME_FONT_COLOR, THEME_FONT_COLOR);
    }
  }
}

void switchMenuItem() {
  const int prevMenuItem = currentMenuItem;
  if(!useSubmenu && currentMenuItem == menuItemsLength - 1) {
    currentMenuItem = 0;
  } else if(useSubmenu && currentMenuItem == subMenuItemsLength - 1) {
    currentMenuItem = 0;
  } else {
    currentMenuItem += 1;
  }
   static uint16_t bh = tft.fontHeight();
  tft.fillRect(0, 2+prevMenuItem*bh, 500, bh, THEME_BG_COLOR);
  tft.fillRect(0, 2+currentMenuItem*bh, 500, bh, THEME_BG_COLOR);
   
  tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_HIGHLIGHT_COLOR);
  const String npp = String(currentMenuItem) + String(". ");
  const String nppPrev = String(prevMenuItem) + String(". ");
  if(useSubmenu){
      if(subMenuItems[currentMenuItem] == MENU_ITEM_EDIT_PWD_RULES) {
        tft.drawString(npp + subMenuItems[currentMenuItem] + " " + currentPwdRule, 0, 2 + currentMenuItem*bh);
      }else{
        tft.drawString(npp + subMenuItems[currentMenuItem], 0, 2 + currentMenuItem*bh);
      }
      tft.setTextColor(THEME_FONT_COLOR, THEME_FONT_COLOR);
      if (subMenuItems[prevMenuItem] == MENU_ITEM_EDIT_PWD_RULES) {
        tft.drawString(nppPrev + subMenuItems[prevMenuItem] + " " + currentPwdRule, 0, 2 + prevMenuItem*bh);
      } else {
        tft.drawString(nppPrev + subMenuItems[prevMenuItem], 0, 2 + prevMenuItem*bh);
      }
  }else {
    tft.drawString(npp + menuItems[currentMenuItem], 0, 2 + currentMenuItem*bh);
    tft.setTextColor(THEME_FONT_COLOR, THEME_FONT_COLOR);
    tft.drawString(nppPrev + menuItems[prevMenuItem], 0, 2 + prevMenuItem*bh);
  }
}


int currentConfirmationItem = 1;
bool confirmationGranted() {
  return currentConfirmationItem == 0;
}
void switchConfirmationItem() {
  static int16_t bx, by; static uint16_t bw, bh;
//  char* pch = strtok(sourceStr, ",");
  bh = tft.fontHeight();
  bx = 0;
  by = 2;
  tft.fillRect(0, bh, 500, 2*bh, THEME_BG_COLOR);
  tft.fillRect(0, 2*bh, 500, 3*bh, THEME_BG_COLOR);
  if (currentConfirmationItem == 1) {
    currentConfirmationItem = 0;
    tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_HIGHLIGHT_COLOR);
    tft.drawString("Yes", 0, bh);
    tft.setTextColor(THEME_FONT_COLOR, THEME_FONT_COLOR);
    tft.drawString("No", 0, 2*bh);
  } else {
    currentConfirmationItem = 1;
    tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_HIGHLIGHT_COLOR);
    tft.drawString("No", 0, 2*bh);
    tft.setTextColor(THEME_FONT_COLOR, THEME_FONT_COLOR);
    tft.drawString("Yes", 0, bh);
  }

}
void renderConfirmation() {
  tft.fillScreen(THEME_BG_COLOR);
  tft.setTextFont(2);
  tft.setTextColor(THEME_FONT_COLOR, THEME_FONT_COLOR);
  static int16_t bx, by; static uint16_t bw, bh;
//  char* pch = strtok(sourceStr, ",");
  bh = tft.fontHeight();
  bx = 0;
  by = 2;
  tft.setTextDatum(TL_DATUM);
  tft.drawString("Allow BLE connect", 0, 0);
  tft.drawString("Yes", 0, bh);
  tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_HIGHLIGHT_COLOR);
  tft.drawString("No", 0, 2*bh);
}
