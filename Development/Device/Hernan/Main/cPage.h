
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

void printHTMLButton(String _htmlDest,String _description)
{
	String tbutton = "<p><a href=\"/" + _htmlDest + "\"><button style=\"font-size: 80pt;text-align:center;margin-left:auto;margin-right:auto;\">" + _description + "</button></a></p>";
	htmlPrint(tbutton);
}

void printPageEnd()
{
	htmlPrint("</body>");
	htmlPrint("</html>");
}
 
