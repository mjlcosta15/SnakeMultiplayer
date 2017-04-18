// Como criar uma DLL

#pragma once
#include <windows.h>
#include <tchar.h>

#ifdef AULADLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif
extern "C" {
	extern DLL_IMP_API int var_global;

	DLL_IMP_API int faz(void);
}