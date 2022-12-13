#      $Revision: 2.6 $ $Date: 1997-06-16 08:01:25 $
#
# This is a make file to create and maintain a system.
#
# EFI 4/6-92:  Aasmund Botnen
#     28/1-93: Generalized version: Roland Kruse
#

include $(MAKE_INCLUDE)/icc_default.mk.$(ICC_TARGET)
#
# Only subdirectories containing [Mm]akefile will be visited.
#
# If make order is of no importance, neither FIRST nor LAST macros
# need be specified.
#
# If FIRST and/or LAST are specified, the directories specified will be made
# first/last, and in the specified order. Directories not specified in
# FIRST/LAST will be made in between in random order.
#
# Specify EXCLUDE if any directories are not to be made.
#
# Note: if FIRST/LAST are empty they must be set to "".
#

# MAKE ORDER SPECIFICATIONS
#
# Specify FIRST if one or more subdirectories must be made FIRST
# example:
#FIRST = 

#
# Specify LAST if one or more subdirectories must be made LAST
# example:
#LAST = executables
#

# EXCLUDED DIRECTORIES
#
# Specifiy EXCLUDE if one or more directories are to be skipped.
# example:
# EXCLUDE=

########## The part below this line should normally not be changed #########

all .DEFAULT:
	@$(MAKE) First Subsystems Last TARGET=$@

include $(MAKE_INCLUDE)/icc_sysrules.mk


