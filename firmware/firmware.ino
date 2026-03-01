//#include "FS.h"
//#include "SPI.h"
#include "SD_MMC.h"

//#include "driver/sdmmc_host.h"
//#include "driver/sdspi_host.h"
//#include "esp_vfs_fat.h"
#include "pin_config.h"
//#include "sdmmc_cmd.h"
#include "USBMSC.h"

#include "USB.h"
#include "USBHIDKeyboard.h"

//#include "ff.h" // FatFs include file
#include "TFT_eSPI.h" 
#include "OneButton.h" // https://github.com/mathertel/OneButton
#include <FastLED.h>  


#include "menu_config.h"
#include "general_config.h"



#include <iostream> // std::cout
#include <string>
#include <algorithm> // std::sort, std::copy
#include <iterator> // std::ostream_iterator
#include <sstream> // std::istringstream
#include <vector>
#include <cctype> // std::isdigit

#define LED_DI_PIN     40
#define LED_CI_PIN     39

CRGB leds[1];
CRGB colors[4] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};


#define TFT_LEDA_PIN   38
//#define SD_CS_PIN 10
#define MOUNT_POINT "/sdcard"
sdmmc_card_t *card;
USBHIDKeyboard Keyboard;
TFT_eSPI tft = TFT_eSPI();
#define MAX_LINES 50
#define MAX_LINE_LENGTH 256

#define BTN_PIN     0
OneButton button(BTN_PIN, true);
USBMSC MSC;
//FILE file;
//
//char feedRowParser() {
//  return file.read();
//}
//bool rowParserFinished() {
//  return ((file.available()>0)?false:true);
//}

int currentInd = 0;
String currentService;
String currentLogin;
String currentPwdRule;
String currentPwd;
String currentPrevPwd;



uint16_t THEME_BG_COLOR = THEME_DARK_BG_COLOR;
uint16_t THEME_HIGHLIGHT_COLOR = THEME_DARK_HIGHLIGHT_COLOR;
uint16_t THEME_FONT_COLOR = THEME_DARK_FONT_COLOR;
uint16_t THEME_TITLE_COLOR = THEME_DARK_TITLE_COLOR;




int prevScreenType = 0; 
int screenType = 0; // 0 - passwords, 1 - main menu, 2 - edit menu, 3 - edit screen input, 4 - ble confirmation

//USBCDC USBSerial;
// use this option for OSX.
// Comment it out if using Windows or Linux:
char ctrlKey = KEY_LEFT_GUI;
// use this option for Windows and Linux.
// leave commented out if using OSX:
//  char ctrlKey = KEY_LEFT_CTRL;
File root;
File getRoot() {
  return SD_MMC.open("/");
}

int line_count = 0;
char lines_array[MAX_LINES][MAX_LINE_LENGTH];
String parsedResult[4];
void parsePwdFile(String fileContents, bool toCurrent = true) {
//    FILE *f = fopen(filename, "a");
    char contents[512];
    strcpy(contents, fileContents.c_str());
    char* pch = strtok(contents, "\n");
    String parsedService = String(pch);
    pch = strtok(NULL, "\n");
    String parsedLogin = String(pch);
    pch = strtok(NULL, "\n");
    String parsedPwdRule = String(pch);
    char *last_token = NULL;
    char *prev_token = NULL;

    // Loop through all tokens, keeping track of the last one found
    while (pch != NULL) {
        prev_token = last_token;
        last_token = pch;        // Save the current token
        pch = strtok(NULL, "\n"); // Get the next token
    }
    String parsedPwd = String(last_token);
    String parsedPrevPwd = String(prev_token);
    parsedPwd.trim();
    parsedPrevPwd.trim();
    parsedPwdRule.trim();
    parsedService.trim();
    parsedLogin.trim();
    if (toCurrent) {
      currentService = parsedService;
      currentLogin = parsedLogin;
      currentPwdRule = parsedPwdRule;
      currentPrevPwd = parsedPrevPwd;
      currentPwd = parsedPwd;
    }
    parsedResult[0] = parsedService;
    parsedResult[1] = parsedLogin;
    parsedResult[2] = parsedPwdRule;
    parsedResult[3] = parsedPwd;
}

std::vector<std::string> filenames;
String getCurrentFileName() {
//  Keyboard.print('/' + filenames[currentInd].c_str());
   return '/' + String(filenames[currentInd].c_str());
//   return '/' + String(currentInd) + String(".pwd");
}

