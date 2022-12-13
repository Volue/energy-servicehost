/*
$Header$
Description:
The PDComModule defines the module concept to be used
together with the PDCom library.

Copyright (C) Powel Data AS
All rights reserved.
*/

//-*-Mode: C++;-*-
#ifndef _PDComModule_h_
#define _PDComModule_h_
#pragma once


//---- PDComModule -----------------------------------------------------------

class PDComModule : public CComModule {
public:
	PDComModule();
	LONG Unlock();
	DWORD dwThreadID;
	void setInteractive(int x) { interactive=x;}
	int interactive;
	HANDLE hEventShutdown;
	void MonitorShutdown();
	bool StartMonitor();
	bool bActivity;
};

extern PDComModule _Module;

/**
* ComHost class used by SimulatorServer and InflowServer (and others)
*/
class ComHost {
public:
    ComHost(int pauseMSecs=1000) {
	factoryActive=false;
	dwPause=pauseMSecs;
    }
    LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2) {
	while (p1 != NULL && *p1 != NULL) {
            LPCTSTR p = p2;
	    while (p != NULL && *p != NULL) {
	        if (*p1 == *p) return CharNext(p1);
		p = CharNext(p);
	    }
	    p1 = CharNext(p1);
	}
	return NULL;
    }
    int MainBegin(bool multipleUse) {
	_Module.StartMonitor();
	HRESULT hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER,(multipleUse?REGCLS_MULTIPLEUSE:REGCLS_SINGLEUSE) | REGCLS_SUSPENDED);
	_ASSERTE(SUCCEEDED(hRes));
	hRes = CoResumeClassObjects();
	_ASSERTE(SUCCEEDED(hRes));
	factoryActive=true;
	return 0;
    }
    void MsgLoop() {
        MSG msg;
        while (GetMessage(&msg, 0, 0, 0))
	   DispatchMessage(&msg);
    }
    int MainEnd() {
	if (factoryActive) {
	    _Module.RevokeClassObjects();
	    Sleep(dwPause); //wait for any threads to finish
	    factoryActive=false;
	}
	return 0;
    }
    int Exit() {
	_Module.Term();
	CoUninitialize();
	return 0;
    }
    void Start(bool multiUse) {
	MainBegin(multiUse);
	MsgLoop();
	MainEnd();
	Exit();
    }
    private:
    bool factoryActive;
    long dwPause;
};

#endif
