#include "CreateDLL.h"

int DLL_EXPORT MsgBox(int x = 0)
{
	LPCWSTR a = L"Nice";
	LPCWSTR b = L"DLL Message";


	MessageBox(0, a, b, MB_OK | MB_ICONINFORMATION);
	

	return x;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	default:
		break;
	}

}
