#include <windows.h>
#include <tchar.h>

#define SERVICE_NAME TEXT("SMServerNT")
#define CAMINHO TEXT("C:\\SM_Server.exe")

int main(int argc, TCHAR *argv[])
{
	SC_HANDLE schSCManager, schService;
	schSCManager = OpenSCManager(
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (schSCManager == NULL)
		_tprintf(TEXT("Erro: OpenSCManager\n"));


	TCHAR *lpszBinaryPathName = CAMINHO;

	schService = CreateService(
		schSCManager,              // SCManager database 
		SERVICE_NAME,               // name of service 
		TEXT("Snake Multiplayer Server"),           // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_WIN32_OWN_PROCESS, // service type 
		SERVICE_DEMAND_START,      // start type 
		SERVICE_ERROR_NORMAL,      // error control type 
		lpszBinaryPathName,        // service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 

	if (schService == NULL)
		_tprintf(TEXT("Error: CreateService\n"));
	else
		_tprintf(TEXT("CreateService SUCCESS.\n"));

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return 0;
}

