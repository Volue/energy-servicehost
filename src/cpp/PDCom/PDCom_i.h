

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for PDCom.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __PDCom_i_h__
#define __PDCom_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPDCom_FWD_DEFINED__
#define __IPDCom_FWD_DEFINED__
typedef interface IPDCom IPDCom;

#endif 	/* __IPDCom_FWD_DEFINED__ */


#ifndef __IPDComScript_FWD_DEFINED__
#define __IPDComScript_FWD_DEFINED__
typedef interface IPDComScript IPDComScript;

#endif 	/* __IPDComScript_FWD_DEFINED__ */


#ifndef __IValCorrServer_FWD_DEFINED__
#define __IValCorrServer_FWD_DEFINED__
typedef interface IValCorrServer IValCorrServer;

#endif 	/* __IValCorrServer_FWD_DEFINED__ */


#ifndef __PDEchoServer_FWD_DEFINED__
#define __PDEchoServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class PDEchoServer PDEchoServer;
#else
typedef struct PDEchoServer PDEchoServer;
#endif /* __cplusplus */

#endif 	/* __PDEchoServer_FWD_DEFINED__ */


#ifndef __PDEchoClient_FWD_DEFINED__
#define __PDEchoClient_FWD_DEFINED__

#ifdef __cplusplus
typedef class PDEchoClient PDEchoClient;
#else
typedef struct PDEchoClient PDEchoClient;
#endif /* __cplusplus */

#endif 	/* __PDEchoClient_FWD_DEFINED__ */


#ifndef __navigatoruis_FWD_DEFINED__
#define __navigatoruis_FWD_DEFINED__

#ifdef __cplusplus
typedef class navigatoruis navigatoruis;
#else
typedef struct navigatoruis navigatoruis;
#endif /* __cplusplus */

#endif 	/* __navigatoruis_FWD_DEFINED__ */


#ifndef __solver_FWD_DEFINED__
#define __solver_FWD_DEFINED__

#ifdef __cplusplus
typedef class solver solver;
#else
typedef struct solver solver;
#endif /* __cplusplus */

#endif 	/* __solver_FWD_DEFINED__ */


#ifndef __exchpntuis_FWD_DEFINED__
#define __exchpntuis_FWD_DEFINED__

#ifdef __cplusplus
typedef class exchpntuis exchpntuis;
#else
typedef struct exchpntuis exchpntuis;
#endif /* __cplusplus */

#endif 	/* __exchpntuis_FWD_DEFINED__ */


#ifndef __measpntuis_FWD_DEFINED__
#define __measpntuis_FWD_DEFINED__

#ifdef __cplusplus
typedef class measpntuis measpntuis;
#else
typedef struct measpntuis measpntuis;
#endif /* __cplusplus */

#endif 	/* __measpntuis_FWD_DEFINED__ */


#ifndef __netareauis_FWD_DEFINED__
#define __netareauis_FWD_DEFINED__

#ifdef __cplusplus
typedef class netareauis netareauis;
#else
typedef struct netareauis netareauis;
#endif /* __cplusplus */

#endif 	/* __netareauis_FWD_DEFINED__ */


#ifndef __customeruis_FWD_DEFINED__
#define __customeruis_FWD_DEFINED__

#ifdef __cplusplus
typedef class customeruis customeruis;
#else
typedef struct customeruis customeruis;
#endif /* __cplusplus */

#endif 	/* __customeruis_FWD_DEFINED__ */


#ifndef __hvcuis_FWD_DEFINED__
#define __hvcuis_FWD_DEFINED__

#ifdef __cplusplus
typedef class hvcuis hvcuis;
#else
typedef struct hvcuis hvcuis;
#endif /* __cplusplus */

#endif 	/* __hvcuis_FWD_DEFINED__ */


#ifndef __impexpuis_FWD_DEFINED__
#define __impexpuis_FWD_DEFINED__

#ifdef __cplusplus
typedef class impexpuis impexpuis;
#else
typedef struct impexpuis impexpuis;
#endif /* __cplusplus */

#endif 	/* __impexpuis_FWD_DEFINED__ */


#ifndef __debituis_FWD_DEFINED__
#define __debituis_FWD_DEFINED__

#ifdef __cplusplus
typedef class debituis debituis;
#else
typedef struct debituis debituis;
#endif /* __cplusplus */

