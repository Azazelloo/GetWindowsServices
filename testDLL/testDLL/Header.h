#pragma once
#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

#include <iostream>
#include <windows.h>
#include <iomanip>

using namespace std;

static SC_HANDLE schSCManager;

struct ServicesObj
{
	WCHAR* Name;
	unsigned int PID=0;
	const WCHAR* DispName;
	unsigned int state=0;
	const WCHAR* group;
	const WCHAR* path;	
};

MYDLL_API int GetServicesList(ServicesObj* s);
MYDLL_API void GetCountServices(DWORD& size);
MYDLL_API int StopSrv(WCHAR* name);
MYDLL_API int StartSrv(WCHAR* name);
MYDLL_API int RestartSrv(WCHAR* name);