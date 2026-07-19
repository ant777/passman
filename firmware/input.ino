#include "general_config.h"

String lowercaseLetters = "abcdefghijklmnopqrstuvwxyz";
String uppercaseLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
String numbers = "0123456789";
String symbols = "_-@.!$&?";

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
    if (parsedPwdRule.indexOf("SYM") != -1) {
      resPossibleStrings += symbols;
      parsedPwdRule.replace("SYM", "");
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
//  Keyboard.print(parsedPwdRuleMinLength);
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
  if(pwdRule.indexOf("SYM") != -1) {
    int r = random(0, symbols.length());
    char randSym = symbols[r] ;
    int ind = random(0, randString.length());
    randString[ind] = randSym;
  }
  return randString;
}
