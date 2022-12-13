/**** File_Header *************************************************************
 *
 *	Author    : Odd Broenner    PD     DATE: 961101
 *	Copyright : Powel Data AS
 *	Compiler  :
 *
 *	Keywords  :
 *	Contents  :
 *
 *	Includes and defines:
 */
#ifndef _ACTIVITYLOG_H_
#define _ACTIVITYLOG_H_

#include <icc.h>
#include <ActivityLogDefinitions.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <icc_api.h>

int
PAL_exit( void );

#define PAL_fatal(x) PAL_log_fatal( __FILE__, __LINE__, x)

#define PAL_LOG_MESSAGE(...) \
if (pal != nullptr) \
{ \
    pal->LogMessage(__VA_ARGS__); \
}

#define PAL_LOG(...) \
if (pal != nullptr) \
{ \
    pal->Log(__VA_ARGS__); \
}

int PAL_info(	const char *msg,
		const AL_MESSAGE_LEVEL messageLevel
#ifdef __cplusplus
= AL_LEV_DEB
#endif
);

int PAL_warning(const char *msg,
		const AL_MESSAGE_LEVEL messageLevel
#ifdef __cplusplus
= AL_LEV_DEB
#endif
);

int PAL_error(	const char *msg,
		const AL_MESSAGE_LEVEL messageLevel
#ifdef __cplusplus
= AL_LEV_DEB
#endif
);

int PAL_log_fatal(	const char *file,
		const int line,
		const char *msg);

void PAL_SetFileInLog( char *filename );

int PAL_logMessage(const int messageId,
                   const char *messageValues );

#include <lang_api.h>

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <stdio.h>

#include <rw/cstring.h>
#include <rw/slistcol.h>
#include <rw/stackcol.h>
#include <rw/tvordvec.h>
#include <rw/tvhdict.h>
#include <rw/rwint.h>

#include <PDLogEntry.h>
#include <PDLogCollMessage.h>

class ICCAppCommInterface;
class ICCAppCommProgram;
struct ICCAppCommMsgCB;


void PAL_SetLogOwner( long key );
void PAL_UseLogKey( long key );

enum class AttachToPdLog
{
    CheckICC_NO_PDLOG,
    Never,
    Always
};

int PAL_init(
    const ICC_MODULE module,
    const ICC_PROGRAM app,
    int argc,
    char **argv,
    ICCAppCommInterface *icc = nullptr,
    AL_MESSAGE_TYPE logLevel = AL_MESS_INFO,
    AttachToPdLog attachToPdLog = AttachToPdLog::CheckICC_NO_PDLOG,
    int maxMsecsToWaitAckFromPdLog = -1 // Fetch from environment
    );


/*	Externals and statics:
 */

/*	Function prototypes
 */

/*****************************************************************************/
class PDLogEntry;

class ActivityLog
{
  public :


	ActivityLog(
        const long _ownerLogKey,
        const long _useLogKey,
        const ICC_MODULE _module,
        const ICC_PROGRAM _program,
        int &_argc,
        char **(&_argv),
        ICCAppCommInterface *_icc = NULL,
        AL_MESSAGE_TYPE logLevel = AL_MESS_INFO,
        AttachToPdLog attachToPdLog = AttachToPdLog::CheckICC_NO_PDLOG,
        int maxMsecsToWaitAckFromPdLog = -1 // Fetch from environment
        );

	~ActivityLog();

	int					InitDone( void ) const {return initDone;}
	int					Exit( void );

	int					Log(	PDLogMessage *p);
	int					Log(	const char *file,
								const int line,
								const char *text,
								const AL_MESSAGE_TYPE messageType,
								const AL_MESSAGE_LEVEL messageLevel=AL_LEV_DEB);

	int	LogMessage ( const int messageId, ... );
	int LogMessage (const char* tag, 
	                const int messageId, ... );
	int LogMessage (const ICC_MODULE _module, 
	                const int messageId, ... );
	int LogMessage (const ICC_MODULE _module,
	                const char* tag, 
	                const int messageId, ... );

	int LogMessageIgnoreLevel(const int messageId, ...);

