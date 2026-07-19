#include "general_config.h"

#define SD_MMC_D0_PIN  14
#define SD_MMC_D1_PIN  17
#define SD_MMC_D2_PIN  21
#define SD_MMC_D3_PIN  18
#define SD_MMC_CLK_PIN 12
#define SD_MMC_CMD_PIN 16


String padStringRight(String str) {
  int requiredLength = MAX_STRING_SIZE;
  int currentLength = str.length();
  for (int i = 0; i < (requiredLength - currentLength); i++) {
    str += ' '; // Append a space character
  }
  return str;
}


void sd_init(void) {
  
    SD_MMC.setPins(SD_MMC_CLK_PIN, SD_MMC_CMD_PIN, SD_MMC_D0_PIN, SD_MMC_D1_PIN, SD_MMC_D2_PIN, SD_MMC_D3_PIN);
    if (!SD_MMC.begin()) {
        Serial.println("Card Mount Failed");
        return;
    }
    Serial.println("Init fs success!");
}

      
String createNewCard(String title, String cname, String number, String cexp, String cvc, String pin) {
    File root = SD_MMC.open("/");
    File file = root.openNextFile();
    int maxIndex = 0;
    while (file) {
        if (!file.isDirectory()) {
          const String name = file.name();
          if(name.indexOf('.card') != -1 && name.toInt() >= maxIndex) {
            maxIndex = name.toInt()+1;
          }
        }
        file = root.openNextFile();
    }

    String newName = '/' + String(maxIndex) + String(".card");
    String initialContents = title + "\n" + cname + "\n" + number + "\n" + cexp + "\n" + cvc + "\n" + pin ;
    writeFile(SD_MMC, newName.c_str(), initialContents.c_str());
    return newName;
}
String createNewNote(String title, String contents) {

    File root = SD_MMC.open("/");
    File file = root.openNextFile();
    int maxIndex = 0;
    while (file) {
        if (!file.isDirectory()) {
          const String name = file.name();
          if(name.indexOf('.note') != -1 && name.toInt() >= maxIndex) {
            maxIndex = name.toInt()+1;
          }
        }
        file = root.openNextFile();
    }

    String newName = '/' + String(maxIndex) + String(".note");
    String initialContents = title + "\n" + contents + String("\n");
    writeFile(SD_MMC, newName.c_str(), initialContents.c_str());
    return newName;
}  
String createNewPwd(String serviceName, String login, String pwdRule, String suggestedPwd) {
    File root = SD_MMC.open("/");
    String pwd = suggestedPwd;
    if(!pwdRule) {
      pwdRule = PWD_RULE_1;
    }
    if (!root) {
        Keyboard.println("Failed to open directory");
        return "";
    }
    if (!root.isDirectory()) {
        Keyboard.println("Not a directory");
        return "";
    }
    if(!pwd) {
      pwd = generateRandomString(pwdRule);
    }
    File file = root.openNextFile();
    int maxIndex = 0;
    while (file) {
        if (!file.isDirectory()) {
          const String name = file.name();
          if(name.indexOf('.pwd') != -1 && name.toInt() >= maxIndex) {
            maxIndex = name.toInt()+1;
          }
        }
        file = root.openNextFile();
    }

    String newName = '/' + String(maxIndex) + String(".pwd");
    String initialContents = padStringRight(serviceName) + "\n" + padStringRight(login) + "\n" + padStringRight(pwdRule) + "\n" + padStringRight(pwd) + String("\n");
    writeFile(SD_MMC, newName.c_str(), initialContents.c_str());
    return newName;
}  

void addPwd(fs::FS &fs, const char * path, String newVal) {
  File file = fs.open(path, FILE_APPEND);
  file.print(padStringRight(newVal)+String("\n"));
  file.close();
}

void updateCardData(fs::FS &fs, const char * path, String title, String cname, String number, String cexp, String cvc, String pin) {
  const String prevContents = readFile(fs, path);
  File file = fs.open(path, FILE_WRITE);
  file.seek(0);
  file.print(title + "\n" + cname + "\n" + number + "\n" + cexp + "\n" + cvc + "\n" + pin );

  file.close();
}
void updateNoteData(fs::FS &fs, const char * path, String title, String contents) {
  const String prevContents = readFile(fs, path);
  File file = fs.open(path, FILE_WRITE);
  file.seek(0);
  file.print(title + "\n" + contents);

  file.close();
}
void updatePwdData(fs::FS &fs, const char * path, String serviceName, String login, String pwdRule) {
  const String prevContents = readFile(fs, path);
  File file = fs.open(path, FILE_WRITE);
  file.seek(0);
  file.print(padStringRight(serviceName) + "\n" + padStringRight(login) + "\n" + padStringRight(pwdRule) + "\n" + prevContents.substring(3*MAX_STRING_SIZE + 3));

  file.close();
}

void updatePwd(fs::FS &fs, const char * path, String newVal, String type) {
  const String prevContents = readFile(fs, path);
//    Keyboard.print(prevContents.substring(0, prevContents.length()-31) + padStringRight(newVal));
//addPwd(fs, path, newVal);
//    return;
  File file = fs.open(path, FILE_WRITE);
  file.seek(0);
  if (type == MENU_ITEM_EDIT_SERVICE) {
    file.print(padStringRight(newVal) + prevContents.substring(MAX_STRING_SIZE));
  } else if (type == MENU_ITEM_EDIT_LOGIN) {
    file.print(prevContents.substring(0, MAX_STRING_SIZE + 1) + padStringRight(newVal) + prevContents.substring(2*MAX_STRING_SIZE));
//    Keyboard.print(prevContents.substring(0, 31) + padStringRight(newVal) + prevContents.substring(60));
  } else if (type == MENU_ITEM_EDIT_PWD_RULES) {
    file.print(prevContents.substring(0, MAX_STRING_SIZE * 2 + 2) + padStringRight(newVal) + prevContents.substring(MAX_STRING_SIZE*3+2));
  } else if(type == MENU_ITEM_EDIT_PWD_REGENERATE){
    file.print(prevContents.substring(0, prevContents.length() - MAX_STRING_SIZE - 1) + padStringRight(newVal));
  }


  file.close();
}
String currentFilePath;
String  readNextFile(File &root) {
  File file = root.openNextFile();
  if (file) {
    currentFilePath = file.path();
    return readFile(SD_MMC, file.path());
  }
  return "!";
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

String readFile(fs::FS &fs, const char * path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return "!";
    }


  // Clear the string variable before writing new content
  String contentVar = "";

  Serial.println("Reading data:");
  // Read file data until no more data is available
  while (file.available()) {
    // Read a byte and append it to the String variable
    contentVar += (char)file.read();
  }
  // Close the file
  file.close();
  return contentVar;
}

String readMeta(fs::FS &fs) {
  const String metaFileName = "pwdman.meta";
  String dataF = readFile(SD_MMC, ('/' + metaFileName).c_str());
    return dataF;
}
void writeFile(fs::FS &fs, const char * path, const char * message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
}
void writeMeta(fs::FS &fs,  const char * message) {
  String metaFileName = "/pwdman.meta";
  writeFile(fs, metaFileName.c_str(), message);
}


void deleteFile(const char *path) {
//    File file = SD_MMC.open(path, FILE_WRITE);
//    file.close();
    
  if (SD_MMC.remove(path)) {
//    Keyboard.println("File deleted");
  } else {
//    Keyboard.println("Delete failed");
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Keyboard.print(file.name());
            Serial.print("  SIZE: ");
            Keyboard.println(file.size());
        }
        file = root.openNextFile();
    }
}
