/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
#define DHT11LIB_VERSION "0.4.1"
#include "DHT.h"
#define DHTLIB_OK				0
#define DHTLIB_ERROR_CHECKSUM	-1
#define DHTLIB_ERROR_TIMEOUT	-2
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include <Arduino.h>
#include <WiFiLink.h>
#include <Bridge.h>

#include <WiFiClient.h>

/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 
char ssid[] = "plorais";     //  your network SSID (name)
char pass[] = "c7o2r1e4";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
WiFiClient wifi;
HttpClient client = HttpClient(wifi, "api.heclouds.com");
HttpClient client1 = HttpClient(wifi, "api.heclouds.com");
//HttpClient client = HttpClient(wifi, "47.100.191.229",80);
//HttpClient client1 = HttpClient(wifi, "47.100.191.229",80);
void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600); 
 sensors.begin(); 
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
} 
void loop(void) 
{ 
  int pin =4;
  
  // BUFFER TO RECEIVE
	uint8_t bits[5];
	uint8_t cnt = 7;
	uint8_t idx = 0;

	// EMPTY BUFFER
	for (int i=0; i< 5; i++) bits[i] = 0;

	// REQUEST SAMPLE
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	delay(18);
	digitalWrite(pin, HIGH);
	delayMicroseconds(40);
	pinMode(pin, INPUT);

	// ACKNOWLEDGE or TIMEOUT
	unsigned int loopCnt = 10000;
	while(digitalRead(pin) == LOW)
		if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;

	loopCnt = 10000;
	while(digitalRead(pin) == HIGH)
		if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;

	// READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
	for (int i=0; i<40; i++)
	{
		loopCnt = 10000;
		while(digitalRead(pin) == LOW)
			if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;

		unsigned long t = micros();

		loopCnt = 10000;
		while(digitalRead(pin) == HIGH)
			if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;

		if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
		if (cnt == 0)   // next byte?
		{
			cnt = 7;    // restart at MSB
			idx++;      // next byte!
		}
		else cnt--;
	}

  int humidity    = bits[0]; 
  Serial.print("\n");
  Serial.print("Humidity (%): ");
  Serial.println((float)humidity, 2);

 sensors.requestTemperatures(); // Send the command to get temperature readings 
 Serial.print("Temperature is: "); 
 Serial.print(sensors.getTempCByIndex(0)-2); // Why "byIndex"?  
 
 Serial.print("\n");
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
   
 if ((status == WL_CONNECTED)) {
   
    // StaticJsonDocument<256> doc;
    // JsonObject root = doc.to<JsonObject>();
    
    // JsonArray datastreams = root.createNestedArray("datastreams");
    
    // JsonObject temperature = datastreams.createNestedObject();
    // temperature["id"]= "temperature";
    // JsonArray datapoints1 = temperature.createNestedArray("datapoints");
    // JsonObject value1 = datapoints1.createNestedObject();
    // value1["value"]=String(sensors.getTempCByIndex(0)-2);
    
    
    // JsonObject moisture =  datastreams.createNestedObject();
    // moisture["id"]= "moisture";
    // JsonArray datapoints2 = moisture.createNestedArray("datapoints");
    // JsonObject value2 = datapoints2.createNestedObject();
    // value2["value"]=String(humidity);
   
    String temp=String(sensors.getTempCByIndex(0)-2);
    float hum=(float)humidity;
    String humidity=String(hum);
    String output="";
    //serializeJson(root, output);
    output="{\"datastreams\":[{\"id\":\"temperature\",\"datapoints\":[{\"value\":\""+temp+"\"}]}]}\n";
    client.beginRequest();
    client.post("/devices/553304452/datapoints");
    //client.post("/");
    client.sendHeader("api-key", "V5BBXsih6tOyXroC==dwuTCIRtA=");
    
    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", output.length());
    client.beginBody();
    
    //serializeJsonPretty(root,Serial);
    client.print(output);
   // client.print("\n");
    client.endRequest();
    
    int statusCode = client.responseStatusCode();
    
    String response = client.responseBody();
    Serial.print(statusCode) ;
    Serial.print(response );
    client.flush();
    client.stop();
    
    delay(1000);
    String output1="";
    output1="{\"datastreams\":[{\"id\":\"moisture\",\"datapoints\":[{\"value\":\""+humidity+"\"}]}]}\n";
    //output1=output;
    client1.beginRequest();
    client1.post("/devices/553304452/datapoints");
    //client1.post("/");
    client1.sendHeader("api-key", "V5BBXsih6tOyXroC==dwuTCIRtA=");
    
    client1.sendHeader("Content-Type", "application/json");
    client1.sendHeader("Content-Length", output1.length());
    client1.beginBody();
    
    //serializeJsonPretty(root,Serial);
    client1.print(output1);
   // client.print("\n");
    client1.endRequest();
    
    statusCode = client.responseStatusCode();
    
    response = client.responseBody();
    Serial.print(statusCode) ;
    Serial.print(response );
    client1.flush();
    client1.stop();
    
    delay(1000); 
    
    
    
    
    

    

      
      
      
   
  }
   delay(1000); 
}