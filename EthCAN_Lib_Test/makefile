
# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      EthCAN_Lib_Test/makefile

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT)

LIBRARIES = ../Libraries/EthCAN_Lib.a $(KMS_LIB_A)

OUTPUT = ../Binaries/EthCAN_Lib_Test

SOURCES =				\
	Device.cpp			\
	Display.cpp			\
	EthCAN_Lib_Test.cpp \
	File.cpp            \
	System.cpp

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

Device.o: Component.h ../Import/Includes/KmsBase.h
Device.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Device.o: ../Import/Includes/KmsTest.h ../Includes/EthCAN/Device.h
Device.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN_Types.h
Device.o: ../Includes/EthCAN/Object.h ../Includes/EthCAN/System.h
Device.o: ../Includes/EthCAN_Protocol.h
Display.o: Component.h ../Import/Includes/KmsBase.h
Display.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Display.o: ../Import/Includes/KmsTest.h ../Includes/EthCAN/Display.h
Display.o: ../Includes/EthCAN_Protocol.h ../Includes/EthCAN_Result.h
Display.o: ../Includes/EthCAN_Types.h
EthCAN_Lib_Test.o: Component.h ../Import/Includes/KmsBase.h
EthCAN_Lib_Test.o: ../Import/Includes/SafeAPI.h
EthCAN_Lib_Test.o: ../Import/Includes/WindowsToLinux.h
EthCAN_Lib_Test.o: ../Import/Includes/KmsTest.h
System.o: Component.h ../Import/Includes/KmsBase.h
System.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
System.o: ../Import/Includes/KmsTest.h ../Includes/EthCAN/Device.h
System.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN_Types.h
System.o: ../Includes/EthCAN/Object.h ../Includes/EthCAN/System.h
System.o: ../Includes/EthCAN_Protocol.h ../Common/Version.h
