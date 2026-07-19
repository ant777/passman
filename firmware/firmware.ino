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
#include "Menu.h"
#include "USBHIDKeyboard.h"

//#include "ff.h" // FatFs include file
#include "TFT_eSPI.h" 
#include "OneButton.h" // https://github.com/mathertel/OneButton
#include <FastLED.h>  


#include "menu_config.h"
#include "general_config.h"
#include "color_config.h"
#include "locale.h"


#include <iostream> // std::cout
#include <string>
#include <algorithm> // std::sort, std::copy
#include <iterator> // std::ostream_iterator
#include <sstream> // std::istringstream
#include <vector>
#include <cctype> // std::isdigit


CRGB leds[1];
CRGB colors[4] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};


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


const String LOCALIZED[1][5] = {
  {"Yes", "No", "Passwords", "Credit Cards", "Notes"}
};

String LABEL_YES = "";
String LABEL_NO = "";
String LABEL_PASSWORDS = "";
String LABEL_CREDIT_CARDS = "";
String LABEL_NOTES = "";

Menu sectionsMenu(tft);
void loadLocale(int localeInd = 0) {
  LABEL_YES = LOCALIZED[localeInd][0];
  LABEL_NO = LOCALIZED[localeInd][1];
  LABEL_PASSWORDS = LOCALIZED[localeInd][2];
  LABEL_CREDIT_CARDS = LOCALIZED[localeInd][3];
  LABEL_NOTES = LOCALIZED[localeInd][4];
}

int prevScreenType = 0; 
int screenType = -1; // -1 - icons menu, 0 - passwords, 1 - main menu, 2 - edit menu, 3 - edit screen input, 4 - ble confirmation

//USBCDC USBSerial;
// use this option for OSX.
// Comment it out if using Windows or Linux:
char ctrlKey = KEY_LEFT_GUI;
// use this option for Windows and Linux.
// leave commented out if using OSX:
//  char ctrlKey = KEY_LEFT_CTRL;
File root;

int line_count = 0;
char lines_array[MAX_LINES][MAX_LINE_LENGTH];
String parsedResult[4];

std::vector<std::string> filenames;
std::vector<std::string> cardnames;
std::vector<std::string> notenames;
const int MAX_FILES = 50;

const int FILE_TYPE_PWD = 0;
const int FILE_TYPE_CARD = 1;
const int FILE_TYPE_NOTE = 2;
int fileType = FILE_TYPE_PWD; // 0 - pwd, 1 - cards, 2 - notes
String sortingPwdConf = "";
String sortingNoteConf = "";
String sortingCardConf = "";

std::vector<std::string> getFilenames(String specificType = "") {
  int type = fileType;
  if(specificType == "pwd") {
    type = FILE_TYPE_PWD;
  } else if(specificType == "note") {
    type = FILE_TYPE_NOTE;
  } else if(specificType == "card") {
    type = FILE_TYPE_CARD;
  }

  if (type == FILE_TYPE_CARD) {
    return cardnames;
  } else if (type == FILE_TYPE_NOTE) {
    return notenames;
  }
  return filenames;
}

struct FileInfo {
  char name[20]; // Buffer for the file name
  // Add other fields you want to sort by, e.g.,
  // unsigned long size;
  // time_t modifiedTime; 
};
FileInfo fileList[MAX_FILES]; // Array to hold file info

int fileCount = 0;

bool autopressEnter = false;
bool connectionConfirmation = true;
bool autostartBluetooth = false;
bool passListEnabled = false;
bool passListVisible = true;
bool passListActive = false;
bool enqueuePwdGenerate = false;

bool hasUpdatesService = false;
bool hasUpdatesLogin = false;
bool hasUpdatesPwdRule = false;
unsigned long  enqueueAllowConnection = 0;



String newName = "";
String newNumber = "";
String newExp = "";
String newCVC = "";
String newPIN = "";
bool enqueueCreateCard = false;
String newTitle = "";
String newContents = "";
String newServiceName = "";
String newLogin = "";
String newPwdRule = "";
String newPwd = "";
String enqueueRemove = "";
bool enqueueRemoveCurrent = false;
bool enqueueCreate = false;
String enqueueUpdate = "";
String enqueueUpdateCardFlag = "";
bool enqueueCreateNote = false;
String enqueueUpdateNoteFlag = "";
String enqueueUpdateMeta = "";
bool enqueueRequestMeta = false;
String enqueueSendList = "";
String enqueueRenameType = "";
int enqueueRenameFrom = -1;
int enqueueRenameTo = -1;

