#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "6fe56dde-8e9b-45c5-96ee-7c8aea360a94" // Custom Service UUID
#define CHARACTERISTIC_UUID "46017b0a-60bd-4d8d-9fd9-22d15de3ae0f" // Custom Characteristic UUID

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
 
            String dataStr = getCurrentDataString();
            pCharacteristic->setValue(String(dataStr).c_str());
            pCharacteristic->notify();
};

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
          
          char contents[512];
          strcpy(contents, value.c_str());
          char* pch = strtok(contents, "\n");
          String parsedInstruction = String(pch);
          pch = strtok(NULL, "\n");
          String parsedService = String(pch);
          pch = strtok(NULL, "\n");
          String parsedLogin = String(pch);
          pch = strtok(NULL, "\n");
          String parsedPwdRule = String(pch);
          pch = strtok(NULL, "\n");
          String parsedPwd = String(pch);
          if (parsedService && parsedInstruction == "create") {
            enqueueNewPwd(parsedService, parsedLogin, parsedPwdRule, parsedPwd);
          } else if(parsedService && parsedInstruction == "update"){
            enqueueUpdatePwd(parsedService, parsedLogin, parsedPwdRule);
          }
//            Serial.print("Characteristic event, written: ");
//            Serial.println(static_cast<int>(value[0])); // Print the integer value
//
//            int receivedValue = static_cast<int>(value[0]);
//            if (receivedValue == 1) {
//                digitalWrite(ledPin, HIGH);
//            } else {
//                digitalWrite(ledPin, LOW);
//            }
        }
    }
};

void setupBLEServer() {
  
    BLEDevice::init("PwdMan"); // Name of the device
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY |
                        BLECharacteristic::PROPERTY_INDICATE
                      );
    pCharacteristic->addDescriptor(new BLE2902());
    // Register the callback for the ON button characteristic
    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
//    pCharacteristic->setValue("Hello World"); // Initial value

    pService->start();
  // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();

        FastLED.show();
    Serial.println("Waiting for a client connection...");
}