#endif 	/* __debituis_FWD_DEFINED__ */


#ifndef __tspanel_FWD_DEFINED__
#define __tspanel_FWD_DEFINED__

#ifdef __cplusplus
typedef class tspanel tspanel;
#else
typedef struct tspanel tspanel;
#endif /* __cplusplus */

#endif 	/* __tspanel_FWD_DEFINED__ */


#ifndef __calcdebit_FWD_DEFINED__
#define __calcdebit_FWD_DEFINED__

#ifdef __cplusplus
typedef class calcdebit calcdebit;
#else
typedef struct calcdebit calcdebit;
#endif /* __cplusplus */

#endif 	/* __calcdebit_FWD_DEFINED__ */


#ifndef __calcdelivery_FWD_DEFINED__
#define __calcdelivery_FWD_DEFINED__

#ifdef __cplusplus
typedef class calcdelivery calcdelivery;
#else
typedef struct calcdelivery calcdelivery;
#endif /* __cplusplus */

#endif 	/* __calcdelivery_FWD_DEFINED__ */


#ifndef __calcreport_FWD_DEFINED__
#define __calcreport_FWD_DEFINED__

#ifdef __cplusplus
typedef class calcreport calcreport;
#else
typedef struct calcreport calcreport;
#endif /* __cplusplus */

#endif 	/* __calcreport_FWD_DEFINED__ */


#ifndef __ediimp_FWD_DEFINED__
#define __ediimp_FWD_DEFINED__

#ifdef __cplusplus
typedef class ediimp ediimp;
#else
typedef struct ediimp ediimp;
#endif /* __cplusplus */

#endif 	/* __ediimp_FWD_DEFINED__ */


#ifndef __ediexp_FWD_DEFINED__
#define __ediexp_FWD_DEFINED__

#ifdef __cplusplus
typedef class ediexp ediexp;
#else
typedef struct ediexp ediexp;
#endif /* __cplusplus */

#endif 	/* __ediexp_FWD_DEFINED__ */


#ifndef __gs2imp_FWD_DEFINED__
#define __gs2imp_FWD_DEFINED__

#ifdef __cplusplus
typedef class gs2imp gs2imp;
#else
typedef struct gs2imp gs2imp;
#endif /* __cplusplus */

#endif 	/* __gs2imp_FWD_DEFINED__ */


#ifndef __gs2exp_FWD_DEFINED__
#define __gs2exp_FWD_DEFINED__

#ifdef __cplusplus
typedef class gs2exp gs2exp;
#else
typedef struct gs2exp gs2exp;
#endif /* __cplusplus */

#endif 	/* __gs2exp_FWD_DEFINED__ */


#ifndef __hydval_FWD_DEFINED__
#define __hydval_FWD_DEFINED__

#ifdef __cplusplus
typedef class hydval hydval;
#else
typedef struct hydval hydval;
#endif /* __cplusplus */

#endif 	/* __hydval_FWD_DEFINED__ */


#ifndef __hydcorr_FWD_DEFINED__
#define __hydcorr_FWD_DEFINED__

#ifdef __cplusplus
typedef class hydcorr hydcorr;
#else
typedef struct hydcorr hydcorr;
#endif /* __cplusplus */

#endif 	/* __hydcorr_FWD_DEFINED__ */


#ifndef __pdaudituis_FWD_DEFINED__
#define __pdaudituis_FWD_DEFINED__

#ifdef __cplusplus
typedef class pdaudituis pdaudituis;
#else
typedef struct pdaudituis pdaudituis;
#endif /* __cplusplus */

#endif 	/* __pdaudituis_FWD_DEFINED__ */


#ifndef __activityloguis_FWD_DEFINED__
#define __activityloguis_FWD_DEFINED__

#ifdef __cplusplus
typedef class activityloguis activityloguis;
#else
typedef struct activityloguis activityloguis;
#endif /* __cplusplus */

#endif 	/* __activityloguis_FWD_DEFINED__ */


#ifndef __tsrserver_FWD_DEFINED__
#define __tsrserver_FWD_DEFINED__

#ifdef __cplusplus
typedef class tsrserver tsrserver;
#else
typedef struct tsrserver tsrserver;
#endif /* __cplusplus */