bool compareNat(const std::string& a, const std::string& b)
{
    if (a.empty())
        return true;
    if (b.empty())
        return false;
    if (std::isdigit(a[0]) && !std::isdigit(b[0]))
        return true;
    if (!std::isdigit(a[0]) && std::isdigit(b[0]))
        return false;
    if (!std::isdigit(a[0]) && !std::isdigit(b[0]))
    {
        if (std::toupper(a[0]) == std::toupper(b[0]))
            return compareNat(a.substr(1), b.substr(1));
        return (std::toupper(a[0]) < std::toupper(b[0]));
    }

    // Both strings begin with digit --> parse both numbers
    std::istringstream issa(a);
    std::istringstream issb(b);
    int ia, ib;
    issa >> ia;
    issb >> ib;
    if (ia != ib)
        return ia < ib;

    // Numbers are the same --> remove numbers and recurse
    std::string anew, bnew;
    std::getline(issa, anew);
    std::getline(issb, bnew);
    return (compareNat(anew, bnew));
}

void read_file_to_array(fs::FS &fs) {
    
    // --- 1. Mount the file system (SPIFFS example) ---
    // Make sure you have initialized SPIFFS somewhere in your app_main or setup function
    // For ESP-IDF setup, refer to the official documentation or tutorials.

    const char *filename = "/sdcard/file.txt";

    
//    File file = fs.open("/file.txt");
    // --- 2. Open the file for reading ---
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        ESP_LOGE("TAG", "Failed to open file for reading");
        return;
    }

    // --- 3. Read lines using fgets in a loop ---
    // fgets reads an entire line up to (n-1) characters or a newline character,
    // storing the result (including the newline) and appending a null terminator.
    while (fgets(lines_array[line_count], MAX_LINE_LENGTH, f) != NULL) {
        // Optional: Remove the trailing newline character '\n' for cleaner strings.
        size_t len = strlen(lines_array[line_count]);
      Serial.println(lines_array[line_count]);
        if (len > 0 && lines_array[line_count][len - 1] == '\n') {
            lines_array[line_count][len - 1] = '\0';
        }

        line_count++;
        
        // Stop if we reach the maximum number of lines our array can hold
        if (line_count >= MAX_LINES) {
            ESP_LOGW("TAG", "Reached max lines limit. Stopping read.");
            break;
        }
    }

    // --- 4. Close the file ---
    fclose(f);
    Serial.printf("File read successfully. Total lines: %d", line_count);

    // --- 5. Access the data stored in the array (for demonstration) ---
    for (int i = 0; i < line_count; i++) {
//      Serial.println("========");
//        Serial.printf("Line %d: %s", i + 1, lines_array[i]);
    }
}

void dump() {
}

const int MAX_FILES = 50;

struct FileInfo {
  char name[20]; // Buffer for the file name
  // Add other fields you want to sort by, e.g.,
  // unsigned long size;
  // time_t modifiedTime; 
};

FileInfo fileList[MAX_FILES]; // Array to hold file info
int fileCount = 0;

void scanDir() {
  File root = SD_MMC.open("/");// Read all filenames into the vector
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      break; // No more files
    }
    // Convert File entry name (char*) to std::string and add to vector
    filenames.push_back(std::string(entry.name())); 
    entry.close();
  }
  root.close();

}


void sortFiles() {
  std::sort(filenames.begin(), filenames.end(), compareNat);

}

