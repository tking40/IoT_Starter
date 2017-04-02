
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>

#define WEBSITE  "api.carriots.com"
#define API_KEY "b913fc3a0414c7b34dee56e71df39d4ffcaffe3b53890de7e080fc11f924aca6"
//#define DEVICE  "TempSensor@tking40"
#define DEVICE "TempSensor@tking40.tking40"

// WLAN parameters
#define WLAN_SSID       "upstairs"
#define WLAN_PASS       "notdownstairs"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2
const char* host = "api.carriots.com";

int value = 0;
const int httpPort = 80;

// Init functions
void connectWifiAccessPoint(String ssid, String password)
{
  Serial.println("Connecting to:\n" + ssid);
  /* 
  the amazon code converts ssid and pw to charactera arraays for passing 
  into the wifi begin code, but that doesn't seem necesary according to 
  the wifi_test code
  */
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup()
{
  // Initialize
	Serial.begin(115200);
	delay(100);

	connectWifiAccessPoint(WLAN_SSID, WLAN_PASS);
}


  
  
void loop()
{
	delay(1000);
	value++;
	publish(String(value));
  
	delay(9000);
}

void publish(String data) {
  String datastr = "{\"protocol\":\"v2\",\"device\":\""+String(DEVICE)+"\",\"at\":\"now\",\"data\":{\"Temperature\":"+String(data) + "}}";
	int length = datastr.length();
	Serial.print("Data length");
	Serial.println(length);
	Serial.println();
	
	// Print request for debug purposes
 	Serial.println("POST /streams HTTP/1.1");
	Serial.println("Host: api.carriots.com");
	Serial.println("Accept: application/json");
	Serial.println("User-Agent: Arduino-Carriots");
	Serial.println("Content-Type: application/json");
	Serial.println("carriots.apikey: " + String(API_KEY));
	Serial.println("Content-Length: " + String(length));
	Serial.print("Connection: close");
	Serial.println();
	Serial.println(datastr);

	// Send request
	Serial.print("connecting to "); 
	Serial.println(host);
	// Use WiFiClient class to create TCP connections
	WiFiClient client;
	if (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
	  return;
	}
 
	if (client.connected()) {
		Serial.println("Connected!");
		client.println("POST /streams HTTP/1.1");
		client.println("Host: api.carriots.com");
		client.println("Accept: application/json");
		client.println("User-Agent: Arduino-Carriots");
		client.println("Content-Type: application/json");
		client.println("carriots.apikey: " + String(API_KEY));
		client.println("Content-Length: " + String(length));
		client.println("Connection: close");
		client.println();

		client.println(datastr);
	}
	else {
		Serial.println(F("Connection failed"));    
		return;
	}

	Serial.println(F("-------------------------------------"));
	while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
	Serial.println(F("-------------------------------------"));
	Serial.println(F("\n\nDisconnecting"));
}