#endif 	/* __tsrserver_FWD_DEFINED__ */


#ifndef __tsrclient_FWD_DEFINED__
#define __tsrclient_FWD_DEFINED__

#ifdef __cplusplus
typedef class tsrclient tsrclient;
#else
typedef struct tsrclient tsrclient;
#endif /* __cplusplus */

#endif 	/* __tsrclient_FWD_DEFINED__ */


#ifndef __logonuis_FWD_DEFINED__
#define __logonuis_FWD_DEFINED__

#ifdef __cplusplus
typedef class logonuis logonuis;
#else
typedef struct logonuis logonuis;
#endif /* __cplusplus */

#endif 	/* __logonuis_FWD_DEFINED__ */


#ifndef __tsriclient_FWD_DEFINED__
#define __tsriclient_FWD_DEFINED__

#ifdef __cplusplus
typedef class tsriclient tsriclient;
#else
typedef struct tsriclient tsriclient;
#endif /* __cplusplus */

#endif 	/* __tsriclient_FWD_DEFINED__ */


#ifndef __tsrviewexcelfile_FWD_DEFINED__
#define __tsrviewexcelfile_FWD_DEFINED__

#ifdef __cplusplus
typedef class tsrviewexcelfile tsrviewexcelfile;
#else
typedef struct tsrviewexcelfile tsrviewexcelfile;
#endif /* __cplusplus */

#endif 	/* __tsrviewexcelfile_FWD_DEFINED__ */


#ifndef __pdlog_FWD_DEFINED__
#define __pdlog_FWD_DEFINED__

#ifdef __cplusplus
typedef class pdlog pdlog;
#else
typedef struct pdlog pdlog;
#endif /* __cplusplus */

#endif 	/* __pdlog_FWD_DEFINED__ */


#ifndef __tsrstartreport_FWD_DEFINED__
#define __tsrstartreport_FWD_DEFINED__

#ifdef __cplusplus
typedef class tsrstartreport tsrstartreport;
#else
typedef struct tsrstartreport tsrstartreport;
#endif /* __cplusplus */

#endif 	/* __tsrstartreport_FWD_DEFINED__ */


#ifndef __tsrviewhtmlfile_FWD_DEFINED__
#define __tsrviewhtmlfile_FWD_DEFINED__

#ifdef __cplusplus
typedef class tsrviewhtmlfile tsrviewhtmlfile;
#else
typedef struct tsrviewhtmlfile tsrviewhtmlfile;
#endif /* __cplusplus */

#endif 	/* __tsrviewhtmlfile_FWD_DEFINED__ */


#ifndef __tssearch_FWD_DEFINED__
#define __tssearch_FWD_DEFINED__

#ifdef __cplusplus
typedef class tssearch tssearch;
#else
typedef struct tssearch tssearch;
#endif /* __cplusplus */

#endif 	/* __tssearch_FWD_DEFINED__ */


#ifndef __ester_FWD_DEFINED__
#define __ester_FWD_DEFINED__

#ifdef __cplusplus
typedef class ester ester;
#else
typedef struct ester ester;
#endif /* __cplusplus */

#endif 	/* __ester_FWD_DEFINED__ */


#ifndef __regmeasuis_FWD_DEFINED__
#define __regmeasuis_FWD_DEFINED__

#ifdef __cplusplus
typedef class regmeasuis regmeasuis;
#else
typedef struct regmeasuis regmeasuis;
#endif /* __cplusplus */

#endif 	/* __regmeasuis_FWD_DEFINED__ */


#ifndef __mvsreport_FWD_DEFINED__
#define __mvsreport_FWD_DEFINED__

#ifdef __cplusplus
typedef class mvsreport mvsreport;
#else
typedef struct mvsreport mvsreport;
#endif /* __cplusplus */

#endif 	/* __mvsreport_FWD_DEFINED__ */


#ifndef __tsrviewpsfile_FWD_DEFINED__
#define __tsrviewpsfile_FWD_DEFINED__

#ifdef __cplusplus
typedef class tsrviewpsfile tsrviewpsfile;
#else
typedef struct tsrviewpsfile tsrviewpsfile;
#endif /* __cplusplus */

#endif 	/* __tsrviewpsfile_FWD_DEFINED__ */


#ifndef __kisclient_FWD_DEFINED__
#define __kisclient_FWD_DEFINED__

