#      $Revision: 12571 $ $Date: 1997-12-18 09:20:08 +0000 (Thu, 18 Dec 1997) $
# 
# This is a template make file to create and maintain a shared library.
# It assumes a directory organisation as described in AN 92.20.66
# Fill in the blanks in the INPUT SECTION. A short explanation is
# given above each entry.
#
# EFI 4/6-92 Aasmund Botnen, Bjoern Frode Langoeren

#*** START INPUT SECTION **************************************************

# 	Define the NAME of current system, i.e. one of the directory names
# 	defined below $(ICC_HOME), f.ex. uis, elcbas, ...  .
# 	SUBNAME is the name component of the actual library, 
#	f.ex : SUBNAME = test  means a that the shared library
#	consisting of the files libtest.so.x.x and libtest.sa.x.x is the target
NAME	= dbi
SUBNAME	= icc_dig
VERSION	= 1.0

# 	If the current system has no subdirectories below src (excl. SCCS),
# 	i.e. the library source is on src directory, 
# 	then set UP = .. and DOWN = ../src
# 	OBS ! Default value for DOWN means that library source is on directory 
# 	src/SUBNAME. 
UP	= ../../..
DOWN	= ../src/icc_basic/dig

# 	Define object files that form this library, ex : file1.o file2.o
#	Associate file with symbol with extension equal to type of source.
#	Object files with static data should also be defined in DATA_OBJECTS
OBJECTS_c =

include ./IccSourceList

OBJECTS_f =
OBJECTS_F =
# 	Define objects that contains static data
DATA_OBJECTS=Dummy.o

# 	Read general ICC flags and macro definitions
include $(MAKE_INCLUDE)/icc_default.mk.$(ICC_TARGET)

#
# The below macros are for include paths and UIL options.
#
CPPFLAGS = $(INCLUDE_PATH) -I$(UP)/include/dig_h \
           -I$(ICC_HOME)/include/dig_h

UILFLAGS = $(INCLUDE_PATH)

#
# Compilation flags. Do _not_ set -g, -O, etc. here, these are controlled
# from the environment.
#
_CFLAGS	 = 
_CCFLAGS = 
_FFLAGS  =

#*** END INPUT SECTION **************************************************

all	: lib$(SUBNAME).a $(UP)/lib/lib$(SUBNAME).a

#	Read common icc rules.
#	Rules overriding the common icc rules can be written before this
#	file is included, or the include command can be commented out. 
include $(MAKE_INCLUDE)/icc_rules.mk.$(ICC_TARGET)
