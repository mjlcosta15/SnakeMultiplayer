#include <windows.h>
#include <tchar.h>

#define SERVICE_NAME TEXT("SMServerNT")

int main(int argc, TCHAR *argv[])
{
	SC_HANDLE schSCManager, schService;
	schSCManager = OpenSCManager(
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (schSCManager == NULL)
		_tprintf(TEXT("Erro: OpenSCManager\n"));

	schService = OpenService(
		schSCManager,       // SCManager database 
		SERVICE_NAME,       // name of service 
		DELETE);            // only need DELETE access 

	if (schService == NULL)
		_tprintf(TEXT("Error: OpenService\n"));

	if (!DeleteService(schService))
		_tprintf(TEXT("Error: DeleteService\n"));
	else
		_tprintf(TEXT("DeleteService SUCCESS\n"));

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return 0;
}

