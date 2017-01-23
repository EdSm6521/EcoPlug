
/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <EEPROM.h>

String ssid = "My-WIFI-Name";
String password = "MyPassword";
const char *sta_ssid = "SHM0A0181"; //"SHM0A0184";
const char *sta_pass = "pass12345"; // "pass4810";
/* Set these to your desired credentials. */

WiFiServer server(80);
WiFiClient client;
IPAddress  localIP;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
* connected to this access point to see it.
*/
#define PORTMAP_PINS 5
int portMap[PORTMAP_PINS];
int portTime[PORTMAP_PINS];
bool bstandaloneServer = true;

// #define LOCAL_CONNECTION;
#define SLEEP_TIMER;

#include "cUtils.h"
#include "cEEPROM.h"
#include "cPage.h"
#include "cServer.h"


void setup()
{
	delay(1);
	Serial.begin(115200);
	delay(1);
	EEPROM.begin(512);
	delay(1);

	bool result = false;

#ifdef LOCAL_CONNECTION
	int sp = readSSIDPASS();
	delay(1);
	int rStatus = readSTATUS();
	delay(1);
	Serial.print("Status " + rStatus);
	if (sp == 0)
	{
		if (rStatus == 0 || rStatus == 2 || rStatus == 3)
		{
			result = startServer();
			if (rStatus == 0 && result)
			{
				writeIP(localIP);
				delay(100);
				writeSTATUS(1);
				delay(100);
				abort();
			}
			if (rStatus == 2 && result)
			{
				writeSTATUS(1);
			}
		}
	}
#endif

	if (!result)
	{
		standaloneServer();
	}

  if(PORTMAP_PINS == 1)
  {
    portMap[0] = 12;
  }
  if (PORTMAP_PINS == 4)
  {
	  portMap[0] = 5;   // 4
	  portMap[1] = 4;   // 5
	  portMap[2] = 12;  // 13
	  portMap[3] = 14;  // 14
  }
  if(PORTMAP_PINS == 5)
  {
  	portMap[0] = 5;   // 4
  	portMap[1] = 4;   // 5
  	portMap[2] = 12;  // 13
  	portMap[3] = 14;  // 14
  	portMap[4] = 13;  // 16
  }

	for (int r = 0; r<PORTMAP_PINS; r++)
	{
		pinMode(portMap[r], OUTPUT);
		portTime[r] = -1;
	}
}



void printPage()
{

	printPageHeader();

#ifdef LOCAL_CONNECTION
	String ssidstr = sta_ssid;
	if (bstandaloneServer)
	{
		String ip = readIP();
		printButtons();
		htmlPrint("<p style=\"font-size: 80pt\"><a href=\"/config/wifi\"><button style=\"font-size: 50pt\">Config Local WIFI <span style=\"background-color:#c0c0c0\"></span></button></a></p>");
		htmlPrint("<p style=\"font-size: 80pt\"><a href=\"/clear/wifi\"><button style=\"font-size: 50pt\">Reset to Standalone Mode " + ssidstr + "<span style=\"background-color:#c0c0c0\"></span></button></a></p>");
	}
	else
	{
		printButtons();
		if (readSTATUS() == 1)
		{
			writeSTATUS(3);
			htmlPrint("<p style=\"font-size: 80pt\"><button style=\"font-size: 50pt\">" + ssid + " configured OK <span style=\"background-color:#c0c0c0\"></span></button></p>");
		}
		else
		{
			htmlPrint("<p style=\"font-size: 80pt\"><a href=\"/clear/wifi\"><button style=\"font-size: 50pt\">Reset to Standalone Mode " + ssidstr + "<span style=\"background-color:#c0c0c0\"></span></button></a></p>");
		}
	}
#else
	printButtons();
#endif

	printPageEnd();

	if (client)
	{
		client.stop();
	}

}

static int timer = 0;
static int subtimer = 0;

