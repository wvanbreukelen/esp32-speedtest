/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "WiFi.h"
#include <WiFiClient.h>

// AP Wi-FI credentials
const char* ssid = "ESP32_Server";
const char* password = "wachtwoord01";

WiFiServer server(24);
WiFiClient client;

const int arraySize = 750;
uint8_t data[arraySize];
int sum = 0;

unsigned int counter = 0;
unsigned long lastTime = millis();

void setup()
{
	Serial.begin(9600);

	
	delay(100);
	WiFi.mode(WIFI_AP_STA);
	setupAccessPoint();
	setupServer();
}

void setupAccessPoint() {
	WiFi.disconnect();

	Serial.println("Starting access point...");
	WiFi.softAP(ssid, password);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("My IP address is: ");
	Serial.println(myIP);
}

void setupServer() {
	Serial.println("Setting up TCP server...");

	server.begin();

	Serial.println("Done!");

}

void loop() {

	client = server.available();

	if (client) {
		Serial.println("Client connected!");


		while (client.connected()) {
			if (client.available() > 0) {
				//char inChar = client.read();
				client.read(data, arraySize);
				counter += arraySize;


				if (millis() - lastTime > 10000) {
					for (int i = 0; i < arraySize; i++) {
						sum += data[i];
					}

					Serial.print("Sum: ");
					Serial.println(sum);
					Serial.print((counter / ((millis() - lastTime) / 1000)) / 1000);
					Serial.println("Kb/s");
					
					counter = 0;
					sum = 0;
					lastTime = millis();
				}

				//Serial.print("Char: ");
				//Serial.println(inChar);
			}

		}
	}
}