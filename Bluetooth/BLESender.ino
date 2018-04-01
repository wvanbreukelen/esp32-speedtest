/*
 * BLESender.ino
 *
 * Created: 4/1/2018 12:25:48 PM
 * Author: Wiebe
 */

#include "BLEDevice.h"
#include "esp_gatt_common_api.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("7d1b0c69-2de6-412a-8dae-bab9d9a29f54");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("35a3896f-d070-44ca-ae52-9ec152a0c7e9");

static BLEAddress *pServerAddress;
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;

const size_t packetSize = 290;

uint8_t dataSend[packetSize];

static void notifyCallback(
	BLERemoteCharacteristic* pBLERemoteCharacteristic,
	uint8_t* pData,
	size_t length,
	bool isNotify) {
		Serial.print("Notify callback for characteristic ");
		Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
		Serial.print(" of data length ");
		Serial.println(length);
}

class AdvertisedDeviceCallback : public BLEAdvertisedDeviceCallbacks {
	void onResult(BLEAdvertisedDevice advertisedDevice) {
		Serial.print("BLE Advertised Device found: ");
		Serial.println(advertisedDevice.toString().c_str());

		// We have found a device, let us now see if it contains the service we are looking for.
		if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {

			//
			Serial.print("Found our device!  address: ");
			advertisedDevice.getScan()->stop();

			pServerAddress = new BLEAddress(advertisedDevice.getAddress());
			doConnect = true;

		} // Found our server
	}
};

bool connectToServer(BLEAddress pAddress) {
	Serial.print("Connecting to ");
	Serial.println(pAddress.toString().c_str());

	BLEClient *pCLient = BLEDevice::createClient();

	pCLient->connect(pAddress);
	Serial.println("- Connected to server");

	BLERemoteService *pRemoteService = pCLient->getService(serviceUUID);
	if (pRemoteService == nullptr) {
		Serial.print("Failed to find device with service UUID: ");
		Serial.println(serviceUUID.toString().c_str());
		return false;
	}

	Serial.println("- Connected to service");

	pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
	if (pRemoteCharacteristic == nullptr) {
		Serial.print("Failed to find our characteristic UUID: ");
		Serial.println(charUUID.toString().c_str());
		return false;
	}
	Serial.println(" - Found our characteristic");

	std::string value = pRemoteCharacteristic->readValue();
	Serial.print("The characteristic value was: ");
	Serial.println(value.c_str());

	pRemoteCharacteristic->registerForNotify(notifyCallback);
}

void setup()
{
	Serial.begin(9600);
	Serial.println("Starting application...");

	

	// Fill array with data
	for (int i = 0; i < packetSize; i++) {
		dataSend[i] = i + 1;
	}

	
	BLEDevice::init("");
	BLEDevice::setMTU(300); // Set higher MTU for larger packets.


	Serial.println("Scanning...");

	

	BLEScan *pBLEScan = BLEDevice::getScan();
	pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallback);
	pBLEScan->setActiveScan(true);
	pBLEScan->start(30); // Scan for 30 seconds


}

void loop()
{
	// Continuesly check if we can connect
	if (doConnect) {
		if (connectToServer(*pServerAddress)) {
			Serial.println("Connected to the BLE server!");
			connected = true;
		} else {
			Serial.println("Failed to connect to the BLE server!");
		}

		doConnect = false;
	}

	  if (connected) {
		 // String newValue = "Time since boot: " + String(millis()/1000);
		  //Serial.println("Setting new characteristic value to \"" + newValue + "\"");
		  
		  // Set the characteristic's value to be the array of bytes that is actually a string.
		  //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
		 
		   pRemoteCharacteristic->writeValue(dataSend, packetSize);
	  }

	  delay(9);
	  // 80 & 2
}
