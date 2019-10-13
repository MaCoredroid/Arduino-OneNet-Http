/********************************************************************/
#include <IRremote.h>
#include <IRremoteInt.h>
#include <boarddefs.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

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
int RECV_PIN = 11;        
IRrecv irrecv(RECV_PIN); 
decode_results results;

IRsend irsend;  

bool flag=0;
int limit=60;
void setup(void) 
{ 
 // start serial port 
  Serial.begin(9600); 
  irrecv.enableIRIn();
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
  int temperature = bits[2]; 
  Serial.print(flag);
  if(humidity>=limit&&flag){
    Serial.print("\nsignalSend2:");
    controlHumidifier();
    flag=0;
  }
  if(humidity<limit&&!flag){
    Serial.print("\nsignalSend2:");
    controlHumidifier();
    flag=1;
  }
  Serial.print("\n");
  Serial.print("Humidity (%): ");
  Serial.println((float)humidity, 2);

  
 Serial.print("Temperature is: "); 
 Serial.print(temperature); // Why "byIndex"?  
 
 Serial.print("\n");
   
 if ((status == WL_CONNECTED)) {
   
    String temp=String(temperature);
    
    String output="";
    output="{\"datastreams\":[{\"id\":\"temperature\",\"datapoints\":[{\"value\":\""+temp+"\"}]}]}\n";
    client.beginRequest();
    client.post("/devices/553304452/datapoints");
    client.sendHeader("api-key", "V5BBXsih6tOyXroC==dwuTCIRtA=");
    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", output.length());
    client.beginBody();
    client.print(output);
    client.endRequest();
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    Serial.print(statusCode) ;
    Serial.print(response );
    Serial.print("\n");
    client.flush();
    client.stop();
    delay(1000);
  
    
    
    
    
    
    
    

    

      
      
      
   
  }
  if ((status == WL_CONNECTED)) {
   
    
    float hum=(float)humidity;
    String humidity=String(hum);
    String output1="";
    output1="{\"datastreams\":[{\"id\":\"moisture\",\"datapoints\":[{\"value\":\""+humidity+"\"}]}]}\n";
    client.beginRequest();
    client.post("/devices/553304452/datapoints");
    client.sendHeader("api-key", "V5BBXsih6tOyXroC==dwuTCIRtA=");
    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", output1.length());
    client.beginBody();
    client.print(output1);
    client.endRequest();
    int statusCode1 = client.responseStatusCode();
    String response1 = client.responseBody();
    Serial.print(statusCode1) ;
    Serial.print(response1);
    Serial.print("\n");
    client.flush();
    client.stop();
    delay(1000);
    
    
    
    
    
    
    
    

    

      
      
      
   
  }
  if ((status == WL_CONNECTED)) {
   
    
   
    client.beginRequest();
    client.get("/devices/553304452/datastreams/threshold");
    client.sendHeader("api-key", "V5BBXsih6tOyXroC==dwuTCIRtA=");
    client.endRequest();
    int statusCode2 = client.responseStatusCode();
    String ans=client.responseBody();
    char response2[ans.length()+1];
    strcpy(response2, ans.c_str());
    limit=10*(response2[114]-'0')+(response2[115]-'0');
    Serial.print(statusCode2);
    Serial.print("\n");
    Serial.print(limit);
    Serial.print("\n");
    client.flush();
    client.stop();
    delay(1000);
    
    
    
    
    
    
    
    

    

      
      
      
   
  }
   delay(1000); 
   
  
}

void controlHumidifier(){
  Serial.print("\nsignalSend1:");
  unsigned long data=0x1FE40BF;
  int nbits=32;
  // Set IR carrier frequency
	irsend.enableIROut(38);

	// Header
	irsend.mark(9000);
	irsend.space(4500);

	// Data
	for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
		if (data & mask) {
			irsend.mark(560);
			irsend.space(1690);
		} else {
			irsend.mark(560);
			irsend.space(560);
		}
	}

	// Footer
	irsend.mark(560);
	for(int i=0;i<20;i++){
	irsend.mark(9000);
	irsend.space(2250);
	irsend.mark(560);
	irsend.space(98190);
	//end =time(NULL);  
	}
	irsend.space(0);  // Always end with the LED off
}