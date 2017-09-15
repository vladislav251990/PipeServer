#pragma once
#include <string>
#include "Pipes/PipeManager.h"
#include <stack>
#include <map>


class Connection
{

public:
	CPipeManager manager;
	std::string Quotations;
	bool busy;
	int terminalID;
	Connection();
	~Connection();
	void setTerminalName(const wchar_t* terminalName);
	void setTerminalName(const char * terminalName);
	bool isServer();
	std::stack<std::string> queries;//запросы к серверу-ресиверу от серверов-личей
	std::map<std::string, std::string> responses;
private:
	bool serverTerminal;
};

