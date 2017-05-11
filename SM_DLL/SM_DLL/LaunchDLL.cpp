#include <iostream>
#include <windows.h>

using namespace std;

typedef int(*MsgFunction)(int);

HINSTANCE hinstDLL;

int main() {

	LPCWSTR file = L"ficheiro.dll";

	MsgFunction MsgBox(0);
	hinstDLL = LoadLibrary(file);
	if (hinstDLL != 0)
		MsgBox = (MsgFunction)GetProcAddress(hinstDLL, "MsgBox");

	if (MsgBox == 0) cout << "MsgBox is NULL" << endl;

	int x = MsgBox(5);
	if (x == 5)
		cout << "Message displayed!" << endl;

	FreeLibrary(hinstDLL);

	return 0;

}