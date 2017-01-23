
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <EEPROM.h>

String ssid  = "SHMBA0001";
String password  = "pass12345";
const char *sta_ssid  = "SHMBA0001";
const char *sta_pass  = "pass12345";
/* Set these to your desired credentials. */

WiFiServer server(80);
WiFiClient client;
IPAddress  localIP;

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

bool ssConnecting = false;

void setup()
{
	delay(1);
	Serial.begin(115200);
	delay(1);
	EEPROM.begin(512);
	delay(1);
	standaloneServer();
}

void loop()
{
	// server.handleClient();
    
	 Serial.println(analogRead(A0));

      client = server.available();
      
      if (!client)
      {
          return;
      }

      Serial.println("new client");
      while(!client.available())
      {
        delay(1);
      }

      String req = client.readStringUntil('\r');
      Serial.println("req -> " + req);
      client.flush();
}