File getRoot() {
  return SD_MMC.open("/");
}


struct NoteFile {
    String title;
    std::string contents;
};
struct CardFile {
    String title;
    String name;
    String number;
    String exp;
    String cvc;
    String pin;
};


CardFile parseCardFile(String fileContents, bool toCurrent = true) {
    char contents[4096];
    strcpy(contents, fileContents.c_str());
    char* pch = strtok(contents, "\n");
    String parsedTitle = String(pch);
    pch = strtok(NULL, "\n");
    String parsedName = String(pch);
    pch = strtok(NULL, "\n");
    String parsedNumber = String(pch);
    pch = strtok(NULL, "\n");
    String parsedExp = String(pch);
    pch = strtok(NULL, "\n");
    String parsedCVC = String(pch);
    pch = strtok(NULL, "\n");
    String parsedPIN = String(pch);
    CardFile res;
    res.title = parsedTitle; 
    res.name = parsedName; 
    res.number = parsedNumber; 
    res.exp = parsedExp; 
    res.cvc = parsedCVC; 
    res.pin = parsedPIN; 
    if (toCurrent) {
      currentPwd = parsedNumber;
    }
    return res;
  
}
NoteFile parseNoteFile(String fileContents, bool toCurrent = true) {
    String title = fileContents.substring(0, fileContents.indexOf('\n'));
    String contents = fileContents.substring(fileContents.indexOf('\n') + 2);
    if (toCurrent) {
      currentService = title;
      currentLogin = contents;
      currentPwd = contents;
    }
    NoteFile res;
    res.title = title; 
    res.contents = std::string(contents.c_str());
    return res;
}


void parsePwdFile(String fileContents, bool toCurrent = true) {
//    FILE *f = fopen(filename, "a");
    char contents[4096];
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

String getCurrentFileName() {
//  Keyboard.print('/' + filenames[currentInd].c_str());
   return '/' + String(getFilenames()[currentInd].c_str());
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



void scanDir() {
  File root = SD_MMC.open("/");// Read all filenames into the vector
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      break; // No more files
    }
    String fileName = (String)entry.name();
    if(fileName.indexOf(".pwd") != -1){
     
      // Convert File entry name (char*) to std::string and add to vector
      filenames.push_back(std::string(entry.name())); 
     
    }else if(fileName.indexOf(".card") != -1){
     
      // Convert File entry name (char*) to std::string and add to vector
      cardnames.push_back(std::string(entry.name())); 
     
    }else if(fileName.indexOf(".note") != -1){
     
      // Convert File entry name (char*) to std::string and add to vector
      notenames.push_back(std::string(entry.name())); 
     
    }
    entry.close();
  }
  root.close();

}


void sortFiles() {
  std::sort(filenames.begin(), filenames.end(), compareNat);
  std::sort(cardnames.begin(), cardnames.end(), compareNat);
  std::sort(notenames.begin(), notenames.end(), compareNat);

}