void loop()
{

	client = server.available();

#ifdef SLEEP_TIMER
    timer++;
	delay(1);
	if ((timer % 60000) == 0)
	{
		for (int r = 0; r < PORTMAP_PINS; r++)
		{
			if (portTime[r] != -1)
			{
				portTime[r]--;
				if (portTime[r] == 0)
				{
					digitalWrite(portMap[r], LOW);
				}
			}
		}
		timer = 0;
	}
#endif

	if (!client)
	{
		return;
	}

	int clientAvailableTimeOut = 500;
	int clientAvailableTimeOutCount = 0;

	Serial.println("new client");
	while (!client.available())
	{
		clientAvailableTimeOutCount++;
		if (clientAvailableTimeOutCount > clientAvailableTimeOut)
		{
			Serial.println(" clientAvailableTimeOut ! ");
			client.stop();
			return;
		}
		delay(1);
	}

	String req = "";
	
	if (client)
	{
		Serial.println("client.readStringUntil ");
		req = client.readStringUntil('\r');
		Serial.println("client.readStringUntil end , req = " + req);
		if (req == "")
		{ 
			return;
		}
	}
	else
	{
		return;
	}

	if (client)
	{
		client.flush();
	}
	else
	{
		return;
	}

	int val;
	int port = -1;
	String data = "";

	if (req.indexOf("/config/page") != -1)
	{
		configPage();
	}

#ifdef LOCAL_CONNECTION
	if (req.indexOf("/clear/wifi") != -1)
	{
		printPageHeader();
		htmlPrint(" WIFI " + ssid + " Configuration was erased \n Standalone WIFI Hotspot " + sta_ssid + " ENABLED");
		printPageEnd();
		clearSSIDPASS();
		client.stop();
		delay(100);
		abort();
	}

	if (req.indexOf("/resettolocal/wifi") != -1)
	{
			String ip = readIP();
			delay(10);
			htmlPrint("<p style=\"font-size: 80pt\"><a href=\"http://" + ip + "\"><button style=\"font-size: 50pt\"> Connect to WIFI : " + ssid + " and press this button ! " + "<span style=\"background-color:#c0c0c0\"></span></button></a></p>");

			printPageEnd();

			client.stop();

			delay(100);
			
			writeSTATUS(2);

			delay(100);

			abort();
	}

	if (req.indexOf("/refresh/wifi") != -1)
	{
		printPageHeader();
		if (ssConnecting)
		{
			htmlPrint("Connectig to WIFI " + ssid + "  " + ssConnectionTimeOutCount + " / " + ssConnectionTimeOut);
		}
		else
		{
			htmlPrint("Connectig to WIFI " + ssid + " FAILED ! ");
		}
		printPageEnd();
		return;
	}

	int wifiindex = req.indexOf("wifiname");
	if (wifiindex != -1)
	{
		ssid = "";
		for (int r = 0; r<30; r++)
		{
			char ca = req[wifiindex + 9 + r];
			if (ca == '&')
			{
				break;
			}
			ssid += ca;
		}
		wifiindex = req.indexOf("wifipass");
		password = "";
		for (int r = 0; r<30; r++)
		{
			char ca = req[wifiindex + 9 + r];
			if (r >= req.length() - 1 || ca == ' ')
			{
				break;
			}
			password += ca;
		}

		Serial.println("wifiname : '" + ssid + "' pass '" + password + "'");
		bool xerror = false;
		printPageHeader();
		if (ssid.length() < 8)
		{
			htmlPrint("<p style=\"font-size: 80pt\">Invalid WIFI NAME length less than 8 characters !</p>");
			xerror = true;
		}
		if (password.length() < 8)
		{
			htmlPrint("<p style=\"font-size: 80pt\">Invalid Password length less than 8 characters !</p>");
			xerror = true;
		}
		else
		{
			htmlPrint("<p style=\"font-size: 40pt\">Local WIFI Name " + ssid + "</p>");
			htmlPrint("<p style=\"font-size: 40pt\">Local WIFI Password " + password + "</p>");
			String ssidstr = sta_ssid;
			htmlPrint("<p style=\"font-size: 80pt\"><a href=\"http://192.168.4.1\"><button style=\"font-size: 50pt\"> Connect to WIFI : " + ssidstr + " and press this button ! " + "<span style=\"background-color:#c0c0c0\"></span></button></a></p>");
		}
		printPageEnd();

		if (!xerror)
		{
			clearSSIDPASS();
			delay(100);
			writeSSIDPASS();
			writeSTATUS(0);
			delay(100);
			abort();
		}

		return;
	}

	if (req.indexOf("/config/wifi") != -1)
	{
		printPageHeader();
		htmlPrint("<form action=\"" + IPAddresstoCharArray(localIP) + "/\">");
		htmlPrint("<p style=\"font-size: 80pt\">WIFI Name : <input  style=\"font-size: 40pt\" type=\"text\" name=\"wifiname\" /></p>");
		htmlPrint("<p style=\"font-size: 80pt\">WIFI Password : <input  style=\"font-size: 40pt\" type=\"text\" name=\"wifipass\" /></p>");
		htmlPrint("<p style=\"font-size: 80pt\"><input  style=\"font-size: 80pt\" type=\"submit\" value=\"Ok\" /></p>");
		htmlPrint("</form>");
		printPageEnd();
		return;
	}
#endif

	for (int r = 0; r<PORTMAP_PINS; r++)
	{
		String sr = String(r);
		
		String th = "/tmr" + sr;
		
		if (req.indexOf(th) != -1)
		{
			if (portTime[r] == -1)
			{
				portTime[r] = 0;
			}
			portTime[r] += 30;
			if (portTime[r]> 120)
			{
				portTime[r] = -1;
			}
		}

		for (int s = 0; s<2; s++)
		{
			String ss = String(s);
			String ch = "/gpio" + sr + "/" + ss;
			if (req.indexOf(ch) != -1)
			{
				val = s;
				port = portMap[r];
				data = ch;
				digitalWrite(port, val);
				Serial.println(" writing output " + (String)port + "  " + (String)val);
				goto end;
			}
		}

	}

end:
	printPage();
}
