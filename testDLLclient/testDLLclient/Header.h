#pragma once
#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

#include <iostream>
#include <windows.h>
#include <iomanip>
#include <wchar.h>
#include <vector>
#include <string>

using namespace std;

static SC_HANDLE schSCManager;

struct ServicesObj
{
	wstring m_name;
	unsigned int m_pid=0;
	wstring m_dispName;
	unsigned int m_state=0;
	wstring  m_group;
	wstring  m_path;	
};

MYDLL_API vector<ServicesObj> GetServicesList();
MYDLL_API int StopSrv(WCHAR* name);
MYDLL_API int StartSrv(WCHAR* name);
MYDLL_API int RestartSrv(WCHAR* name);