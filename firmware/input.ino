#include "general_config.h"

static uint32_t last = 0;
int currentSymbol = -1;
int nextSymbol = 0;
String lowercaseLetters = "abcdefghijklmnopqrstuvwxyz";
String uppercaseLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
String numbers = "0123456789";
String symbols = "_-@.!$&?";

char inputResult[MAX_STRING_SIZE+1];
int editingPos = 0;

String editTarget;

String inputSympbols = lowercaseLetters + numbers + symbols;
const int inputSympbolsLength = inputSympbols.length();

//a-zA-Z0-9!@#$_-||8-10
String getPossibleString(String pwdRule) {
  char contents[MAX_STRING_SIZE];
  strcpy(contents, pwdRule.c_str());
  String parsedPwdRule = String(strtok(contents, "||"));
  
  if (parsedPwdRule == PWD_RULE_1) {
      return lowercaseLetters + uppercaseLetters + numbers + symbols;
  } else if(parsedPwdRule == PWD_RULE_2) {
      return lowercaseLetters + uppercaseLetters + numbers + String("_");
  } else {
    String resPossibleStrings = "";
    if (parsedPwdRule.indexOf("a-z") != -1) {
      resPossibleStrings += lowercaseLetters;
      parsedPwdRule.replace("a-z", "");
    }
    if (parsedPwdRule.indexOf("A-Z") != -1) {
      resPossibleStrings += uppercaseLetters;
      parsedPwdRule.replace("A-Z", "");
    }
    if (parsedPwdRule.indexOf("0-9") != -1) {
      resPossibleStrings += numbers;
      parsedPwdRule.replace("0-9", "");
    }
    return resPossibleStrings + parsedPwdRule;
  }
}

unsigned int getPasswordLength(String pwdRule) {
  int minL = 10;
  int maxL = 16;
  
  char contents[MAX_STRING_SIZE];
  strcpy(contents, pwdRule.c_str());
  strtok(contents, "||");
  char * pch = strtok (NULL, "||");
  if (pch == NULL) { 
    return random(minL, maxL);
  }
  String parsedPwdRuleLength = String(pch);

  strcpy(contents, parsedPwdRuleLength.c_str());
  
  String parsedPwdRuleMinLength = String(strtok(contents, "-"));
  String parsedPwdRuleMaxLength = String(strtok(NULL, "-"));
  Keyboard.print(parsedPwdRuleMinLength);
  if (parsedPwdRuleMinLength.toInt()) {
    minL = parsedPwdRuleMinLength.toInt();
  }
  if (parsedPwdRuleMaxLength.toInt()) {
    maxL = parsedPwdRuleMaxLength.toInt();
  }
  

  return random(minL, maxL);
}

String generateRandomString(String pwdRule) {
  unsigned int len = getPasswordLength(pwdRule);
  String randString = "";
  String possible = getPossibleString(pwdRule);
  int possibleLength = possible.length();
  // Optional: reserve memory to prevent fragmentation, especially with long strings
  // randString.reserve(len); 

  for (unsigned int i = 0; i < len; i++) {
    int r = random(0, possibleLength);
    randString += possible[r];
  }
  return randString;
}


void setEditTarget(String newTarget) {
  editTarget = newTarget;
}
String getEditTarget(void) {
  return editTarget;
}
String getInputResult(void) {
  return String(inputResult);
}

void renderInput() {
  tft.fillScreen(THEME_BG_COLOR);
  tft.setTextFont(2);
}

void addCurrentLetterToInputResult() {
  inputResult[editingPos] = inputSympbols.charAt(currentSymbol);
  if (editingPos < MAX_STRING_SIZE - 1) { 
    editingPos+=1;
  }
  currentSymbol = -1;
  nextSymbol = 0;
  redrawResult();
}

void editPrevLetterOfInputResult() {
  if (editingPos > 0) {
    
    editingPos-=1;
  }
  inputResult[editingPos] = ' ';
  currentSymbol = -1;
  nextSymbol = 0;
  redrawResult();
}

void tickInput() {
  
    if( millis() - last > INPUT_AUTO_SWITCH_TIMEOUT ) {
      last = millis();
      if (currentSymbol + 1 >= inputSympbolsLength) {
        currentSymbol = 0;
      } else {
        currentSymbol += 1;
      }
      if (nextSymbol + 1 >= inputSympbolsLength) {
        nextSymbol = 0;
      } else {
        nextSymbol += 1;
      }
      redrawInput();
    }
}


void redrawResult() {
  static uint16_t bw, bh;
  tft.setTextFont(2);
  tft.setTextColor(THEME_FONT_COLOR, THEME_FONT_COLOR);
  bh = tft.fontHeight();
  bw = tft.textWidth(String(inputResult));
  tft.fillRect(0, 0, 500, bh, THEME_BG_COLOR);
  tft.drawString(String(inputResult), 0, 0);
}


void redrawInput() {
  static uint16_t bw, bh;
  tft.setTextFont(2);
  bh = tft.fontHeight();
  tft.fillRect(0, bh, 500, 500, THEME_BG_COLOR);
  tft.setTextFont(4);
  tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_HIGHLIGHT_COLOR);
  tft.drawString(String(inputSympbols.charAt(currentSymbol)), 5, bh);
  tft.setTextColor(THEME_FONT_COLOR, THEME_FONT_COLOR);
  bw = tft.textWidth(String(inputSympbols.charAt(currentSymbol)));
  tft.drawString(String(inputSympbols.charAt(nextSymbol)), bw+10, bh);
}
