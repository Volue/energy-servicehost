PRODIR=icc/api/PDCom
RELDIR=../../..
MKDIR=iccruntime/build
PCH_FILE=PrePDCom
PCH_CXXEXT=cpp

include $(RELDIR)/$(MKDIR)/lib.mk
TLB_FILE=IPDCom.tlb
IDL_GENERATED_FILES= IPDCom.h IPDCom_i.c IPDCom_p.c dlldata.c $(TLB_FILE)
IDL_CMD= XINCLUDE='$(ENV_INCLUDE)';export XINCLUDE;chmod +w $(IDL_GENERATED_FILES);midl /nologo /char signed /env win32 /Oicf IPDCom.idl /h IPDCom.h /tlb $(TLB_FILE) 
vpath %.idl $(SOURCE_VPATH)
idl: $(IDL_GENERATED_FILES) Makefile; @echo Done 
$(IDL_GENERATED_FILES): IPDCom.idl ;@$(IDL_CMD)
$(PCH_PCHFILE):$(IDL_GENERATED_FILES)
$(OBJECT_FILES):$(IDL_GENERATED_FILES)
CLEANUP += *.tlb *.res $(IDL_GENERATED_FILES)