void renderPass() {
  //  
  //  tft.init();
  //  tft.setRotation(1);
  tft.fillScreen(THEME_BG_COLOR);
  
  digitalWrite(TFT_LEDA_PIN, 0);
  tft.setTextFont(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  const String msg = "TEsT"; 
  static int16_t bx, by; static uint16_t bw, bh;
  static bool big = true;
  static uint16_t color = THEME_FONT_COLOR;
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(big ? 4 : 2);
  tft.setTextColor(color, THEME_FONT_COLOR);
  char sourceStr[512];
  strcpy(sourceStr, lines_array[currentInd]);
  //  !!
  size_t flength = filenames.size();
  if (flength < 1) {
    
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_BG_COLOR);
    bw = tft.textWidth("Empty");
    bh = tft.fontHeight();
      bx = (tft.width()  - (int)bw) / 2;
      by = (tft.height()  - (int)bh) / 2;
    tft.drawString("Empty", bx, by);
    return;
  }

  String fileName = getCurrentFileName();
//  Keyboard.print(fileList[48]);
  const char* fName = fileName.c_str();
  String readValue = readFile(SD_MMC, fName);
//    String readValue = readNextFile(root);

  if (readValue == "!" && currentInd != 0) {
    currentInd = 0;
    fileName = getCurrentFileName();
    const char* fName2 = fileName.c_str();
    root = getRoot();
    readValue = readNextFile(root);
  }
  if (readValue == "!") {
    Serial.println(lines_array[currentInd]);
    char* pch = strtok(sourceStr, ",");
    tft.setTextFont(4);
    tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_BG_COLOR);
    bw = tft.textWidth(pch);
    bh = tft.fontHeight();
    //  bx = (tft.width()  - (int)bw) / 2;
    bx = 0;
    by = 2;
    tft.setTextDatum(TL_DATUM);
    tft.drawString(pch, bx, by);
  
    pch = strtok(NULL, ",");
    tft.setTextFont(4);
    tft.setTextColor(THEME_TITLE_COLOR, THEME_BG_COLOR);
    bw = tft.textWidth(pch);
    bh = tft.fontHeight();
    //  bx = (tft.width() - (int)bw) / 2;
    bx = 0;
    by = (tft.height() - (int)bh) / 2;
    tft.setTextDatum(TL_DATUM);
    tft.drawString(pch, bx, by);
    pch = strtok(NULL, ",");
    pch = strtok(NULL, ",");
    //
    tft.setTextFont(4);
    tft.setTextColor(THEME_FONT_COLOR, THEME_BG_COLOR);
    bw = tft.textWidth(pch);
    bh = tft.fontHeight();
    //  bx = (tft.width() - (int)bw) / 2;       
    bx = 0;
    by = (tft.height() - (int)bh - 2);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(pch, bx, by);

  } else {
    
//  updatePwd(SD_MMC, fName, String("rnd2"), MENU_ITEM_EDIT_PWD_RULES);
//  
//  addPwd(SD_MMC, fName, String("test"));
  
    parsePwdFile(readValue);
    tft.setTextDatum(TL_DATUM);
    tft.setTextFont(4);
    tft.setTextColor(THEME_HIGHLIGHT_COLOR, THEME_BG_COLOR);
    bw = tft.textWidth(currentService);
    bh = tft.fontHeight();
    //  bx = (tft.width()  - (int)bw) / 2;
    bx = 0;
    by = 2;
    tft.drawString(currentService, bx, by);
  
    tft.setTextFont(4);
    tft.setTextColor(THEME_TITLE_COLOR, THEME_BG_COLOR);
    bw = tft.textWidth(currentLogin);
    bh = tft.fontHeight();
    //  bx = (tft.width() - (int)bw) / 2;
    bx = 0;
    by = (tft.height() - (int)bh) / 2;
    tft.drawString(currentLogin, bx, by);

    tft.setTextFont(4);
    tft.setTextColor(THEME_FONT_COLOR, THEME_BG_COLOR);
    bw = tft.textWidth(currentPwd);
    bh = tft.fontHeight();
    //  bx = (tft.width() - (int)bw) / 2;       
    bx = 0;
    by = (tft.height() - (int)bh - 2);
    tft.drawString(currentPwd, bx, by);
  }
  
  
}

bool hasUpdatesService = false;
bool hasUpdatesLogin = false;
bool hasUpdatesPwdRule = false;

