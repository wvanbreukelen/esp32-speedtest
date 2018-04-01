/*
 * BLEReceiver.ino
 *
 * Created: 4/1/2018 12:45:05 PM
 * Author: Wiebe
 */ 

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "7d1b0c69-2de6-412a-8dae-bab9d9a29f54"
#define CHARACTERISTIC_UUID "35a3896f-d070-44ca-ae52-9ec152a0c7e9"


BLECharacteristic *pCharacteristic;
BLERemoteCharacteristic *pRCharacteristic;

int counter = 0;
long curTime = millis();

class MyCallbacks: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic *pCharacteristic) {
		std::string value = pCharacteristic->getValue();

		if (value.length() > 0) {
			counter += value.length();
		}
	}
};


void setup()
{
	Serial.begin(9600);
	Serial.println("Starting BLE server...");

	BLEDevice::init("ESP32");
	BLEServer *pServer = BLEDevice::createServer();
	BLEService *pService = pServer->createService(SERVICE_UUID);
	pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
	 //pCharacteristic->addDescriptor(new BLE2902());
	 pCharacteristic->setCallbacks(new MyCallbacks());
	//pCharacteristic->setValue("Hello World!");
	pService->start();
	BLEAdvertising *pAdvertising = pServer->getAdvertising();
	pAdvertising->addServiceUUID(pService->getUUID());
	pAdvertising->start();
	Serial.println("Server started!");
}

void loop()
{
	if (millis() - curTime > 5000) {
		Serial.print((counter / ((millis() - curTime) / 1000.0) / 1000.0));
		Serial.println("kb/s");
		curTime = millis();
		counter = 0;
	}
	
	delay(20);
}
