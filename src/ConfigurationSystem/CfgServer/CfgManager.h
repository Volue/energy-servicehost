// CfgManager.h : Declaration of the CfgManager

#pragma once
#include "CfgServer_rc.h"       // main symbols
#include "CfgServer_i.h"

// CfgManager

class ATL_NO_VTABLE CfgManager : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CfgManager, &CLSID_CfgManagerFactory>,
	public ISupportErrorInfo,
	public IDispatchImpl<ICfgManager, &IID_ICfgManager, &LIBID_PowelCfgServer>
{
	otl_connect db; // connect object
public:
	CfgManager();
	~CfgManager();

	DECLARE_REGISTRY_RESOURCEID(IDR_CFGSERVER)

	BEGIN_COM_MAP(CfgManager)
		COM_INTERFACE_ENTRY(ICfgManager)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
	END_COM_MAP()

	// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

	STDMETHOD(createParameters)(VARIANT pname, VARIANT ptype, VARIANT pdesc, VARIANT* result);
	STDMETHOD(deleteParameters)(VARIANT pname, VARIANT* result);
	STDMETHOD(findParameters)(BSTR searchMask, VARIANT* pname, VARIANT* result);
	STDMETHOD(getParameters)(VARIANT pname, VARIANT* ptype, VARIANT* pdesc, VARIANT* result);
	STDMETHOD(logon)(BSTR usr, BSTR pwd, BSTR db, VARIANT* result);
	STDMETHOD(logoff)(void);
	STDMETHOD(createGroups)(VARIANT gname, VARIANT gtype, VARIANT gpri, VARIANT gdesc, VARIANT* result);
	STDMETHOD(deleteGroups)(VARIANT gname, VARIANT* result);
	STDMETHOD(findGroups)(BSTR searchGname, VARIANT* foundGroup, VARIANT* result);
	STDMETHOD(getGroups)(VARIANT gname, VARIANT* gtype, VARIANT* gpri, VARIANT* gdesc, VARIANT* result);
	STDMETHOD(renameGroups)(VARIANT gname, VARIANT newGname, VARIANT * result);
	STDMETHOD(createGroupMembers)(VARIANT mname, VARIANT * result);
	STDMETHOD(deleteGroupMembers)(VARIANT mname, VARIANT * result);
	STDMETHOD(findGroupMembers)(BSTR searchMname, VARIANT* foundGroupMember, VARIANT* result);
	STDMETHOD(renameGroupMembers)(VARIANT mname, VARIANT newMname, VARIANT* result);
	STDMETHOD(addMembersToGroup)(BSTR gname, VARIANT mname, VARIANT* result);
	STDMETHOD(removeMembersFromGroup)(BSTR gname, VARIANT mname, VARIANT* result);
	STDMETHOD(getMembersForGroup)(BSTR gname, VARIANT* mname, VARIANT* result);
	STDMETHOD(getParametersForGroup)(BSTR gname, VARIANT* pname, VARIANT* ptype, VARIANT* pdesc, VARIANT* pval, VARIANT* result);
	STDMETHOD(removeParametersFromGroup)(BSTR gname, VARIANT pname, VARIANT* result);
	STDMETHOD(setGroupParameter)(BSTR gname, BSTR pname, VARIANT pval, VARIANT* result);
	STDMETHOD(getGroupMembers)(VARIANT mname, VARIANT* minfo, VARIANT* result);
	STDMETHOD(updateGroups)(VARIANT gname, VARIANT newGname, VARIANT newGtype, VARIANT newGpri, VARIANT newGdescr, VARIANT* result);
	STDMETHOD(updateParameters)(VARIANT pname, VARIANT newPname, VARIANT newPdescr, VARIANT* result);
	STDMETHOD(getParametersForGroupWithDate)(BSTR gname, VARIANT* pname, VARIANT* ptype, VARIANT* pdesc, VARIANT* pval, VARIANT* pmod, VARIANT* result);
	STDMETHOD(setGroupParameterWithDate)(BSTR gname, BSTR pname, VARIANT pval, DATE pmod, VARIANT* result);
	STDMETHOD(getRolesForMember)(VARIANT mname, VARIANT* roleTable, VARIANT* roleUserTable, VARIANT* result);
};

OBJECT_ENTRY_AUTO(__uuidof(CfgManagerFactory), CfgManager)