const int PASS_LIST_SIZE = 5;
String passList[PASS_LIST_SIZE];
void getPassList(String (&arr)[PASS_LIST_SIZE]) {
  const int maxInd = getFilenames().size() - 1 > currentInd + PASS_LIST_SIZE ? currentInd + PASS_LIST_SIZE : getFilenames().size() - 1;
  int roundedCurrentInd = currentInd - currentInd % PASS_LIST_SIZE;

  for (int i = 0; i < PASS_LIST_SIZE; i++) {
    if (roundedCurrentInd + i > maxInd) {
      arr[i] = "";
    } else {
      std::string item = getFilenames()[roundedCurrentInd + i];
      String fName1 = '/' + String(item.c_str());
//      String sortIndKey = String(";")+ String(roundedCurrentInd + i+1)+String("=");
//      String sortNameKey = String("=")+ String(fName1.substring(1,fName1.indexOf(".")))+String(";");
      
//      if (fileType == 0){
//        if (sortingPwdConf.indexOf(sortNameKey) != -1) {
//          sortExtra += 1;
//          continue;
//        }
//        if (sortingPwdConf.indexOf(sortIndKey) != -1) {
//          int startInd = sortingPwdConf.indexOf(sortIndKey);
//          String overridden = sortingPwdConf.substring(startInd + sortIndKey.length(), sortingPwdConf.indexOf(";", startInd + sortIndKey.length()));
//      Keyboard.print(overridden);15
//          fName1 = '/' + String(overridden.c_str())+".pwd";
//      
//        }
//      }
      String readValue = readFile(SD_MMC, fName1.c_str());
      if(fileType == 0) {
        
        parsePwdFile(readValue);
        arr[i] = parsedResult[0] + " " + parsedResult[1];
      } else if (fileType == 2) {
        String title = readValue.substring(0, readValue.indexOf('\n'));
        String contents = readValue.substring(readValue.indexOf('\n') + 2);
        arr[i] = title + " " + contents;
      } else if (fileType == 1) {
        String title = readValue.substring(0, readValue.indexOf('\n'));
        arr[i] = title;
      }
    }
  }
}