#ifdef __cplusplus
typedef class kisclient kisclient;
#else
typedef struct kisclient kisclient;
#endif /* __cplusplus */

#endif 	/* __kisclient_FWD_DEFINED__ */


#ifndef __kisdataload_FWD_DEFINED__
#define __kisdataload_FWD_DEFINED__

#ifdef __cplusplus
typedef class kisdataload kisdataload;
#else
typedef struct kisdataload kisdataload;
#endif /* __cplusplus */

#endif 	/* __kisdataload_FWD_DEFINED__ */


#ifndef __structloaduis_FWD_DEFINED__
#define __structloaduis_FWD_DEFINED__

#ifdef __cplusplus
typedef class structloaduis structloaduis;
#else
typedef struct structloaduis structloaduis;
#endif /* __cplusplus */

#endif 	/* __structloaduis_FWD_DEFINED__ */


#ifndef __meteruis_FWD_DEFINED__
#define __meteruis_FWD_DEFINED__

#ifdef __cplusplus
typedef class meteruis meteruis;
#else
typedef struct meteruis meteruis;
#endif /* __cplusplus */

#endif 	/* __meteruis_FWD_DEFINED__ */


#ifndef __sim_FWD_DEFINED__
#define __sim_FWD_DEFINED__

#ifdef __cplusplus
typedef class sim sim;
#else
typedef struct sim sim;
#endif /* __cplusplus */

#endif 	/* __sim_FWD_DEFINED__ */


#ifndef __hbv_FWD_DEFINED__
#define __hbv_FWD_DEFINED__

#ifdef __cplusplus
typedef class hbv hbv;
#else
typedef struct hbv hbv;
#endif /* __cplusplus */

#endif 	/* __hbv_FWD_DEFINED__ */


#ifndef __biduis_FWD_DEFINED__
#define __biduis_FWD_DEFINED__

#ifdef __cplusplus
typedef class biduis biduis;
#else
typedef struct biduis biduis;
#endif /* __cplusplus */

#endif 	/* __biduis_FWD_DEFINED__ */


#ifndef __tshyd_FWD_DEFINED__
#define __tshyd_FWD_DEFINED__

#ifdef __cplusplus
typedef class tshyd tshyd;
#else
typedef struct tshyd tshyd;
#endif /* __cplusplus */

#endif 	/* __tshyd_FWD_DEFINED__ */


#ifndef __simnt_FWD_DEFINED__
#define __simnt_FWD_DEFINED__

#ifdef __cplusplus
typedef class simnt simnt;
#else
typedef struct simnt simnt;
#endif /* __cplusplus */

#endif 	/* __simnt_FWD_DEFINED__ */


#ifndef __accounting_FWD_DEFINED__
#define __accounting_FWD_DEFINED__

#ifdef __cplusplus
typedef class accounting accounting;
#else
typedef struct accounting accounting;
#endif /* __cplusplus */

#endif 	/* __accounting_FWD_DEFINED__ */


#ifndef __mvsapp_FWD_DEFINED__
#define __mvsapp_FWD_DEFINED__

#ifdef __cplusplus
typedef class mvsapp mvsapp;
#else
typedef struct mvsapp mvsapp;
#endif /* __cplusplus */

#endif 	/* __mvsapp_FWD_DEFINED__ */


#ifndef __chart_FWD_DEFINED__
#define __chart_FWD_DEFINED__

#ifdef __cplusplus
typedef class chart chart;
#else
typedef struct chart chart;
#endif /* __cplusplus */

#endif 	/* __chart_FWD_DEFINED__ */


#ifndef __rceditor_FWD_DEFINED__
#define __rceditor_FWD_DEFINED__

#ifdef __cplusplus
typedef class rceditor rceditor;
#else
typedef struct rceditor rceditor;
#endif /* __cplusplus */

#endif 	/* __rceditor_FWD_DEFINED__ */


#ifndef __integrator_FWD_DEFINED__
#define __integrator_FWD_DEFINED__

#ifdef __cplusplus
typedef class integrator integrator;
#else
typedef struct integrator integrator;
#endif /* __cplusplus */

#endif 	/* __integrator_FWD_DEFINED__ */


#ifndef __createvts_FWD_DEFINED__
#define __createvts_FWD_DEFINED__

