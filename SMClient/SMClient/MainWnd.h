#pragma once
#include <string>
#include <windows.h>

using namespace std;

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string
#endif

//---------------------------------------------------------------------------
class WWindow
{
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
	static tstring AppName;
	static bool started;
	static HINSTANCE hInstance;
public:
	// Utilizaremos o construtor para criar e inicializar
	WWindow(LPCTSTR clsname,
		LPCTSTR wndname,
		HWND parent = NULL,
		DWORD dStyle = WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
		DWORD dXStyle = 0L,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT);

	// Registo da estrutura (WNDCLASSEX) da janela
	bool Register();

	// Método para mostrar a janela
	BOOL Show(int dCmdShow = SW_SHOWNORMAL);

	//Uma vez que cada janela é do tipo HWND, utilizaremos 
	//um modo de reconhecer o handle da janela quando utilizado 
	//na aplicação
	operator HWND();

	void StartUp(void);
	
protected:
	//Este será o handle global disponível para esta e para outras janelas
	HWND _hwnd;
	//Confirmar fecho da janela
	bool doConfirmation(void);
};
//---------------------------------------------------------------------------