void renderPass() {
  //  
  //  tft.init();
  //  tft.setRotation(1);
  tft.fillScreen(THEME_BG_COLOR);
  static int16_t bx, by; static uint16_t bw, bh;
  size_t flength = getFilenames().size();
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
  if (passListEnabled && passListVisible) {
//    static_cast<int>(float_result);
     getPassList(passList);
     renderPassList(passList, passListActive == false ? -1 : currentInd % PASS_LIST_SIZE, currentInd - currentInd % PASS_LIST_SIZE);
     return;
  }
  
  digitalWrite(TFT_LEDA_PIN, 0);
  tft.setTextFont(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  const String msg = "TEsT"; 
  static bool big = true;
  static uint16_t color = THEME_FONT_COLOR;
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(big ? 4 : 2);
  tft.setTextColor(color, THEME_FONT_COLOR);
  char sourceStr[512];
  strcpy(sourceStr, lines_array[currentInd]);
  //  !!

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
    if (fileType == FILE_TYPE_CARD) {
      tft.setTextFont(2);
      CardFile fileData = parseCardFile(readValue);
      tft.drawString(fileData.title, bx, by);
      bh = tft.fontHeight();
      tft.setTextColor(THEME_FONT_COLOR, THEME_BG_COLOR);
      by = ((int)bh);
      tft.drawString(fileData.name, bx, by);
      by = (2*(int)bh);
      tft.setTextColor(THEME_FONT_COLOR, THEME_BG_COLOR);
      tft.drawString(fileData.number, bx, by);
      by = (3*(int)bh);
      tft.drawString(fileData.cvc, bx, by);
      tft.drawString(fileData.exp, bx+40, by);
      tft.drawString(fileData.pin, bx+90, by);
      return;
    }
    if (fileType == FILE_TYPE_NOTE) {
      parseNoteFile(readValue);
      tft.drawString(currentService, bx, by);
    
      tft.setTextFont(4);
      tft.setTextColor(THEME_TITLE_COLOR, THEME_BG_COLOR);
      bw = tft.textWidth(currentLogin);
      bh = tft.fontHeight();
      //  bx = (tft.width() - (int)bw) / 2;
      bx = 0;
      by = (tft.height() - (int)bh) / 2;
      tft.drawString(currentLogin, bx, by);
      return;
    }
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


void handleLongClick() {
  if (screenType == 0) {
        
    screenType = 1;
    if (passListEnabled && passListVisible) {
      renderMainMenuShort();
    } else { 
      renderMainMenu();
    }
  } else if (screenType == -1) {
    screenType = 1;
    renderMainMenuShort();
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
    size_t length = getFilenames().size();
    if (passListEnabled && passListVisible && passListActive == false) {
      int nextInd = currentInd - currentInd % PASS_LIST_SIZE + PASS_LIST_SIZE;
      if (nextInd < length) {
        currentInd = nextInd;
      } else {
        currentInd = 0;
      }
    } else if (passListEnabled) {
      passListActive = true;
      passListVisible = true;
      if (currentInd % PASS_LIST_SIZE == PASS_LIST_SIZE-1 || currentInd +1 >= getFilenames().size()) {
        currentInd = currentInd - currentInd % PASS_LIST_SIZE ;
      } else {
        currentInd += 1;
      } 
      
    } else {
      if (currentInd < length - 1) {
        currentInd += 1;
      } else {
        currentInd = 0;
      } 
    }
    renderPass();
    updateCurrentStateBle();
    // Toggle built-in LED on a single click
  //  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  } else if (screenType == -1) {
    sectionsMenu.next();
  } else if (screenType == 4) {
    //
    switchConfirmationItem();
  } else {
    switchMenuItem();
  }
}
void handleIncomingBLERequest() {
  if (connectionConfirmation) {
    
    screenType = 4;
    renderConfirmation( "Allow BLE connect");
  }else {
    setEnqueueAllowConnection();
  }
}

void handleMultiClick() {
  int clicks = button.getNumberClicks();
  if (clicks == 3) {
    if (screenType == 0) {
      if (passListEnabled && passListVisible && passListActive == true) {
        passListActive = false;
        renderPass();
      } else if(passListEnabled && passListVisible && !passListActive){  
        screenType = -1;
        currentInd = 0;
        sectionsMenu.render();
//        Keyboard.print(currentLogin);
      }else {
        passListVisible = true;
        passListActive = false;
        renderPass();
      
      }
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
  
    if (passListEnabled && passListVisible && passListActive == false) {
      passListActive = true;
      renderPass();
    }else if (passListActive == true){
      passListVisible = false;
      passListActive = false;
      renderPass();
    }else{
      Keyboard.print(currentPwd);
      if(autopressEnter) {
        Keyboard.write(KEY_RETURN);
      }
        
    }
      return;
    //  Serial.println("Double Clicked!");
    //  // Turn built-in LED off on a double click
    //  digitalWrite(LED_BUILTIN, LOW);
  } else if (screenType == -1) {
    screenType = 0;
    fileType = sectionsMenu.getCurrentInd();
    renderPass();
  } else if(screenType == 1) {


    if (getCurrentMenuItem() == MENU_ITEM_NEW) {
      createNewPwd("???", "???", "", "");
    } else if (getCurrentMenuItem() == MENU_ITEM_ENABLE_BT) {
      if (isBLERunning()) {
        stopBLEServer();
      } else {
        setupBLEServer();
      }
    } else if (getCurrentMenuItem() == MENU_ITEM_EDIT) {
      screenType = 2;
      renderSubMenu();
    } else if (getCurrentMenuItem() == MENU_ITEM_PREV_PWD) {
      Keyboard.print(currentPrevPwd);    
    }  else if (getCurrentMenuItem() == MENU_ITEM_DUMP_ITEM) {
      dumpItem(currentService, currentLogin, currentPwd);
    } else if (getCurrentMenuItem() == MENU_ITEM_DUMP_ALL) {
      dumpAll(SD_MMC);
    }  else if (getCurrentMenuItem() == MENU_ITEM_EDIT_PWD_REGENERATE) {
      String fileName = getCurrentFileName();
      const char* fName = fileName.c_str();
      String newVal = generateRandomString(currentPwdRule);
      updatePwd(SD_MMC, fName, newVal, MENU_ITEM_EDIT_PWD_REGENERATE);
      
    }  else if (getCurrentMenuItem() == MENU_ITEM_EDIT_PWD_GENERATE) {
      enqueuePwdGenerate = true;
      renderConfirmation("Generate new password?");
    }  else  {
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
    if (getCurrentMenuItem() == MENU_ITEM_EDIT_PWD_REGENERATE) {
      String fileName = getCurrentFileName();
      const char* fName = fileName.c_str();
      String newVal = generateRandomString(currentPwdRule);
      updatePwd(SD_MMC, fName, newVal, MENU_ITEM_EDIT_PWD_REGENERATE);
      
    }  else if (getCurrentMenuItem() == MENU_ITEM_EDIT_PWD_GENERATE) {
      String newVal = generateRandomString(currentPwdRule);
      String fileName = getCurrentFileName();
      const char* fName = fileName.c_str();
      addPwd(SD_MMC, fName, newVal);
    }
  } else if (screenType == 4) {
    //
    if (confirmationGranted()){
      if (enqueuePwdGenerate) {
        String newVal = generateRandomString(currentPwdRule);
        String fileName = getCurrentFileName();
        const char* fName = fileName.c_str();
        addPwd(SD_MMC, fName, newVal);
        screenType = 0;
        renderPass();
      } else {
        allowConnection();  
      }
      
    } else {
      // 
    }
    screenType = 0;
    renderPass();
  }
}

String getCurrentMetaString() {
  String dataF = readMeta(SD_MMC);
  return dataF;
}
String getCurrentDataString() {
  return currentService + "\n" + currentLogin + "\n" + currentPwdRule;
}

String getCommonDataString(String type, int offset = 0) {

  String result = "list\n";
  String postfix = "\n";
  
  for (std::size_t i = offset*5; i < offset*5 + 5; i++) {
    if (i < getFilenames(type).size()) {
      std::string item = getFilenames(type)[i];
      const String fName1 = '/' + String(item.c_str());
      String readValue = readFile(SD_MMC, fName1.c_str());
      if (i == getFilenames(type).size() - 1) {
        postfix += "endoflist";
      }
      if(type == "note") {
        NoteFile res = parseNoteFile(readValue);
        result += String(fName1) + "||" + res.title + "||" + String(res.contents.c_str()) + postfix;
      } else if(type == "card") {
        CardFile res = parseCardFile(readValue); 
        result += String(fName1) + "||" + res.title + "||" + res.name + "||" + res.number + "||" + res.exp + "||" + res.cvc + "||" + res.pin + postfix;
      } else { 
        parsePwdFile(readValue);
        result += String(fName1) + "||" + parsedResult[0] + "||" + parsedResult[1] + "||" + parsedResult[2] + postfix;
      }
    }
  }
  if (0 == getFilenames(type).size()) {
    result += "endoflist";
  }
  return result;
}

void loadMetaData() {
  
  THEME_BG_COLOR = THEME_DARK_BG_COLOR;
  THEME_HIGHLIGHT_COLOR = THEME_DARK_HIGHLIGHT_COLOR;
  THEME_FONT_COLOR = THEME_DARK_FONT_COLOR;
  THEME_TITLE_COLOR = THEME_DARK_TITLE_COLOR;
  String dataF = readMeta(SD_MMC);
//  dataF = "THEME_CUSTOM:255,0,0,128,0,0,255,0,0,180,46,26";
  passListEnabled = false;
  if (dataF.indexOf("PASS_LIST_ENABLED") != -1) {
    passListEnabled = true; 
  }
  if (dataF.indexOf("AUTOSTART_BLUETOOTH") != -1) {
    autostartBluetooth = true; 
  }
  if (dataF.indexOf("AUTOPRESS_ENTER") != -1) {
    autopressEnter = true; 
  }
  if (dataF.indexOf("CONNECTION_CONFIRMATION_DISABLED") != -1) {
    connectionConfirmation = false; 
  }
//  if(dataF.indexOf("SORTING_PWD") != -1) {
//    int sortingStart = dataF.indexOf("SORTING_PWD") + 11;
//    int sortingEnd = dataF.indexOf('\n', sortingStart);
//    sortingPwdConf = dataF.substring(sortingStart);
//    if (sortingEnd != -1) {
//      sortingPwdConf = dataF.substring(sortingStart, sortingEnd);
//    }
//  }
//  if(dataF.indexOf("SORTING_NOTE") != -1) {
//    int sortingStart = dataF.indexOf("SORTING_NOTE") + 8;
//    int sortingEnd = dataF.indexOf('\n', sortingStart);
//    sortingPwdConf = dataF.substring(sortingStart);
//    if (sortingEnd != -1) {
//      sortingNoteConf = dataF.substring(sortingStart, sortingEnd);
//    }
//  }
//  if(dataF.indexOf("SORTING_CARD") != -1) {
//    int sortingStart = dataF.indexOf("SORTING_CARD") + 8;
//    int sortingEnd = dataF.indexOf('\n', sortingStart);
//    sortingPwdConf = dataF.substring(sortingStart);
//    if (sortingEnd != -1) {
//      sortingCardConf = dataF.substring(sortingStart, sortingEnd);
//    }
//  }
  if(dataF.indexOf("THEME_LIGHT") != -1) {
    
    THEME_BG_COLOR = THEME_LIGHT_BG_COLOR;
    THEME_HIGHLIGHT_COLOR = THEME_LIGHT_HIGHLIGHT_COLOR;
    THEME_FONT_COLOR = THEME_LIGHT_FONT_COLOR;
    THEME_TITLE_COLOR = THEME_LIGHT_TITLE_COLOR;

  } else if(dataF.indexOf("THEME_CUSTOM") != -1) {
    int confStart = dataF.indexOf("THEME_CUSTOM") + 13;
    int confEnd = dataF.indexOf('\n', confStart);
    String themeConf = dataF.substring(confStart);
    if (confEnd != -1) {
      themeConf = dataF.substring(confStart, confEnd);
    }
    char themeConfChar[512];
    strcpy(themeConfChar, themeConf.c_str());
    
    char* bgColorR = strtok(themeConfChar, ",");
    char* bgColorG = strtok(NULL, ",");
    char* bgColorB = strtok(NULL, ",");
    
    char* fontColorR = strtok(NULL, ",");
    char* fontColorG = strtok(NULL, ",");
    char* fontColorB = strtok(NULL, ",");
    
    char* hilitColorR = strtok(NULL, ",");
    char* hilitColorG = strtok(NULL, ",");
    char* hilitColorB = strtok(NULL, ",");

    char* titleColorR = strtok(NULL, ",");
    char* titleColorG = strtok(NULL, ",");
    char* titleColorB = strtok(NULL, ",");
//  Keyboard.print(",");
//  pch1 = strtok(NULL, ",");
    THEME_BG_COLOR = tft.color565(atoi(bgColorR), atoi(bgColorG), atoi(bgColorB));
    THEME_HIGHLIGHT_COLOR = tft.color565(atoi(hilitColorR), atoi(hilitColorG), atoi(hilitColorB));
    THEME_FONT_COLOR = tft.color565(atoi(fontColorR), atoi(fontColorG), atoi(fontColorB));
    THEME_TITLE_COLOR = tft.color565(atoi(titleColorR), atoi(titleColorG), atoi(titleColorB));
  }
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  Keyboard.begin();
  USB.begin();
  Serial.println("Start");
  tft.init();
  tft.setRotation(-1);
  tft.fillScreen(THEME_BG_COLOR);

  sectionsMenu.setColor(THEME_FONT_COLOR, THEME_BG_COLOR);
  

  digitalWrite(TFT_LEDA_PIN, 0);

    FastLED.addLeds<APA102, LED_DI_PIN, LED_CI_PIN, BGR>(leds, 1);  // BGR ordering is typical
    FastLED.setBrightness(100);
        leds[0] = colors[2];

  sd_init();

  
  scanDir();
  sortFiles();
  root = getRoot();

  loadMetaData();
  loadLocale(0);
  if (autostartBluetooth) {
    setupBLEServer();
  }

  String labels[3] = {LABEL_PASSWORDS, LABEL_CREDIT_CARDS, LABEL_NOTES};
  sectionsMenu.setLabels(labels);
  sectionsMenu.render();
  button.attachLongPressStop(handleLongClick);
  button.attachDoubleClick(handleDoubleClick);
  button.attachMultiClick(handleMultiClick);
  button.attachClick(handleClick);

  return;
}

void setEnqueueRemoveCurrent() {
  enqueueRemoveCurrent = true;
}

void setEnqueueRemove (String path) {
  enqueueRemove = path;
}

void setEnqueueUpdateMeta (String dataStr) {
  enqueueUpdateMeta = dataStr;
}

void setEnqueueRename (String renType, int from, int to) {
  enqueueRenameType = renType;
  enqueueRenameFrom = from;
  enqueueRenameTo = to;
}

void setEnqueueRequestMeta() {
  enqueueRequestMeta = true;
}
void setEnqueueRequestList(String val) {
  enqueueSendList = val;
}
void setEnqueueAllowConnection() {
  enqueueAllowConnection = 1;
}




void enqueueNewCard(String title, String cname, String number, String cexp, String cvc, String pin) {

  newTitle = title;
  newName = cname;
  newNumber = number;
  newExp = cexp;
  newCVC = cvc;
  newPIN = pin;
  enqueueCreateCard = true;

}
void enqueueNewNote(String title, String contents) {
  newTitle = title;
  newContents = contents;
  enqueueCreateNote = true;
}
void enqueueUpdateCard(String fileName, String title, String cname, String number, String cexp, String cvc, String pin) {

  newTitle = title;
  newName = cname;
  newNumber = number;
  newExp = cexp;
  newCVC = cvc;
  newPIN = pin;
  enqueueUpdateCardFlag = fileName;
  
}
void enqueueUpdateNote(String fileName, String title, String contents) {
  newTitle = title;
  newContents = contents;
  enqueueUpdateNoteFlag = fileName;
  
}
void enqueueUpdatePwd(String fileName, String nServiceName, String nLogin, String nPwdRule) {
  newServiceName = nServiceName;
  newLogin = nLogin;
  newPwdRule = nPwdRule;
  enqueueUpdate = fileName;
}

void enqueueNewPwd(String nServiceName, String nLogin, String nPwdRule, String nPwd) {
  newServiceName = nServiceName;
  newLogin = nLogin;
  newPwdRule = nPwdRule;
  newPwd = nPwd;
  enqueueCreate = true;
}

void renameRecurse (String renType, int from, int to) {
  char fromName[512];
  strcpy(fromName, ("/" + String(from) + "."+renType).c_str());
  char toName[512];
  strcpy(toName, ("/" + String(to) + "."+renType).c_str());
  char tempName[512];
  strcpy(tempName, ("/temp."+renType).c_str());
  renameFile(SD_MMC, fromName, tempName);
   if (from == to) {
    return;
   }else 
   if (from > to) {
     for (int i = from - 1; i >= to; i--) {
       if ( std::find(getFilenames(renType).begin(), getFilenames(renType).end(), std::string((String(i)+ "."+renType).c_str())) != getFilenames(renType).end() ) {
         char from1Name[512];
         strcpy(from1Name, ("/" + String(i)+ "."+renType).c_str());  
         char to1Name[512];
         strcpy(to1Name, ("/" + String(i+1)+ "."+renType).c_str());
         renameFile(SD_MMC, from1Name, to1Name);
       }
     }
   }else {
    
     for (int i = from + 1; i <= to; i++) {
       if ( std::find(getFilenames(renType).begin(), getFilenames(renType).end(), std::string((String(i)+ "."+renType).c_str())) != getFilenames(renType).end() ) {
         char from1Name[512];
         strcpy(from1Name, ("/" + String(i)+ "."+renType).c_str());  
         char to1Name[512];
         strcpy(to1Name, ("/" + String(i-1)+ "."+renType).c_str());
         renameFile(SD_MMC, from1Name, to1Name);
       }
     }
   }
  renameFile(SD_MMC, tempName, toName);
}



void loop() {
  button.tick();
  if (enqueueRemoveCurrent) {
    enqueueRemoveCurrent = false;
//    const char* cFileName = getCurrentFileName().c_str();
    getFilenames().erase(getFilenames().begin() + currentInd);
//    String fname = "/" + String(currentInd) + ".pwd";
    deleteFile(getCurrentFileName().c_str());
    if (currentInd > 0) {
      currentInd -= 1; 
    }
    renderPass();
    updateCurrentStateBle();
  }
  if (enqueueRenameType != "") {

    renameRecurse(enqueueRenameType, enqueueRenameFrom, enqueueRenameTo);
    enqueueRenameType = "";
    enqueueRenameFrom = -1;
    enqueueRenameTo = -1;
  }
  if (enqueueUpdateMeta != "") {

      char fName[512];
      strcpy(fName, (enqueueUpdateMeta.substring(11)).c_str());
      enqueueUpdateMeta = "";
      writeMeta(SD_MMC, fName);
    
      loadMetaData();
      renderPass();
  }
  if (enqueueRequestMeta) {
    enqueueRequestMeta = false;
    sendMeta();
  }
  if (enqueueSendList != "") {
    fetchData(enqueueSendList);
    enqueueSendList = "";
  }
  if (enqueueRemove != "") {
    char fName[120];
    strcpy(fName, enqueueRemove.c_str());
    if(String(fName).indexOf(".pwd") != -1) {
      
      filenames.erase(std::remove(filenames.begin(), filenames.end(), std::string(enqueueRemove.substring(1).c_str())), filenames.end());
    }
    if(String(fName).indexOf(".note") != -1) {
      
      notenames.erase(std::remove(notenames.begin(), notenames.end(), std::string(enqueueRemove.substring(1).c_str())), notenames.end());
    }
    if(String(fName).indexOf(".card") != -1) {
      
      cardnames.erase(std::remove(cardnames.begin(), cardnames.end(), std::string(enqueueRemove.substring(1).c_str())), cardnames.end());
    }
    deleteFile(fName);
    enqueueRemove = "";
    if (screenType == 1) {
      renderPass();  
    }
    updateCurrentStateBle();
  }
  if (enqueueAllowConnection == 1  ) {
    enqueueAllowConnection = millis();
  }
  if (enqueueAllowConnection != 1 && enqueueAllowConnection != 0 && millis() - enqueueAllowConnection > 2000) {
    enqueueAllowConnection = 0;
    allowConnection();
  }
  if (enqueueCreate && newServiceName) {
    enqueueCreate = false;
    String newName = createNewPwd(newServiceName, newLogin, newPwdRule, newPwd);
    newServiceName = "";
    newLogin = "";
    newPwdRule = "";
    newPwd = "";

    filenames.push_back(std::string(newName.substring(1).c_str())); 
    size_t length = getFilenames().size();
    currentInd = length - 1;
    if (screenType == 1) {
      renderPass();  
    }
    updateCurrentStateBle();
  }
  if (enqueueCreateCard && newNumber) {
    enqueueCreateCard = false;
    String newFileName = createNewCard(newTitle, newName, newNumber, newExp, newCVC, newPIN);
    cardnames.push_back(std::string(newFileName.substring(1).c_str())); 
    newTitle = "";
    newName = "";
    newNumber = "";
    newCVC = "";
    newPIN = "";
    newExp = "";
    if (screenType == 1) {
      renderPass();  
    }
    
    updateCurrentStateBle();
    
  }
  if (enqueueCreateNote && newContents) {
    enqueueCreateNote = false;
    String newName = createNewNote(newTitle,newContents);
    newTitle = "";
    newContents = "";

    notenames.push_back(std::string(newName.substring(1).c_str())); 
    size_t length = getFilenames().size();
    currentInd = length - 1;
    if (screenType == 1) {
      renderPass();  
    }
    updateCurrentStateBle();
  }
  if (enqueueUpdateNoteFlag != "" && newTitle) {
    char fName[120];
    strcpy(fName, enqueueUpdateNoteFlag.c_str());
    enqueueUpdateNoteFlag = "";
    updateNoteData(SD_MMC, fName, newTitle, newContents);
    newTitle = "";
    newContents = "";
    
  }
  if (enqueueUpdateCardFlag != "" && newTitle) {
    char fName[120];
    strcpy(fName, enqueueUpdateCardFlag.c_str());
    enqueueUpdateCardFlag = "";
    updateCardData(SD_MMC, fName, newTitle, newName, newNumber, newExp, newCVC, newPIN);
    newTitle = "";
    newName = "";
    newNumber = "";
    newCVC = "";
    newPIN = "";
    newExp = "";
    
  }
  if (enqueueUpdate != "" && newServiceName) {
    char fName[120];
    strcpy(fName, enqueueUpdate.c_str());
    enqueueUpdate = "";
    updatePwdData(SD_MMC, fName, newServiceName, newLogin, newPwdRule);
    newServiceName = "";
    newLogin = "";
    newPwdRule = "";
  }
  delay(5);
}
//Wrote 1310752 bytes (758620 compressed) at 0x00010000 in 11.7 seconds (effective 893.1 kbit/s)...
//Wrote 1309872 bytes (758081 compressed) at 0x00010000 in 11.6 seconds (effective 901.9 kbit/s)...
