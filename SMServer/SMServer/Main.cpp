#include "Server.h"
using namespace std;


int _tmain()
{
	Server server;
	server.startServer();

}



#define SERVICE_NAME TEXT("SMServerNT")
SERVICE_STATUS MyServiceStatus;
SERVICE_STATUS_HANDLE MyServiceStatusHandle;
Server server;
int status;

VOID WINAPI MyServiceStart(DWORD argc, TCHAR **argv);
VOID WINAPI MyServiceCtrlHandler(DWORD opcode);

void _tmain()
{
	SERVICE_TABLE_ENTRY   DispatchTable[] =
	{
		{ SERVICE_NAME, MyServiceStart },
		{ NULL,              NULL }
	};

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		//MostrarErro(TEXT(" [ServNT] StartServiceCtrlDispatcher error = %d\n"), GetLastError());
	}
}

void WINAPI MyServiceStart(DWORD argc, TCHAR **argv)
{

	MyServiceStatus.dwServiceType = SERVICE_WIN32;
	MyServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	MyServiceStatus.dwControlsAccepted = 0;
	MyServiceStatus.dwWin32ExitCode = 0;
	MyServiceStatus.dwServiceSpecificExitCode = 0;
	MyServiceStatus.dwCheckPoint = 0;
	MyServiceStatus.dwWaitHint = 0;

	MyServiceStatusHandle = RegisterServiceCtrlHandler(
		SERVICE_NAME,
		MyServiceCtrlHandler);

	if (MyServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
	{
		//MostrarErro(TEXT(" [ServNT] RegisterServiceCtrlHandler failed %d\n"), GetLastError());
		return;
	}

	if (!SetServiceStatus(MyServiceStatusHandle, &MyServiceStatus))
	{
		status = GetLastError();
		//MostrarErro(TEXT(" [ServNT] SetServiceStatus error %ld\n"), status);
		return;
	}

	// Initialization complete - report running status. 
	MyServiceStatus.dwCurrentState = SERVICE_RUNNING;
	MyServiceStatus.dwCheckPoint = 0;
	MyServiceStatus.dwWaitHint = 0;
	MyServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP; // | SERVICE_ACCEPT_PAUSE_CONTINUE; 

	if (!SetServiceStatus(MyServiceStatusHandle, &MyServiceStatus))
	{
		status = GetLastError();
		//MostrarErro(TEXT(" [ServNT] SetServiceStatus error %ld\n"), status);
		return;
	}

	
	//Start the server
	server.startServer();
	

	MyServiceStatus.dwCurrentState = SERVICE_STOPPED;
	MyServiceStatus.dwCheckPoint = 0;
	MyServiceStatus.dwWaitHint = 0;

	if (!SetServiceStatus(MyServiceStatusHandle, &MyServiceStatus))
	{
		status = GetLastError();
		//MostrarErro(TEXT(" [ServNT] SetServiceStatus error %ld\n"), status);
	}
	return;
}

VOID WINAPI MyServiceCtrlHandler(DWORD Opcode)
{
	switch (Opcode)
	{
	case SERVICE_CONTROL_PAUSE:
		// Do whatever it takes to pause here. 
		MyServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;

	case SERVICE_CONTROL_CONTINUE:
		// Do whatever it takes to continue here. 
		MyServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;

	case SERVICE_CONTROL_STOP:
		// Do whatever it takes to stop here. 
		MyServiceStatus.dwWin32ExitCode = 0;
		MyServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		MyServiceStatus.dwCheckPoint = 0;
		MyServiceStatus.dwWaitHint = 0;

		if (!SetServiceStatus(MyServiceStatusHandle,
			&MyServiceStatus))
		{
			status = GetLastError();
			//MostrarErro(TEXT(" [ServNT] SetServiceStatus error %ld\n"), status);
		}
		//flag = FALSE;

		//SetEvent(Event);

		//MostrarErro(TEXT(" [ServNT] Leaving MyService \n"), 0);
		return;

	case SERVICE_CONTROL_INTERROGATE:
		// Fall through to send current status. 
		break;

	default:
		//MostrarErro(TEXT(" [ServNT] Unrecognized opcode %ld\n"),
		//	Opcode);
		break;
	}

	// Send current status. 
	if (!SetServiceStatus(MyServiceStatusHandle, &MyServiceStatus))
	{
		status = GetLastError();
		//MostrarErro(TEXT(" [ServNT] SetServiceStatus error %ld\n"), status);
	}
	return;
}

/*
NAO DESCOMENTAR
void MostrarErro(TCHAR *str, DWORD val)
{
	TCHAR buf[TAM * 4];
	TCHAR *msg[2] = { buf,NULL };

	_stprintf_s(buf, TAM * 4, str, val);

	if (!hEventSource) {
		hEventSource = RegisterEventSource(NULL,            // local machine
			NOME_DO_SERVICO); // source name
	}

	if (hEventSource) {
		ReportEvent(hEventSource,
			EVENTLOG_INFORMATION_TYPE,
			0,
			0,
			NULL,   // sid
			1,
			0,
			(const TCHAR**)msg,
			NULL);
	}

}
*/