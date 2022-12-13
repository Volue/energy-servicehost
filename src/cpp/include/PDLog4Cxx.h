//-*-Mode: C++;-*-
#ifndef _PDLog4Cxx_h_
#define _PDLog4Cxx_h_

#include <string>

#if __GNUG__ >= 2
#  pragma interface
#endif

/* Usage:
Initialization:
	#include <PDLog4Cxx.h>
	........
	int main(argc,argv)
	{
		try
		{
			// Read settings for log4cxx
			PDLog4Cxx::Configure(szAppName);
		}
		catch (exception ex)
		{
			// stMsg is just for debugging:
			string stMsg = ex.what();
		}
	}

Logging:
	#include <log4cxx/logger.h> // Include early to avoid C4251.
	using namespace log4cxx;
	static LoggerPtr _l4n_cb(Logger::getLogger("Powel.Icc.LibSolver.TsRModel.CallBacks"));
	LOG4CXX_TRACE_ENTRY(_l4n_cb, <entry message>);
	...........
	LOG4CXX_TRACE(_l4n_cb, <trace message>);
	...........
	LOG4CXX_TRACE_EXIT(_l4n_cb, <exit message>);

Compilation:
	-I../../../icc/api/log/src -I../../../Log4Powel/apache-log4cxx/src/main/include

Runtime:
	log4cxx.dll must be found
	log4powel.xml must be found in cwd/$ICC_HOME\bin
*/
#include <log4cxx/logger.h> // Include early to avoid C4251.

#if !defined(LOG4CXX_THRESHOLD) || LOG4CXX_THRESHOLD <= 10000

#define GET_LOGGER(name) log4cxx::Logger::getLogger(name)


/**
Logs a message to a specified logger with the DEBUG level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_DEBUG(logger, message) do { \
        if (LOG4CXX_UNLIKELY(logger->isDebugEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getDebug(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_DEBUG_ENTRY(logger, message) do { \
        if (LOG4CXX_UNLIKELY(logger->isDebugEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getDebug(), oss_.str(oss_ << "\n\n****** Entering ******\n" << __FUNCSIG__ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_DEBUG_EXIT(logger, message) do { \
        if (LOG4CXX_UNLIKELY(logger->isDebugEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getDebug(), oss_.str(oss_ << message << "\n" << __FUNCTION__ << ". Exit"), LOG4CXX_LOCATION); }} while (0)
#else
#define LOG4CXX_DEBUG(logger, message)
#define LOG4CXX_DEBUG_ENTRY(logger, message)
#define LOG4CXX_DEBUG_EXIT(logger, message)
#endif

#if !defined(LOG4CXX_THRESHOLD) || LOG4CXX_THRESHOLD <= 5000
/**
Logs a message to a specified logger with the TRACE level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_TRACE(logger, message) do { \
        if (LOG4CXX_UNLIKELY(logger->isTraceEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getTrace(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_TRACE_ENTRY(logger, message) do { \
        if (LOG4CXX_UNLIKELY(logger->isTraceEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getTrace(), oss_.str(oss_ << "\n\n****** Entering ******\n" << __FUNCSIG__ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_TRACE_EXIT(logger, message) do { \
        if (LOG4CXX_UNLIKELY(logger->isTraceEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getTrace(), oss_.str(oss_ << message << "\n" << __FUNCTION__ << ". Exit"), LOG4CXX_LOCATION); }} while (0)
#else
#define LOG4CXX_TRACE(logger, message)
#define LOG4CXX_TRACE_ENTRY(logger, message)
#define LOG4CXX_TRACE_EXIT(logger, message)
#endif

#if !defined(LOG4CXX_THRESHOLD) || LOG4CXX_THRESHOLD <= 20000
/**
Logs a message to a specified logger with the INFO level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_INFO(logger, message) do { \
        if (logger->isInfoEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getInfo(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_INFO_ENTRY(logger, message) do { \
        if (logger->isInfoEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getInfo(), oss_.str(oss_ << "\n\n****** Entering ******\n" << __FUNCSIG__ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_INFO_EXIT(logger, message) do { \
        if (logger->isInfoEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getInfo(), oss_.str(oss_ << message << "\n" << __FUNCTION__ << ". Exit"), LOG4CXX_LOCATION); }} while (0)
#else
#define LOG4CXX_INFO(logger, message)
#define LOG4CXX_INFO_ENTRY(logger, message)
#define LOG4CXX_INFO_EXIT(logger, message)
#endif

#if !defined(LOG4CXX_THRESHOLD) || LOG4CXX_THRESHOLD <= 30000
/**
Logs a message to a specified logger with the WARN level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_WARN(logger, message) do { \
        if (logger->isWarnEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getWarn(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_WARN_ENTRY(logger, message) do { \
        if (logger->isWarnEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getWarn(), oss_.str(oss_ << "\n\n****** Entering ******\n" << __FUNCSIG__ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_WARN_EXIT(logger, message) do { \
        if (logger->isWarnEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getWarn(), oss_.str(oss_ << message << "\n" << __FUNCTION__ << ". Exit"), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_SPECIAL(specialLog, logger, message) do { \
        if (specialLog && logger->isWarnEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getWarn(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }\
        else if (logger->isDebugEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getDebug(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }} while (0)
#else
#define LOG4CXX_WARN(logger, message)
#define LOG4CXX_WARN_ENTRY(logger, message)
#define LOG4CXX_WARN_EXIT(logger, message)
#endif

#if !defined(LOG4CXX_THRESHOLD) || LOG4CXX_THRESHOLD <= 40000
/**
Logs a message to a specified logger with the ERROR level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_ERROR(logger, message) do { \
        if (logger->isErrorEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getError(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_ERROR_ENTRY(logger, message) do { \
        if (logger->isErrorEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getError(), oss_.str(oss_ << "\n\n****** Entering ******\n" << __FUNCSIG__ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_ERROR_EXIT(logger, message) do { \
        if (logger->isErrorEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getError(), oss_.str(oss_ << message << "\n" << __FUNCTION__ << ". Exit"), LOG4CXX_LOCATION); }} while (0)

/**
Logs a error if the condition is not true.

@param logger the logger to be used.
@param condition condition
@param message the message string to log.
*/
#define LOG4CXX_ASSERT(logger, condition, message) do { \
        if (!(condition) && logger->isErrorEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getError(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }} while (0)

