/*
Copyright (C) Powel Data AS
All rights reserved.
*/

#include <PrePDCom.h>
#define STRICT // Enable STRICT Type Checking in Windows headers
#define WIN32_LEAN_AND_MEAN // To speed the build process exclude rarely-used services from Windows headers
#define NOMINMAX // Exclude min/max macros from Windows header
#define IID_DEFINED
#include <initguid.h>
#include <PDCom_i.h>
int exitLockCount=0;			// Used to keep track of when we can quit this application,
								// ref. also PDComClient::attach && InternalRelease.
const DWORD dwTimeOut = 5000;	// Time for EXE to be idle before shutting down.
const DWORD dwPause = 1000;		// Time to wait for threads to finish up.

PDComModule::PDComModule()
{
	interactive=false;
	hEventShutdown=0;
	bActivity=false;
}

LONG PDComModule::Unlock()
{
	LONG l = CComModule::Unlock();
	if (l == exitLockCount && ! interactive) {
		if(hEventShutdown) {
			bActivity = true;
			SetEvent(hEventShutdown); // tell monitor that we transitioned to zero
		} else {
			CoSuspendClassObjects();
			PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
		}
	}
	return l;
}
// Passed to CreateThread to monitor the shutdown event
static DWORD WINAPI MonitorProc(void* pv)
{
	PDComModule* p = (PDComModule*)pv;
	p->MonitorShutdown();
	return 0;
}

#include <time.h>
//Monitors the shutdown event
void PDComModule::MonitorShutdown() {
	time_t Tstart = time(0);// Tstart=PDTime::now();
	const char *CfgUptimeName="ICC_SERVER_UPTIME";
	time_t maxUpTime= (time_t)(getenv(CfgUptimeName)?atoi(getenv(CfgUptimeName)):0);
	bool suspended = false;

	if ( maxUpTime > 0 ) 
		WaitForSingleObject(hEventShutdown, DWORD( maxUpTime)*1000);
	else
		WaitForSingleObject(hEventShutdown, INFINITE);
	while (1) {
		DWORD dwWait=0;
		do {
			bActivity = false;
			dwWait = WaitForSingleObject(hEventShutdown, dwTimeOut);
		} while (dwWait == WAIT_OBJECT_0);
		// timed out
		if (!bActivity && m_nLockCnt == exitLockCount) // if no activity let's really bail
		{
			if ( !suspended ) CoSuspendClassObjects();
			if (!bActivity && m_nLockCnt == exitLockCount)
				break;
		} else if(maxUpTime > 0 && !suspended ) {
			if( (time(0) - Tstart) > maxUpTime ) {
				// wait for no activity and then quit
				CoSuspendClassObjects();	// new requests for this component will go to another instance
				suspended = true;
			}
		}
	}
	CloseHandle(hEventShutdown);
	PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
}

bool PDComModule::StartMonitor() {
	hEventShutdown = CreateEvent(NULL, false, false, NULL);
	if (hEventShutdown == NULL)
		return false;
	DWORD dwThreadID;
	HANDLE h = CreateThread(NULL, 0, MonitorProc, this, 0, &dwThreadID);
	return (h != NULL);
}

PDComModule _Module;

