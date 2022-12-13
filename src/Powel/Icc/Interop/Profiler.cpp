#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>  
#include <crtdbg.h>  
#include <map>  

#include <PDLog4Cxx.h> // Include early to avoid C4251.
#include "Profiler.h"
//TOSA #include "Mutex.h"
//TOSA #include "Lock.h"
//TOSA #include "PDTime.h"

using namespace std;

//TOSA Powel::MultiThreading::Mutex profilerMutex;
map<string, map<const char *, Profile>> Profiler::m_profiles;
bool Profiler::m_bCurProfilesNotReported = true;
stack<map<string, map<const char *, Profile>>> Profiler::m_notReportedProfiles;

// Profiling is off by default:
Profiler::Level Profiler::m_nMinLevel = Profiler::LEVEL_OFF;
string Profiler::m_stStartMessage = "";
time_t Profiler::m_nStartTime = 0;
map<const char *, Profile> * Profiler::m_curGroup = nullptr;
Profile * Profiler::m_curProfile = nullptr;
string Profiler::m_stPrevGroup = "";
const char * Profiler::m_szEntry = "Entry";
const char * Profiler::m_szExit = "Exit";
const char * Profiler::m_szDefaultGroup = "ProfilerDefaultGroup";

static log4cxx::LoggerPtr _l4n(log4cxx::Logger::getLogger("Powel.Icc.LibUtil.Profiler"));

// static char szMsg[40000];
static const int nGroupWidth = 60;
static const int nProfileTagWidth = 50;
static const int nCountWidth = 8;
static const int nElapsedWidth = 8;
static const int statsWidth = 12;
static const int pdTimesWidth = 52;
static const int nMaxWidth = 12;
static const int lineWidth = 
	nGroupWidth + 
	nProfileTagWidth + 
	nElapsedWidth + 
	nCountWidth + 
	nElapsedWidth + 
    statsWidth +
    pdTimesWidth +
	nMaxWidth + 
	40
	;
static char szLine[lineWidth];
static const char cProfilerTag = '¨';
static const char * szDivider = 
	"-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

void Profiler::Log(const char * szGroup, const char * szProfileTag, Level nLevel)
{
	/*TOSA if (nLevel < m_nMinLevel || szGroup == nullptr || szProfileTag == nullptr)
		return;

	time_t nFinishTime = clock();
	
	Powel::MultiThreading::Lock scopeLock(profilerMutex);
	
	if (m_nStartTime == 0)
		m_nStartTime = nFinishTime;

	time_t nTimeElapsed = nFinishTime - m_nStartTime;

	if (m_curProfile != 0)
	{
		m_curProfile->m_nElapsed += nTimeElapsed;
		if (m_curProfile->maxElapsed_ < nTimeElapsed)
			m_curProfile->maxElapsed_ = nTimeElapsed;
	}
		
	// if (m_profiles[szProfileTag] == 0)
	//	m_profiles[szProfileTag] == new Profile();

	if (m_stPrevGroup != string(szGroup))
	{
		m_stPrevGroup = string(szGroup);
		m_curGroup = &(m_profiles[m_stPrevGroup]);
	}

	m_curProfile = &((*m_curGroup)[szProfileTag]);
		
	m_curProfile->m_nCount++;
	// If we do not count for overhead caused by this method between nFinishTime = clock() and next statement
	// then it may cause smaller inconsistencies between total elapsed time and sum of all profiles elapsed time values.
	// On the other hand setting m_nStartTime = nFinishTime may result in slightly inaccurate profiling:
	m_nStartTime = nFinishTime;
	// m_nStartTime = clock();*/
}