#else
#define LOG4CXX_ERROR(logger, message)
#define LOG4CXX_ERROR_ENTRY(logger, message)
#define LOG4CXX_ERROR_EXIT(logger, message)
#define LOG4CXX_ASSERT(logger, condition, message)
#endif

#if !defined(LOG4CXX_THRESHOLD) || LOG4CXX_THRESHOLD <= 50000
/**
Logs a message to a specified logger with the FATAL level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_FATAL(logger, message) do { \
        if (logger->isFatalEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getFatal(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_FATAL_ENTRY(logger, message) do { \
        if (logger->isFatalEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getFatal(), oss_.str(oss_ << "\n\n****** Entering ******\n" << __FUNCSIG__ << message), LOG4CXX_LOCATION); }} while (0)
#define LOG4CXX_FATAL_EXIT(logger, message) do { \
        if (logger->isFatalEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getFatal(), oss_.str(oss_ << message << "\n" << __FUNCTION__ << ". Exit"), LOG4CXX_LOCATION); }} while (0)
#else
#define LOG4CXX_FATAL(logger, message)
#define LOG4CXX_FATAL_ENTRY(logger, message)
#define LOG4CXX_FATAL_EXIT(logger, message)
#endif

class PDLog4Cxx
{
public:
	PDLog4Cxx(bool Init = false)
		: _init(Init)
	{}

	PDLog4Cxx& operator=(bool Init)
	{
		_init = Init; return *this;
	}

	static void Configure(std::string stAppName);

	const char * application() const
	{
		return _appl.c_str();
	}

protected:

private:
	std::string _appl;	// Application name to use if name hierarchy shall be used from libraries.
	std::string _logPref;	// Log prefix for use in libraries
	bool _init;
};

#endif
