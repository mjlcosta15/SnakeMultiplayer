/* ===================================================== */
/* BASE.cpp                                               */
/* Programa base (esqueleto) para aplicações Windows     */
/* ===================================================== */
// Cria uma janela de nome "Janela Principal" e pinta fundo
// de branco

// Modelo para programas Windows:
//  Composto por 2 funções: 
//	   WinMain() = Ponto de entrada dos programas windows
//			1) Define, cria e mostra a janela
//			2) Loop de recepção de mensagens provenientes do Windows
//     WinProc() = Processamentos da janela (pode ter outro nome)
//			1) É chamada pelo Windows (callback) 
//			2) Executa código em função da mensagem recebida

//	   WinMain()
//	1. Definir características de uma classe de janela
//  2. Registar a classe no Windows NT
//  3. Criar uma janela dessa classe
//  4. Mostrar a janela
//  5. Iniciar a execução do loop de mensagens
//    
//     WinProc()
//  1. Switch em função da mensagem recebida do Windows

// ============================================================================
// Início do programa
// ============================================================================
// Este header tem de se incluir sempre porque define os protótipos das funções 
// do Windows API e os tipos usados na programação Windows
#include <windows.h>
#include <tchar.h>
#include "MainWnd.h"

// Pré-declaração da função WndProc (a que executa os procedimentos da janela por
// "callback") 
LRESULT CALLBACK TreatEvents(HWND, UINT, WPARAM, LPARAM);

// Nome da classe da janela (para programas de uma só janela, normalmente este 
// nome é igual ao do próprio programa)
// "szprogName" é usado mais abaixo na definição das propriedades 
// da classe da janela
TCHAR *szProgName = TEXT("Snake Multiplayer");


// ============================================================================
// FUNÇÂO DE INÍCIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa começa sempre a sua execução na função WinMain()
// que desempenha o papel da função main() do C em modo consola
// WINAPI indica o "tipo da função" (WINAPI para todas as declaradas nos headers
// do Windows e CALLBACK para as funções de processamento da janela)
// Parâmetros:
//   hInst: Gerado pelo Windows, é o handle (número) da instância deste programa 
//   hPrevInst: Gerado pelo Windows, é sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Gerado pelo Windows, é um ponteiro para uma string terminada por 0
//              destinada a conter parâmetros para o programa 
//   nCmdShow:  Parâmetro que especifica o modo de exibição da janela (usado em  
//				ShowWindow()

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, 
				   LPSTR lpCmdLine, int nCmdShow) {
	MSG lpMsg;			// MSG é uma estrutura definida no Windows para as mensagens

// ============================================================================
// 1. Criar a janela com a definição das características e registo da estrutura
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
// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa
// fila de espera até que GetMessage(...) possa ler "a mensagem seguinte"	

// Parâmetros de "getMessage":
//  1)	"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi 
//		declarada no início de WinMain()):
/*			HWND hwnd		handler da janela a que se destina a mensagem
			UINT message	Identificador da mensagem
			WPARAM wParam	Parâmetro, p.e. código da tecla premida
			LPARAM lParam	Parâmetro, p.e. se ALT também estava premida
			DWORD time		Hora a que a mensagem foi enviada pelo Windows
			POINT pt		Localização do mouse (x, y) 
	2)   handle da window para a qual se pretendem receber mensagens
		 (=NULL se se pretendem receber as mensagens para todas as janelas 
		 pertencentes ao thread actual)
	3)	 Código limite inferior das mensganes que se pretendem receber
	4)   Código limite superior das mensagens que se pretendem receber
*/

// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
// 	     terminando então o loop de recepção de mensagens, e o programa 

	while (GetMessage(&lpMsg,NULL,0,0)) {	
		TranslateMessage(&lpMsg);			// Pré-processamento da mensagem
											// p.e. obter código ASCII da tecla
											// premida
		DispatchMessage(&lpMsg);			// Enviar a mensagem traduzida de volta
											// ao Windows, que aguarda até que a 
											// possa reenviar à função de tratamento
											// da janela, CALLBACK WndProc (mais 
											// abaixo)
	}
	/*
	//É mais SEGURO o seguinte ciclo de recepção de mensagens, para saber se houve um erro
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
	return((int)lpMsg.wParam);		// Retorna-se sempre o parâmetro "wParam" da
								// estrutura "lpMsg"

}


