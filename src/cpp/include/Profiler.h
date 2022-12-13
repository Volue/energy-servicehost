//TOSA #pragma once
#include <xstring>
#include <ctime>
#include <map>
#include <stack>

class Profile
{
friend class Profiler;
public:
	Profile(void) : 
          m_nElapsed(0), m_nCount(0), minPdTime_(0x7FFFFFFFFFFFFFFFL) //MAXPDTIME. We do not include PDTime.h to avoid circular references
        , maxPdTime_(0), maxElapsed_(0), stats1_(0)
    { }
		
	~Profile(void) {}

	time_t Elapsed() { return m_nElapsed; }
	int Count() { return (int) m_nCount; }

protected:
	time_t m_nElapsed;
	long m_nCount;
	time_t maxElapsed_;
    size_t stats1_; //Arbitrary staistics counter, e.g. number of loop iterations, that can be accumulated by call to AddStatistics
    time_t minPdTime_;
    time_t maxPdTime_;
};


class Profiler
{
public:
    typedef enum
    {
        LEVEL_UNDEFINED = -1
        , LEVEL_TRACE = 0
        , LEVEL_DEBUG = 1
        , LEVEL_INFO = 2
        , LEVEL_WARN = 3
        , LEVEL_ERROR = 4
        , LEVEL_FATAL = 5
        , LEVEL_OFF = 100
    } Level;

    Profiler(void)
    {
        m_szCurGroup = m_szDefaultGroup;
    }

    Profiler(const char * szGroup, bool bStartProfiling = false, Profiler::Level nLevel = Profiler::Level::LEVEL_TRACE)
    {
        m_szCurGroup = szGroup;
        Profiler::LogEntry(m_szCurGroup, bStartProfiling, nLevel);
    }

    void Log(const char * szProfileTag, Profiler::Level nLevel = Profiler::Level::LEVEL_TRACE)
    {
        Profiler::Log(m_szCurGroup, szProfileTag, nLevel);
    }

    ~Profiler(void)
    {
        Profiler::LogExit(m_szCurGroup);
        // If we use m_szDefaultGroup here then it results in unresolved ext. reference.
        // To avoid that we have to move it to *.cpp file, but we do not care for the time being:
        // m_szCurGroup = m_szDefaultGroup;
    }

    static Profile & GetProfile(const char * szGroup, const char * szProfileTag);

    static void Log(const char * szGroup, const char * szProfileTag, Level nLevel = LEVEL_TRACE);

    static void LogEntry(const char * szGroup, bool bStartProfiling = false, Level nLevel = LEVEL_TRACE)
    {
        // StartProfiling must be called before (nLevel < m_nMinLevel) check because StopProfiling resets m_nMinLevel to LEVEL_EMERGENCY,
        // and StartProfiling sets it to nLevel:
        if (bStartProfiling)
        {
            StopProfiling(szGroup, true);
            StartProfiling(szGroup, nLevel);
        }

        if (nLevel < m_nMinLevel || szGroup == 0)
            return;
        Log(szGroup, m_szEntry);
    }

    static void LogExit(const char * szGroup, bool bStopProfiling = false, Level nLevel = LEVEL_TRACE)
    {
        if (nLevel < m_nMinLevel || szGroup == 0)
            return;
        //TOSA Log(szGroup, m_szExit);
        if (bStopProfiling)
            StopProfiling(szGroup, true);
    }

    static void AddStatistics(const size_t stats);

    static void SetMinPdTime(const time_t time);

    static void SetMaxPdTime(const time_t time);

    static time_t ReportProfiling(bool bUseCounter = false);

    static void StartProfiling(std::string stStartMessage, Level nMinLevel = LEVEL_UNDEFINED);

    static time_t StopProfiling(std::string stStopMessage, bool bUseCounter = false, bool bKeepProfileInfo = true);

    static void SetMinLevel(Level nMinLevel = LEVEL_OFF)
    {
        m_nMinLevel = nMinLevel;
    }

protected:
    static inline std::string TruncateGroup(std::string stGroup, std::string::size_type nGroupWidth)
    {
        size_t nNextColonPos;
        size_t nAfterPrevColonPos = 0;
        size_t nAfterLastColonPos = 0;
        int i = 0;
        size_t nSignatureBeginPos = stGroup.find("__cdecl ", 0);
        if (nSignatureBeginPos != std::string::npos)
            stGroup = stGroup.substr(nSignatureBeginPos + 8);

        if ((int)stGroup.length() > nGroupWidth)
        {
            auto firstParenthesPos = stGroup.find("(");
            if (firstParenthesPos != std::string::npos && (int)firstParenthesPos < nGroupWidth)
                stGroup = stGroup.substr(0, nGroupWidth);
        }

        while ((nNextColonPos = stGroup.find("::", nAfterLastColonPos)) != std::string::npos)
        {
            // Ignore occurences of "std::"
            if (nNextColonPos < 3 || stGroup.find("std", nNextColonPos - 3) != 0)
            {
                i++;
            }

            nAfterPrevColonPos = nAfterLastColonPos;
            nAfterLastColonPos = nNextColonPos + 2;
            //if (stGroup.length() > nGroupWidth)
            //{
            //	LOG4CXX_FATAL(_l4n, 
            //		"\n\tstGroup = " << stGroup.c_str() <<
            //		"\n\t" << i <<
            //		". nAfterPrevColonPos = " << nAfterPrevColonPos <<
            //		". nAfterLastColonPos = " << nAfterLastColonPos <<
            //		". stGroup.substr(nAfterPrevColonPos).length() = " << stGroup.substr(nAfterPrevColonPos).length() <<
            //		"\n\tstGroup.substr(nAfterPrevColonPos) = " << stGroup.substr(nAfterPrevColonPos) <<
            //		"");
            //}
            if ((i >= 4) || ((int)stGroup.substr(nAfterPrevColonPos).length() <= nGroupWidth))
                break;
        }
        return stGroup.substr((int)stGroup.substr(nAfterPrevColonPos).length() <= nGroupWidth ? nAfterPrevColonPos : nAfterLastColonPos, nGroupWidth);
    }

    const char * m_szCurGroup;
    static const char * m_szDefaultGroup;

    static std::map<std::string, std::map<const char*, Profile>> m_profiles;
    static bool m_bCurProfilesNotReported;
    static std::stack<std::map<std::string, std::map<const char*, Profile>>> m_notReportedProfiles;
    static Level m_nMinLevel;
    static std::string m_stStartMessage;
    static time_t m_nStartTime;
    static std::string m_stPrevGroup;
    static const char * m_szEntry;
    static const char * m_szExit;
    static Profile * m_curProfile;
    static std::map<const char*, Profile>* m_curGroup;
    static int m_nCounter;
};