void handleLongClick() {
  if (screenType == 0) {
    screenType = 1;
    renderMainMenu();
  } else if (screenType == 2) {
    String fileName = getCurrentFileName();
    const char* fName = fileName.c_str();
    if (hasUpdatesService) {
      updatePwd(SD_MMC, fName, currentService, MENU_ITEM_EDIT_SERVICE);
    }
    if (hasUpdatesLogin) {
      updatePwd(SD_MMC, fName, currentLogin, MENU_ITEM_EDIT_LOGIN);
    }
    if (hasUpdatesPwdRule) {
      updatePwd(SD_MMC, fName, currentPwdRule, MENU_ITEM_EDIT_PWD_RULES);
      
    }
    hasUpdatesService = false;
    hasUpdatesLogin = false;
    hasUpdatesPwdRule = false;
    
    screenType = 1;
    renderMainMenu();
  } else if (screenType == 3) {
    screenType = 2;
    if (getEditTarget() == MENU_ITEM_EDIT_SERVICE) {
      const String inputResult = getInputResult();
      if (currentService != inputResult) {
        hasUpdatesService = true;
        currentService = inputResult;
      }
    } else if (getEditTarget() == MENU_ITEM_EDIT_LOGIN) {
      const String inputResult = getInputResult();
      if (currentLogin != inputResult) {
        hasUpdatesLogin = true;
        currentLogin = inputResult;
      }
    }
    renderSubMenu();
  } else {
    screenType = 0;
    renderPass();
  }
//  Serial.println(lines_array[currentInd]);
//  char sourceStr[512];
//  strcpy(sourceStr, lines_array[currentInd]);
//  char* pch1 = strtok(sourceStr, ",");
//  Keyboard.print(pch1);
//  Keyboard.print(",");
//  pch1 = strtok(NULL, ",");
//  Keyboard.print(pch1);
//  Keyboard.print(",");
//  pch1 = strtok(NULL, ",");
//  pch1 = strtok(NULL, ",");
//  Keyboard.print(pch1);
  return;
}

void handleClick() {
  if(screenType == 0) {
    size_t length = filenames.size();
    if (currentInd < length - 1) {

      currentInd += 1;
    } else {
      
      currentInd = 0;
    }
    renderPass();
    updateCurrentStateBle();
    // Toggle built-in LED on a single click
  //  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  } else if (screenType == 3) {
    //
    addCurrentLetterToInputResult();
  } else if (screenType == 4) {
    //
    switchConfirmationItem();
  } else {
    switchMenuItem();
  }
}
void handleIncomingBLERequest() {
  screenType = 4;
  renderConfirmation();
}

void handleMultiClick() {
  int clicks = button.getNumberClicks();
  if (clicks == 3) {
    if (screenType == 0) {
      Keyboard.print(currentLogin);
    }
  }
  if (clicks == 4) {
    if (screenType == 0) {
      Keyboard.print(currentPrevPwd);
    }
  }
}

void handleDoubleClick() {
  if (screenType == 0) {
  
      Keyboard.print(currentPwd);
      return;
    //  Serial.println("Double Clicked!");
    //  // Turn built-in LED off on a double click
    //  digitalWrite(LED_BUILTIN, LOW);
  } else if(screenType == 1) {


    if (getCurrentMenuItem() == MENU_ITEM_NEW) {
      createNewPwd("???", "???", "", "");
    } else if (getCurrentMenuItem() == MENU_ITEM_ENABLE_BT) {
      setupBLEServer();
    } else if (getCurrentMenuItem() == MENU_ITEM_EDIT) {
      screenType = 2;
      renderSubMenu();
    } else if (getCurrentMenuItem() == MENU_ITEM_PREV_PWD) {
      Keyboard.print(currentPrevPwd);    
    }  else if (getCurrentMenuItem() == MENU_ITEM_DUMP_ITEM) {
      dumpItem(currentService, currentLogin, currentPwd);
    } else if (getCurrentMenuItem() == MENU_ITEM_DUMP_ALL) {
      dumpAll(SD_MMC);
    } else  {
      listDir(SD_MMC, "/", 0);  
    }
    
//    const char *filename = "/sdcard/file.txt";
//    // --- 2. Open the file for reading ---
//    FILE *f = fopen(filename, "r+");
//    const char *data_to_write = "!";
//    fseek(f, 2, SEEK_SET);
//    fputs(data_to_write, f);
//    // --- 4. Close the file ---
//    fclose(f);
    
  } else if(screenType == 2) {
    if (getCurrentMenuItem() == MENU_ITEM_EDIT_SERVICE) {
      screenType = 3;
      setEditTarget(MENU_ITEM_EDIT_SERVICE);
      renderInput();
    }  else if (getCurrentMenuItem() == MENU_ITEM_EDIT_PWD_REGENERATE) {
      String fileName = getCurrentFileName();
      const char* fName = fileName.c_str();
      String newVal = generateRandomString(currentPwdRule);
      updatePwd(SD_MMC, fName, newVal, MENU_ITEM_EDIT_PWD_REGENERATE);
      
    }  else if (getCurrentMenuItem() == MENU_ITEM_EDIT_PWD_GENERATE) {
      String newVal = generateRandomString(currentPwdRule);
      String fileName = getCurrentFileName();
      const char* fName = fileName.c_str();
      addPwd(SD_MMC, fName, newVal);
    }  else if (getCurrentMenuItem() == MENU_ITEM_EDIT_PWD_RULES) {
      
      if (currentPwdRule == PWD_RULE_1) {
        currentPwdRule = PWD_RULE_2;
      } else if (currentPwdRule == PWD_RULE_2) {
        currentPwdRule = PWD_RULE_1;
      }
      hasUpdatesPwdRule = true;
      redrawMainMenu();

    }  else if (getCurrentMenuItem() == MENU_ITEM_EDIT_LOGIN) {
      screenType = 3;
      setEditTarget(MENU_ITEM_EDIT_LOGIN);
      renderInput();
    }
  } else if(screenType == 3) {
    //
    editPrevLetterOfInputResult();
  } else if (screenType == 4) {
    //
    if (confirmationGranted()){
      allowConnection();
    } else {
      // 
    }
    screenType = 0;
    renderPass();
  }
}

