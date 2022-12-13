/*
Description:
Takes care of reading environment from registry before
main() is called.
_____________________________
Copyright (C) Powel Data AS
All rights reserved.
*/
#define _WIN32_DCOM 
#include <log4cxx/logger.h> // Include early to avoid C4251.
#include "icc_env.h"
#include <stdlib.h>
#include <string.h>
#include <string>
#include <comdef.h>
#include <atlbase.h>
#include <atlconv.h>
#include <process.h>
#include "../../ConfigurationSystem/CfgServer/CfgServer_i.h"
#include "../../ConfigurationSystem/CfgServer/CfgServer_i.c"
#include "../../cpp/include/PDVariantUtility.h"
//#include "../../include/ICC_InitExit.h"
#include <ctime>
#include <sstream>
#include "Profiler.h"
#include <PDLog4Cxx.h>
#include <list>

static log4cxx::LoggerPtr _l4n_PDInitEnv(log4cxx::Logger::getLogger("PDInitEnvironment"));

using namespace PowelVariantUtility;
extern "C" 
{
	int icc_cleanup_env();
	char ** icc_env_ptr();
	char **icc_env_setup_internal(char *key, int *count);
	int icc_putenv(char *);
};

void LoadConfigurationParameters() 
{
	auto count = 0;
	icc_env_setup_internal(getenv("ICC_ENVKEY"), &count);
}

void UnloadConfigurationParameters() 
{
	icc_cleanup_env();
}

/*TOSA class ICCConfigurationParametersFirst :private ICCInitExitBase
{ /*
	o Register this class to register the FIRST global
	object to be done, - load configuration parameters.
	*
public:
	ICCConfigurationParametersFirst() 
	{
        
        //The warning "Please initialize the log4cxx system properly." 
        //comes from Hierarchy::emitNoAppenderWarning in
        //c:\Projects\ICCMDMS\Log4Powel\apache-log4cxx\src\main\cpp\hierarchy.cpp
        //It disappears when we call PDLog4Cxx::Configure from here.

        //TODO. SIV. We use a dirty trick here to avoid side effects while testing 
        //!RestrictionExpand expressions in Nimbus.
        //Logging/profiling from SimService is redirected to the files in PowelSmartLogs\PDInitEnvironment directory
        //after call to CreateSolverExpressionString.
        //Attempt to use global mutex in Configure did not yield meaningful results.
        //PDLog4Cxx::Configure("Simulator.SimService"); // "PDInitEnvironment");
        //LOG4CXX_STR("LoadConfigurationParameters. After PDLog4Cxx::Configure");
        const auto log4cxxOn = getenv("LOG_FROM_PD_INIT_ENVIRONMENT");
        if (log4cxxOn == nullptr)
        {
            log4cxx::LevelPtr levelOff(log4cxx::Level::toLevel("OFF"));
            _l4n_PDInitEnv->setLevel(levelOff);
        }

        const auto isConfigReader = true;
        ICCInitExit::addInitExit(*this, isConfigReader);
	}
private:
	void init() 
	{
		LoadConfigurationParameters();
	}
	void exit() 
	{
		UnloadConfigurationParameters();
	}
};

ICCConfigurationParametersFirst loadConfigurationParametersFirst;*/

//#define GET_CONFIG_Asynchronously

#ifdef GET_CONFIG_Asynchronously
static void __cdecl CfgReaderThread(void *cfgReader);// entry point for workerthread
#endif GET_CONFIG_Asynchronously


class CfgReader 
{
	char *key;
	char *userName;
	char *pcName;
	char *progName;
	std::vector<std::string> configParams_;
	std::vector<std::string> encryptParams_;
    std::string status_;
#ifdef GET_CONFIG_Asynchronously
    unsigned long m_thread;
	HANDLE workThreadExit;// workthread signals here at exit
#endif GET_CONFIG_Asynchronously
public:
	CfgReader(char *keyA, char *userNameA, char*pcNameA, char *progNameA) 
	{
		key = keyA ? _strdup(keyA) : 0;
		userName = userNameA ? _strdup(userNameA) : 0;
		pcName = pcNameA ? _strdup(pcNameA) : 0;
		progName = progNameA ? _strdup(progNameA) : 0;
#ifdef GET_CONFIG_Asynchronously
        m_thread = -1;
        workThreadExit = CreateSemaphore(0, 0, 100, NULL);
#endif GET_CONFIG_Asynchronously
    }
	~CfgReader() 
	{
#ifdef GET_CONFIG_Asynchronously
        waitForFinish(1000);// wait one sec and kill if not finished
		CloseHandle(workThreadExit);
#endif GET_CONFIG_Asynchronously
        free(userName);
        free(progName); 
        free(pcName);
	}
	void start() 
	{
#ifdef GET_CONFIG_Asynchronously
		m_thread = _beginthread(CfgReaderThread, 0, this);
		//return m_thread != -1 ? true : false;
#else
#endif
	}
	