Profile & Profiler::GetProfile(const char * szGroup, const char * szProfileTag)
{
  //if (m_nMinLevel == LEVEL_OFF)
  //{
  //  // Throw exception:
  //  char szMsg[1000];
  //  sprintf_s(szMsg, sizeof(szMsg), "%s. The method cannot be called when profiling is disabled", __FUNCTION__);
  //  throw exception(szMsg);
  //}

	/*TOSA Powel::MultiThreading::Lock scopeLock(profilerMutex);

	LOG4CXX_TRACE_ENTRY(_l4n, 
		"\n\tm_profiles.size() = " << m_profiles.size() <<
		"\n\tszGroup = " << szGroup <<
		"\n\tszProfileTag = " << szProfileTag <<
		"");

	map<string, map<const char *, Profile>>::iterator git = m_profiles.find(szGroup);
	int nProfileCountLimit = 100;*/
	Profile * profile = NULL;
	/*TOSA bool bDummyProfile = false;

	if (git != m_profiles.end()) 
	{
		LOG4CXX_TRACE(_l4n, "\n\tGroup " << szGroup << " is found");

		// We cannot search in Profile map by providing literal string. The map contains addresses of respective
		// strings, not their values. Hence we use plain comparison loop:
		// map<const char *, Profile>::iterator pit = git->second.find(szProfileTag);
		for (map<const char *, Profile>::iterator pit = git->second.begin(); pit != git->second.end(); ++pit) 
		{
			if (strcmp(pit->first, szProfileTag) == 0)
			{
				profile = &(pit->second);
				break;
			}
		}
	}
	if (profile == nullptr)
	{
		profile = new Profile();
		profile->m_nCount = (m_profiles.size() != 0 ? 0 : -1);
		profile->m_nElapsed = 0;
		profile->minPdTime_ = MAXPDTIME;
		profile->maxPdTime_ = 0;
		profile->maxElapsed_ = 0;
		bDummyProfile = true;
		LOG4CXX_TRACE(_l4n, "\n\tProfile " << szProfileTag << " is not found. Empty profile is returned");
	}
	
	if (abs(profile->m_nCount) > nProfileCountLimit)
	{
		// Throw debug exception:
		char szMsg[1000];
		sprintf_s(szMsg, sizeof(szMsg), "%s. Debug exception: profile %s count %d is too large (abs > %d)"
			, __FUNCTION__, (bDummyProfile ? "(dummy)" : szProfileTag), profile->m_nCount, nProfileCountLimit);
		throw exception(szMsg);
	}

	LOG4CXX_TRACE_EXIT(_l4n,
		"\n\tprofile->m_nCount = " << profile->m_nCount <<
		"; m_nElapsed = " << profile->m_nElapsed <<
		"");*/

	return *profile;
}

void Profiler::AddStatistics(const size_t stats)
{
    /*TOSA if (m_nMinLevel == LEVEL_OFF || stats <= 0)
        return;
    
    Powel::MultiThreading::Lock scopeLock(profilerMutex);

    if (m_curProfile != nullptr)
        m_curProfile->stats1_ += stats;*/
}

void Profiler::SetMinPdTime(const time_t time)
{
    /*TOSA if (m_nMinLevel == LEVEL_OFF)
        return;

    Powel::MultiThreading::Lock scopeLock(profilerMutex);

    if (m_curProfile != nullptr && m_curProfile->minPdTime_ > time)
        m_curProfile->minPdTime_ = time;*/
}

void Profiler::SetMaxPdTime(const time_t time)
{
    /*TOSA if (m_nMinLevel == LEVEL_OFF)
        return;

    Powel::MultiThreading::Lock scopeLock(profilerMutex);

    if (m_curProfile != nullptr && m_curProfile->maxPdTime_ < time)
        m_curProfile->maxPdTime_ = time;*/
}

