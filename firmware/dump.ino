String getSecureValue(String value) {
  char charToReplace = '*';

  for (int i = 0; i < value.length(); i++) {
    // Check if the current index (i) is even
    if (i % 2 != 0) {
      // Replace the character at the even position
      value.setCharAt(i, charToReplace);
    }
  }
  return value;
}

void dumpItem(String service, String login, String pwd) {
  Keyboard.print(getSecureValue(service) + "\n" + getSecureValue(login) + "\n" + pwd);
}

void dumpAll(fs::FS &fs){
  
    File root = fs.open("/");
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
//            Serial.print("  DIR : ");
//            Serial.println(file.name());
//            if (levels) {
//                listDir(fs, file.path(), levels - 1);
//            }
        } else {
          
  String fileName = String("/") + file.name();
if(fileName.indexOf(String(".pwd")) != -1) {
  const char* fName = fileName.c_str();
  String readValue = readFile(SD_MMC,  fName);
//  Keyboard.println(readValue);
          parsePwdFile(readValue, false);
//  Keyboard.print(result[0]);
          dumpItem(parsedResult[0], parsedResult[1], parsedResult[3]);
          Keyboard.print(String("\n-----------\n") );
}
//            Serial.print("  FILE: ");
//            Keyboard.print(file.name());
//            Serial.print("  SIZE: ");
//            Keyboard.println(file.size());
        }
        file = root.openNextFile();
    }
}