    void GetConfiguration(std::string & status) 
	{
#ifdef GET_CONFIG_Asynchronously
        const int maxWaitMsec = 20000;
        if (waitForFinish(maxWaitMsec)) // 20 seconds max
		{ 
#else GET_CONFIG_Asynchronously
        workerThreadMain();
#endif
        LOG4CXX_INFO(_l4n_PDInitEnv, __FUNCTION__ "\n\tconfigParams_.size() = " << configParams_.size());
        for (const auto & param : configParams_)
		{
			icc_putenv((char *)param.c_str());
		}
#ifdef GET_CONFIG_Asynchronously
        }
        LOG4CXX_ERROR(_l4n_PDInitEnv, __FUNCTION__ "\n\tWaited more than maxWaitMsec = " << maxWaitMsec << 
            "\n\tEnvironment variables not initialized");
#endif GET_CONFIG_Asynchronously
        status = status_;
    }

void GetEncryptedParameters(std::vector<std::string>& encryptedParams, std::string& status)
{
#ifdef GET_CONFIG_Asynchronously
	const int maxWaitMsec = 20000;
	if (waitForFinish(maxWaitMsec)) // 20 seconds max
	{
#else GET_CONFIG_Asynchronously
	workerThreadMain();
#endif
	LOG4CXX_INFO(_l4n_PDInitEnv, __FUNCTION__ "\n\tencryptParams_.size() = " << encryptParams_.size());
	encryptedParams = encryptParams_;
#ifdef GET_CONFIG_Asynchronously
	}
LOG4CXX_ERROR(_l4n_PDInitEnv, __FUNCTION__ "\n\tWaited more than maxWaitMsec = " << maxWaitMsec <<
	"\n\tEnvironment variables not initialized");
#endif GET_CONFIG_Asynchronously
	status = status_;
}
private:
#ifdef GET_CONFIG_Asynchronously
    friend void __cdecl CfgReaderThread(void *cfgReader);
    bool waitForFinish(int ms)
	{
        LOG4CXX_INFO_ENTRY(_l4n_PDInitEnv, 
            "\n\tms (wait time) = " << ms << " ms" <<
            "\n\tworkThreadExit = " << workThreadExit <<
            "");
        if (m_thread != -1 && (WaitForSingleObject(workThreadExit, ms) != WAIT_OBJECT_0))
		{
			LOG4CXX_INFO_EXIT(_l4n_PDInitEnv, "\n\tAbout to termnate the thread m_thread = " << m_thread);
            TerminateThread((HANDLE)m_thread, -1);//fatal terminate thread
			return false;
		}

        m_thread = -1;
		LOG4CXX_INFO_EXIT(_l4n_PDInitEnv, "\n\tWaited less than " << ms << " ms");
        return true;
	}
#endif GET_CONFIG_Asynchronously

