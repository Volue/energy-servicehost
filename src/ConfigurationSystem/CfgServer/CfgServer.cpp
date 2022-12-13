// CfgServer.cpp : Implementation of WinMain

#include "stdafx.h"
#include "CfgServer_rc.h"
#include "CfgServer_i.h"
#include "PDLog4Cxx.h"

using namespace std;

log4cxx::LoggerPtr _l4nCfgServer(log4cxx::Logger::getLogger("Powel.CfgServer"));

long initCount=0; // refCounting common library
void CfgModuleInit() 
{
    try
    {
        PDLog4Cxx::Configure("CfgServer");
        LOG4CXX_WARN(_l4nCfgServer, __FUNCTION__ "\n\tPDLog4Cxx::Configure OK");
    }
    catch (exception ex)
    {
        // Just for debugging:
        auto dummy = ex.what();
    }
}
void CfgModuleExit() 
{
}
class PowelCfgServerModule : public CAtlExeModuleT<PowelCfgServerModule> {
public:
	DECLARE_LIBID(LIBID_PowelCfgServer)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CFGSERVER, "{00B20303-30A7-466D-9F41-7829F150DE8A}")

	PowelCfgServerModule() {
		CfgModuleInit();
		m_dwTimeOut=30000;
	}
	~PowelCfgServerModule() {
		CfgModuleExit();
	}
};

PowelCfgServerModule _AtlModule;

extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}
