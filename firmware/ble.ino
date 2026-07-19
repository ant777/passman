#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "6fe56dde-8e9b-45c5-96ee-7c8aea360a94" // Custom Service UUID
#define CHARACTERISTIC_UUID "46017b0a-60bd-4d8d-9fd9-22d15de3ae0f" // Custom Characteristic UUID
BLEService *pService;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool deviceAllowed = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        deviceAllowed = false;
        handleIncomingBLERequest();
        Serial.println("Device Connected");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        deviceAllowed = false;
        Serial.println("Device Disconnected");
        pServer->startAdvertising(); // Restart advertising
    }
};

void allowConnection() {
  deviceAllowed = true;
  String dataStr;
  int offset = 0;
  pCharacteristic->setValue(String("allowed").c_str());
  pCharacteristic->notify();

};

void fetchData(String type) {
  String dataStr;
  int offset = 0;
  while (dataStr.indexOf("\nendoflist") == -1) {

    dataStr = getCommonDataString(type, offset);
    offset += 1;
    pCharacteristic->setValue(String(dataStr).c_str());
    pCharacteristic->notify();
  }

};

void sendMeta() {
  
            String dataStr = getCurrentMetaString();
            pCharacteristic->setValue(String("meta\n" + dataStr).c_str());
            pCharacteristic->notify();
}
void updateCurrentStateBle() {
  if(!deviceConnected) {

    return;
  }
    String dataStr = getCurrentDataString();
    pCharacteristic->setValue(String(dataStr).c_str());
    pCharacteristic->notify();
}

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* onCharacteristic) {
        if(!deviceAllowed){
          return;
        }
        std::string value = onCharacteristic->getValue();
        if (value.length() > 0) {
          if (value == "requestmeta") {
            setEnqueueRequestMeta();
            return;
          }
          if (value == "requestlist_pwd" || value == "requestlist_card" || value == "requestlist_note") {
//            fetchData(value.replace(0, 12,"").c_str());
            setEnqueueRequestList(value.replace(0, 12,"").c_str());
            return;
          }
          if (value == "request") {
            String dataStr = getCurrentDataString();
            pCharacteristic->setValue(String(dataStr).c_str());
            pCharacteristic->notify();
            return;
          }
          if (value.find("remove_") == 0) {
            if(value == ("remove_" + getCurrentDataString()).c_str()) {
              setEnqueueRemoveCurrent();
            }
            return;
          }
          if (value.find("remove") == 0) {
              char contents[512];
              strcpy(contents, value.c_str());
              char* pch = strtok(contents, "\n");
              String parsedInstruction = String(pch);
              pch = strtok(NULL, "\n");
              String parsedFilename = String(pch);
              setEnqueueRemove(parsedFilename);
            return;
          }
          if (value.find("updatemeta") == 0) {
              char contents[512];
              strcpy(contents, value.c_str());
              setEnqueueUpdateMeta(String(contents));
            return;
          }
          if (value.find("updatesorting") == 0) {
              char contents[512];
              strcpy(contents, value.c_str());
              char* pch = strtok(contents, "\n");
              pch = strtok(NULL, "\n");
              String parsedInstruction = String(pch);
              pch = strtok(NULL, "\n");
//              
              String parsedValue = String(pch);  
//              contents = "";            
//              char valueContents[512];
              strcpy(contents, parsedValue.c_str());
              char* pch1 = strtok(contents, "=");
              int parsedTo = std::stoi(pch1);
              pch1 = strtok(NULL, "=");
              int parsedFrom = std::stoi(pch1);
              setEnqueueRename(parsedInstruction, parsedFrom, parsedTo);
            return;
          }
          if (value.find("create") == 0 || value.find("update") == 0) {
            
            char *rest = NULL;
            char contents[1024];
            strcpy(contents, value.c_str());
            char* pch = strtok_r(contents, "\n", &rest);
            String parsedInstruction = String(pch);
            pch = strtok_r(NULL, "\n", &rest);
            String parsedType = String(pch);
            if (parsedType == "note") {
              pch = strtok_r(NULL, "\n", &rest);
              String parsedFilename = String(pch);
              pch = strtok_r(NULL, "\n", &rest);
              String parsedTitle = String(pch); 
              if (parsedInstruction == "create") {
                enqueueNewNote(parsedTitle, String(rest));
              }else if (parsedInstruction == "update"){
                enqueueUpdateNote(parsedFilename, parsedTitle, String(rest));
              }
            } else if (parsedType == "card") {
                pch = strtok_r(NULL, "\n", &rest);
                String parsedFilename = String(pch);
                pch = strtok_r(NULL, "\n", &rest);
                String parsedTitle = String(pch); 
                pch = strtok_r(NULL, "\n", &rest);
                String parsedName = String(pch); 
                pch = strtok_r(NULL, "\n", &rest);
                String parsedNumber = String(pch); 
                pch = strtok_r(NULL, "\n", &rest);
                String parsedExp = String(pch); 
                pch = strtok_r(NULL, "\n", &rest);
                String parsedCVC = String(pch); 
                pch = strtok_r(NULL, "\n", &rest);
                String parsedPIN = String(pch); 
                if (parsedInstruction == "create") {
                  enqueueNewCard(parsedTitle, parsedName, parsedNumber, parsedExp, parsedCVC, parsedPIN);
                }else if (parsedInstruction == "update"){
                  enqueueUpdateCard("/1.card", parsedTitle, parsedName, parsedNumber, parsedExp, parsedCVC, parsedPIN);
                }
              
            } else {
              
              pch = strtok_r(NULL, "\n", &rest);
              String parsedFilename = String(pch);
              pch = strtok_r(NULL, "\n", &rest);
              String parsedService = String(pch);
              pch = strtok_r(NULL, "\n", &rest);
              String parsedLogin = String(pch);
              pch = strtok_r(NULL, "\n", &rest);
              String parsedPwdRule = String(pch);
              pch = strtok_r(NULL, "\n", &rest);
              String parsedPwd = String(pch);
              if (parsedService && parsedInstruction == "create") {
                enqueueNewPwd(parsedService, parsedLogin, parsedPwdRule, parsedPwd);
              } else if(parsedService && parsedInstruction == "update"){
                enqueueUpdatePwd(parsedFilename, parsedService, parsedLogin, parsedPwdRule);
              }
            }
          }
        }
    }
};

bool isBLERunning() {
  return BLEDevice::getInitialized();
}
void setupBLEServer() {
  
    BLEDevice::init("PassMan"); // Name of the device
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY |
                        BLECharacteristic::PROPERTY_INDICATE
                      );
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();

    FastLED.addLeds<APA102, LED_DI_PIN, LED_CI_PIN, BGR>(leds, 1);  // BGR ordering is typical
    FastLED.setBrightness(100);
    leds[0] = colors[2];
    FastLED.show();
    Serial.println("Waiting for a client connection...");
}

void stopBLEServer() {
    BLEDevice::stopAdvertising();
    BLEDevice::deinit();
    pService->stop();
    FastLED.clear(true);

}
