
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
int sensorPin = 0;     // the cell and 10K pulldown are connected to a0
int sensorReading;     // the analog reading from the sensor divider
int LEDbrightness = 0;

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


int lightsOn = 0;
int lightStatus = 0;

  
void loop()
{
	sensorReading = analogRead(sensorPin);
  float voltage = sensorReading * 3.3;
  voltage /= 1024.0;
  Serial.print(voltage);
  Serial.println(" volts");
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); Serial.println(" degrees C");
  // now convert to Fahrenheit
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF); Serial.println(" degrees F");

  /*
  if (photocellReading < 400) {
    lightsOn = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (photocellReading > 700) {
    lightsOn = 1;
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (lightsOn != lightStatus) {
    if(lightsOn){
      publish(String(1));
    } else {
      publish(String(0));
    }
    lightStatus = lightsOn;
  }
  */
  publish(String(temperatureF));
	delay(360000);
}

void publish(String data) {
  String datastr = "{\"protocol\":\"v2\",\"device\":\""+String(DEVICE)+"\",\"at\":\"now\",\"data\":{\"Temperature\":"+String(data)+"}}";
	//String datastr = "{\"protocol\":\"v2\",\"device\":\""+String(DEVICE)+"\",\"at\":\"now\",\"data\":{\"dynamic_conf\":1.2,\"static_conf\":1.0,\"firmware\":0.9,\"battery_level\":72}}";
  
	int length = datastr.length();


	// Send request
	Serial.print("connecting to "); 
	Serial.println(host);
	// Use WiFiClient class to create TCP connections
	WiFiClient client;
  int loopNum = 0;
	while (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
    loopNum++;
    if (loopNum > 3) {
      Serial.println("Restarting.");
      ESP.restart();
    }
    Serial.print("Retrying in ");
    for (int i=6;i--;i>2) {
      Serial.print(i);
      delay(333);
      Serial.print(".");
      delay(333);
      Serial.print(".");
      delay(333);
    }
    Serial.print("\nConnecting...");
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
