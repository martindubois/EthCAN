
# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      EthCAN_Tool/makefile

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT)

LIBRARIES = ../Libraries/EthCAN_Lib.a $(KMS_LIB_A)

OUTPUT = ../Binaries/EthCAN_Tool

SOURCES =			\
	EthCAN_Tool.cpp \
	Setup.cpp

# ===== Rules / Regles =======================================================

.cpp.o:
	g++ -c $(COMPILE_FLAGS) -o $@ $(INCLUDES) $<

# ===== Macros ===============================================================

OBJECTS = $(SOURCES:.cpp=.o)

# ===== Targets / Cibles =====================================================

$(OUTPUT) : $(OBJECTS) $(LIBRARIES)
	g++ -pthread -o $@ $^

depend:
	makedepend -Y $(INCLUDES) $(SOURCES)
	
# DO NOT DELETE

EthCAN_Tool.o: Component.h ../Import/Includes/KmsBase.h
EthCAN_Tool.o: ../Import/Includes/SafeAPI.h
EthCAN_Tool.o: ../Import/Includes/WindowsToLinux.h
EthCAN_Tool.o: ../Import/Includes/KmsLib/ToolBase.h
EthCAN_Tool.o: ../Import/Includes/KmsLib/Exception.h
EthCAN_Tool.o: ../Import/Includes/KmsTool.h ../Includes/EthCAN/File.h
EthCAN_Tool.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN_Types.h
EthCAN_Tool.o: ../Includes/EthCAN/Display.h ../Includes/EthCAN_Protocol.h
EthCAN_Tool.o: ../Common/Version.h Setup.h ../Includes/EthCAN/Device.h
EthCAN_Tool.o: ../Includes/EthCAN/Object.h ../Includes/EthCAN/System.h
Setup.o: Component.h ../Import/Includes/KmsBase.h
Setup.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Setup.o: ../Includes/EthCAN/Display.h ../Includes/EthCAN_Protocol.h
Setup.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN_Types.h Setup.h
Setup.o: ../Includes/EthCAN/Device.h ../Includes/EthCAN/Object.h
Setup.o: ../Includes/EthCAN/System.h
