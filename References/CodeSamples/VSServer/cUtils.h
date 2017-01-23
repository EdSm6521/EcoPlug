#ifndef _CUTILS_h
#define _CUTILS_h


String IPAddresstoCharArray(IPAddress _address)
{
	String str = String(_address[0]);
	str += ".";
	str += String(_address[1]);
	str += ".";
	str += String(_address[2]);
	str += ".";
	str += String(_address[3]);
	return str;
}

#endif