	void workerThreadMain() 
	{
        Profiler profiler(__FUNCTION__);
        const auto specialLog = _l4n_PDInitEnv->isWarnEnabled();
        std::stringstream ss;

        if (specialLog)
            ss << __FUNCTION__;

        std::stringstream ssErr;
        try
		{
            auto threadModel = COINIT_MULTITHREADED;
            HRESULT res = CoInitializeEx(0, threadModel);
            if (res == RPC_E_CHANGED_MODE)
            {
                if (specialLog)
                    ss << "\n\tCaling CoInitializeEx(0, COINIT_APARTMENTTHREADED) after COINIT_MULTITHREADED failed";
                auto threadModel = COINIT_APARTMENTTHREADED;
                res = CoInitializeEx(0, threadModel);
            }

			if (res == S_OK || res == S_FALSE) //S_FALSE <=> CoInitializeEx has been already called with the same concurrency model
			{
                //get cfg reader and parameters
				CComPtr<ICfgReader> srv = nullptr;
				auto clsid = CLSID_CfgReaderFactory; // CLSID_CfgManagerFactory; // To test error handling
				//clsid.Data1 = 0; // To test error handling
				//HRESULT res=CLSIDFromProgID(OLESTR("Powel.ConfigurationServer"),&clsid);
				//_com_util::CheckError(res);
				IClassFactory *factory = nullptr;
				try
				{
					res = CoGetClassObject(clsid, CLSCTX_ALL, NULL, IID_IClassFactory, (void**) & factory);
				}
				catch (_com_error & ex) 
				{
                    ssErr << "_com_error after CoGetClassObject. ex.Error() = " << ex.Error() 
                        << ", Description():\n\t" << ex.Description();
				}
				catch (...) 
				{
                    ssErr << "Unexpected exception after CoGetClassObject";
				}
				if (res == S_OK && factory != nullptr)
				{
					try
					{
						res = factory->CreateInstance(NULL, IID_ICfgReader, (void**)&srv);
					}
					catch (_com_error & ex)
					{
                        ssErr << "_com_error after CreateInstance(IID_ICfgReader). ex.Error() = " << ex.Error()
                            << ", Description():\n\t" << ex.Description();
					}
					catch (...)
					{
                        ssErr << "Unexpected exception after CreateInstance(IID_ICfgReader)";
					}
                    if (res != S_OK)
                    {
                        ssErr << "Error after CreateInstance(IID_ICfgReader). res = " << res;
                    }
				}
				else
				{
					if (factory == nullptr) 
                    {
                        ssErr << "Error after CoGetClassObject(CLSID_CfgReaderFactory). res = " << res
                            << ", factory == nullptr";
					} 
                    else 
                    {
                        ssErr << "Error after CoGetClassObject(CLSID_CfgReaderFactory). res = " << res;
					}
				}
				
				if (factory != nullptr)
					factory->Release();

				if (srv != nullptr)
				{

					VARIANT configResult;
					VariantInit(&configResult);
					VARIANT configCount;
					VariantInit(&configCount);

					auto u_name = _bstr_t(userName ? userName : "");
					auto pc_name = _bstr_t(pcName ? pcName : "");
					auto prog_name = _bstr_t(progName ? progName : "");
                    if (specialLog)
                    {
                        ss << "\n\tsrv->GetConfiguration(" << //key = " << key << ", " <<
                            "userName = " << (userName == nullptr ? "" : userName) <<
                            ", pcName = " << (pcName == nullptr ? "" : pcName) <<
                            ", progName = " << (progName == nullptr ? "" : progName) <<
                            ")";
                    }
					HRESULT cres = srv->GetConfiguration(u_name, pc_name, prog_name, &configResult, &configCount);
					if (cres == S_OK) 
					{
						_bstr_t bnull;
						const VVAccessor<_bstr_t> cfgList(configResult, bnull);
                        if (specialLog)
                            ss << "\n\tcfgList.size() = " << cfgList.size();
                        for (size_t i = 0; i != cfgList.size(); i++)
						{
							const auto env_var = std::string((const char*)cfgList[i]);
							configParams_.push_back(env_var);
							if (specialLog && (
                                   (env_var.substr(0, 10) == "ICC_DBUSER")
                                || (env_var.substr(0, 5) == "LOCAL")
                                || (env_var.substr(0, 12) == "ICC_NO_PDLOG")
                                || (env_var.substr(0, 17) == "SMG_EXTERNAL_AUTH")
                                ))
							{
                                ss << "\n\t\t" << i << ". env_var = " << env_var;
							}
						}
						VariantClear(&configResult);
						VariantClear(&configCount);
					}
					else 
					{
                        ssErr << "Error after GetConfiguration. cres = " << cres;
					}
					VARIANT encryptResult;
					VariantInit(&encryptResult);
					VARIANT encryptCount;
					VariantInit(&encryptCount);
					if (specialLog)
					{
						ss << "\n\tsrv->GetEcryptedParameters(" << //key = " << key << ", " <<
							"userName = " << (userName == nullptr ? "" : userName) <<
							", pcName = " << (pcName == nullptr ? "" : pcName) <<
							", progName = " << (progName == nullptr ? "" : progName) <<
							")";
					}
					HRESULT cres2 = srv->GetEncryptedParameters(&encryptResult, &encryptCount);
					if (cres2 == S_OK)
					{
						_bstr_t bnull;
						const VVAccessor<_bstr_t> encryptList(encryptResult, bnull);
						if (specialLog)
							ss << "\n\tencryptList.size() = " << encryptList.size();
						for (size_t i = 0; i != encryptList.size(); i++)
						{
							const auto encrypt_var = std::string((const char*)encryptList[i]);
							encryptParams_.push_back(encrypt_var);
						}
						VariantClear(&encryptResult);
						VariantClear(&encryptCount);
					}
					else
					{
						ssErr << "Error after GetEcryptedParameters. cres2 = " << cres2;
					}
				}
			}
			else
			{
                ssErr << "Error after CoInitializeEx. res = " << res;
                if (res == RPC_E_CHANGED_MODE)
                    ssErr << " = RPC_E_CHANGED_MODE (Cannot change thread model to " << threadModel << "after it is set)";
			}
            CoUninitialize();
		}
		catch (_com_error & ex) 
		{ 
			// all other kind of failure
            ssErr << "_com_error after CoUninitialize(?). ex.Error() = " << ex.Error()
                << ", Description():\n\t" << ex.Description();
		}
		catch (...) 
		{
            ssErr << "Unexpected exception after CoUninitialize(?)";
		}
#ifdef GET_CONFIG_Asynchronously
        long prevCount;
        //LOG4CXX_WARN(_l4n_PDInitEnv, "\n\tPrior ReleaseSemaphore. workThreadExit = " << workThreadExit);
        ReleaseSemaphore(workThreadExit, 1, &prevCount);// tell everyone we are done
		LOG4CXX_WARN_EXIT(_l4n_PDInitEnv, "\n\tAfter ReleaseSemaphore. prevCount = " << prevCount);
#endif GET_CONFIG_Asynchronously
        status_ = ssErr.str();
        if (specialLog)
        {
            if (!status_.empty())
                ss << "\n\tstatus_ = " << status_;
            LOG4CXX_WARN(_l4n_PDInitEnv, ss.str());
        }
    }
};