time_t Profiler::ReportProfiling(bool bUseCounter)
{
    /*TOSA LOG4CXX_TRACE_ENTRY(_l4n, "\n\tnm_nMinLevel = " << m_nMinLevel);

    if (m_nMinLevel == LEVEL_OFF)
        return 0;

    Powel::MultiThreading::Lock scopeLock(profilerMutex);
    long nGroupCount = m_profiles.size();
    LOG4CXX_TRACE(_l4n, "\n\tnGroupCount = m_profiles.size() = " << nGroupCount);

    if (nGroupCount == 0)
    {
        LOG4CXX_TRACE_EXIT(_l4n, "No info for profiling report");
        return 0;
    }

    time_t nElapsed = 0;
    time_t nTotalElapsed = 0;
    time_t nTotalUnaccounted = 0;*/
    time_t nTotal = 0;
    /*TOSA size_t nProfileCount = 0;
    map<string, Profile> mapGroups;
    multimap<time_t, string> mapGroupsByTime;
    char szGroup[nGroupWidth + 1];
    char szProfileTag[nProfileTagWidth + 1];

    szGroup[nGroupWidth] = '\0';
    szProfileTag[nProfileTagWidth] = '\0';

    for (auto & git = m_profiles.cbegin(); git != m_profiles.cend(); ++git)
    {
        nProfileCount += git->second.size();
        time_t nGroupTotalElapsed = 0;
        time_t nGroupTotalUnaccounted = 0;
        size_t groupStats1 = 0;
        for (auto & pit = git->second.cbegin(); pit != git->second.cend(); ++pit)
        {
            if (pit->first != m_szExit)
            {
                nGroupTotalElapsed += pit->second.m_nElapsed;
                groupStats1 += pit->second.stats1_;
            }
            else
            {
                nGroupTotalUnaccounted += pit->second.m_nElapsed;
            }
        }
        nTotalElapsed += nGroupTotalElapsed;
        nTotalUnaccounted += nGroupTotalUnaccounted;
        mapGroups[git->first].m_nElapsed = nGroupTotalElapsed;
        mapGroups[git->first].stats1_ = groupStats1;
        LOG4CXX_TRACE(_l4n, "\n\tmapGroups[" << git->first << "] set to nGroupTotalElapsed = " << nGroupTotalElapsed);
        mapGroupsByTime.emplace(make_pair(nGroupTotalElapsed, git->first));
    }

    nTotal = nTotalElapsed + nTotalUnaccounted;

    size_t nMaxMsgSize = (nGroupCount + nProfileCount + 30) * lineWidth;
    LOG4CXX_TRACE(_l4n,
        "\n\tnLineWidth = " << lineWidth <<
        "\n\tnMaxMsgSize = " << nMaxMsgSize
    );
    char * szMsg = (char *)_malloca(nMaxMsgSize);

    char const * szDividerFormat = "\n%s";
    char const * szSummaryHeaderFormat =
        "\n\tTotal elapsed: %u\n\t"
        "Total unaccounted (after exit): %u\n\t"
        "Total elapsed + unaccounted: %u\n\t"
        "Total groups: %u\n\t"
        "Total profiles: %u\n"
        "%s"
        "\n%-60s|%10s |%10s |%10s |%10s |\n%s        %c"
        ;
    char const * szSummaryLineFormat = "\n%-60s|%10u |%10u |%10.1f |%10.1f |";
    char const * szDetailHeaderFormat = "\n%-60s %-50s |%8s |%8s |%10s |%-52s |%12s |";
    char const * szDetailLineFormat = "\n%-60s %-50s |%8u |%8u |%10u |%-52s |%12u |";

    sprintf_s(szMsg
        , nMaxMsgSize
        , szSummaryHeaderFormat
        , long(nTotalElapsed)
        , long(nTotalUnaccounted)
        , long(nTotal)
        , long(nGroupCount)
        , long(nProfileCount)
        , szDivider
        , "Profile group"
        , "Elapsed"
        , "Stats"
        , "% (elaps)"
        , "% (total)"
        , szDivider
        , cProfilerTag
    );

    for (auto tit = mapGroupsByTime.crbegin(); tit != mapGroupsByTime.crend(); ++tit)
    {
        auto git = mapGroups.find(tit->second);
        if (git == mapGroups.end())
            continue;

        nElapsed = git->second.m_nElapsed;
        const auto stats1 = git->second.stats1_;
        auto elapsedMsec = floor((double)nElapsed * 1000.0 / (double)CLOCKS_PER_SEC);
        sprintf_s(szLine
            , lineWidth
            , szSummaryLineFormat
            , TruncateGroup(git->first, nGroupWidth).c_str()
            , long(elapsedMsec)
            , long(stats1)
            , double(nElapsed) * 100 / double(nTotalElapsed == 0 ? 1 : nTotalElapsed) // % to elapsed
            , double(nElapsed) * 100 / double(nTotal == 0 ? 1 : nTotal) // % to total
        );
        // LOG4CXX_TRACE(_l4n, "szLine(1): " << szLine);
        strcat_s(szMsg, nMaxMsgSize, szLine);
    }
    auto totalUnaccountedMsec = floor((double)nTotalUnaccounted * 1000.0 / (double)CLOCKS_PER_SEC);
    sprintf_s(szLine
        , lineWidth
        , szSummaryLineFormat
        , "Unaccounted"
        , long(totalUnaccountedMsec)
        , 0 // Stats
        , double(nTotalUnaccounted) * 100 / double(nTotalElapsed == 0 ? 1 : nTotalElapsed) // % to elapsed
        , double(nTotalUnaccounted) * 100 / double(nTotal == 0 ? 1 : nTotal) // % to total
    );
    // LOG4CXX_TRACE(_l4n, "szLine(2): " << szLine);
    strcat_s(szMsg, nMaxMsgSize, szLine);

    sprintf_s(szLine
        , lineWidth
        , szDividerFormat
        , szDivider
    );
    // LOG4CXX_TRACE(_l4n, "szLine(3): " << szLine);
    strcat_s(szMsg, nMaxMsgSize, szLine);

    strcat_s(szMsg, nMaxMsgSize, "\n\nDetails:");// (average is per execution, not per count) : ");

    sprintf_s(szLine
        , lineWidth
        , szDividerFormat
        , szDivider
    );
    // LOG4CXX_TRACE(_l4n, "szLine(4): " << szLine);
    strcat_s(szMsg, nMaxMsgSize, szLine);

    sprintf_s(szLine
        , lineWidth
        , szDetailHeaderFormat
        , "Profile group"
        , "Profile tag"
        , "Count"
        , "Elapsed"
        , "Stats"
        , "Min/max PDTime"
        , "Max elapsed"
    );
    // LOG4CXX_TRACE(_l4n, "szLine(5): " << szLine);
    strcat_s(szMsg, nMaxMsgSize, szLine);

    sprintf_s(szLine
        , lineWidth
        , szDividerFormat
        , szDivider
    );
    // LOG4CXX_TRACE(_l4n, "szLine(6): " << szLine);
    strcat_s(szMsg, nMaxMsgSize, szLine);

    multimap<time_t, string> mapExits;
    size_t nTotalExitCount = 0;
    for (auto tit = mapGroupsByTime.crbegin(); tit != mapGroupsByTime.crend(); ++tit)
    {
        auto git = m_profiles.find(tit->second);
        if (git == m_profiles.end())
            continue;

        for (auto pit = git->second.cbegin(); pit != git->second.cend(); ++pit)
        {
            nElapsed = pit->second.m_nElapsed;
            if (nElapsed != 0)
            {
                strncpy_s(szProfileTag, sizeof(szProfileTag), pit->first, nProfileTagWidth);
                stringstream ssPdTimes;
                if (pit->second.minPdTime_ < MAXPDTIME)
                    ssPdTimes << PDTime(pit->second.minPdTime_) << " ";
                if (pit->second.maxPdTime_ > 0)
                    ssPdTimes << PDTime(pit->second.maxPdTime_);
                //LOG4CXX_TRACE(_l4n, "\n\tssPdTimes.str()" << ssPdTimes.str());

                auto elapsedMsec = floor((double)nElapsed * 1000.0 / (double)CLOCKS_PER_SEC);
                sprintf_s(szLine
                    , lineWidth
                    , szDetailLineFormat
                    , TruncateGroup(git->first, nGroupWidth).c_str()
                    , szProfileTag
                    , long(pit->second.m_nCount)
                    , long(elapsedMsec)
                    , long(pit->second.stats1_)
                    , ssPdTimes.str().c_str()
                    , long(floor((double)(pit->second.maxElapsed_) * 1000.0 / (double)CLOCKS_PER_SEC))
                );
                //LOG4CXX_TRACE(_l4n, "\n\tszLine(7): " << szLine);
                if (pit->first != m_szExit)
                {
                    strcat_s(szMsg, nMaxMsgSize, szLine);
                }
                else
                {
                    mapExits.emplace(make_pair(nElapsed, szLine));
                    nTotalExitCount += pit->second.m_nCount;
                }
            }
        }
    }

    if (mapExits.size() != 0)
    {
        sprintf_s(szLine
            , lineWidth
            , szDividerFormat
            , szDivider
        );
        strcat_s(szMsg, nMaxMsgSize, szLine);
        const char * szUnaccounted = "Unaccounted";
        const char * szUnaccountedDescr = "(after exit)";
        auto totalUnaccountedMsec = floor((double)nTotalUnaccounted * 1000.0 / (double)CLOCKS_PER_SEC);
        sprintf_s(szLine
            , lineWidth
            , szDetailLineFormat
            , szUnaccounted
            , szUnaccountedDescr
            , nTotalExitCount
            , long(totalUnaccountedMsec)
            , 0 // Stats
            , "" // PDTimes
            , 0
        );
        strcat_s(szMsg, nMaxMsgSize, szLine);
        // LOG(_logger, trace, "szLine(8): " << szLine);
        for (auto ex = mapExits.crbegin(); ex != mapExits.crend(); ++ex)
        {
            strcat_s(szMsg, nMaxMsgSize, ex->second.c_str());
        }
    }

    sprintf_s(szLine
        , lineWidth
        , szDividerFormat
        , szDivider
    );
    // LOG4CXX_TRACE(_l4n, "szLine(8): " << szLine);
    strcat_s(szMsg, nMaxMsgSize, szLine);

    m_nStartTime = 0;

    // We log profiling report unconditionally, whatever _l4n level is:
    LOG4CXX_FATAL(_l4n, "\n" << m_stStartMessage << szMsg);

    _freea(szMsg);

    LOG4CXX_TRACE_EXIT(_l4n, "nTotal = " << nTotal);*/
    
    return nTotal;
}

