#include "SIM808.h"



AT_COMMAND(SET_BEARER_SETTING_PARAMETER, "+SAPBR=3,1,\"%S\",\"%s\"");
AT_COMMAND(SET_BEARER_SETTING, "+SAPBR=%d,%d");
AT_COMMAND(GPRS_ATTACH, "+CGATT=%d");

AT_COMMAND_PARAMETER(BEARER, CONTYPE);
AT_COMMAND_PARAMETER(BEARER, APN);
AT_COMMAND_PARAMETER(BEARER, USER);
AT_COMMAND_PARAMETER(BEARER, PWD);

TOKEN_TEXT(GPRS, "GPRS");
TOKEN_TEXT(CGATT, "+CGATT");
TOKEN_TEXT(CIPSHUT, "+CIPSHUT");
TOKEN_TEXT(SHUT_OK, "SHUT OK");
TOKEN_TEXT(CGREG, "+CGREG");
TOKEN_TEXT(CIPMUX, "+CIPMUX");


/*  TCP */
AT_COMMAND(CIPMUX, "+CIPMUX=%d");
AT_COMMAND(TCP_CSTT, "+CSTT=\"%S\"");
TOKEN_TEXT(CIICR, "+CIICR");
TOKEN_TEXT(CIFSR, "+CIFSR");
AT_COMMAND(TCP_CIPSTART, "+CIPSTART=\"TCP\",\"%S\",\"%S\"");
TOKEN_TEXT(CIPSEND, "+CIPSEND");
TOKEN_TEXT(CONNECT_OK, "CONNECT OK");

/*
"\x10\x2c\x00\x04\x4d\x51\x54\x54\x04\x02\x00\x3c\x00\x20\x39\x37" \
"\x32\x63\x31\x30\x38\x64\x35\x31\x34\x62\x34\x65\x30\x35\x61\x30" \
"\x35\x62\x38\x61\x61\x37\x66\x37\x39\x34\x39\x33\x63\x39"
*/
/*
0000   10 2c 00 04 4d 51 54 54 04 02 00 3c 00 20 39 37   .,..MQTT...<. 97
0010   32 63 31 30 38 64 35 31 34 62 34 65 30 35 61 30   2c108d514b4e05a0
0020   35 62 38 61 61 37 66 37 39 34 39 33 63 39         5b8aa7f79493c9
*/

//,MQTT< 972c108d514b4e05a05b8aa7f79493c9


bool SIM808::setBearerSetting(ATConstStr parameter, const char* value)
{
	sendFormatAT(TO_F(AT_COMMAND_SET_BEARER_SETTING_PARAMETER), parameter, value);
	return waitResponse() == 0;
}

bool SIM808::getGprsPowerState(bool *state)
{
	uint8_t result;

	sendAT(TO_F(TOKEN_CGATT), TO_F(TOKEN_READ));

	if(waitResponse(10000L, TO_F(TOKEN_CGATT)) != 0 ||
		!parseReply(',', 0, &result) ||
		waitResponse())
		return false;

	*state = result;
	return true;
}

bool SIM808::enableGprs(const char *apn, const char* user, const char *password)
{
	char gprsToken[5];
	strcpy_P(gprsToken, TOKEN_GPRS);

	return 
		(sendAT(TO_F(TOKEN_CIPSHUT)), waitResponse(65000L, TO_F(TOKEN_SHUT_OK)) == 0) &&					//AT+CIPSHUT
		(sendFormatAT(TO_F(AT_COMMAND_GPRS_ATTACH), 1), waitResponse(10000L) == 0) &&						//AT+CGATT=1

		(setBearerSetting(TO_F(AT_COMMAND_PARAMETER_BEARER_CONTYPE), gprsToken)) &&							//AT+SAPBR=3,1,"CONTYPE","GPRS"

		(setBearerSetting(TO_F(AT_COMMAND_PARAMETER_BEARER_APN), apn)) &&									//AT+SAPBR=3,1,"APN","xxx"
		(user == NULL || setBearerSetting(TO_F(AT_COMMAND_PARAMETER_BEARER_USER), user)) &&					//AT+SAPBR=3,1,"USER","xxx"
		(password == NULL || setBearerSetting(TO_F(AT_COMMAND_PARAMETER_BEARER_PWD), password)) &&			//AT+SAPBR=3,1,"PWD","xxx"

		(sendFormatAT(TO_F(AT_COMMAND_SET_BEARER_SETTING), 1, 1), waitResponse(65000L) == 0);				//AT+SAPBR=1,1
}

bool SIM808::disableGprs()
{
	return 
		(sendFormatAT(TO_F(AT_COMMAND_SET_BEARER_SETTING), 0, 1), waitResponse(65000L) != -1) &&			//AT+SAPBR=0,1
		(sendAT(TO_F(TOKEN_CIPSHUT)), waitResponse(65000L, TO_F(TOKEN_SHUT_OK)) == 0) &&					//AT+CIPSHUT
		(sendFormatAT(TO_F(AT_COMMAND_GPRS_ATTACH), 0), waitResponse(10000L) == 0);							//AT+CGATT=0
}

SIM808NetworkRegistrationState SIM808::getNetworkRegistrationStatus()
{
	uint8_t stat;
	sendAT(TO_F(TOKEN_CGREG), TO_F(TOKEN_READ));
	
	if(waitResponse(TO_F(TOKEN_CGREG)) != 0 ||
		!parseReply(',', (uint8_t)SIM808RegistrationStatusResponse::Stat, &stat) ||
		waitResponse() != 0)
		return SIM808NetworkRegistrationState::Error;

	return (SIM808NetworkRegistrationState)stat;
}

String SIM808::enableTcp(const char* ip,const char* port, const char *apn){
	bool res =
	    //(sendFormatAT(TO_F(AT_COMMAND_CIPMUX), 1), waitResponse(10000L) == 0) &&
		(sendFormatAT(TO_F(AT_COMMAND_TCP_CSTT), apn), waitResponse(10000L) == 0) &&
		(sendAT(TO_F(TOKEN_CIICR)), waitResponse(10000L) == 0);	
	if(!res)return "";
	sendAT(TO_F(TOKEN_CIFSR));	
	char message[160];
	sendCommand(TO_F(TOKEN_CIFSR), message, 160);
	String msg=message;
	String myIp=msg.substring(0,15);
	sendFormatAT(TO_F(AT_COMMAND_TCP_CIPSTART), ip,port), waitResponse(10000L, TO_F(TOKEN_CONNECT_OK)) == 0;

	String cnt=",MQTT< 972c108d514b4e05a05b8aa7f79493c9";

	SendTcpMessage(cnt.c_str());
	return myIp;
}

bool SIM808::SendTcpMessage(const char* message){
	sendAT(TO_F(TOKEN_CIPSEND));
	Serial.println(message);
	if (!waitResponse(S_F(">")) == 0) return false;
	SENDARROW;
	print(message);
	print((char)0x1A);
  return true;
}