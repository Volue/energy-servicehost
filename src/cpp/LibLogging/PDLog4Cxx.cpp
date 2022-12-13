#include "PDLog4Cxx.h"
// include log4cxx header files.
#pragma warning(disable: 4275 4251) //NOTE: disabled log4cxx/vector warnings
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <iostream>

using namespace log4cxx;
using namespace log4cxx::xml;
using namespace log4cxx::helpers;

#include <windows.h>

#include <sys/stat.h>

using namespace std;

static std::string _curAppName = "";

void DeleteLog4Powel()
{
	// Method for closing and flushing the logger buffers.
	LoggerPtr logr = Logger::getRootLogger();
	if (!logr) 
        return;
	
	//LOG4CXX_INFO(logr,"Exiting application " << PSingleton<PDLog4Cxx>::getInstance()->application());
	logr->closeNestedAppenders();
}

void RecordConfig(string stAppName, string stConfigParams)
{
    string stConfigParamsFile = "LOG4CXXConfigParams_" + stAppName + ".log";
    string stConfigParamsFilePath = "c:/PowelSmartLogs/" + stAppName;
    struct _stat statbuffer;
    if (_stat(stConfigParamsFilePath.c_str(), &statbuffer) == 0)
    {
        // Directory does exist:
        stConfigParamsFile = stConfigParamsFilePath + "/" + stConfigParamsFile;
    }
    FILE * fileHandle;
    auto errNo = fopen_s(&fileHandle, stConfigParamsFile.c_str(), "at+");
    if (errNo == 0)
    {
        char szCurTime[100];
        tm curTime;
        time_t lTime;
        _time64(&lTime);
        // Init tm to current time:
        errNo = _localtime64_s(&curTime, &lTime);
        if (errNo == 0)
        {
            strftime(szCurTime, sizeof(szCurTime) - 1, "%Y-%m-%d %H:%M:%S", &curTime);
            stConfigParams = szCurTime + string("\n") + stConfigParams;
        }
        stConfigParams = "*******************************************\n" + stConfigParams;
        if (fwrite(stConfigParams.c_str(), sizeof(char), stConfigParams.length(), fileHandle) != stConfigParams.length())
        {
            std::cout << __FUNCTION__ << "fwrite failed.\n" << stConfigParams.c_str();
        }
        fclose(fileHandle);
    }
    else
    {
        string stMsg = string(__FUNCTION__) + ". Cannot open LOG4CXX config parameters log file: \n" + stConfigParamsFile;
        throw exception(stMsg.c_str());
    }
}