#ifdef GET_CONFIG_Asynchronously
static void __cdecl CfgReaderThread(void *cfgReader)
{// entrypoint for workerthread
	((CfgReader*)cfgReader)->workerThreadMain();
}
#endif GET_CONFIG_Asynchronously

extern "C" 
{
	void RefInitEnv() 
	{ // Just to ref this class so it get linked in.
		//(void)GetInitEnvironment();
	}
	
    char* getStdErrFileName()
    {
		static char stdErrFname[1024]="";
		static int iSf = -1;
		if(iSf==0)return NULL;
		if (strlen(stdErrFname) && iSf == 1)return stdErrFname;
		if (const char* envvar = getenv("ICC_CFG_STDERR_FILE")) {
			strcpy(stdErrFname, envvar);
		} else {
			_bstr_t registryFolderPath(_T("Software\\Powel"));
			HKEY registryFolder; bool success = false;
			LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, registryFolderPath, NULL, KEY_READ, &registryFolder);
			if (res == ERROR_SUCCESS) {
				iSf = 0;
				char regCfgSTDerrFile[1024]; DWORD regCfgSTDerrFileSz = sizeof(regCfgSTDerrFile), regCfgSTDerrFileType = 0;
				long fnFound = RegQueryValueEx(registryFolder, "ICC_CFG_STDERR_FILE", NULL, &regCfgSTDerrFileType, (LPBYTE)regCfgSTDerrFile, &regCfgSTDerrFileSz);
				if (fnFound == ERROR_SUCCESS) {
					strcpy(stdErrFname, regCfgSTDerrFile);
					iSf = 1;
				}
			}
			RegCloseKey(registryFolder);
		}
		if (strlen(stdErrFname) && iSf == 1)return stdErrFname;
		return NULL;
	}

	bool stdErrToWindowsLog() {
		/*TOSA static int logStd2log = -1;
		if (logStd2log == 0)return false;
		if (logStd2log == 1)return true;
		const char* envvar = getenv("ICC_CFG_STDERR_TO_LOG");
		if (envvar && stricmp(envvar, "FALSE") == 0) {
			logStd2log = 0;
		} else {
			_bstr_t registryFolderPath(_T("Software\\Powel"));
			HKEY registryFolder; bool success = false;
			LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, registryFolderPath, NULL, KEY_READ, &registryFolder);
			logStd2log = 1;//Default is logging to windowsLog
			if (res == ERROR_SUCCESS) {
				char regCfgSTDerrFile[1024]; DWORD regCfgSTDerrFileSz = sizeof(regCfgSTDerrFile), regCfgSTDerrFileType = 0;
				long fnFound = RegQueryValueEx(registryFolder, "ICC_CFG_STDERR_TO_LOG", NULL, &regCfgSTDerrFileType, (LPBYTE)regCfgSTDerrFile, &regCfgSTDerrFileSz);
				if (fnFound == ERROR_SUCCESS) {
					if (regCfgSTDerrFile && *regCfgSTDerrFile) {
						if(!strcmp(regCfgSTDerrFile,"0") || !stricmp(regCfgSTDerrFile,"FALSE"))
							logStd2log=0;
					}
				}
			}
			RegCloseKey(registryFolder);
		}
		if (logStd2log == 1)return true;*/
		return false;
	}
	bool stdErrToWindowsDialog() {
		/*TOSA static int logStd2Dia = -1;
		if (logStd2Dia == 0)return false;
		if (logStd2Dia == 1)return true;
		const char* envvar = getenv("ICC_CFG_STDERR_TO_DIA");
		if (envvar && stricmp(envvar, "FALSE") == 0) {
			logStd2Dia = 0;
		} else {
			_bstr_t registryFolderPath(_T("Software\\Powel"));
			HKEY registryFolder; bool success = false;
			LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, registryFolderPath, NULL, KEY_READ, &registryFolder);
			const char* envvar = getenv("ICC_WINDOWS");
			if (envvar && strcmp(envvar, "0") !=0 && stricmp(envvar,"FALSE") !=0) {
				logStd2Dia = 1; // Default for Windows application is logging to windowsDialog
			} else {
				logStd2Dia = 0; // Default for libraries and console applications is not to use windowsDialog
			}
			if (res == ERROR_SUCCESS) {
				//logStd2Dia = 0;
				char regCfgSTDerrFile[1024]; DWORD regCfgSTDerrFileSz = sizeof(regCfgSTDerrFile), regCfgSTDerrFileType = 0;
				long fnFound = RegQueryValueEx(registryFolder, "ICC_CFG_STDERR_TO_DIA", NULL, &regCfgSTDerrFileType, (LPBYTE)regCfgSTDerrFile, &regCfgSTDerrFileSz);
				if (fnFound == ERROR_SUCCESS) {
					if (regCfgSTDerrFile && *regCfgSTDerrFile) {
						if (!strcmp(regCfgSTDerrFile, "0") || !stricmp(regCfgSTDerrFile, "FALSE"))
							logStd2Dia = 0;
					}
				}
			}
			RegCloseKey(registryFolder);
		}
		if (logStd2Dia == 1)return true;*/
		return false;
	}
	
    int DisplayResourceNAMessageBox(const char* errM)
	{
		/*TOSA int msgboxID = MessageBox(
			NULL,
			(LPCSTR)errM,
			(LPCSTR)"Fatal Error!",
			MB_ICONSTOP | MB_OK | MB_DEFBUTTON1
		);

		/*switch (msgboxID)
		{
		case IDCANCEL:
			// TODO: add code
			break;
		case IDTRYAGAIN:
			// TODO: add code
			break;
		case IDCONTINUE:
			// TODO: add code
			break;
		}

		return msgboxID;*/
		return 0;
	}

	void reportError(char *errMess, char * userName, char * progName) 
    {
		// Always write to stderr
		fprintf(stderr,"%s\n", errMess);
		// Optionally write to error file and/or show Windows dialog
		char *fname = getStdErrFileName();
		if (!fname && !stdErrToWindowsLog())return;
		std::string outIds;
		outIds.clear();
		if (userName && *userName) 
        {
			outIds = " User: ";
			outIds += userName;
			outIds += " ";
		}
		if (progName && *progName) 
        {
			outIds = " Application: ";
			outIds += progName;
			outIds += " ";
		}
		if (fname) 
        {
			FILE *repFile = fopen(fname, "a");
			if (repFile) {
				time_t now = time(0);
				char* dt = strtok(ctime(&now), "\n");
				if (outIds.size()) {
					fprintf(repFile, "%s: %s. %s\n", dt, outIds.c_str(), errMess);
				}
				else {
					fprintf(repFile, "%s: %s\n", dt, errMess);
				}
				fclose(repFile);
			}
		}
		if(stdErrToWindowsLog()){
			const char* custom_log_name = "Powel Configuration";
			HANDLE event_log = RegisterEventSource(NULL, custom_log_name);
			std::string outErr;
			outErr.clear();
			if (outIds.size()) {
				outErr = outIds;
				outErr += ". ";
			}
			outErr += errMess;
			const char* message = outErr.c_str();
			ReportEvent(event_log, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 1, 0, &message, NULL);
		}
		if (stdErrToWindowsDialog()) {
			DisplayResourceNAMessageBox(errMess);
		}
	}
	
    char **icc_db_setup_(char *key, int *count, char * userName, char * pcName, char * progName) 
	{
		CfgReader cfgReader(key, userName, pcName, progName);
		char ** result = nullptr;
		try
		{
			cfgReader.start();
            std::string status;
			cfgReader.GetConfiguration(status);
			if (!status.empty())
            {
				reportError((char *)("Powel Config Server cannot get configuration parameters:\n" + status).c_str()
                    , userName, progName);
			}
			// copy _environ to a owned list which is destroyed later.
			int n = 0;
			for (n = 0; _environ[n] != nullptr; n++);
			result = (char**)calloc(n+1, sizeof(char**));
			for (int i = 0; i < n; i++) {
				try {
					auto e = _environ[i];
					result[i] = strdup(_environ[i]);
				}
				catch (std::exception& ex) {
					reportError((char*)ex.what(), userName, progName);
				}
			}
			result[n] = nullptr;
			*count = n;
		}
		catch (_com_error & ex)
		{
			// all other kind of failure
			char errMess[1024];
			sprintf(errMess, "_com_error after cfgReader.GetConfiguration. Error code %#lX. Description:\n\t%s", ex.Error(), (char *)ex.Description());
			reportError(errMess, userName, progName);
		}
		catch (...)
		{
			char errMess[1024];
			sprintf(errMess, "Unexpected exception after cfgReader.GetConfiguration.");
			reportError(errMess, userName, progName);
		}

		return result;
	}

	char** icc_encrypted_parameters_(char* key, int* count, char* userName, char* pcName, char* progName)
	{
		CfgReader cfgReader(key, userName, pcName, progName);
		char** result = nullptr;
		try
		{
			cfgReader.start();
			std::string status;
			std::vector<std::string> encryptedParams;
			cfgReader.GetEncryptedParameters(encryptedParams, status);
			if (!status.empty())
			{
				reportError((char*)("Powel Config Server cannot get info of encrypted parameters:\n" + status).c_str()
					, userName, progName);
			}
			// copy _environ to a owned list which is destroyed later.
			int n = encryptedParams.size();
			result = (char**)calloc(n + 1, sizeof(char**));
			for (int i = 0; i < n; i++) {
				try {
					auto e = encryptedParams[i];
					result[i] = strdup(encryptedParams[i].c_str());
				}
				catch (std::exception& ex) {
					reportError((char*)ex.what(), userName, progName);
				}
			}
			result[n] = nullptr;
			*count = n;
		}
		catch (_com_error& ex)
		{
			// all other kind of failure
			char errMess[1024];
			sprintf(errMess, "_com_error after cfgReader.GetEncryptedParameters. Error code %#lX. Description:\n\t%s", ex.Error(), (char*)ex.Description());
			reportError(errMess, userName, progName);
		}
		catch (...)
		{
			char errMess[1024];
			sprintf(errMess, "Unexpected exception after cfgReader.GetEncryptedParameters.");
			reportError(errMess, userName, progName);
		}

		return result;
	}
};

