#
# Copyright 2013-2021 Robert Newgard
#
# This file is part of SyscFCBus.
#
# SyscFCBus is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SyscFCBus is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SyscFCBus.  If not, see <http://www.gnu.org/licenses/>.
#

SYSCMAKE  := ../../../SyscMake

include $(SYSCMAKE)/vars.mk

# accumulated variables
ACCUM_BLD_LIBS         +=
ACCUM_CPP_INCLUDES     +=
ACCUM_CPP_OPTS         +=
ACCUM_INTERMEDIATE     +=
ACCUM_LINKER_LIBS      +=
ACCUM_LINKER_LIB_DIRS  +=
ACCUM_PHONY_TARGS      +=
ACCUM_PREREQ_BLD       +=
ACCUM_PREREQ_CLEAN     +=
ACCUM_PREREQ_HELP      +=
ACCUM_PREREQ_LIB       +=
ACCUM_PREREQ_SIM       +=
ACCUM_PYTHONPATH       += /home/bobn/git/SyscDrv
ACCUM_SIM_LIB_DIRS     +=
ACCUM_VLTR_OPTS        +=

# specify library sources
define lib-source
        test.cxx
        sc_main.cxx
endef
LIB_SRC := $(strip $(lib-source))

# accumulate external header specifications here
# * these specs add -I args to gcc
#------------|------- build-process -|------- hdr-dir ------------|
$(eval $(call $(strip hdr-spec      ),$(strip ../../../SyscClk   )))
$(eval $(call $(strip hdr-spec      ),$(strip ../../../SyscFCBus )))

# accumulate external library specifications here
# * these specs add -I, -L and -l args to gcc
#------------|------- build-process -|------- lib-dir ------------|------- lib-name -------|
$(eval $(call $(strip lib-spec      ),$(strip ..                 ),$(strip tb0          )))
$(eval $(call $(strip lib-spec      ),$(strip ../../../SyscMsg   ),$(strip syscmsg      )))
$(eval $(call $(strip lib-spec      ),$(strip ../../../SyscDrv   ),$(strip syscdrv      )))
$(eval $(call $(strip lib-spec      ),$(strip ../../../SyscJson  ),$(strip syscjson     )))

# instantiate local library build processes here
# * Name must be unique within project
#------------|------- build-process -|------- name -----|------- sources --------|
$(eval $(call $(strip build-sysc-lib),$(strip test00   ),$(strip $(LIB_SRC)     )))

# instantiate local executable build processes here
# * Name must be unique within project
#------------|------- build-process -|------- name -----|
$(eval $(call $(strip build-exe     ),$(strip exe00    )))

include $(SYSCMAKE)/targ.mk

# add auxiliary targets for testbench here, if needed
