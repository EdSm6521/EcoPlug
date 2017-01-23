
//-----------------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <EEPROM.h>

String ssid				    = "ECOPLUG0001";
String password			  = "pass12345";
const char *sta_ssid	= "ECOPLUG0001";
const char *sta_pass	= "pass12345";

/* Set these to your desired credentials. */

WiFiServer server(80);
WiFiClient client;
IPAddress  localIP;

#include "cPage.h"
#include "cCircularBuffer.h"

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */

void standaloneServer()
{
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(sta_ssid,sta_pass);
  // WiFi.config(saip);
  localIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(localIP);
  server.begin();
  Serial.println("HTTP server started");  
}

IPAddress ssip(192,168,4,1);
IPAddress dns(255,0, 0, 0); 

static int ssConnectionTimeOut = 30;
static int ssConnectionTimeOutCount = 0;

IPAddress newIP;

bool ssConnecting	= false;
int relayPin		= 14;

cCircularBuffer cmaxmin(100);
cCircularBuffer cbamps(10);

bool start = true;
bool initialized = false;

void setup()
{
	delay(1);
	Serial.begin(9600);
	delay(1);
	EEPROM.begin(512);
	delay(1);
	standaloneServer();
	pinMode(relayPin, OUTPUT);
	delay(10);
	initialized = false;
}

float wattsResult = 0.0f;

void loop()
{

    client = server.available();

	  if (!client)
	  {
		  if (initialized)
		  {
			  float result = analogRead(A0);
			  if (result <= cmaxmin.maxindex && result >= cmaxmin.minindex)
			  {
				  result = 0;
          wattsResult = 0.0f;
			  }
			  else
			  if (result > 0)
			  {
				  Serial.println(result);
				  cbamps.addElement(result);
				  if (cbamps.lindex != cbamps.findex)
				  {
					  result = fabs(cbamps.getAvg() - cmaxmin.avgindex);
					  Serial.println("#########");
					  Serial.println(result*6.0f, 3);
           wattsResult = result*6.0f;
				  }
			  }
			  delay(1);
		  }
          return;
      }

      Serial.println("new client");
      while(!client.available())
      {
        delay(1);
      }

      String req = client.readStringUntil('\r');
      Serial.println("req -> " + req);
      // client.print("48 watts");
      client.flush();

	  if (req.indexOf("/get_watts") != -1)
	  {
		  delay(5);
		  client.print(wattsResult);
	  }

	  String strIsOn = "OFF";

	  if (req.indexOf("/get_switch") != -1)
	  {
		  int isOn = digitalRead(relayPin);
		  if (isOn == HIGH)
		  {
			  strIsOn = "ON";
		  }
		  else
		  {
			  strIsOn = "OFF";
        wattsResult = 0.0f;
		  }
		  // delay(5);
      Serial.println(strIsOn);
		  client.print(strIsOn);
	  }

	  if (req.indexOf("/switchOFF") != -1)
	  {
		  strIsOn = "OFF";
		  digitalWrite(relayPin, LOW);
	  }

	  if (req.indexOf("/switchON") != -1)
	  {
		  strIsOn = "ON";
		  digitalWrite(relayPin, HIGH);
	  }


}

