#ifndef _CEEPROM_h
#define _CEEPROM_h

static const int SSID_LENGTH = 32;
static const int PASS_LENGTH = 64;
static const int STATUS_LENGTH = 1;
static const int IP_LENGTH = 16;
static const int NAMES_LENGTH = 8;


int readSSIDPASS()
{
	// read eeprom for ssid and pass
	Serial.println("Reading EEPROM ssid");
	String esid;
	for (int i = 0; i < SSID_LENGTH; ++i)
	{
		char ca = char(EEPROM.read(i));
		if (i == 0 && ca == 255)
		{
			return 1;
		}
		if (ca != 255)
		{
			esid += ca;
		}
	}
	Serial.print("SSID: ");
	Serial.println("'" + esid + "'");
	Serial.println("Reading EEPROM pass");
	ssid = esid;
	String epass = "";
	for (int i = SSID_LENGTH; i < (SSID_LENGTH + PASS_LENGTH); ++i)
	{
		char ca = char(EEPROM.read(i));
		if (i == SSID_LENGTH && ca == 255)
		{
			return 2;
		}
		if (ca != 255)
		{
			epass += ca;
		}
	}
	Serial.print("PASS: ");
	Serial.println("'" + epass + "'");
	password = epass;
	return 0;
}

String readIP()
{
	String ip = "";
	int s = 0;
	Serial.print("readIP ");
	for (int r = (SSID_LENGTH + PASS_LENGTH + STATUS_LENGTH); r<(SSID_LENGTH + PASS_LENGTH + STATUS_LENGTH + IP_LENGTH); ++r)
	{
		char ca = char(EEPROM.read(r));
		if (ca == 255){ break; }
		ip += ca;
		s++;
	}
	Serial.println("'" + ip + "'");
	return ip;
}

char readSTATUS()
{
	return char(EEPROM.read(SSID_LENGTH + PASS_LENGTH));
}

void writeIP(IPAddress _address)
{
	String ip = IPAddresstoCharArray(_address);
	Serial.println("writeIP '" + ip + "'");
	int s = 0;
	for (int r = (SSID_LENGTH + PASS_LENGTH + STATUS_LENGTH); r<(SSID_LENGTH + PASS_LENGTH + STATUS_LENGTH + ip.length()); ++r)
	{
		EEPROM.write(r, ip[s]);
		Serial.print(ip[s]);
		s++;
	}
	EEPROM.commit();
	Serial.println("commit");
	delay(10);
}

void clearSSIDPASS()
{
	for (int i = 0; i < (SSID_LENGTH + PASS_LENGTH + STATUS_LENGTH + IP_LENGTH); ++i)
	{
		EEPROM.write(i, 255);
	}
	EEPROM.commit();
}

void writeSSIDPASS()
{
	Serial.println("writing eeprom ssid:");
	for (int i = 0; i < ssid.length(); ++i)
	{
		EEPROM.write(i, ssid[i]);
		Serial.print("Wrote: ");
		Serial.println(ssid[i]);
	}
	Serial.println("writing eeprom pass:");
	for (int i = 0; i < password.length(); ++i)
	{
		EEPROM.write(32 + i, password[i]);
		Serial.print("Wrote: ");
		Serial.println(password[i]);
	}

	EEPROM.commit();
}

void writeSTATUS(char _value)
{
	EEPROM.write(SSID_LENGTH + PASS_LENGTH, _value);
	EEPROM.commit();
	delay(1);
}


#endif

