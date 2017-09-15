#include "stdafx.h"
#include "Connection.h"
#include "global.h"
//#define serverTerminalNumber 11858728
Connection::Connection()
{
	busy = false;
	serverTerminal = false;
//	this->queries.push("EURUSD:HIGH:H1:0:50:11858728");// тестовый запрос
}

Connection::~Connection()
{
}

void Connection::setTerminalName(const wchar_t * terminalName)
{
	this->terminalID = 0;
	for (int i = 0;i< wcslen(terminalName);++i) this->terminalID += terminalName[i];
}
void Connection::setTerminalName(const char* terminalName)
{
	this->terminalID = atoi(terminalName);
	if (this->terminalID == serverTerminalNumber) this->serverTerminal = true;
	else this->serverTerminal = false;
}

bool Connection::isServer()
{
	return this->serverTerminal;
}
