#
SNIFF_MAKE_DIR=.sniffdir

include .sniffdir/macros.incl

IMPLEMENTATION_DIR = $(SNIFF_Implementation_DIR)
XINCLUDE           = $(SNIFF_INCLUDE)
LINK_TARGET       = $(SNIFF_LINK_TARGET)
RELINK_TARGET     = $(SNIFF_RELINK_TARGET)
LIB_TARGET        = $(SNIFF_LIB_TARGET)
SUB_LIBS          = $(SNIFF_SUB_LIBS)
SUB_RELINK_OFILES = $(SNIFF_SUB_RELINK_OFILES)
OTHER_LIBS        = $(SNIFF_OTHER_LIBS)
RELINK_OTHER_LIBS = $(SNIFF_RELINK_OTHER_LIBS)
PRE_TARGETS     = $(SNIFF_PRE_TARGETS)
ifdef SNIFF_DIR4
MAKE_SUPPORT=$(BUILD_ROOT)/iccruntime/build
else
SUBDIRS           = $(SNIFF_SUBDIRS)
endif
include $(MAKE_SUPPORT)/general.mk
include $(SNIFF_MAKEDIR)/$(SNIFF_DEPEND_INCL)

ifndef SNIFF_DIR4
OTHER_CFLAGS    = $(SLIB_CFLAGS) 
OTHER_CXXFLAGS  = $(SLIB_CCFLAGS)
DEBUG_FLAGS= -O
ifdef windir
ifndef OBJECT_DIR
OBJECT_DIR=.
endif
$(OBJECT_DIR)/%$O:%.C;$(ALL_CXX_CMD)
endif
#
# Compability library: libicc_dig.a
# Currently, this should be done as a manual operation when
# building SO
#

libicc_dig.a: TBEx/libTBEx.a TB/libTB.a IccGet/libIccGet.a
	@echo Rebuilding compability library icc_dig from TBEx TB and IccGet libs
	$(RM) libicc_dig.a
	$(AR) $(ARFLAGS) libicc_dig.a TBEx/*.o TB/*.o IccGet/*.o
	$(RANLIB) libicc_dig.a
	
#
# This part of Makefile is for the $(DIG) code generator,
# 
#
# This has to be done in two Makefiles because dig.mk includes
# $(OUTPUT_FILE) produced by dig.
#

DIG= $(ICC_HOME)/tools/bin/dig
INPUT_FILE= $(ICC_SCHEMA_PATH)/$(ICC_SCHEMA_VERSION)/common/icc.dig
OUTPUT_FILE=GENERATE_DIG_FILES
GENERATE_DIG_CMD=	$(DIG) -I ../../../include/dig_h -x TSERI_VL \
	-n AGHYD_DS -n CATMAREA \
	-n HYDGENUN -n HYDPLANT -n HYDTURB -n RESERVOI \
	-n WTGATE -n WTROUJUN -n WTROUTE -n BUSBAR \
	-n TRANSLIN -n TRAFO_2W -n GENERATO -n BREAKER \
	-v TIMESER -v HOUR_VL -v PRRUN_DS $(INPUT_FILE)

$(OUTPUT_FILE): $(INPUT_FILE) $(DIG) Makefile
	@if [ "$(BUILD_DIG)" = "YES" ]; then \
	    @echo "Generating new code for `pwd` using dig...";\
	    $(GENERATE_DIG_CMD);\
	    rm -f $(LIBNAME);\
	else\
	    echo "***WARNING:NOTHING DONE\nDefine BUILD_DIG=YES to actually do this:";\
	    echo "***This feature should be run in shared source";\
	    echo "***The command would be :" $(GENERATE_DIG_CMD);\
	fi;
endif
