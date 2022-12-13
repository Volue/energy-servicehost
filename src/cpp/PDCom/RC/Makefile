# $Id: Makefile,v 1.11 2008-02-21 11:01:05 jma Exp $
# Standard Project Makefile
SNIFF_MAKEDIR=.sniffdir
include .sniffdir/macros.incl

IMPLEMENTATION_DIR = $(SNIFF_Implementation_DIR)
XINCLUDE           =  $(SNIFF_INCLUDE) 
LINK_TARGET       = $(SNIFF_LINK_TARGET)
RELINK_TARGET     = $(SNIFF_RELINK_TARGET)
LIB_TARGET        = libdummyrc.a 
#$(SNIFF_LIB_TARGET)
SUB_LIBS          = $(SNIFF_SUB_LIBS)
SUB_RELINK_OFILES = $(SNIFF_SUB_RELINK_OFILES)
OTHER_LIBS        = $(SNIFF_OTHER_LIBS)
RELINK_OTHER_LIBS = $(SNIFF_RELINK_OTHER_LIBS)
RC_RULES=TRUE
PRE_TARGETS     = $(SNIFF_PRE_TARGETS)
ifdef SNIFF_DIR4
MAKE_SUPPORT=$(BUILD_ROOT)/iccruntime/build
else
SUBDIRS           = $(SNIFF_SUBDIRS)
endif

include $(MAKE_SUPPORT)/general.mk
include $(SNIFF_MAKEDIR)/$(SNIFF_DEPEND_INCL)

ifdef windir
#RESFILES=echoserver.res
ifndef TARGET_DIR
TARGET_DIR=.
endif
OBJFILES=$(TARGET_DIR)/allpdcomrc.res
OFILES=$(TARGET_DIR)/allpdcomrc.res
vpath %.ico $(SOURCE_VPATH)
SNIFF_Windows_Registry_Script_FILES= *.rgs
$(TARGET_DIR)/allpdcomrc.res: allpdcomrc.rc  $(SNIFF_Windows_Registry_Script_FILES) ../IPDCom.tlb ../allpdcomrc.h ilvicon.ico
ifneq ("$(TARGET_DIR)",".") 
allpdcomrc.res:$(TARGET_DIR)/allpdcomrc.res;
endif
../IPDCom.tlb :
		cd ..;gmake idl
else
# just an empty library, should be filtered out by
# the general.mk when composing the library list
# since it does not end on .a or .so or .sl
#
allpdcomrc.res:
		touch allpdcomrc.res
endif
