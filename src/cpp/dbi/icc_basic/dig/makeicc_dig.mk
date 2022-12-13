#      $Revision: 31063 $ $Date: 2003-01-12 16:17:52 +0000 (Sun, 12 Jan 2003) $
#
# EFI 8/2-93  Odd Broenner, Roland Kruse
ifeq ($(PURESNIFFMAKE),YES)
include makeicc_dig.mk
else

#
# This Makefile first runs the $(DIG) code generator, next runs dig.mk to
# compile the source files.
#
# This has to be done in two Makefiles because dig.mk includes
# $(OUTPUT_FILE) produced by dig.
#

DIG= d:/projects//HEAD/iccruntime/bin/dig
INPUT_FILE= $(ICC_SCHEMA_PATH)/$(ICC_SCHEMA_VERSION)/common/icc.dig
OUTPUT_FILE=IccSourceList
LIBNAME=libicc_dig.a

# include $(MAKE_INCLUDE)/icc_default.mk.$(ICC_TARGET)

all: $(OUTPUT_FILE)
	$(MAKE) -f dig.mk CCFLAGS=$(MAX_OPT.C) $*

$(OUTPUT_FILE): $(INPUT_FILE)
	@echo "Generating new code for `pwd` using dig..."
	$(DIG) -I ../../../include/dig_h \
	-n AGHYD_DS -n CATMAREA \
	-n HYDGENUN -n HYDPLANT -n HYDTURB -n RESERVOI \
	-n WTGATE -n WTROUJUN -n WTROUTE -n BUSBAR \
	-n TRANSLIN -n TRAFO_2W -n GENERATO -n BREAKER \
	-v TIMESER -v HOUR_VL -v PRRUN_DS $(INPUT_FILE)
	-rm -f $(LIBNAME)

clean:
	-rm *.C *.o *.cyi $(OUTPUT_FILE) $(LIBNAME)
endif
