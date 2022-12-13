PRODIR=icc/ConfigurationSystem/CfgServer
RELDIR=../../..
BLDDIR=iccruntime/build

PCH_FILE=stdafx
PCH_CXXEXT=cpp
RC_RULES=1
RESFILES=$(TARGET_DIR)/CfgServer.res
PDCOMRESFILTER=../../../icc/api/PDCom/RC/$(OBJECT_DIR)/allpdcomrc.res
include $(RELDIR)/$(BLDDIR)/lib.mk

IDL_GENERATED_FILES= _CfgServer.h _CfgServer_i.c _CfgServer_p.c _CfgServer.tlb

$(OBJECT_FILES):$(IDL_GENERATED_FILES)
vpath %.rc .;$(RELDIR)/icc/api/PDCom/RC
$(RESFILES): CfgServer.rc version.rc CompanyStringFileInfo.rc

CXXFLAGS += -D_ATL_ATTRIBUTES -D_VC80_UPGRADE=0x0710
RCFLAGS += -D_ATL_ATTRIBUTES -D_VC80_UPGRADE=0x0710 -I$(OBJECT_DIR) -l 0x409
OS_LDFLAGS = /IDLOUT:_CfgServer.idl /IMPLIB:$(OBJECT_DIR)/CfgServer.lib /OPT:REF
OS_LIBS= $(OS_RTL_LIBS)  $(ORACLE_LIBS)
CLEANUP += *.tlb *.res $(IDL_GENERATED_FILES)
CXX_RTL = -MT -D_NODEBUG 