void PDLog4Cxx::Configure(string stAppName)
{
	if (_curAppName.size() != 0)
	{
		// We do not use multiple log destinations for the same process. Quit:
		return;
	}

	_curAppName = stAppName;

	atexit(DeleteLog4Powel);

    auto recordConfig = getenv("RECORD_LO4CXX_CONFIG") != nullptr;

	string stENVAppName("CUR_APPNAME=");
	stENVAppName += stAppName;
		
	// _putenv must be called before DOMConfigurator::configure. It sets run time version of env. variable for the current process:
	_putenv(stENVAppName.c_str()); 

	string stConfigParams = "";

    if (recordConfig)
    {
        stringstream ss;
        ss << stENVAppName << "\n";
        // << "_ghMutex = " << _ghMutex << "\n";
        stConfigParams += ss.str();
    }

	// In TSS service context ICC_HOME is copied from the caller (client) value.
	// This makes no sense on the server where TSS is installed.
	////**********
	//// Possible variant is to fetch ICC_HOME from registry (?)
	//// but so far it is not clear
	////	a. Can it be stored there
	////	b. What should be Powel policy rhgarding it:
	//HKEY hKey;
	//auto lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Perl", 0, KEY_READ, &hKey);
	//bool bExistsAndSuccess(lRes == ERROR_SUCCESS);
	//std::wstring strValueName;
	//std::wstring strValue;
	//std::wstring strDefaultValue = L"c:/Powel/icc/bin/log4powel.xml";
	//strValue = strDefaultValue;
	//WCHAR szBuffer[512];
	//DWORD dwBufferSize = sizeof(szBuffer);
	//ULONG nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	//if (ERROR_SUCCESS == nError)
	//{
	//	strValue = szBuffer;
	//}

	////**********

	string stLog4cxxConfigFile(getenv("ICC_HOME") == nullptr ? "c:/Powel/icc" : getenv("ICC_HOME")); 
	if (stLog4cxxConfigFile.length() > 0)
	{
		string stDirSeparator = "\\";
		stLog4cxxConfigFile += stDirSeparator + "bin" + stDirSeparator;
	}
	// We use only DOMConfigurator, not obsolete PropertyConfigurator:
	stLog4cxxConfigFile += "log4powel.xml";

	if (recordConfig)
		stConfigParams += "Log4cxxConfigFile:\n" + stLog4cxxConfigFile + "\n";

	struct _stat statbuffer;
	if (_stat(stLog4cxxConfigFile.c_str(), &statbuffer) != 0)
	{
		// File/directory does not exist. Fall back to default:
		stLog4cxxConfigFile = "c:/Powel/icc/bin/log4powel.xml"; 
		if (recordConfig)
			stConfigParams += "The file does not exist. Stipulated Log4cxxConfigFile:\n" + stLog4cxxConfigFile + "\n";
	}

    if (_stat(stLog4cxxConfigFile.c_str(), &statbuffer) == 0)
    {
        // Do not open cout statements in production - they spoil test logs:
        //std::cout 
        //	<< std::endl << "stLog4cxxConfigFile = "
        //	<< std::endl << stLog4cxxConfigFile << std::endl << "exists"
        //	<< std::endl << "Calling DOMConfigurator::configureAndWatch" 
        //	<< std::endl;
        DOMConfigurator::configure(stLog4cxxConfigFile.c_str());
        if (recordConfig)
            stConfigParams += "DOMConfigurator::configure was called\n";
    }
    else
	{
        // Do not open cout statements in production - they spoil test logs:
		//std::cout 
		//	<< std::endl << "stLog4cxxConfigFile = "
		//	<< std::endl << stLog4cxxConfigFile << std::endl << "DOES NOT EXIST"
		//	<< std::endl << "BasicConfigurator::configure" 
		//	<< std::endl;
		LoggerPtr rootLogger = Logger::getRootLogger();
		auto appenders = rootLogger->getAllAppenders();
		if (recordConfig)
			stConfigParams += "The stipulated file does not exist. ";	
		if (appenders.size() == 0)
		{
			BasicConfigurator::configure();
			if (recordConfig)
				stConfigParams += "BasicConfigurator::configure was called\n";
		}
		else if (recordConfig)
		{
			auto appender = appenders[0];
			auto appenderInfo = /*(*appender)->getName() + L"; " + */(appender)->getClass().getName();
			auto newSize = appenderInfo.length() * 2 + 2;
			size_t convertedChars = 0;
			char * szAppenderInfo = new char[newSize];
			wcstombs_s(&convertedChars, szAppenderInfo, newSize, appenderInfo.c_str(), _TRUNCATE);
			stConfigParams += string(szAppenderInfo) + string(" is used\n");
		}
		string stLevel = 
			// "ERROR" // Level::ERROR_INT
			"OFF" // Level::OFF_INT
			;
			
		Level::OFF_INT;
        LevelPtr level(Level::toLevel(stLevel));
		if (rootLogger != nullptr && level != nullptr)
		{
			rootLogger->setLevel(level);
			rootLogger->getLevel()->toInt();
			if (recordConfig)
				stConfigParams += "LOG4CXX level is set to '" + stLevel + "'\n";
			// Do not open cout statements in production - they spoil test logs:
			//std::cout 
			//	<< std::endl << "rootLogger->getLevel()->toInt() = " << rootLogger->getLevel()->toInt()
			//	<< "(" << rootLogger->getLevel()->toString().c_str() << ")"
			//	<< std::endl;
		}
		else
		{
			const auto szMsg = "Cannot create root logger for log4cxx BasicConfigurator\n";
			if (recordConfig)
			{
				stConfigParams += szMsg;
				RecordConfig(stAppName, stConfigParams);
			}
			throw exception(szMsg);
		}
    }

	LoggerPtr rootLogger = Logger::getRootLogger();
	if (rootLogger != nullptr) 
	{
		// setApplication(stAppName.c_str());
		LOG4CXX_INFO(rootLogger, "Entering application: " << stAppName);
		if (recordConfig)
		{
			//stConfigParams += L"rootLogger->getLevel() = " + rootLogger->getLevel()->toString();
			char szLevel[100];
			auto level = rootLogger->getLevel()->toInt();
			string stLevel = "UNKNOWN";
			switch (level)
			{
				case Level::OFF_INT:
					stLevel = "OFF";
					break;
                case Level::FATAL_INT:
				    stLevel = "FATAL";
				    break;
                case Level::ERROR_INT:
					stLevel = "ERROR";
					break;
                case Level::WARN_INT:
					stLevel = "WARN";
					break;
                case Level::INFO_INT:
					stLevel = "INFO";
					break;
                case Level::DEBUG_INT:
					stLevel = "DEBUG";
					break;
                case Level::TRACE_INT:
					stLevel = "TRACE";
					break;
                case Level::ALL_INT:
					stLevel = "ALL";
					break;
				default:
					break;
			}
			sprintf_s(szLevel, sizeof(szLevel) - 1, "rootLogger->getLevel() = %d, %s\n", level, stLevel.c_str());
			stConfigParams += szLevel;
			stConfigParams += "Appenders:\n";
			auto appenders = rootLogger->getAllAppenders();
            // In VS2013 usage of std::[c]begin/[c]end causes unresolved external refs in dependent projects, e.g. TimeSeriesReport:
			//for (auto appender = appenders.cbegin(); appender != appenders.cend(); appender++)
            for (auto i = 0; i < (int)appenders.size(); i++)
            {
				//auto appenderInfo = (*appender)->getName() + L"; " + (*appender)->getClass().getName();
                auto appenderInfo = appenders[i]->getName() + L"; " + appenders[i]->getClass().getName();
                auto newSize = appenderInfo.length() * 2 + 2;
				size_t convertedChars = 0;
				char * szAppenderInfo = new char[newSize];
				wcstombs_s(&convertedChars, szAppenderInfo, newSize, appenderInfo.c_str(), _TRUNCATE);
				stConfigParams += string("\t") + string(szAppenderInfo) + string("\n");
			}

			if (recordConfig)
				RecordConfig(stAppName, stConfigParams);

			LOG4CXX_INFO(rootLogger, "stConfigParams:\n" << stConfigParams);
		}
		// Do not open cout statements in production - they spoil test logs:
		//if (rootLogger->getLevel() != nullptr)
		//{
		//	std::cout 
		//		<< std::endl << "rootLogger->getLevel()->toInt() = " << rootLogger->getLevel()->toInt()
		//		<< "(" << rootLogger->getLevel()->toString().c_str() << ")"
		//		<< std::endl;
		//}
	}
	else
	{
		const auto szMsg = "Cannot get log4cxx rootLogger\n";
		if (recordConfig)
		{
			stConfigParams += szMsg;
			RecordConfig(stAppName, stConfigParams);
		}
		throw exception(szMsg);
	}

	// Following exception is for debugging only. It is to avoid complex set up in case of 
	// debugging accross CLI/native boundaries:
	// throw exception(stLog4cxxConfigFile.c_str());
}

