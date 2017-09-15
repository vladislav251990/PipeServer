#include "stdafx.h"
#include "global.h"
#include "Pipes\PipeManager.h"
#include <thread>
#include "Connection.h"
#include <locale>
#include <codecvt>
#include <vector>
#include <iterator>
#include <sstream>
using namespace std;

//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
Connection serverConnection;
int serverTerminalNumber = 0;

/*template<typename T,typename P> 
T convertStrings(P convertedString) {
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	if(typeid(T)==typeid(string)&&typeid(P)==typeid(wstring)) return converter.to_bytes(convertedString.c_str());
	if (typeid(T) == typeid(wstring) && typeid(P) == typeid(string)) return converter.from_bytes(convertedString.c_str());
	T empty;
	return empty;
}*/
template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}
wstring StrToWstr(string str)
{
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
	
}

int startIndividualPipeServerLeecher(char str[])
{
	string str2(str);
	delete[] str;
	wstring wc = StrToWstr(str2);
	wstring message = wstring(L"MQL5 Pipe Server") + wc + wstring(L"\n");
	wprintf(message.c_str());
	Connection connection;
	wstring createServerString = wstring(L"\\\\.\\pipe\\MQL5.Pipe.Server") + wc;
	if (!connection.manager.Create(createServerString.c_str()))
		return(-1);
	//--- wait for client
	char          answer[1024];

	wprintf(L"Client: waiting for connection...\n");
	while (true) {
		while (!connection.manager.IsConnected())
		{
			if (!connection.manager.ReadString(answer, _countof(answer) - 1))
			{
				Sleep(250);
				return 0;
			}
			if (atoi(str2.c_str()) != atoi(answer)) {
				wprintf(L"Undefined account number!");
				return 0;
			}
			wprintf(L"Success!");
		}
		string sendedStr;
		if (serverConnection.responses.count(str2) == 1 && serverConnection.responses[str2] != "") {
			sendedStr = "response>"+serverConnection.responses[str2];
			serverConnection.responses.erase(str2);
		}
		else sendedStr = "quotation>"+serverConnection.Quotations;
		if (!connection.manager.SendString(sendedStr.c_str()))
		{
			Sleep(250);
			return 0;
		}
		if (!connection.manager.ReadString(answer, _countof(answer) - 1))
		{
			Sleep(250);
			return 0;
		}
		if (answer == "") {
			connection.manager.DisconnectClient();
			continue;
		}
		if (string(answer) == string("ok")) continue;
		serverConnection.queries.push(string(answer)+":"+str2);
		printf("Query got");
		Sleep(250);

	}

};

int startIndividualPipeServerReceiver(char str[])
{
	string str2(str);
	delete[] str;
	wstring wc = StrToWstr(str2);
	wstring message = wstring(L"MQL5 Pipe Server") + wc + wstring(L"\n");
	wprintf(message.c_str());
	Connection connection;
	wstring createServerString = wstring(L"\\\\.\\pipe\\MQL5.Pipe.Server") + wc;
	if (!connection.manager.Create(createServerString.c_str()))
		return(-1);
	//--- wait for client
	char          answer[1024];
	int pointsCount = 0;
	string pointsArray[] = {"",".","..","..."};

	wprintf(L"Client: waiting for connection...\n");
	while (true) {
		while (!connection.manager.IsConnected())
		{
			if (!connection.manager.ReadString(answer, _countof(answer) - 1))
			{
				Sleep(250);
				return 0;
			}
			if (atoi(str2.c_str()) != atoi(answer)) {
				wprintf(L"Undefined account number!");
				return 0;
			}
			wprintf(L"Success!");
		}
		if (!connection.manager.ReadString(answer, _countof(answer) - 1))
		{
			Sleep(250);
		}
		if (answer == "") {
			serverConnection.manager.DisconnectClient();
			continue;
		}
		serverConnection.Quotations = answer;
		//system("cls");
		for (int i = 0;i < 50;++i) putchar('\b');
		for (int i = 0;i < 50;++i) putchar(' ');
		putchar('\r');
		printf(("Quotation got"+pointsArray[pointsCount]).c_str());
		pointsCount++;
		if (pointsCount == 4) pointsCount = 0;
		if (serverConnection.queries.empty())
		{
			if (!connection.manager.SendString("ok"))
			{
				Sleep(250);
				return 0;
			}
		}
		else
		{
			string query =serverConnection.queries.top();
			serverConnection.queries.pop();
			if (!connection.manager.SendString(query.c_str()))
			{
				Sleep(250);
				return 0;
			}
			string response = "";
			do {
				if (!connection.manager.ReadString(answer, _countof(answer) - 1))
				{
					Sleep(250);
					return 0;
				}
				response += string(answer);
			} while (answer[strlen(answer) - 1] != '!');
			vector<string> responseWithId = split(response, ';');
			while(serverConnection.responses.count(responseWithId[0])!=0)  this_thread::sleep_for(std::chrono::milliseconds(250));//ќжидаем пока личер не заберет нужную информацию
			serverConnection.responses[responseWithId[0]] = responseWithId[1];
		}
		
	}
	 
};

int PipeServer(void)
  {
	wprintf(L"MQL5 Pipe Server\n");
	wprintf(L"Copyright 2012, MetaQuotes Software Corp.\n");
	//--- create named pipe
	//Connection Connection;
	Connection distributorServer;
	if (!distributorServer.manager.Create(L"\\\\.\\pipe\\MQL5.Pipe.Server"))
		return(-1);
	while (true) {
		bool justConnected = false;
		//--- wait for client
		char          answer[256];
		int           value = 1234567890;

		wprintf(L"Client: waiting for connection...\n");
		while (!distributorServer.manager.IsConnected())
		{
			if (!distributorServer.manager.ReadString(answer, _countof(answer) - 1))
			{
				Sleep(250);
				continue;
			}
			wprintf(L"Client: connected as '%S'\n", answer);
			justConnected = true;
		}
		if (justConnected) distributorServer.setTerminalName(answer);
		value = atoi(answer);
		while (!distributorServer.manager.Send(&value, sizeof(value)))
		{
			Sleep(250);

		}
		distributorServer.manager.DisconnectClient();
		int answerLen = strlen(answer);
		char *newValue = new char[answerLen + 1];
		newValue[answerLen] = '\0';
		for (int i = 0;i < answerLen;++i) newValue[i] = answer[i];
		if (distributorServer.isServer()) {

			thread Thread(startIndividualPipeServerReceiver, newValue);
			Thread.detach();
		}
		else
		{
			thread Thread(startIndividualPipeServerLeecher, newValue);
			Thread.detach();
		}

		continue;
	}
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int _tmain(int argc, _TCHAR* argv[])
  {
//--- create named pipe
	printf("Please enter account number for server terminal:\n");
	scanf("%i",&serverTerminalNumber);
	std::ostringstream oss;
	oss << serverTerminalNumber;
	printf(oss.str().c_str());
    PipeServer();

//---
   return(0);
  }
//+------------------------------------------------------------------+
