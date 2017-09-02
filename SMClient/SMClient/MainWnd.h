#pragma once
#include <string>
#include <windows.h>
#include "MainHeader.h"
#include "resource.h"

using namespace std;

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#define GOING_UP 1
#define GOING_DOWN 2
#define GOING_LEFT 3
#define GOING_RIGHT 4 

//---------------------------------------------------------------------------
class WWindow
{
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK TreatDialogCreateGame(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK TreatDialogJoinGame(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK TreatDialogConnectToServer(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK TreatDialogEditSkins(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
	static tstring AppName;
	static bool started;
	static HINSTANCE hInstance;  

public:
	// Utilizaremos o construtor para criar e inicializar
	WWindow(LPCTSTR clsname,
		LPCTSTR wndname,
		HWND parent = NULL,
		DWORD dStyle = WS_OVERLAPPEDWINDOW,
		DWORD dXStyle = 0L,
		int x = 0,
		int y = 0,
		//int width = (int)GetSystemMetrics(SM_CXSCREEN),
		//int height = (int)GetSystemMetrics(SM_CYSCREEN));

		int width = 720,
		int height = 480);

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

	HWND hWnd;
};
//---------------------------------------------------------------------------