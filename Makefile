# Project Name
TARGET = soundpad

# Sources
CPP_SOURCES = main.cpp sample.cpp sp_ui.cpp controls.cpp playback.cpp

# Library Locations
LIBDAISY_DIR = ./libDaisy
DAISYSP_DIR = ./DaisySP

# Includes FatFS source files within project.
USE_FATFS = 1

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

#LDFLAGS += -u _printf_float #allows floats in printline()

include $(SYSTEM_FILES_DIR)/Makefile

