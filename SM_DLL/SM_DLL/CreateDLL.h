#ifndef __CREATEDLL_H__
#define __CREATEDLL_H__

#include <windows.h>

#define DLL_EXPORT __declspec(dllexport)

#ifdef __cplusplus

extern "C"
{
#endif
	int DLL_EXPORT MsgBox(int x);
}

#ifdef __cpulsplus

#endif

#endif //__CREATEDLL_H__