void StopProfilingAtExit()
{
	/*TOSA string stProfilerStopMsg = string(__FUNCTION__);
	// True - use execution count to calculate averages:
	Profiler::StopProfiling(stProfilerStopMsg, true, false);*/
}

void Profiler::StartProfiling(string stStartMessage, Level nMinLevel)
{
	/*TOSA Powel::MultiThreading::Lock scopeLock(profilerMutex);

	LOG4CXX_TRACE_ENTRY(_l4n, 
		"\n\tstStartMessage = " << stStartMessage <<
		"\n\tnMinLevel = " << nMinLevel <<
		"\n\tm_profiles.size() = " << m_profiles.size() <<
		"");

	if (m_profiles.size() != 0 && m_bCurProfilesNotReported)
	{
		// Save current profiling info to be reported later:
		m_notReportedProfiles.push(m_profiles);
		LOG4CXX_TRACE(_l4n, 
			"\n\tPrevious profiling session is stored. m_notReportedProfiles.size() = " << m_notReportedProfiles.size() <<
			"");
	}

	if (nMinLevel == LEVEL_UNDEFINED)
	{
		//if (_l4n->isTraceEnabled())
		//	m_nMinLevel = LEVEL_TRACE;
		//else if (_l4n->isDebugEnabled())
		//	m_nMinLevel = LEVEL_DEBUG;
		//else if (_l4n->isWarnEnabled())
		//	m_nMinLevel = LEVEL_WARN;
		//else if (_l4n->isInfoEnabled())
		//	m_nMinLevel = LEVEL_INFO;
		//else if (_l4n->isErrorEnabled())
		//	m_nMinLevel = LEVEL_ERROR;
		//else if (_l4n->isFatalEnabled())
		//	m_nMinLevel = LEVEL_FATAL;
		m_nMinLevel = LEVEL_OFF;
	}
	else if (!(_l4n->isWarnEnabled()))
	{
		// Disable profiling if log4cxx level is in {ERROR, FATAL, OFF}:
		m_nMinLevel = LEVEL_OFF;
	}
	else
	{
		m_nMinLevel = nMinLevel;
	}

	//LOG4CXX_FATAL(_l4n, 
	//	"\n" << __FUNCTION__ <<
	//	"\n\tstStartMessage = " << stStartMessage << 
	//	"\n\tnMinLevel = " << nMinLevel << 
	//	"\n\tm_nMinLevel = " << m_nMinLevel << 
	//	cProfilerTag
	//	);

	m_profiles.clear();
	m_stStartMessage = stStartMessage;
	m_curProfile = 0;
	m_stPrevGroup = "";

	atexit(StopProfilingAtExit);

	LOG4CXX_TRACE_EXIT(_l4n, "m_nMinLevel = " << m_nMinLevel);

	m_nStartTime = clock();*/

}

