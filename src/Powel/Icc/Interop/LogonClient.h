#pragma once
struct ICCAppCommMsgCB;
#include <string>
#using <mscorlib.dll>
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;

class LogonClient
{
public:
	LogonClient(std::string applicationName,
		int iccModule,
		int iccProgram,
		Powel::Icc::Interop::IccApplication::ExitDelegate^ exit,
		Powel::Icc::Interop::IccApplication::HideDelegate^ hide);
	~LogonClient(void);
	void Logon(bool connectToGui, bool showLogWindow);
	bool GuiUsed();
	bool GetLogonParameters(std::string &username, std::string &password, std::string &server);
	void InitializeLogging(void);
	std::string GetMessageDefinition(int messageID);
	int Log(int messageID, std::string messageString, std::string argString);
	int GetAppKey();
	void StartReport(std::string command);
	void StartProgram(std::string program, std::string command);
	void RequestLogoff(void);
	void RequestStop(void);
	bool IsRunning(void);
	bool LogonOk(void);
private:
	void DoLogging();
	ICCAppCommInterface *icc;
	lang_id langid;
	std::string applicationName;
	int iccModule;
	int iccProgram;
	void msgHandler(ICCAppCommMsgCB& msgCB);
	ICCAppCommProgram *TSshow;
	PDLogExtMessage *logMessage;
	int logResult;
	bool logResultHasBeenSet;
	bool logonUisOk;
	bool loggingOk;
	bool logoffRequested;
	bool stopRequested;
	bool isRunning;
	bool guiUsed;
	std::string username;
	std::string password;
	std::string server;
	CRITICAL_SECTION cs;
	void lock() { EnterCriticalSection(&cs);}
	void unlock() { LeaveCriticalSection(&cs);}
	gcroot<Powel::Icc::Interop::IccApplication::ExitDelegate^> exit;
	gcroot<Powel::Icc::Interop::IccApplication::HideDelegate^> hide;
};