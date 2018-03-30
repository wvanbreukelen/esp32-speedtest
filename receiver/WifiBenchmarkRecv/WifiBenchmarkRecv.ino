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

// Static network configuration
IPAddress ip(192, 168, 4, 4);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiClient client;

const char *host = "192.168.4.1";
const int port = 24;
const int arraySize = 750;
uint8_t data_send[arraySize];
int sum = 0;

void setup()
{
    Serial.begin(9600);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    //WiFi.mode(WIFI_STA);
    WiFi.disconnect();

	WiFi.config(ip, gateway, subnet);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print('.');
	}

    Serial.println("Connected!");
	setupClient();

	// Fill with dummy data
	for (int i = 0; i < arraySize; i++) {
		data_send[i] = i + 1;
	}

	for (int i = 0; i < arraySize; i++)
		sum += data_send[i];

	Serial.print("Sum: ");
	Serial.println(sum);
}

void setupClient() {
	//client.setNoDelay(1); // https://github.com/esp8266/Arduino/issues/1853

	while (!client.connect(host, port)) {
		delay(500);
	}

	Serial.print("Client successfully configured!");
}

void loop() {
	client.write( (const uint8_t *) data_send, arraySize);
	//delayMicroseconds(1);
}