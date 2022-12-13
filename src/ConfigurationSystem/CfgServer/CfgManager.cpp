// CfgManager.cpp : Implementation of CfgManager

#include "stdafx.h"
#include "CfgManager.h"
#include ".\cfgmanager.h"

using namespace std;

/* TYpe Conversion routines between COM/VARIANT containers and 
OTL DB */
otl_string_vec&  VariantToOtl(VARIANT& v,otl_string_vec& o) { /*
	o returns a  new vector
	*/
	_bstr_t bnull;
	const VVAccessor<_bstr_t> vV(v,bnull);
	o.set_len((int)vV.size());
	for(int i=0;i<(int) vV.size();i++) {
		o[i]= string((const char *)vV[i]);
		o.set_non_null(i);
	}
	return o;
}

VARIANT& OtlToVariant(otl_string_vec& o,VARIANT& v){ /*
	o construct a VARIANT array of bstr from otl_string_vec
	*/
	// hmm.VariantClear(&v);
	v= createSafeArray<_bstr_t>(o.len(),VT_BSTR,0);
	_bstr_t bnull;
	VVAccessor<_bstr_t> vV(v,bnull);
	for(int i=0;i<o.len();i++) {
		string os= o[i];
		vV[i]= _bstr_t(os.c_str());
	}
	return v;
}
otl_int_vec&  VariantToOtl(VARIANT& v,otl_int_vec& o) { /*
	o returns a  new vector
	*/
	int inull=0;
	const VVAccessor<int> vV(v,inull);
	o.set_len((int)vV.size());
	for(int i=0;i<(int) vV.size();i++) {
		o[i]= vV[i];
		o.set_non_null(i);
	}
	return o;
}

VARIANT& OtlToVariant(otl_int_vec& o,VARIANT& v){ /*
	o construct a VARIANT array of bstr from otl_string_vec
	*/
	// hmm.VariantClear(&v);
	v= createSafeArray<int>(o.len(),VT_I4,0);
	_bstr_t bnull;
	VVAccessor<_bstr_t> vV(v,bnull);
	for(int i=0;i<o.len();i++) {
		vV[i]= o[i];
	}
	return v;
}
VARIANT& OtlToVariant(otl_int_vec& ivalO,otl_double_vec& fvalO,otl_string_vec& svalO,VARIANT& v){ /*
	o construct a VARIANT array of VARIANT from supplied int,double and string _vec
	o assume same length on all vectors.
	*/
	// hmm.VariantClear(&v);
	_variant_t vnull;	
	v= createSafeArray<VARIANT>(ivalO.len(),VT_VARIANT,&vnull);
	VVAccessor<VARIANT> vV(v,vnull);
	for(int i=0;i<ivalO.len();i++) {
		if( !ivalO.is_null(i) ) {
			vV[i]= _variant_t(long(ivalO[i]));
		} else if( !fvalO.is_null(i) ) {
			vV[i]= _variant_t(double(fvalO[i]));
		} else if( !svalO.is_null(i) ) {
			vV[i]= _variant_t(svalO[i].c_str());
		} else {
			vV[i]= _variant_t();
		}
	}
	return v;
}
#include <ATLComTime.h>
DATE toDATE(otl_datetime o) {
	//TODO: Decide timezone conversion/offset from otl_datetime to OleDateTime.
	if (o.year < 0) return COleDateTime();
	return COleDateTime(o.year,o.month,o.day,o.hour,o.minute,o.second);
}
otl_datetime toDate(DATE o) {
	COleDateTime tmp = COleDateTime(o);
	return otl_datetime(tmp.GetYear(),tmp.GetMonth(),tmp.GetDay(),tmp.GetHour(),tmp.GetMinute(),tmp.GetSecond());
}
VARIANT& OtlToVariant(otl_datetime_vec& o,VARIANT& v){ /*
	o construct a VARIANT array of variant from otl_datetime_vec
	*/
	v= createSafeArray<DATE>(o.len(),VT_DATE,0);
	DATE bnull=0.0;
	VVAccessor<DATE> vV(v,bnull);
	for(int i=0;i<o.len();i++) {

		vV[i]= DATE(toDATE(o[i]));
	}
	return v;
}

// CfgManager
CfgManager::CfgManager() {
	CfgModuleInit();
}
CfgManager::~CfgManager() {
	logoff();// make sure we are logged off..
	CfgModuleExit();
}


