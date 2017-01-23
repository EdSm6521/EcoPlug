
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
float wattsResult = 0.0f;
int i;
float average;
bool ssConnecting	= false;
int relayPin		= 14;

cCircularBuffer cmaxmin(200);
cCircularBuffer cbamps(100);
cCircularBuffer cprom(1000);
bool start = true;
bool initialized = false;

void setup()
{
  i=0;
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
float total;
int ceros = 0;
int noceros = 0;
void loop()
{

    client = server.available();

	  if (!client)
	  {
        if (i<2*cmaxmin.numElements){
          if(i%2==0){
            cmaxmin.addElement(analogRead(A0));
            delay(10);
          }
          i++;
          return;
        }
        else if (i==2*cmaxmin.numElements) {
          average = cmaxmin.getAvg();
          initialized = true;
          Serial.println(average);
          Serial.println(cmaxmin.maxindex);
          Serial.println(cmaxmin.minindex);
          i++;
          return;
        }
		  if (initialized)
		  {
        if(digitalRead(relayPin) != HIGH){
          cprom.addElement(0.0f);
        }
			  float result = analogRead(A0);
			  if (result <= cmaxmin.maxindex && result >= cmaxmin.minindex)
			  {
				  result = 0;
          wattsResult = 0.0f;
          cprom.addElement(wattsResult);
			  }
			  else
			  if (result > 0)
			  {
				  //Serial.println(result);
				  cbamps.addElement(result);
				  if (cbamps.lindex != cbamps.findex)
				  {
					  result = fabs(cbamps.getAvg() - average);
					  Serial.println("#########");
           wattsResult = result*7.0f;
           Serial.println(wattsResult);
           cprom.addElement(wattsResult);
           Serial.println(cprom.cindex);
				  }
			  }
			  delay(10);
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
     for(int j=0; j<cprom.numElements;j++){
      if(cprom.elements[j]!=0){
        noceros++;
      }
     }
      switch(noceros){
          case 5:
            total = cprom.getAvg()* 184;
            Serial.println("5 no cero");
            break;
          case 4:
            total = cprom.getAvg() * 250;
            Serial.println("4 no cero");
            break;
          case 3:
            total = cprom.getAvg() * 310;
            Serial.println("3 no cero");
            break;
          case 2:
            total = cprom.getAvg() * 510;
            Serial.println("2 no cero");
            break;
          case 6:
            total = cprom.getAvg() * 150;
            break;
          case 7:
          total = cprom.getAvg() * 125;
          break;
          case 8:
          total = cprom.getAvg() * 110;
          break;
          case 9:
          case 10:
            total = cprom.getAvg() * 100;
            break;
          case 1:
            Serial.println("1 no cero");
            total = 0.0f;
            break;
          default:
            total = 0.0f;
            Serial.println("0 no cero");
            break;
        }      
        
		  client.print(String(total)+" "+String(noceros));
      noceros = 0; 
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

