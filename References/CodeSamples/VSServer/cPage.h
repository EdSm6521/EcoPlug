// cPage.h

#ifndef _CPAGE_h
#define _CPAGE_h

#include "cServer.h"

void htmlPrint(String _str)
{
	Serial.println(_str);
	client.print(_str + "\n");
}

void printPageHeader()
{
	htmlPrint("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>");
	htmlPrint("<html xmlns=\"http://www.w3.org/1999/xhtml\">");
	htmlPrint("<head>");
	htmlPrint("<meta http-equiv=\"content-type\" content=\"text/html; charset=iso-8859-1\" />");
	htmlPrint("<meta name=\"generator\" content=\"Amaya, see http://www.w3.org/Amaya/\" />");
	htmlPrint("</head>");
	htmlPrint("<body style=\"background-color:#E5E5E5;margin-left:10em;margin-right:auto;\">");
	htmlPrint("<p style=\"font-size: 80pt\"><button style=\"font-size: 80pt\"></button></p>");
}

void printPageEnd()
{
	htmlPrint("</body>");
	htmlPrint("</html>");
}

void printButtons()
{
	for (int r = 0; r<PORTMAP_PINS; r++)
	{

		String sstate = "ON";
		String svalue = "0";
		String tvalue = "0";
		if (digitalRead(portMap[r]) == LOW)
		{
			sstate = "OFF";
			svalue = "1";
		}
		tvalue = (String)portTime[r];
		String sindex = (String)(r + 1);
		String sr = String(r);
		String ch = "/gpio" + sr + "/" + svalue;

		String startbutton = "<p style = \"font-size: 80pt\">";
		String tbuttonDesc = "Timer - OFF";
		if (portTime[r] != -1)
		{
			tbuttonDesc = "Timer - " + (String)tvalue + " mins";
		}
		String tbutton = "<a href=\"/tmr" + sr + "/" + tvalue + "\"><button style=\"font-size: 40pt;margin-left:1em\">" + tbuttonDesc + "</button></a>";
		String sbutton = "<a href=\"" + ch + "\"><button style=\"font-size: 80pt\">" + sindex + " - " + sstate + "</button></a>";
		String endbutton = "</p>";
		String strButton = startbutton + sbutton + endbutton;
 #ifdef SLEEP_TIMER
		if (sstate == "ON")
		{
			strButton = startbutton + sbutton + tbutton + endbutton;
		}
		else
		{
			portTime[r] = -1;
		}
 #endif
		htmlPrint(strButton);
	}
	htmlPrint("<p style=\"font-size: 80pt\"><a href=\"http://" + SIP + "\"><button style=\"font-size: 80pt\">Refresh</button></a></p>");
	// htmlPrint("<p style=\"font-size: 80pt\"><a href=\"config/page\"><button style=\"font-size: 80pt\">Config</button></a></p>");
}

void configPage()
{
	printPageHeader();
	// htmlPrint("<p style=\"font-size: 80pt\"><a href=\"config/timers\"><button style=\"font-size: 80pt\">Timers</button></a></p>");
	// htmlPrint("<p style=\"font-size: 80pt\"><a href=\"config/names\"><button style=\"font-size: 80pt\">Names</button></a></p>");
	htmlPrint("<p style=\"font-size: 80pt\"><a href=\"config/wifi\"><button style=\"font-size: 80pt\">Config Local Wifi</button></a></p>");
	htmlPrint("<p style=\"font-size: 80pt\"><a href=\"http://" + SIP + "\"><button style=\"font-size: 80pt\">Back</button></a></p>");
	printPageEnd();
}


#endif