time_t Profiler::StopProfiling(string stStopMessage, bool bUseCounter, bool bKeepProfileInfo)
{
	/*TOSA LOG4CXX_TRACE_ENTRY(_l4n,
		"\n\tm_nMinLevel = " << m_nMinLevel <<
		"\n\tstStopMessage = " << stStopMessage <<
		"\n\tbUseCounter = " << (int)bUseCounter <<
		"\n\tbKeepProfileInfo = " << (int)bKeepProfileInfo <<
		"\n\tm_profiles.size() = " << m_profiles.size() <<
		"\n\tm_notReportedProfiles.size() = " << m_notReportedProfiles.size() <<
		"");

	if (m_nMinLevel == LEVEL_OFF)
		return 0;
	
	Powel::MultiThreading::Lock scopeLock(profilerMutex);

    m_stStartMessage = m_stStartMessage.empty() ? stStopMessage : m_stStartMessage + "\n" + stStopMessage;*/
	auto nTotal = ReportProfiling(bUseCounter);/*TOSA
	// We log profiling report unconditionally, whatever _l4n level is:

	if (m_notReportedProfiles.empty() || !bKeepProfileInfo)
	{
		LOG4CXX_TRACE(_l4n, 
			"\n\tm_notReportedProfiles.empty() = " << (int)m_notReportedProfiles.empty() << 
			"\n\tbKeepProfileInfo = " << (int)bKeepProfileInfo <<
			"");
		//m_nMinLevel = LEVEL_OFF;
		// We need profiling info in case it will be calls to GetProfile for automatic profile analysis:
		if (!bKeepProfileInfo)
		{
			//LOG4CXX_TRACE(_l4n, "\n\tPrior m_profiles.clear()");
			m_profiles.clear();
			auto notReportedCount = m_notReportedProfiles.size();
			//LOG4CXX_TRACE(_l4n, "\n\tnotReportedCount = " << notReportedCount);
			while (!m_notReportedProfiles.empty())
			{
				m_profiles = m_notReportedProfiles.top();
				m_notReportedProfiles.pop();	
				m_profiles.clear();
			}
			LOG4CXX_TRACE(_l4n, "\n\tm_profiles and " << notReportedCount << " m_notReportedProfiles cleared");
		}
		m_bCurProfilesNotReported = false;
		m_stStartMessage = "";
		m_curProfile = 0;
		m_stPrevGroup = "";
	}
	else
	{
		// Restore last not reported profiles:
		m_profiles = m_notReportedProfiles.top();
		m_bCurProfilesNotReported = true;
		///TODO: extend m_notReportedProfiles to store m_nMinLevel, m_stStartMessage, m_curProfile, m_stPrevGroup so that we can restore them here:
		// Inherit m_nMinLevel from the profiling being stopped:
		// m_nMinLevel = LEVEL_FATAL;
		m_stStartMessage = "";
		m_curProfile = 0;
		m_stPrevGroup = "";
		m_notReportedProfiles.pop();	
		LOG4CXX_TRACE(_l4n, 
			"\n\tPrevious profiling session is restored. m_profiles.size() = " << m_profiles.size() << 
			"\n\tm_notReportedProfiles.size() = " << m_notReportedProfiles.size() <<
			"");
	}
	
	m_nStartTime = 0;

	LOG4CXX_TRACE_EXIT(_l4n,
		"\tm_nMinLevel = " << m_nMinLevel <<
		"\t\nnTotal = " << nTotal <<
		"");*/

	return nTotal;
}
