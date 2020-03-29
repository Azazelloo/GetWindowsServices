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

using namespace std;

static SC_HANDLE schSCManager;

struct ServicesObj
{
	WCHAR* m_name;
	unsigned int m_pid=0;
	const WCHAR* m_dispName;
	unsigned int m_state=0;
	WCHAR* m_group;
	WCHAR* m_path;	
public:
	ServicesObj()
	{
		m_path=new WCHAR[128]();
		m_group=new WCHAR[128]();
	}
};

MYDLL_API int GetServicesList(ServicesObj* s);
MYDLL_API int GetCountServices(DWORD& size);
MYDLL_API int StopSrv(WCHAR* name);
MYDLL_API int StartSrv(WCHAR* name);
MYDLL_API int RestartSrv(WCHAR* name);
MYDLL_API void MyWcsncpy(WCHAR* s_to,const WCHAR* s_from,size_t count);