#ifdef __cplusplus
typedef class createvts createvts;
#else
typedef struct createvts createvts;
#endif /* __cplusplus */

#endif 	/* __createvts_FWD_DEFINED__ */


#ifndef __instrreg_FWD_DEFINED__
#define __instrreg_FWD_DEFINED__

#ifdef __cplusplus
typedef class instrreg instrreg;
#else
typedef struct instrreg instrreg;
#endif /* __cplusplus */

#endif 	/* __instrreg_FWD_DEFINED__ */


#ifndef __prodcode_FWD_DEFINED__
#define __prodcode_FWD_DEFINED__

#ifdef __cplusplus
typedef class prodcode prodcode;
#else
typedef struct prodcode prodcode;
#endif /* __cplusplus */

#endif 	/* __prodcode_FWD_DEFINED__ */


#ifndef __tradeorg_FWD_DEFINED__
#define __tradeorg_FWD_DEFINED__

#ifdef __cplusplus
typedef class tradeorg tradeorg;
#else
typedef struct tradeorg tradeorg;
#endif /* __cplusplus */

#endif 	/* __tradeorg_FWD_DEFINED__ */


#ifndef __inflow_FWD_DEFINED__
#define __inflow_FWD_DEFINED__

#ifdef __cplusplus
typedef class inflow inflow;
#else
typedef struct inflow inflow;
#endif /* __cplusplus */

#endif 	/* __inflow_FWD_DEFINED__ */


#ifndef __nimbusconfig_FWD_DEFINED__
#define __nimbusconfig_FWD_DEFINED__

#ifdef __cplusplus
typedef class nimbusconfig nimbusconfig;
#else
typedef struct nimbusconfig nimbusconfig;
#endif /* __cplusplus */

#endif 	/* __nimbusconfig_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IPDCom_INTERFACE_DEFINED__
#define __IPDCom_INTERFACE_DEFINED__

