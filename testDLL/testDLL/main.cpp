#include "pch.h"
#include "Header.h"

vector<ServicesObj> GetServicesList()
{
	setlocale(LC_ALL, "Russian");

	SC_HANDLE schSCService;
	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);  //открываем дескриптор БД

	void* buf_for_status = NULL, *buf_for_config=NULL;
	DWORD bufSize = 0;
	DWORD moreBytesNeeded, serviceCount;
	ENUM_SERVICE_STATUS_PROCESS* servicesStatus;
	QUERY_SERVICE_CONFIG* servicesConfig;

	/*получаем массив структур служб*/
	for (;;) 
	{
		if (EnumServicesStatusEx(schSCManager,SC_ENUM_PROCESS_INFO,SERVICE_WIN32,SERVICE_STATE_ALL,
			(LPBYTE)buf_for_status,bufSize,&moreBytesNeeded,&serviceCount,NULL,NULL)) 
			{
				servicesStatus = (ENUM_SERVICE_STATUS_PROCESS*)buf_for_status;
				break;
			}

		int err = GetLastError();
		if (ERROR_MORE_DATA != err) 
			{
				free(buf_for_status);
				return vector<ServicesObj>();
			}
		bufSize += moreBytesNeeded;
		buf_for_status =new LPSTR[bufSize]();
		moreBytesNeeded = 0;
	}

	/*заполняем массив объектами типа ServicesObj*/
	vector<ServicesObj> servList(serviceCount);
	wstring tmpStr;

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

			int flag=tmpStr.find(L"-k");
			if(flag>0)
			{
				servList[i].m_path.insert(servList[i].m_path.begin(),tmpStr.begin(),tmpStr.begin()+flag);
				servList[i].m_group.insert(servList[i].m_group.begin(),tmpStr.begin()+flag+2,tmpStr.end());
			}
			else servList[i].m_path.insert(servList[i].m_path.begin(),tmpStr.begin(),tmpStr.end());

			
		}
		CloseServiceHandle(schSCService);
	}
	
	//CloseServiceHandle(schSCManager); //не закрываем дескриптор для дальнейшего использования в функциях
	//start,stop,restart
	return servList;
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