String getCurrentDataString() {
  return currentService + "\n" + currentLogin + "\n" + currentPwdRule;
}

void setup() {
  // make pin 0 an input and turn on the pull-up resistor so it goes high unless
  // connected to ground:
  Serial.begin(115200);
    // Seed the random number generator using an unconnected analog pin
  randomSeed(analogRead(0));
//  pinMode(0, INPUT_PULLUP);
  // initialize control over the keyboard:
  Keyboard.begin();
  USB.begin();
  Serial.println("Start");

  tft.init();
  tft.setRotation(-1);
  tft.fillScreen(THEME_BG_COLOR);
  digitalWrite(TFT_LEDA_PIN, 0);

    FastLED.addLeds<APA102, LED_DI_PIN, LED_CI_PIN, BGR>(leds, 1);  // BGR ordering is typical
    FastLED.setBrightness(100);
        leds[0] = colors[2];

  sd_init();
  scanDir();
  sortFiles();
  root = getRoot();
  //  read_file_to_array(SD_MMC);
  renderPass();
  button.attachLongPressStop(handleLongClick);
  button.attachDoubleClick(handleDoubleClick);
  button.attachMultiClick(handleMultiClick);
  button.attachClick(handleClick);

  // Keyboard.print(generateRandomString("a-z||2-4"));
  return;
}


String newServiceName = "";
String newLogin = "";
String newPwdRule = "";
String newPwd = "";
bool enqueueRemoveCurrent = false;
bool enqueueCreate = false;
bool enqueueUpdate = false;

void setEnqueueRemoveCurrent() {
  enqueueRemoveCurrent = true;
}

void enqueueUpdatePwd(String nServiceName, String nLogin, String nPwdRule) {
  newServiceName = nServiceName;
  newLogin = nLogin;
  newPwdRule = nPwdRule;
  enqueueUpdate = true;
}

void enqueueNewPwd(String nServiceName, String nLogin, String nPwdRule, String nPwd) {
  newServiceName = nServiceName;
  newLogin = nLogin;
  newPwdRule = nPwdRule;
  newPwd = nPwd;
  enqueueCreate = true;
}

void loop() {
  button.tick();
  if (enqueueRemoveCurrent) {
    enqueueRemoveCurrent = false;
//    const char* cFileName = getCurrentFileName().c_str();
    filenames.erase(filenames.begin() + currentInd);
//    String fname = "/" + String(currentInd) + ".pwd";
    deleteFile(getCurrentFileName().c_str());
    if (currentInd > 0) {
      currentInd -= 1; 
    }
    renderPass();
    updateCurrentStateBle();
  }
  if (enqueueCreate && newServiceName) {
    enqueueCreate = false;
    String newName = createNewPwd(newServiceName, newLogin, newPwdRule, newPwd);
    newServiceName = "";
    newLogin = "";
    newPwdRule = "";
    newPwd = "";

    filenames.push_back(std::string(newName.substring(1).c_str())); 
    size_t length = filenames.size();
    currentInd = length - 1;
    renderPass();
    updateCurrentStateBle();
  }
  if (enqueueUpdate && newServiceName) {
    enqueueUpdate = false;
    const char* fName = getCurrentFileName().c_str();
    updatePwdData(SD_MMC, fName, newServiceName, newLogin, newPwdRule);
    newServiceName = "";
    newLogin = "";
    newPwdRule = "";
  }
  if (screenType == 3) {
    tickInput();
  }
  delay(5);
}
