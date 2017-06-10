#include "MainWnd.h"

bool WWindow::started = false;
tstring WWindow::AppName;
HINSTANCE WWindow::hInstance = NULL;
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
	if (_hwnd == NULL) {
		MessageBox(NULL, TEXT("na criacao da janela"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		exit(1);
	}
	//Armazenar o ponteiro this na zona cbClsExtra da estrutura WNDCLASSEX
	SetWindowLongPtr(_hwnd, 0, (long) this);
}
//---------------------------------------------------------------------------
bool WWindow::Register() {
	WNDCLASSEX _WndClsEx;
	//Definição das características da janela "_WndClsEx"
	_WndClsEx.cbSize = sizeof(WNDCLASSEX);
	_WndClsEx.style = CS_HREDRAW | CS_VREDRAW;
	_WndClsEx.lpfnWndProc = WndProc;	//Função Membro
	_WndClsEx.cbClsExtra = 0;
	_WndClsEx.cbWndExtra = sizeof(WWindow *);
	_WndClsEx.hInstance = GetModuleHandle(AppName.c_str());
	_WndClsEx.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON));
	_WndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	_WndClsEx.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	_WndClsEx.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	_WndClsEx.lpszClassName = AppName.c_str();
	_WndClsEx.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON));
	if (!RegisterClassEx(&_WndClsEx)) {
		MessageBox(NULL, TEXT("no registo da classe"), TEXT("Erro"), MB_OK | MB_ICONERROR);
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
bool WWindow::doConfirmation(void) {
	if (MessageBox(_hwnd, TEXT("Tem certeza?"), TEXT("Sair"), MB_ICONQUESTION | MB_YESNO) == IDYES)
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
void WWindow::StartUp(void) {
	hInstance = GetModuleHandle(AppName.c_str());
	if (Register())
		started = true;

}
//---------------------------------------------------------------------------
LRESULT WWindow::WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	//Saber sobre que janela estamos a trabalhar
	HDC hdc;
	PAINTSTRUCT PtStc;
	WWindow *pWin = (WWindow *)GetWindowLongPtr(hWnd, 0);
	BOOL eRato = FALSE;
	int direction;
	switch (messg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		//Menu		
		case ID_NOVOJOGO_CRIARJOGO:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_CREATE_GAME), hWnd, (DLGPROC)TreatDialogCreateGame);
			break;
		case ID_JOGO_SAIR:
			PostQuitMessage(0);
			break;
		case ID_NOVOJOGO_PROCURARJOGO:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_JOIN), hWnd, (DLGPROC)TreatDialogJoinGame);
			break;
		case ID_SOBRE:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)TreatDialogJoinGame);
			break;

		//Keyboard
		case WM_KEYDOWN:
			if (wParam == VK_LEFT)
				direction = GOING_LEFT;
			if (wParam == VK_RIGHT)
				direction = GOING_RIGHT;
			if (wParam == VK_UP)
				direction = GOING_UP;
			if (wParam == VK_DOWN)
				direction = GOING_DOWN;
			
			//enviar a direção
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &PtStc);
			break;
		default:
			return FALSE;
		}
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;
	}

	return(0);
}

LRESULT CALLBACK WWindow::TreatDialogCreateGame(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	int numPlayers;
	int numObjects;
	int width;
	int height;
	int initialSnakeTam;
	int numSnakesAI;
	TCHAR playerName[256];

	switch (messg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_DIALOG_CREATE_BUTTON:
			GetWindowText(GetDlgItem(hWnd, IDC_EDIT_PLAYER_NAME), playerName, 256);
			numPlayers = GetDlgItemInt(hWnd, IDC_EDIT_NUM_PLAYERS , NULL, FALSE);
			if (numPlayers < 0 || numPlayers > 10) {
				MessageBox(hWnd, TEXT("Numero de Jogadores errado [1,10]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			numObjects = GetDlgItemInt(hWnd, IDC_EDIT_NUM_OBJECTS, NULL, FALSE);
			if (numObjects <= 0) {
				MessageBox(hWnd, TEXT("Numero de Objetos errado [1,10]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			width = GetDlgItemInt(hWnd, IDC_EDIT_MAP_WIDTH, NULL, FALSE);
			if (width < 0 || width > 80) {
				MessageBox(hWnd, TEXT("Comprimento errado [1,80]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			height = GetDlgItemInt(hWnd, IDC_EDIT_MAP_HEIGHT, NULL, FALSE);
			if (height < 0 || height > 80) {
				MessageBox(hWnd, TEXT("Largura errada [1,80]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			initialSnakeTam = GetDlgItemInt(hWnd, IDC_EDIT_SNAKE_SIZE, NULL, FALSE);
			if (initialSnakeTam < 0 || initialSnakeTam > 10) {
				MessageBox(hWnd, TEXT("Tamanho inicial das cobras errado [1,10]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			numSnakesAI = GetDlgItemInt(hWnd, IDC_EDIT_NUM_SNAKES_AI, NULL, FALSE);
			if (numSnakesAI < 0 || numSnakesAI > 10) {
				MessageBox(hWnd, TEXT("Numero de Cobras AI errado [1,10]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}

			//Enviar o comando aqui

			return TRUE;
		case ID_DIALOG_CREATE_CANCEL:
			EndDialog(hWnd, TRUE);
			return TRUE;
		default:
			return FALSE;
		}
		return TRUE;
	default:
		return FALSE;
	}
}

LRESULT CALLBACK WWindow::TreatDialogJoinGame(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	TCHAR playerName[256];

	switch (messg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_DLG_JOIN_OK:
			GetWindowText(GetDlgItem(hWnd, ID_DLG_JOIN_PLAYER_NAME), playerName, 256);
			//Enviar o comando aqui

			return TRUE;
		case ID_DLG_JOIN_CANCEL:
			EndDialog(hWnd, TRUE);
			return TRUE;
		default:
			return FALSE;
		}
		return TRUE;
	default:
		return FALSE;
	}
}
