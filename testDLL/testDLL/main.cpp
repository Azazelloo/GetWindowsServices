#include "pch.h"
#include "Header.h"


void GetCountServices(DWORD& size)
{
	schSCManager=OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);  //открываем дескриптор БД;
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
			free(buf_for_status);
			break;
		}
		bufSize += moreBytesNeeded;
		buf_for_status = new LPSTR[bufSize]();
		moreBytesNeeded = 0;
	}

	CloseServiceHandle(schSCManager);
	delete[] buf_for_status;
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
		free(buf_for_status);
		buf_for_status = malloc(bufSize);
		moreBytesNeeded = 0;
	}

	/*заполняем массив объектами типа ServicesObj*/

	for(size_t i = 0; i < serviceCount; i++)
	{
		servList[i].Name = servicesStatus[i].lpServiceName;
		servList[i].PID = servicesStatus[i].ServiceStatusProcess.dwProcessId;
		servList[i].DispName = servicesStatus[i].lpDisplayName;
		servList[i].state = servicesStatus[i].ServiceStatusProcess.dwCurrentState;

		/*вытаскиваем group и path*/
		//schSCService = OpenService(schSCManager, servicesStatus[i].lpServiceName, SERVICE_ALL_ACCESS);
		//QueryServiceConfig(schSCService, (LPQUERY_SERVICE_CONFIGA)buf_for_config, 4096, &moreBytesNeeded);
		//servicesConfig = (QUERY_SERVICE_CONFIG*)buf_for_config;

		//if (servicesConfig->lpBinaryPathName != nullptr)
		//{
		//	tmp_str = servicesConfig->lpBinaryPathName;
		//	//int indexFlag=tmp_str.find("-k");
		//	//if (indexFlag >= 0)
		//	//{
		//	//	servList[i].path.insert(servList[i].path.begin(),tmp_str.begin(),tmp_str.begin()+indexFlag);
		//	//	servList[i].group.insert(servList[i].group.begin(), tmp_str.begin() + indexFlag+3, tmp_str.end()); //+3 -> убираем -k
		//	//}
		//	//else servList[i].path.insert(servList[i].path.begin(), tmp_str.begin(), tmp_str.end());
		//}
		//else tmp_str = " - ";

		//CloseServiceHandle(schSCService);
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

	StartService(schSCService, 0, NULL);

	CloseServiceHandle(schSCService);
	return 1;
}