/* interface IPDCom */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPDCom;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CCE84200-DB31-11CF-9D75-00A0C90391D3")
    IPDCom : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendMsg( 
            /* [in] */ BSTR stringMsg,
            /* [in] */ DWORD cpid) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPDComVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPDCom * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPDCom * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPDCom * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPDCom * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPDCom * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPDCom * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPDCom * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SendMsg )( 
            IPDCom * This,
            /* [in] */ BSTR stringMsg,
            /* [in] */ DWORD cpid);
        
        END_INTERFACE
    } IPDComVtbl;

    interface IPDCom
    {
        CONST_VTBL struct IPDComVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPDCom_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPDCom_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPDCom_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPDCom_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPDCom_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPDCom_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPDCom_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IPDCom_SendMsg(This,stringMsg,cpid)	\
    ( (This)->lpVtbl -> SendMsg(This,stringMsg,cpid) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPDCom_INTERFACE_DEFINED__ */


#ifndef __IPDComScript_INTERFACE_DEFINED__
#define __IPDComScript_INTERFACE_DEFINED__

/* interface IPDComScript */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPDComScript;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CC084200-DB31-11CF-9D75-00A0C90391D3")
    IPDComScript : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoCommand( 
            /* [in] */ BSTR cmdStr,
            /* [retval][out] */ VARIANT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LastMessage( 
            /* [retval][out] */ VARIANT *lastMsg) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPDComScriptVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPDComScript * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPDComScript * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPDComScript * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPDComScript * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPDComScript * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPDComScript * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPDComScript * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoCommand )( 
            IPDComScript * This,
            /* [in] */ BSTR cmdStr,
            /* [retval][out] */ VARIANT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LastMessage )( 
            IPDComScript * This,
            /* [retval][out] */ VARIANT *lastMsg);
        
        END_INTERFACE
    } IPDComScriptVtbl;

    interface IPDComScript
    {
        CONST_VTBL struct IPDComScriptVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPDComScript_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPDComScript_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPDComScript_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPDComScript_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPDComScript_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPDComScript_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPDComScript_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IPDComScript_DoCommand(This,cmdStr,result)	\
    ( (This)->lpVtbl -> DoCommand(This,cmdStr,result) ) 

#define IPDComScript_LastMessage(This,lastMsg)	\
    ( (This)->lpVtbl -> LastMessage(This,lastMsg) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPDComScript_INTERFACE_DEFINED__ */


#ifndef __IValCorrServer_INTERFACE_DEFINED__
#define __IValCorrServer_INTERFACE_DEFINED__

/* interface IValCorrServer */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IValCorrServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CC184200-DB31-11CF-9D75-00A0C90391D3")
    IValCorrServer : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VCOneTS( 
            /* [in] */ BSTR tscodeA,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VCAuto( 
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VCFile( 
            /* [in] */ VARIANT file,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VCSql( 
            /* [in] */ VARIANT Sqlcodes,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VCAbsEndOneTS( 
            /* [in] */ BSTR tscodeA,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VCAbsEndAuto( 
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VCAbsEndFile( 
            /* [in] */ VARIANT file,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VCAbsEndSql( 
            /* [in] */ VARIANT Sqlcodes,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IValCorrServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IValCorrServer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IValCorrServer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IValCorrServer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IValCorrServer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IValCorrServer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IValCorrServer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IValCorrServer * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VCOneTS )( 
            IValCorrServer * This,
            /* [in] */ BSTR tscodeA,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VCAuto )( 
            IValCorrServer * This,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VCFile )( 
            IValCorrServer * This,
            /* [in] */ VARIANT file,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VCSql )( 
            IValCorrServer * This,
            /* [in] */ VARIANT Sqlcodes,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VCAbsEndOneTS )( 
            IValCorrServer * This,
            /* [in] */ BSTR tscodeA,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VCAbsEndAuto )( 
            IValCorrServer * This,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VCAbsEndFile )( 
            IValCorrServer * This,
            /* [in] */ VARIANT file,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VCAbsEndSql )( 
            IValCorrServer * This,
            /* [in] */ VARIANT Sqlcodes,
            /* [in] */ BSTR TstartA,
            /* [in] */ BSTR TendA,
            /* [in] */ long valAndCorr,
            /* [retval][out] */ VARIANT *result);
        
        END_INTERFACE
    } IValCorrServerVtbl;

    interface IValCorrServer
    {
        CONST_VTBL struct IValCorrServerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IValCorrServer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IValCorrServer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IValCorrServer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IValCorrServer_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IValCorrServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IValCorrServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IValCorrServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IValCorrServer_VCOneTS(This,tscodeA,TstartA,TendA,valAndCorr,result)	\
    ( (This)->lpVtbl -> VCOneTS(This,tscodeA,TstartA,TendA,valAndCorr,result) ) 

#define IValCorrServer_VCAuto(This,TstartA,TendA,valAndCorr,result)	\
    ( (This)->lpVtbl -> VCAuto(This,TstartA,TendA,valAndCorr,result) ) 

#define IValCorrServer_VCFile(This,file,TstartA,TendA,valAndCorr,result)	\
    ( (This)->lpVtbl -> VCFile(This,file,TstartA,TendA,valAndCorr,result) ) 

#define IValCorrServer_VCSql(This,Sqlcodes,TstartA,TendA,valAndCorr,result)	\
    ( (This)->lpVtbl -> VCSql(This,Sqlcodes,TstartA,TendA,valAndCorr,result) ) 

#define IValCorrServer_VCAbsEndOneTS(This,tscodeA,TstartA,TendA,valAndCorr,result)	\
    ( (This)->lpVtbl -> VCAbsEndOneTS(This,tscodeA,TstartA,TendA,valAndCorr,result) ) 

#define IValCorrServer_VCAbsEndAuto(This,TstartA,TendA,valAndCorr,result)	\
    ( (This)->lpVtbl -> VCAbsEndAuto(This,TstartA,TendA,valAndCorr,result) ) 

#define IValCorrServer_VCAbsEndFile(This,file,TstartA,TendA,valAndCorr,result)	\
    ( (This)->lpVtbl -> VCAbsEndFile(This,file,TstartA,TendA,valAndCorr,result) ) 

#define IValCorrServer_VCAbsEndSql(This,Sqlcodes,TstartA,TendA,valAndCorr,result)	\
    ( (This)->lpVtbl -> VCAbsEndSql(This,Sqlcodes,TstartA,TendA,valAndCorr,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IValCorrServer_INTERFACE_DEFINED__ */



#ifndef __IPDComLib_LIBRARY_DEFINED__
#define __IPDComLib_LIBRARY_DEFINED__

/* library IPDComLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_IPDComLib;

EXTERN_C const CLSID CLSID_PDEchoServer;

#ifdef __cplusplus

class DECLSPEC_UUID("CCE84202-DB31-11CF-9D75-00A0C90391D3")
PDEchoServer;
#endif

EXTERN_C const CLSID CLSID_PDEchoClient;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48161-F2F3-11d1-9107-0000F80355F3")
PDEchoClient;
#endif

EXTERN_C const CLSID CLSID_navigatoruis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48162-F2F3-11d1-9107-0000F80355F3")
navigatoruis;
#endif

EXTERN_C const CLSID CLSID_solver;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48163-F2F3-11d1-9107-0000F80355F3")
solver;
#endif

EXTERN_C const CLSID CLSID_exchpntuis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48164-F2F3-11d1-9107-0000F80355F3")
exchpntuis;
#endif

EXTERN_C const CLSID CLSID_measpntuis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48165-F2F3-11d1-9107-0000F80355F3")
measpntuis;
#endif

EXTERN_C const CLSID CLSID_netareauis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48166-F2F3-11d1-9107-0000F80355F3")
netareauis;
#endif

EXTERN_C const CLSID CLSID_customeruis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48167-F2F3-11d1-9107-0000F80355F3")
customeruis;
#endif

EXTERN_C const CLSID CLSID_hvcuis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48168-F2F3-11d1-9107-0000F80355F3")
hvcuis;
#endif

EXTERN_C const CLSID CLSID_impexpuis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48169-F2F3-11d1-9107-0000F80355F3")
impexpuis;
#endif

EXTERN_C const CLSID CLSID_debituis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48170-F2F3-11d1-9107-0000F80355F3")
debituis;
#endif

EXTERN_C const CLSID CLSID_tspanel;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48171-F2F3-11d1-9107-0000F80355F3")
tspanel;
#endif

EXTERN_C const CLSID CLSID_calcdebit;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48172-F2F3-11d1-9107-0000F80355F3")
calcdebit;
#endif

EXTERN_C const CLSID CLSID_calcdelivery;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48173-F2F3-11d1-9107-0000F80355F3")
calcdelivery;
#endif

EXTERN_C const CLSID CLSID_calcreport;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48174-F2F3-11d1-9107-0000F80355F3")
calcreport;
#endif

EXTERN_C const CLSID CLSID_ediimp;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48175-F2F3-11d1-9107-0000F80355F3")
ediimp;
#endif

EXTERN_C const CLSID CLSID_ediexp;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48176-F2F3-11d1-9107-0000F80355F3")
ediexp;
#endif

EXTERN_C const CLSID CLSID_gs2imp;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48177-F2F3-11d1-9107-0000F80355F3")
gs2imp;
#endif

EXTERN_C const CLSID CLSID_gs2exp;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48178-F2F3-11d1-9107-0000F80355F3")
gs2exp;
#endif

EXTERN_C const CLSID CLSID_hydval;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48179-F2F3-11d1-9107-0000F80355F3")
hydval;
#endif

EXTERN_C const CLSID CLSID_hydcorr;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48180-F2F3-11d1-9107-0000F80355F3")
hydcorr;
#endif

EXTERN_C const CLSID CLSID_pdaudituis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48181-F2F3-11d1-9107-0000F80355F3")
pdaudituis;
#endif

EXTERN_C const CLSID CLSID_activityloguis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48182-F2F3-11d1-9107-0000F80355F3")
activityloguis;
#endif

EXTERN_C const CLSID CLSID_tsrserver;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48183-F2F3-11d1-9107-0000F80355F3")
tsrserver;
#endif

EXTERN_C const CLSID CLSID_tsrclient;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48184-F2F3-11d1-9107-0000F80355F3")
tsrclient;
#endif

EXTERN_C const CLSID CLSID_logonuis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48185-F2F3-11d1-9107-0000F80355F3")
logonuis;
#endif

EXTERN_C const CLSID CLSID_tsriclient;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48186-F2F3-11d1-9107-0000F80355F3")
tsriclient;
#endif

EXTERN_C const CLSID CLSID_tsrviewexcelfile;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48187-F2F3-11d1-9107-0000F80355F3")
tsrviewexcelfile;
#endif

EXTERN_C const CLSID CLSID_pdlog;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48188-F2F3-11d1-9107-0000F80355F3")
pdlog;
#endif

EXTERN_C const CLSID CLSID_tsrstartreport;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48189-F2F3-11d1-9107-0000F80355F3")
tsrstartreport;
#endif

EXTERN_C const CLSID CLSID_tsrviewhtmlfile;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48190-F2F3-11d1-9107-0000F80355F3")
tsrviewhtmlfile;
#endif

EXTERN_C const CLSID CLSID_tssearch;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48191-F2F3-11d1-9107-0000F80355F3")
tssearch;
#endif

EXTERN_C const CLSID CLSID_ester;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48192-F2F3-11d1-9107-0000F80355F3")
ester;
#endif

EXTERN_C const CLSID CLSID_regmeasuis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48193-F2F3-11d1-9107-0000F80355F3")
regmeasuis;
#endif

EXTERN_C const CLSID CLSID_mvsreport;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48194-F2F3-11d1-9107-0000F80355F3")
mvsreport;
#endif

EXTERN_C const CLSID CLSID_tsrviewpsfile;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48195-F2F3-11d1-9107-0000F80355F3")
tsrviewpsfile;
#endif

EXTERN_C const CLSID CLSID_kisclient;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48196-F2F3-11d1-9107-0000F80355F3")
kisclient;
#endif

EXTERN_C const CLSID CLSID_kisdataload;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48197-F2F3-11d1-9107-0000F80355F3")
kisdataload;
#endif

EXTERN_C const CLSID CLSID_structloaduis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48198-F2F3-11d1-9107-0000F80355F3")
structloaduis;
#endif

EXTERN_C const CLSID CLSID_meteruis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48200-F2F3-11d1-9107-0000F80355F3")
meteruis;
#endif

EXTERN_C const CLSID CLSID_sim;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48201-F2F3-11d1-9107-0000F80355F3")
sim;
#endif

EXTERN_C const CLSID CLSID_hbv;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48202-F2F3-11d1-9107-0000F80355F3")
hbv;
#endif

EXTERN_C const CLSID CLSID_biduis;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48203-F2F3-11d1-9107-0000F80355F3")
biduis;
#endif

EXTERN_C const CLSID CLSID_tshyd;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48204-F2F3-11d1-9107-0000F80355F3")
tshyd;
#endif

EXTERN_C const CLSID CLSID_simnt;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48205-F2F3-11d1-9107-0000F80355F3")
simnt;
#endif

EXTERN_C const CLSID CLSID_accounting;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48206-F2F3-11d1-9107-0000F80355F3")
accounting;
#endif

EXTERN_C const CLSID CLSID_mvsapp;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48207-F2F3-11d1-9107-0000F80355F3")
mvsapp;
#endif

EXTERN_C const CLSID CLSID_chart;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48208-F2F3-11d1-9107-0000F80355F3")
chart;
#endif

EXTERN_C const CLSID CLSID_rceditor;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48209-F2F3-11d1-9107-0000F80355F3")
rceditor;
#endif

EXTERN_C const CLSID CLSID_integrator;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48210-F2F3-11d1-9107-0000F80355F3")
integrator;
#endif

EXTERN_C const CLSID CLSID_createvts;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48211-F2F3-11d1-9107-0000F80355F3")
createvts;
#endif

EXTERN_C const CLSID CLSID_instrreg;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48212-F2F3-11d1-9107-0000F80355F3")
instrreg;
#endif

EXTERN_C const CLSID CLSID_prodcode;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48213-F2F3-11d1-9107-0000F80355F3")
prodcode;
#endif

EXTERN_C const CLSID CLSID_tradeorg;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48214-F2F3-11d1-9107-0000F80355F3")
tradeorg;
#endif

EXTERN_C const CLSID CLSID_inflow;

#ifdef __cplusplus

class DECLSPEC_UUID("43C48251-F2F3-11d1-9107-0000F80355F3")
inflow;
#endif

EXTERN_C const CLSID CLSID_nimbusconfig;

#ifdef __cplusplus

class DECLSPEC_UUID("486B027F-2483-4DC9-B90D-DE47C7855ADF")
nimbusconfig;
#endif
#endif /* __IPDComLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize64(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal64(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree64(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


