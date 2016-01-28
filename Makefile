#############################################################################
#
# Makefile for IoT Domotics on Raspberry Pi
#
# Orginal file used from:
# License: GPL (General Public License)
# Author:  gnulnulf <arco@appeltaart.mine.nu>
# Date:    2013/02/07 (version 1.0)
#
# Adjusted by:  Michel Roke
# Date:         2016/01/11
#
# Description:
# ------------
# Use make or make all to create an executable to run
# Use make clean to remove the files created by make
# You can change the install directory by editing the prefix line
# make install is disabled until further notice
#
prefix := /usr/local

# Detect the Raspberry Pi by the existence of the bcm_host.h file
BCMLOC = /opt/vc/include/bcm_host.h

ifneq ("$(wildcard $(BCMLOC))","")
# The recommended compiler flags for the Raspberry Pi
CCFLAGS = -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
endif

# define all programs
PROGRAM = iotMain
OBJECTS = iotMain.o iotComMan.o iotNRFCom.o iotDebug.o iotSocketCom.o
SOURCES = iotMain.cpp iotComMan.cpp iotNRFCom.cpp iotDebug.cpp iotSocketCom.cpp
HEADER = iotInit.h iotComMan.h iotNRFCom.h iotDebug.h iotSocketCom.h

all: ${PROGRAM}

${PROGRAM}: ${OBJECTS}
	g++ ${CCFLAGS} -Wall -I./ -lrf24-bcm -o ${PROGRAM} ${OBJECTS}

${OBJECTS}: ${HEADER}

.PHONY: install clean

clean:
	rm -rf $(PROGRAM) $(OBJECTS)
    
# Remnants of the prototype makefile I used
# Will be altered at a later stadium for the final product
# install: all
# 	test -d $(prefix) || mkdir $(prefix)
# 	test -d $(prefix)/bin || mkdir $(prefix)/bin
# 	for prog in $(PROGRAMS); do \
# 		install -m 0755 $$prog $(prefix)/bin; \
# 	done
