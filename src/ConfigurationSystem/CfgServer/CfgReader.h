// CfgReader.h : Declaration of the CfgReader

#pragma once
#include "stdafx.h"
#include "CfgServer_rc.h"       // main symbols
#include "CfgServer_i.h"

// CfgReader

class ATL_NO_VTABLE CfgReader : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CfgReader, &CLSID_CfgReaderFactory>,
	public ISupportErrorInfo,
	public IDispatchImpl<ICfgReader, &IID_ICfgReader, &LIBID_PowelCfgServer>
{
private:
	CComAutoCriticalSection& mx();
	static CComAutoCriticalSection * mx_;// used to sequence external access to shared globals
	//otl_connect db_; // connect object
	OtlConnection conn_;
	std::string tnsName_; // location and account info for
	std::string dbUser_;   // the configuration database
	std::string dbPassword_;
	bool attached_;

	// Initialize tnsName_, dbUser_, and dbPassword_ from the registry, environment,
	// or default values. Looks up `ICC_CFGSERVER`, `ICC_CFGUSER` and `ICC_CFGPASSWD`
	// from the following sources, using the first values found:
	//
	// 1. HKLM:\SOFTWARE\Powel\%USERNAME%
	// 2. HKLM:\SOFTWARE\Powel
	// 3. Environment variables
	// 4. The default string "icc"
	//
	// For the registry keys values will only be used if _all_ the options are set.
	void InitDbInfo();

	bool GetExpandedConfigurationInternal(BSTR osUser, BSTR computerName, BSTR progName, VARIANT* pname, VARIANT* pvalue, VARIANT* bResult);
    HRESULT server_attach_session_init();
public:
	CfgReader();
	~CfgReader();

	DECLARE_REGISTRY_RESOURCEID(IDR_CFGSERVER)

	BEGIN_COM_MAP(CfgReader)
		COM_INTERFACE_ENTRY(ICfgReader)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
	END_COM_MAP()

	// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();
public:

	STDMETHOD(GetConfiguration)(BSTR osUser, BSTR computerName, BSTR progName, VARIANT* configResult, VARIANT* bResult);
	STDMETHOD(GetEncryptedParameters)(VARIANT* configResult, VARIANT* bResult);
	STDMETHOD(GetConfigurationWithGroup)(BSTR osUser, BSTR computerName, BSTR progName, VARIANT* gname, VARIANT* pname, VARIANT* pvalue, VARIANT* bResult);
	STDMETHOD(GetRolesForUser)(BSTR osUser, VARIANT* roles, VARIANT* users, VARIANT* bResult);
	STDMETHOD(GetExpandedConfiguration)(BSTR osUser, BSTR computerName, BSTR progName, VARIANT* pname, VARIANT* pvalue, VARIANT* bResult);
	STDMETHOD(GetExpandedConfigurationByRole)(int roleKey, BSTR computerName, BSTR progName, VARIANT* pname, VARIANT* pvalue, VARIANT* bResult);
	STDMETHOD(GetRoles)(VARIANT* ids, VARIANT* roleNames, VARIANT* bResult);
	STDMETHOD(GetRolesWithRoleUser)(VARIANT* ids, VARIANT* roleNames, VARIANT* roleUsers, VARIANT* bResult);
	STDMETHOD(GetUsers)(VARIANT* userNames, VARIANT* bResult);

};

OBJECT_ENTRY_AUTO(__uuidof(CfgReaderFactory), CfgReader)

