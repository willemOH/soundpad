# Project Name
TARGET = soundpad

# Sources
CPP_SOURCES = main.cpp sample.cpp sp_ui.cpp controls.cpp

# Library Locations
LIBDAISY_DIR = ./libDaisy
DAISYSP_DIR = ./DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

#LDFLAGS += -u _printf_float #allows floats in printline()

include $(SYSTEM_FILES_DIR)/Makefile

