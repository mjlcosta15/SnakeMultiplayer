#include "MainWnd.h"
#include "Client.h"

bool WWindow::started = false;
tstring WWindow::AppName;
HINSTANCE WWindow::hInstance = NULL;

Client client;

//---------------------------------------------------------------------------
WWindow::WWindow(LPCTSTR clsname, LPCTSTR wndname,
	HWND parent,
	DWORD dStyle,
	DWORD dXStyle,
	int x,
	int y,
	int width,
	int height)
{	
	AppName = clsname;
	//Registar a classe se ainda não foi registada antes
	if (!started)
		StartUp();

	// Criar uma nova janela
	_hwnd = CreateWindowEx(dXStyle, clsname, wndname, dStyle, x, y, width,
		height, parent, NULL, hInstance, NULL);

	// Se a janela não foi criada terminar o programa!
	if (_hwnd == NULL){
		MessageBox(NULL, TEXT("na criacao da janela"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		exit(1);
	}	
	//Armazenar o ponteiro this na zona cbClsExtra da estrutura WNDCLASSEX
	SetWindowLongPtr(_hwnd, 0, (long) this);
}
//---------------------------------------------------------------------------
bool WWindow::Register(){
	WNDCLASSEX _WndClsEx;
	//Definição das características da janela "_WndClsEx"
	_WndClsEx.cbSize = sizeof(WNDCLASSEX);
	_WndClsEx.style = CS_HREDRAW|CS_VREDRAW;
	_WndClsEx.lpfnWndProc = WndProc;	//Função Membro
	_WndClsEx.cbClsExtra = 0;
	_WndClsEx.cbWndExtra = sizeof(WWindow *);
	_WndClsEx.hInstance = GetModuleHandle(AppName.c_str());
	_WndClsEx.hIcon = LoadIcon(NULL, IDI_SHIELD);
	_WndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	_WndClsEx.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	_WndClsEx.lpszMenuName = NULL;
	_WndClsEx.lpszClassName = AppName.c_str();
	_WndClsEx.hIconSm = LoadIcon(NULL, IDI_SHIELD);
	if (!RegisterClassEx(&_WndClsEx)){
		MessageBox(NULL,TEXT("no registo da classe"),TEXT("Erro"), MB_OK|MB_ICONERROR);
		return false;
	}
	return true;
}


//---------------------------------------------------------------------------
BOOL WWindow::Show(int dCmdShow)
{
	// ============================================================================
	// Mostrar a janela
	// ============================================================================
	if (!ShowWindow(_hwnd, dCmdShow))	// "hWnd"= handler da janela, devolvido 
		return FALSE;					// por "CreateWindow"; "dCmdShow"= modo de
										// exibição (p.e. normal, modal); é passado
										// como parâmetro de WinMain()

	if (!UpdateWindow(_hwnd))			// Refrescar a janela (Windows envia à janela
		return FALSE;					// uma mensagem para pintar, mostrar dados,
										// (refrescar), etc)
	return TRUE;
}
//---------------------------------------------------------------------------
//Confirmar fecho da janela
bool WWindow::doConfirmation(void){
	if (MessageBox(_hwnd,  TEXT("Tem certeza?"), TEXT("Sair"),MB_ICONQUESTION|MB_YESNO) == IDYES)
		return true;
	return false;
}
//---------------------------------------------------------------------------
WWindow::operator HWND()
{
	//Uma vez que cada janela é do tipo HWND, utilizaremos 
	//um modo de reconhecer o handle da janela quando utilizado 
	//na aplicação
	return _hwnd;
}
//---------------------------------------------------------------------------
void WWindow::StartUp(void){
	hInstance = GetModuleHandle(AppName.c_str());

	client.start();

	if (Register())
		started = true;
	
}
//---------------------------------------------------------------------------
LRESULT WWindow::WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam){
	//Saber sobre que janela estamos a trabalhar
	WWindow *pWin = (WWindow *) GetWindowLongPtr(hWnd, 0);
	TCHAR str[100];
	BOOL eRato = FALSE;
	switch (messg) {
	case WM_CLOSE:	// Destruir a janela e terminar o programa
		PostQuitMessage(0);
//		if (pWin->doConfirmation())
//			PostQuitMessage(0);
		break;
	default:

		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar", "maximizar",
		// "restaurar") não é efectuado nenhum processamento, apenas se segue 
		// o "default" do Windows DefWindowProc()
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;
	}
	return(0);
}


