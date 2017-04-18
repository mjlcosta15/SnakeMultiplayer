// Como criar uma DLL
#include "Header.h"



int var_global = 1;

int faz(void) {
	int result = MessageBox(NULL, TEXT("Estou dentro da DLL"), TEXT("Teste"), MB_YESNO);
	return result;
}