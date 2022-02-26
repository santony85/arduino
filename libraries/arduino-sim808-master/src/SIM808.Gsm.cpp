#include "SIM808.h"

AT_COMMAND(SEND_SMS, "+CMGS=\"%s\"");

TOKEN_TEXT(CPIN, "+CPIN");
TOKEN_TEXT(CSQ, "+CSQ");
TOKEN_TEXT(CMGS, "+CMGS");
TOKEN_TEXT(CMGL, "+CMGL");
TOKEN_TEXT(CMGR, "+CMGR");

bool SIM808::simUnlock(const char* pin)
{
	sendAT(TO_F(TOKEN_CPIN), TO_F(TOKEN_WRITE), pin);

	return waitResponse(5000L) == 0;
}

size_t SIM808::getSimState(char *state, size_t stateSize)
{
	sendAT(TO_F(TOKEN_CPIN), TO_F(TOKEN_READ));
	if(waitResponse(5000L, TO_F(TOKEN_CPIN)) != 0) return 0;

	copyCurrentLine(state, stateSize, strlen_P(TOKEN_CPIN) + 2);

	return waitResponse() == 0 ?
		strlen(state) :
		0;
}

size_t SIM808::getImei(char *imei, size_t imeiSize)
{
	//AT+GSN does not have a response prefix, so we need to flush input
	//before sending the command
	flushInput();

	sendAT(S_F("+GSN"));	
	waitResponse(SIMCOMAT_DEFAULT_TIMEOUT, NULL); //consuming an extra line before the response. Undocumented

	if(waitResponse(SIMCOMAT_DEFAULT_TIMEOUT, NULL) != 0) return 0;
	copyCurrentLine(imei, imeiSize);

	return waitResponse() == 0?
		strlen(imei) :
		0;
}

SIM808SignalQualityReport SIM808::getSignalQuality()
{
	uint8_t quality;
	uint8_t errorRate;

	SIM808SignalQualityReport report = {99, 99, 1};

	sendAT(TO_F(TOKEN_CSQ));
	if(waitResponse(TO_F(TOKEN_CSQ)) != 0 ||
		!parseReply(',', (uint8_t)SIM808SignalQualityResponse::SignalStrength, &quality) ||
		!parseReply(',', (uint8_t)SIM808SignalQualityResponse::BitErrorrate, &errorRate) ||
		waitResponse())
		return report;

	report.rssi = quality;
	report.ber = errorRate;

	if (quality == 0) report.attenuation = -115;
	else if (quality == 1) report.attenuation = -111;
	else if (quality == 31) report.attenuation = -52;
	else if (quality > 31) report.attenuation = 1;
	else report.attenuation = map(quality, 2, 30, -110, -54);

	return report;
}

bool SIM808::setSmsMessageFormat(SIM808SmsMessageFormat format)
{
	sendAT(S_F("+CMGF="), (uint8_t)format);
	return waitResponse() == 0;
}

bool SIM808::sendSms(const char *addr, const char *msg)
{
	if (!setSmsMessageFormat(SIM808SmsMessageFormat::Text)) return false;
	sendFormatAT(TO_F(AT_COMMAND_SEND_SMS), addr);

	if (!waitResponse(S_F(">")) == 0) return false;

	SENDARROW;
	print(msg);
	print((char)0x1A);

	return waitResponse(60000L, TO_F(TOKEN_CMGS)) == 0 &&
		waitResponse() == 0;
}
/* ok */
int SIM808::isSMSunread(char *response, size_t responseSize){
	flushInput();
	char lstsms[3];
	sendAT(S_F("+CMGL=\"REC UNREAD\",1"));
	if(waitResponse(TO_F(TOKEN_CMGL)) == 0) {
	  copyCurrentLine(lstsms, 3, strlen_P(TOKEN_CMGL) + 2);//2
	  return atoi(lstsms); 
	}
	return 0;
}


String SIM808::readSms(int messageIndex){
	//flushInput();
	String cmd="+CMGR="+String(messageIndex);
	char message[160];
	sendCommand(cmd.c_str(), message, 160);

	char* result;
	uint8_t index = 1;
	char* p = find(message, '\n', index);
	if (p == NULL) {
		Serial.println("Not found");
		return "";
	}
	String res=p;
	if(res.indexOf("ERROR")>-1)return "";
	else {
		if(res.indexOf("\r\n")>-1){
			String vmess= res.substring(0, res.indexOf("\r\n"));
			return vmess;
		}
	}
	return "";

}

bool SIM808::deleteAllSms(){
  flushInput();
  sendAT(S_F("+CMGD=1,4"));
  return waitResponse(5000L) == 0;

}

bool SIM808::deleteSms(int messageIndex)
{
	String cmd="+CMGD="+String(messageIndex);
	sendAT(S_F(cmd.c_str()));
	return waitResponse() == 0;
}