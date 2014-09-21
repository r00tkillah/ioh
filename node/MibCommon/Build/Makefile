############################################################################
#
# MODULE              JN-AN-1162 JenNet-IP Smart Home
#
# DESCRIPTION         MibCommon Library Makefile
#
############################################################################
#
# This software is owned by NXP B.V. and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on NXP products [NXP Microcontrollers such as JN5148, JN5142, JN5139]. 
# You, and any third parties must reproduce the copyright and warranty notice
# and any other legend of ownership on each copy or partial copy of the 
# software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Copyright NXP B.V. 2012. All rights reserved
#
############################################################################
# Application target name
TARGET = MibCommon

##############################################################################
# Path definitions
# Select definitions for either single or multiple targets

# Use if application directory contains multiple targets
SDK_BASE_DIR   	 	= $(abspath ../../../..)
APP_BASE            	= $(abspath ../..)
APP_BLD_DIR		= $(APP_BASE)/$(TARGET)/Build
APP_SRC_DIR 	        = $(APP_BASE)/$(TARGET)/Library
APP_INC_DIR		= $(APP_BASE)/$(TARGET)/Include
# APP_LIB_DIR 	        = $(APP_BASE)/Library
APP_LIB_DIR 	        = $(APP_BASE)/$(TARGET)/Build

##############################################################################
# User definable make parameters that may be overwritten from the command line

# Optional config.mk for network id and channel
sinclude $(APP_BASE)/Common/Build/config.mk

# Default target device is the JN5148
JENNIC_CHIP ?= JN5148

# Default DK2 development kit target hardware
JENNIC_PCB ?= DEVKIT2

# Select the network stack (e.g. MAC, ZBPRO)
JENNIC_STACK ?= None

# Define TRACE to use with DBG module
#TRACE ?=1

# Debug options define DEBUG for HW debug
#DEBUG ?=HW

# Define which UART to use for debug
DEBUG_PORT ?= UART0

# Library build
CFLAGS += -DLIBRARY_BUILD

# Zero version if not passed in on the command line 
VERSION ?= 0

# Need to create a version number ? 
ifeq ($(VERSION), 0)
# Get touched time
TEMP := $(shell touch touch.txt)
TOUCHED := $(word 6, $(shell ls -l --time-style=+%w%H%M touch.txt))
VERSION := $(TOUCHED)
else ifeq ($(VERSION), 1)
# Get touched time
TEMP := $(shell touch touch.txt)
TOUCHED := $(word 6, $(shell ls -l --time-style=+%y%m%d touch.txt))
VERSION := $(TOUCHED)
else
TOUCHED := 0
endif

# Set makefile defines 
CFLAGS += -DMK_TARGET=\"$(TARGET)\"
CFLAGS += -DMK_VERSION=\"$(VERSION)\"
CFLAGS += -DEMBEDDED
CFLAGS += -DUSER_VSR_HANDLER
CFLAGS += -fno-common
ifeq ($(TRACE), 1)
CFLAGS += -DDBG_ENABLE
BUILD = _DEBUG
endif

##############################################################################
# Application Source files

# Note: Path to source file is found using vpath below, so only .c filename is required
APPSRC += MibGroup.c
APPSRC += MibNode.c
APPSRC += MibAdcStatus.c
APPSRC += MibNodeStatus.c
APPSRC += MibNodeControl.c
APPSRC += MibNwkStatus.c
APPSRC += MibNwkConfig.c
APPSRC += MibNwkSecurity.c
APPSRC += Table.c
APPSRC += MibCommon_patch.S

##############################################################################
# Standard Application header search paths
INCFLAGS += -I$(APP_SRC_DIR)
INCFLAGS += -I$(APP_INC_DIR)

# Application specific include files
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/DBG/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/JenNet/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/6LP/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/OS/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/PDM/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Recal/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/JIP/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Utilities/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Aes/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/PATCH/Include

##############################################################################
# You should not need to edit below this line

##############################################################################
# Configure for the selected chip or chip family

include $(SDK_BASE_DIR)/Platform/Common/Build/Config.mk
include $(SDK_BASE_DIR)/Chip/Common/Build/config.mk
include $(SDK_BASE_DIR)/Stack/Common/Build/config.mk

##############################################################################

TEMPOBJS = $(APPSRC:.c=.o)
APPOBJS = $(TEMPOBJS:.S=.o)

##############################################################################
# Application dynamic dependencies

APPDEPS = $(APPOBJS:.o=.d)

#########################################################################

ifeq ($(BUILD), ROM)
CFLAGS += -fno-data-sections -fno-function-sections
CFLAGS += -DPATCH_ENABLE
CFLAGS += -DROM_CORE_LIBRARY
endif

#########################################################################
# Archiver

ARFLAGS = rcs

#########################################################################
# Dependency rules

.PHONY: all clean
# Path to directories containing application source 
vpath %.c $(APP_SRC_DIR)
vpath %.S $(APP_SRC_DIR)

all: $(APP_LIB_DIR)/lib$(TARGET)_$(JENNIC_CHIP_FAMILY)$(BUILD).a

-include $(APPDEPS)
%.d:
	rm -f $*.o


%.o: %.S
	$(info Assembling $< ...)
	$(CC) -c -o $(subst Source,Build,$@) $(CFLAGS) $(INCFLAGS) $< -MD -MF $*.d -MP
	@echo

%.o: %.c 
	$(info Compiling $< ...)
	$(CC) -c -o $(subst Source,Build,$@) $(CFLAGS) $(INCFLAGS) $< -MD -MF $*.d -MP
	@echo

$(APP_LIB_DIR)/lib$(TARGET)_$(JENNIC_CHIP_FAMILY)$(BUILD).a: $(APPOBJS)
	$(info Archiving $@ ...)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(APPOBJS)
	$(SIZE) -t $@
	
#########################################################################

clean:
	rm -f $(APP_LIB_DIR)/lib$(TARGET)_$(JENNIC_CHIP_FAMILY)$(BUILD).a
	rm -f *.o
	rm -f *.d

#########################################################################
