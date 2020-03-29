#include "Header.h"
#include <iostream>
#include <cstring>
using namespace std;


int main()
{
	DWORD size=0;
	int err=1;
	err=GetCountServices(size);
	ServicesObj* ss=new ServicesObj[size](); //добавить функцию считающую количество служб
	GetServicesList(ss);

	for (size_t i=0;i<size;i++)
	{
		cout << "[" << i << "] = ";
		wcout<<ss[i].m_name <<"->"<<ss[i].m_pid<<"->"<<ss[i].m_group<< endl;
	}	

	//err=StopSrv(ss[0].Name); //остановка службы
	//err=StartSrv(ss[0].Name);// запуск службы
	//err=RestartSrv(ss[0].Name);
 	return 0;
}