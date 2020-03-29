#include "pch.h"
#include "Header.h"


int GetCountServices(DWORD& size)
{
	schSCManager=OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);  //открываем дескриптор БД;

	if(schSCManager==0) return 0;

	void* buf_for_status = new LPSTR[4096]();
	DWORD moreBytesNeeded=0, serviceCount,bufSize=0;

	for (;;)
	{
		if (EnumServicesStatusEx(schSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
			(LPBYTE)buf_for_status, bufSize, &moreBytesNeeded, &size, NULL, NULL))
		{
			break;
		}

		int err = GetLastError();
		if (ERROR_MORE_DATA != err)
		{
			CloseServiceHandle(schSCManager);
			delete[] buf_for_status;
			return 0;
		}
		bufSize += moreBytesNeeded;
		buf_for_status = new LPSTR[bufSize]();
		moreBytesNeeded = 0;
	}

	CloseServiceHandle(schSCManager);
	delete[] buf_for_status;

	return 1;
}

int GetServicesList(ServicesObj* servList)
{
	setlocale(LC_ALL, "Russian");

	SC_HANDLE schSCService;
	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);  //открываем дескриптор БД

	void* buf_for_status = NULL, *buf_for_config=NULL;
	DWORD bufSize = 0;
	DWORD moreBytesNeeded, serviceCount;
	ENUM_SERVICE_STATUS_PROCESS* servicesStatus;
	QUERY_SERVICE_CONFIG* servicesConfig;
	const WCHAR* tmpStr;
	WCHAR *str1,*str2;

	/*получаем массив структур служб*/
	for (;;) 
	{
		if (EnumServicesStatusEx(schSCManager,SC_ENUM_PROCESS_INFO,SERVICE_WIN32,SERVICE_STATE_ALL,
			(LPBYTE)buf_for_status,bufSize,&moreBytesNeeded,&serviceCount,NULL,NULL)) 
			{
				servicesStatus = (ENUM_SERVICE_STATUS_PROCESS*)buf_for_status;
				bufSize = 0;
				break;
			}

		int err = GetLastError();
		if (ERROR_MORE_DATA != err) 
			{
				free(buf_for_status);
				return err;
			}
		bufSize += moreBytesNeeded;
		buf_for_status = new LPSTR[bufSize]();
		moreBytesNeeded = 0;
	}

	/*заполняем массив объектами типа ServicesObj*/

	for(size_t i = 0; i < serviceCount; i++)
	{
		servList[i].m_name = servicesStatus[i].lpServiceName;
		servList[i].m_pid = servicesStatus[i].ServiceStatusProcess.dwProcessId;
		servList[i].m_dispName = servicesStatus[i].lpDisplayName;
		servList[i].m_state = servicesStatus[i].ServiceStatusProcess.dwCurrentState;

		/*вытаскиваем group и path*/
		buf_for_config=new QUERY_SERVICE_CONFIG[4096]();

		schSCService = OpenService(schSCManager, servicesStatus[i].lpServiceName, SERVICE_ALL_ACCESS);
		QueryServiceConfig(schSCService, (LPQUERY_SERVICE_CONFIGW)buf_for_config, 4096, &moreBytesNeeded);
		servicesConfig = (LPQUERY_SERVICE_CONFIGW)buf_for_config;

		if (servicesConfig->lpBinaryPathName != nullptr)
		{
			tmpStr = servicesConfig->lpBinaryPathName;
			size_t j;
			for(j=0;j<wcslen(tmpStr);j++)// ищем -k
			{
				if(tmpStr[j] == '-')
				{
					if(tmpStr[j + 1] == 'k')
					{
						MyWcsncpy(servList[i].m_path,tmpStr,j-2); // не захватываем пробел и '-' 
						MyWcsncpy(servList[i].m_group,tmpStr+j+2,wcslen(tmpStr)-j);
						break;
					}
				}

				if(tmpStr[j+1]=='\0')//если в строке нет -k
				{
					MyWcsncpy(servList[i].m_path,tmpStr,j+1); //не теряем '\0'
					break;
				}
			}
		}
		CloseServiceHandle(schSCService);
	}
	
	//CloseServiceHandle(schSCManager); //не закрываем дескриптор для дальнейшего использования в функциях
	//start,stop,restart
	return 0;
}


int StopSrv(WCHAR* name)
{
	SERVICE_STATUS lastStatus;
	SC_HANDLE schSCService = OpenService(schSCManager, name, SERVICE_ALL_ACCESS);
	if (schSCService == NULL) return 0;

	if (!ControlService(schSCService, SERVICE_CONTROL_STOP, &lastStatus))
	{
		CloseServiceHandle(schSCService);
		return 0;
	}

	CloseServiceHandle(schSCService);
	return 1;
}

int StartSrv(WCHAR* name)
{
	SERVICE_STATUS lastStatus;
	SC_HANDLE schSCService = OpenService(schSCManager, name, SERVICE_ALL_ACCESS);
	if (schSCService == NULL) return 0;

	if (!StartService(schSCService, 0, NULL))
	{
		CloseServiceHandle(schSCService);
		return 0;
	}

	CloseServiceHandle(schSCService);
	return 1;
}

int RestartSrv(WCHAR* name)
{
	int err;
	SERVICE_STATUS lastStatus;
	SC_HANDLE schSCService = OpenService(schSCManager, name, SERVICE_ALL_ACCESS);
	if (schSCService == NULL) return 0;

	ControlService(schSCService, SERVICE_CONTROL_STOP, &lastStatus);

	while (lastStatus.dwCurrentState != 1) //ждем остановки, службы обновляя структуру lastStatus
	{ 
		err=ControlService(schSCService, SERVICE_CONTROL_STOP, &lastStatus); 
	} 

	if(StartService(schSCService, 0, NULL))
	{
		CloseServiceHandle(schSCService);
		return 1;
	}

}


void MyWcsncpy(WCHAR* s_to, const WCHAR* s_from, size_t count)
{
	for(size_t i=0;i<=count;i++)
	{
		s_to[i]=s_from[i];
	}
	s_to[count+1]=L'\0';
}