	int LogMessageV(const ICC_MODULE _module,
	                const char* tag, 
	                const int messageId, 
					bool ignoreLevel,
	                va_list argptr
					);

	int	Log(PDLogExtMessage *p, bool ignoreLevel = false);
	PDLogExtMessage* GetLogMessage(const ICC_MODULE _module, 
	                               const int messageId );
	PDLogExtMessage*	GetLogMessage( const int messageId );

	int					SendMail(	const char *address,
									const char *subject );

	lang_id				Langid( void ) { return langid;}

	int					GetStatus( void );
	int					GetRealStatus( void );
	void				ClearStatus( void );
	int					SetStatus( const int errorCondition ) ;

	int					SetHandlerMode( AL_HANDLER_TYPE _handler );

	int GetApplicationLog(const char *where,
	                      RWSlistCollectables *(&errorMessages));

	int					UseNvoy( void ) const { return use_nvoy;}
	int					GetLogId( void );

	int					addSearchItem( const char *text );

	int					GetArgc( void ) const { return argc; }
	char			  **GetArgv( void ) const { return argv; }
	char				*GetLogName(void ) { return logName.GetString(); }
	char				*GetNativeName(void ) { return nativeName.GetString(); }
	ICC_PROGRAM			GetProgram() const { return program; }
	ICC_MODULE			GetModule() const { return module; }
    AL_MESSAGE_TYPE     GetLogLevel() { return _logLevel; }
    void AlwaysCallback(bool alwaysCallback) { _alwaysCallback = alwaysCallback; }
  private:

	void				registerApplication( void );
	void				attachTo(int maxMsecsToWaitAckFromPdLog);
	void				msgHandler(ICCAppCommMsgCB& m);
	void msgToken(ICCAppCommProgram *sender, 
	              PDLogCollMessage *msg);

	int					localMessage( PDLogMessage &lm );
	int					localMessage( PDLogExtMessage &lm );
	int					localMessage( PDLogMailTo &mt );
	int					localMessage( PDLogHeader &lh );
	int					localMessage( PDLogCollMessage &lcm );


	void				init( void );
	int					_InitDone( void );
	int					checkIn( void );

	int					_Log(	const int dummy,
								const char *file,
								const int line,
								const char *text,
								const AL_MESSAGE_TYPE messageType,
								const AL_MESSAGE_LEVEL messageLevel);


	int					storeMessage( const char *text);
	int					resetMessages( void );
	int					buildMessage( PDLogMessage *m, const char *text );
	int					buildMessage( PDLogExtMessage *m);

	RWInteger			loadMessageContainer( ICC_MODULE _module = M_NOMODULE );


	int									internalInterface;
	ICCAppCommInterface					*icc;
	ICCAppCommProgram					*prg;

	ICC_MODULE							module;
	ICC_PROGRAM							program;

	int									argc;
	char								**argv;
	int									use_nvoy;

	RWCString							logFileName;
	AL_HANDLER_TYPE						handler;

	int									initDone;
	int									exitDone;

	AL_MESSAGE_TYPE						messageType;
	AL_MESSAGE_LEVEL					messageLevel;

	lang_id								langid;
	RWCString							msg;

	int									gotHandlerMode;
	RWSlistCollectables					collectionLogMessages;

	RWSlistCollectablesStack			application;
	PDLogCollMessage					buffer;

    AttachToPdLog                       _attachToPdLog; //	usePDLog;
	PDLogEntry							*logEntry;

	RWTValOrderedVector<RWInteger> 		knownModules;
	RWSlistCollectables					knownMessages;

	int									alive;

	OSLong								logKeyOwner;
	OSLong								useLogKey;

	OSString							logName;
	OSString							appName;
	OSString							nativeName;

    AL_MESSAGE_TYPE _logLevel;
    bool _alwaysCallback; // if set to true forces to call callback hooks regardless of _logLevel value 
};


#ifdef _IMPLEMENT_PALINIT
ActivityLog *pal = NULL;
#else
extern ActivityLog *pal;
#endif

#endif /* __cplusplus */

#endif /* _ACTIVITYLOG_H_ */


