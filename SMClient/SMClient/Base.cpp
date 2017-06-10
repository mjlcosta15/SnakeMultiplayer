/* ===================================================== */
/* BASE.cpp                                               */
/* Programa base (esqueleto) para aplica��es Windows     */
/* ===================================================== */
// Cria uma janela de nome "Janela Principal" e pinta fundo
// de branco

// Modelo para programas Windows:
//  Composto por 2 fun��es: 
//	   WinMain() = Ponto de entrada dos programas windows
//			1) Define, cria e mostra a janela
//			2) Loop de recep��o de mensagens provenientes do Windows
//     WinProc() = Processamentos da janela (pode ter outro nome)
//			1) � chamada pelo Windows (callback) 
//			2) Executa c�digo em fun��o da mensagem recebida

//	   WinMain()
//	1. Definir caracter�sticas de uma classe de janela
//  2. Registar a classe no Windows NT
//  3. Criar uma janela dessa classe
//  4. Mostrar a janela
//  5. Iniciar a execu��o do loop de mensagens
//    
//     WinProc()
//  1. Switch em fun��o da mensagem recebida do Windows

// ============================================================================
// In�cio do programa
// ============================================================================
// Este header tem de se incluir sempre porque define os prot�tipos das fun��es 
// do Windows API e os tipos usados na programa��o Windows
#include <windows.h>
#include <tchar.h>
#include "MainWnd.h"

// Pr�-declara��o da fun��o WndProc (a que executa os procedimentos da janela por
// "callback") 
LRESULT CALLBACK TreatEvents(HWND, UINT, WPARAM, LPARAM);

// Nome da classe da janela (para programas de uma s� janela, normalmente este 
// nome � igual ao do pr�prio programa)
// "szprogName" � usado mais abaixo na defini��o das propriedades 
// da classe da janela
TCHAR *szProgName = TEXT("Snake Multiplayer");


// ============================================================================
// FUN��O DE IN�CIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa come�a sempre a sua execu��o na fun��o WinMain()
// que desempenha o papel da fun��o main() do C em modo consola
// WINAPI indica o "tipo da fun��o" (WINAPI para todas as declaradas nos headers
// do Windows e CALLBACK para as fun��es de processamento da janela)
// Par�metros:
//   hInst: Gerado pelo Windows, � o handle (n�mero) da inst�ncia deste programa 
//   hPrevInst: Gerado pelo Windows, � sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Gerado pelo Windows, � um ponteiro para uma string terminada por 0
//              destinada a conter par�metros para o programa 
//   nCmdShow:  Par�metro que especifica o modo de exibi��o da janela (usado em  
//				ShowWindow()

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, 
				   LPSTR lpCmdLine, int nCmdShow) {
	MSG lpMsg;			// MSG � uma estrutura definida no Windows para as mensagens

// ============================================================================
// 1. Criar a janela com a defini��o das caracter�sticas e registo da estrutura
//	WNDCLASSEX no Windows
// ============================================================================
	WWindow Wnd(szProgName, TEXT("Snake Multiplayer"), (HWND)HWND_DESKTOP);
// ============================================================================
// 4. Mostrar a janela
// ============================================================================
	Wnd.Show();
// ============================================================================
// 5. Loop de Mensagens
// ============================================================================
// O Windows envia mensagens �s janelas (programas). Estas mensagens ficam numa
// fila de espera at� que GetMessage(...) possa ler "a mensagem seguinte"	

// Par�metros de "getMessage":
//  1)	"&lpMsg"=Endere�o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi 
//		declarada no in�cio de WinMain()):
/*			HWND hwnd		handler da janela a que se destina a mensagem
			UINT message	Identificador da mensagem
			WPARAM wParam	Par�metro, p.e. c�digo da tecla premida
			LPARAM lParam	Par�metro, p.e. se ALT tamb�m estava premida
			DWORD time		Hora a que a mensagem foi enviada pelo Windows
			POINT pt		Localiza��o do mouse (x, y) 
	2)   handle da window para a qual se pretendem receber mensagens
		 (=NULL se se pretendem receber as mensagens para todas as janelas 
		 pertencentes ao thread actual)
	3)	 C�digo limite inferior das mensganes que se pretendem receber
	4)   C�digo limite superior das mensagens que se pretendem receber
*/

// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
// 	     terminando ent�o o loop de recep��o de mensagens, e o programa 

	while (GetMessage(&lpMsg,NULL,0,0)) {	
		TranslateMessage(&lpMsg);			// Pr�-processamento da mensagem
											// p.e. obter c�digo ASCII da tecla
											// premida
		DispatchMessage(&lpMsg);			// Enviar a mensagem traduzida de volta
											// ao Windows, que aguarda at� que a 
											// possa reenviar � fun��o de tratamento
											// da janela, CALLBACK WndProc (mais 
											// abaixo)
	}
	/*
	//� mais SEGURO o seguinte ciclo de recep��o de mensagens, para saber se houve um erro
	BOOL bRet;
	while( (bRet = GetMessage( &lpMsg, NULL, 0, 0 )) != 0)
	{ 
    if (bRet == -1)
    {
        // handle the error and possibly exit
    }
    else
    {
        TranslateMessage(&lpMsg); 
        DispatchMessage(&lpMsg); 
    }
	}*/
// ============================================================================
// 6. Fim do programa
// ============================================================================
	return((int)lpMsg.wParam);		// Retorna-se sempre o par�metro "wParam" da
								// estrutura "lpMsg"

}