STDMETHODIMP 
CfgManager::createParameters(VARIANT pname, VARIANT ptype, VARIANT pdesc, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.createParameters( "
					"        :pname<char[513],in[1500]>,:ptype<char[2],in[1500]>,:pdesc<char[1001],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec pnameO,ptypeO,pdescO;
				p<<VariantToOtl(pname,pnameO);
				p<<VariantToOtl(ptype,ptypeO);
				p<<VariantToOtl(pdesc,pdescO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t((const char*)p.msg);
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::deleteParameters(VARIANT pname, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.deleteParameters( "
					"        :pname<char[513],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec pnameO;
				p<<VariantToOtl(pname,pnameO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));

			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::findParameters(BSTR searchMask, VARIANT* pname, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.findParameters( "
					"        :searchMask<char[100],in>,:pname<char[513],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec nameO;
				USES_CONVERSION;
				char *srchMask= strdup(W2A(searchMask));
				if(!srchMask) srchMask=strdup("%");
				p<<srchMask;
				p>>nameO;
				int resO;
				p>>resO;
				*pname=OtlToVariant(nameO,*pname);
				*result=_variant_t(long(resO));
				free(srchMask);
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::getParameters(VARIANT pname, VARIANT* ptype, VARIANT* pdesc, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.getParameters( "
					"        :pname<char[513],in[1500]>,:ptype<char[2],out[1500]>,:pdesc<char[1001],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec nameO;VariantToOtl(pname,nameO);
				p<<nameO;
				otl_string_vec ptypeO;
				otl_string_vec pdescO;
				int resO;
				p>>ptypeO;
				p>>pdescO;
				p>>resO;
				*ptype=OtlToVariant(ptypeO,*ptype);
				*pdesc=OtlToVariant(pdescO,*pdesc);
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::logon(BSTR usr, BSTR pwd, BSTR dbC, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		char connStr[2048];
		_snprintf(connStr,sizeof(connStr)-1,"%s/%s@%s",(const char*)_bstr_t(usr),(const char*)_bstr_t(pwd),(const char*)_bstr_t(dbC));
		try {
			//db.rlogon(connStr);
			db.set_connect_mode(true);
			db.server_attach((const char*)_bstr_t(dbC),0,0,true);
			db.session_begin((const char*)_bstr_t(usr),(const char*)_bstr_t(pwd));
			if(db.connected) {
				try {
					otl_stream p(1, // buffer size
						"select u.ustp_key from users u where u.DBUSR_ID=UID", // SELECT statement
						db // connect object
						);
					int ustp_key;
					p>>ustp_key;
					if( ustp_key > 10 ) {
						logoff();
					}
				} catch(...) {
					logoff();
				}
				*result=_variant_t(long(db.connected?0:2));
			} else 
				*result=_variant_t(long(db.connected?0:1));
		} catch(otl_exception& p){ // intercept OTL exceptions
			cerr<<p.msg<<endl; // print out error message
			cerr<<p.stm_text<<endl; // print out SQL that caused the error
			cerr<<p.var_info<<endl; // print out the variable that caused the error
			*result=_variant_t(long(1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::logoff(void) {
	HRESULT res=S_OK;
	try {
		try {
			if(db.connected) {
				otl_stream o(1, // buffer size
					"begin "
					" if ( pdmisc.get_log_number > 0 ) then "
					"	pdlog.Exit;"
					" end if;"
					"end;", // SELECT statement
					db // connect object
					);
				o.set_commit(0);
				o.flush();
				db.session_end();
			}
		} catch(otl_exception& p){ // intercept OTL exceptions
			cerr<<p.msg<<endl; // print out error message
			cerr<<p.stm_text<<endl; // print out SQL that caused the error
			cerr<<p.var_info<<endl; // print out the variable that caused the error
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::createGroups(VARIANT gname, VARIANT gtype, VARIANT gpri, VARIANT gdesc, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.createGroups( "
					"        :gname<char[513],in[1500]>,:gtype<char[2],in[1500]>,:gpri<int,in[1500]>,:gdesc<char[1001],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec gnameO,gtypeO,gdescO;
				otl_int_vec gpriO;
				p<<VariantToOtl(gname,gnameO);
				p<<VariantToOtl(gtype,gtypeO);
				p<<VariantToOtl(gpri,gpriO);
				p<<VariantToOtl(gdesc,gdescO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t((const char*)p.msg);
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::deleteGroups(VARIANT gname, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.deleteGroups( "
					"        :gname<char[513],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec gnameO;
				p<<VariantToOtl(gname,gnameO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::findGroups(BSTR searchGname, VARIANT* foundGroup, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.findGroups( "
					"        :searchMask<char[100],in>,:gname<char[513],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec nameO;
				USES_CONVERSION;
				char *srchMask= strdup(W2A(searchGname));
				if(!srchMask) srchMask=strdup("%");
				p<<srchMask;
				p>>nameO;
				int resO;
				p>>resO;
				*foundGroup=OtlToVariant(nameO,*foundGroup);
				*result=_variant_t(long(resO));
				free(srchMask);
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::getGroups(VARIANT gname, VARIANT* gtype, VARIANT* gpri, VARIANT* gdesc, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.getGroups( "
					"        :gname<char[513],in[1500]>,:gtype<char[2],out[1500]>,:gpri<int,out[1500]>,:gdesc<char[1001],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec nameO;VariantToOtl(gname,nameO);
				p<<nameO;
				otl_string_vec gtypeO;
				otl_int_vec gpriO;
				otl_string_vec gdescO;
				int resO;
				p>>gtypeO;
				p>>gpriO;
				p>>gdescO;
				p>>resO;
				*gtype=OtlToVariant(gtypeO,*gtype);
				*gpri =OtlToVariant(gpriO,*gpri);
				*gdesc=OtlToVariant(gdescO,*gdesc);
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::getRolesForMember(VARIANT mname, VARIANT* roleTable, VARIANT* roleUserTable, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.getRolesForUser( "
					"        :osUser<char[513],in>,:roleTable<char[513],out[1500]>,:roleUserTable<char[513],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec nameO;VariantToOtl(mname,nameO);
				p<<nameO;
				otl_string_vec roleTableO;
				otl_string_vec roleUserTableO;
				int resO;
				p>>roleTableO;
				p>>roleUserTableO;
				p>>resO;
				*roleTable=OtlToVariant(roleTableO,*roleTable);
				*roleUserTable=OtlToVariant(roleUserTableO,*roleUserTable);
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP CfgManager::renameGroups(VARIANT gname, VARIANT newGname, VARIANT * result)
{
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.renameGroups( "
					"        :gname<char[513],in[1500]>,:newGname<char[513],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec gnameO;
				otl_string_vec newGnameO;
				//USES_CONVERSION;
				//char *newMnameS= W2A(newMname);
				//p<<newMnameS;
				p<<VariantToOtl(gname,gnameO);
				p<<VariantToOtl(newGname,newGnameO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::createGroupMembers(VARIANT mname, VARIANT * result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.createGroupMembers( "
					"        :mname<char[513],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec mnameO;
				p<<VariantToOtl(mname,mnameO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t((const char*)p.msg);
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::deleteGroupMembers(VARIANT mname, VARIANT * result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.deleteGroupMembers( "
					"        :mname<char[513],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec mnameO;
				p<<VariantToOtl(mname,mnameO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::findGroupMembers(BSTR searchMname, VARIANT* foundGroupMember, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.findGroupMembers( "
					"        :searchMask<char[100],in>,:mname<char[513],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec nameO;
				USES_CONVERSION;
				char *srchMask= strdup(W2A(searchMname));
				if(!srchMask) srchMask=strdup("%");
				p<<srchMask;
				p>>nameO;
				int resO;
				p>>resO;
				*foundGroupMember=OtlToVariant(nameO,*foundGroupMember);
				*result=_variant_t(long(resO));
				free(srchMask);
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::renameGroupMembers(VARIANT mname, VARIANT newMname, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.renameGroupMembers( "
					"        :mname<char[513],in[1500]>,:newMname<char[513],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec mnameO;
				otl_string_vec newMnameO;
				//USES_CONVERSION;
				//char *newMnameS= W2A(newMname);
				//p<<newMnameS;
				p<<VariantToOtl(mname,mnameO);
				p<<VariantToOtl(newMname,newMnameO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
//	return S_OK;
}

STDMETHODIMP 
CfgManager::addMembersToGroup(BSTR gname, VARIANT mname, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.addMembersToGroup( "
					"        :gname<char[513],in>,:mname<char[513],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec mnameO;
				USES_CONVERSION;
				char *gnameS= W2A(gname);
				p<<gnameS;
				p<<VariantToOtl(mname,mnameO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::removeMembersFromGroup(BSTR gname, VARIANT mname, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.removeMembersFromGroup( "
					"        :gname<char[513],in>,:mname<char[513],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec mnameO;
				USES_CONVERSION;
				char *gnameS= W2A(gname);
				p<<gnameS;
				p<<VariantToOtl(mname,mnameO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::getMembersForGroup(BSTR gname, VARIANT* mname, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.getMembersForGroup( "
					"        :gname<char[513],in>,:mname<char[513],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec mnameO;
				USES_CONVERSION;
				char *gnameS= W2A(gname);
				p<<gnameS;
				p>>mnameO;
				int resO;
				p>>resO;
				*mname=OtlToVariant(mnameO,*mname);
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::getParametersForGroup(BSTR gname, VARIANT* pname, VARIANT* ptype, VARIANT* pdesc, VARIANT* pval, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.getParametersForGroup( "
					"        :gname<char[513],in>,:pname<char[513],out[1500]>,:ptype<char[2],out[1500]>,:pdesc<char[1001],out[1500]>,"
					"        :ival<int,out[1500]>,:fval<double,out[1500]>,:sval<char[1001],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec pnameO,ptypeO,pdescO,svalO;
				otl_int_vec ivalO;
				otl_double_vec fvalO;
				USES_CONVERSION;
				char *gnameS= W2A(gname);
				p<<gnameS;
				p>>pnameO;
				p>>ptypeO;
				p>>pdescO;
				p>>ivalO;
				p>>fvalO;
				p>>svalO;
				int resO;
				p>>resO;
				*pname=OtlToVariant(pnameO,*pname);
				*ptype=OtlToVariant(ptypeO,*ptype);
				*pdesc=OtlToVariant(pdescO,*pdesc);
				*pval =OtlToVariant(ivalO,fvalO,svalO,*pval);// convert into one vector of variants according to each type value.
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::removeParametersFromGroup(BSTR gname, VARIANT pname, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.removeParametersFromGroup( "
					"        :gname<char[513],in>,:pname<char[513],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec pnameO;
				USES_CONVERSION;
				char *gnameS= W2A(gname);
				p<<gnameS;
				p<<VariantToOtl(pname,pnameO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::setGroupParameter(BSTR gname, BSTR pname, VARIANT pval, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.setParameter( "
					"        :gname<char[513],in>,:pname<char[513],in>,:ival<int,in>,:fval<double,in>,:sval<char[1001],in>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				USES_CONVERSION;
				char *gnameS= strdup(W2A(gname));
				char *pnameS= strdup(W2A(pname));
				p<<gnameS;
				p<<pnameS;
				// now figure out values to insert.
				otl_value<int> ival;
				otl_value<double> fval;
				otl_value<string> sval;
				ival.set_null();fval.set_null();sval.set_null();
				switch(pval.vt) {
					case VT_I4:case VT_I2: ival=int(long(_variant_t(pval)));ival.set_non_null();break;
					case VT_R4:case VT_R8: fval=double(_variant_t(pval));fval.set_non_null();break;
					case VT_BSTR: sval= string((const char*)_bstr_t(_variant_t(pval)));sval.set_non_null();break;
				}
				p<<ival<<fval<<sval;
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
				free(gnameS);free(pnameS);
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}
STDMETHODIMP 
CfgManager::setGroupParameterWithDate(BSTR gname, BSTR pname, VARIANT pval, DATE pmod, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.setParameterWithDate( "
					"        :gname<char[513],in>,:pname<char[513],in>,:ival<int,in>,:fval<double,in>,:sval<char[1001],in>,:pmod<timestamp[500],in>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				USES_CONVERSION;
				char *gnameS= strdup(W2A(gname));
				char *pnameS= strdup(W2A(pname));
				p<<gnameS;
				p<<pnameS;
				// now figure out values to insert.
				otl_value<int> ival;
				otl_value<double> fval;
				otl_value<string> sval;
				ival.set_null();fval.set_null();sval.set_null();
				switch(pval.vt) {
					case VT_I4:case VT_I2: ival=int(long(_variant_t(pval)));ival.set_non_null();break;
					case VT_R4:case VT_R8: fval=double(_variant_t(pval));fval.set_non_null();break;
					case VT_BSTR: sval= string((const char*)_bstr_t(_variant_t(pval)));sval.set_non_null();break;
				}
				p<<ival<<fval<<sval;
				otl_datetime pmodD = toDate(pmod);
				p<<pmodD;
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
				free(gnameS);free(pnameS);
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				//*result=_variant_t(long(-2));
				setGroupParameter(gname,pname,pval,result);
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::getGroupMembers(VARIANT mname, VARIANT* minfo, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.getGroupMembers( "
					"        :mname<char[513],in[1500]>,:gdesc<char[1001],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec nameO;VariantToOtl(mname,nameO);
				p<<nameO;
				otl_string_vec minfoO;
				int resO;
				p>>minfoO;
				p>>resO;
				*minfo=OtlToVariant(minfoO,*minfo);
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t(long(-2));
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP
CfgManager::updateGroups(VARIANT gname, VARIANT newGname, VARIANT newGtype, VARIANT newGpri, VARIANT newGdescr, VARIANT* result)
{
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.updateGroups( "
					"        :gname<char[513],in[1500]>,:newGname<char[513],in[1500]>,:newGtype<char[2],in[1500]>,:newGpri<int,in[1500]>,:newGdescr<char[1001],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec gnameO,newGnameO,newGtypeO,newGdescrO;
				otl_int_vec newGpriO;
				p<<VariantToOtl(gname,gnameO);
				p<<VariantToOtl(newGname,newGnameO);
				p<<VariantToOtl(newGtype,newGtypeO);
				p<<VariantToOtl(newGpri,newGpriO);
				p<<VariantToOtl(newGdescr,newGdescrO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t((const char*)p.msg);
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::updateParameters(VARIANT pname, VARIANT newPname, VARIANT newPdescr, VARIANT* result)
{
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.updateParameters( "
					"        :pname<char[513],in[1500]>,:newPname<char[513],in[1500]>,:newPdescr<char[1001],in[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec pnameO,newPnameO,newPdescrO;
				p<<VariantToOtl(pname,pnameO);
				p<<VariantToOtl(newPname,newPnameO);
				p<<VariantToOtl(newPdescr,newPdescrO);
				int resO;
				p>>resO;
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				*result=_variant_t((const char*)p.msg);
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP 
CfgManager::getParametersForGroupWithDate(BSTR gname, VARIANT* pname, VARIANT* ptype, VARIANT* pdesc, VARIANT* pmod, VARIANT* pval, VARIANT* result) {
	HRESULT res=S_OK;
	try {
		if(db.connected) { 
			try { 
				otl_stream p(1, // buffer size
					"begin "
					" icc_cfgsystem.getParametersForGroupWithDate( "
					"        :gname<char[513],in>,:pname<char[513],out[1500]>,:ptype<char[2],out[1500]>,:pdesc<char[1001],out[1500]>,"
					"        :pmod<timestamp,out[1500]>,:ival<int,out[1500]>,:fval<double,out[1500]>,:sval<char[1001],out[1500]>,:res<int,out>"
					" );"
					"end;", // SELECT statement
					db // connect object
					);
				otl_string_vec pnameO,ptypeO,pdescO,svalO;
				otl_datetime_vec pmodO;
				otl_int_vec ivalO;
				otl_double_vec fvalO;
				USES_CONVERSION;
				char *gnameS= W2A(gname);
				p<<gnameS;
				p>>pnameO>>ptypeO>>pdescO>>pmodO>>ivalO>>fvalO>>svalO;
				int resO;
				p>>resO;
				*pname=OtlToVariant(pnameO,*pname);
				*ptype=OtlToVariant(ptypeO,*ptype);
				*pdesc=OtlToVariant(pdescO,*pdesc);
				*pmod =OtlToVariant( pmodO,*pmod);
				*pval =OtlToVariant(ivalO,fvalO,svalO,*pval);// convert into one vector of variants according to each type value.
				*result=_variant_t(long(resO));
			} catch(otl_exception& p){ // intercept OTL exceptions
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				//*result=_variant_t(long(-2));
				//*pmod = 0;
				getParametersForGroup(gname,pname,ptype,pdesc,pval,result);
			}
		} else {
			*result=_variant_t(long(-1));
		}
	} catch ( _com_error e) {
		res=e.Error();
	} catch (...) {
		res=E_POINTER;
	}
	return res;
}

STDMETHODIMP
CfgManager::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICfgReader
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}
