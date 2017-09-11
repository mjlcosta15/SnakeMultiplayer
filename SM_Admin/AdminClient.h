#ifndef ADMINCLIENT_H
#define ADMINCLIENT_H

#include "MainHeader.h"
#define msg_sz sizeof(Message)

class AdminClient
{
	HANDLE hPipe;
	DWORD dwMode;
	BOOL fSuccess = false;
	DWORD cbWritten;
	DWORD cbBytesRead = 0;
	LPTSTR pipeNameAdmin = TEXT("\\\\.\\pipe\\pipeAdmin");

	HANDLE ReadReady;
	OVERLAPPED OverlRd = { 0 };
	HANDLE WriteReady; // Handle para o evento da leitura (cada thread tem um)
	OVERLAPPED OverlWr = { 0 };

	Message msg;
public:
	AdminClient();
	~AdminClient();

	void connectToServer();
	void startMessaging();
	void finishAdminClient();
	vector<string> getCommand();
	bool commandParser(vector<string> command);
	void treatCommand(vector<string> command);
	string commandToUpperCase(string command);
};

#endif ADMINCLIENT_H