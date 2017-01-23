// cServer.h

#ifndef _CSERVER_h
#define _CSERVER_h

#include "cUtils.h"

IPAddress ssip(192, 168, 4, 1);
IPAddress dns(255, 0, 0, 0);

static int ssConnectionTimeOut = 30;
static int ssConnectionTimeOutCount = 0;

IPAddress newIP;
String SIP = "";

bool ssConnecting = false;

void standaloneServer()
{
	Serial.println();
	Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	// WiFi.disconnect();
	WiFi.mode(WIFI_AP);
	WiFi.softAP(sta_ssid, sta_pass);
	localIP = WiFi.softAPIP();
  Serial.println("AP Name: " + (String)sta_ssid);
	Serial.print("AP IP address: ");
	Serial.println(localIP);
	SIP = IPAddresstoCharArray(localIP);
	server.begin();
	Serial.println("HTTP server started");
	bstandaloneServer = true;
}

bool startServer(bool _test = true)
{
	Serial.print("\nConnecting to ");
	ssConnecting = true;
	ssConnectionTimeOutCount = 0;
	Serial.println(ssid);
	char cssid[ssid.length() + 2];
	char cpassword[password.length() + 2];
	ssid.toCharArray(cssid, ssid.length() + 1);
	password.toCharArray(cpassword, password.length() + 1);
	WiFi.disconnect();
	WiFi.mode(WIFI_AP);
	WiFi.begin(cssid, cpassword, 1);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
		ssConnectionTimeOutCount++;
		if (ssConnectionTimeOutCount > ssConnectionTimeOut)
		{
			Serial.print("Timeout reached Not connected\n");
			ssConnecting = false;
			return false;
		}
	}
	Serial.println("WiFi connected");
	server.begin();                      // Iniciamos el servidor
	Serial.println("Server started");
	localIP = WiFi.localIP();
	Serial.println(localIP);      // Imprimimos la IP
	SIP = IPAddresstoCharArray(localIP);
	bstandaloneServer = false;
	return true;
}

#endif

