
# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      EthCAN_Lib/makefile

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT)

OUTPUT = ../Libraries/EthCAN_Lib.a

SOURCES =				\
	Device_Impl.cpp		\
	Device.cpp			\
	Display.cpp     	\
	File.cpp			\
	L_OS.cpp            \
	L_Serial.cpp		\
	L_System_Impl.cpp	\
	L_Thread.cpp		\
	L_UDPSocket.cpp		\
	Object.cpp			\
	Serial.cpp      	\
	System_Impl.cpp 	\
	System.cpp			\
	Thread.cpp			\
	UDPSocket.cpp

# ===== Rules / Regles =======================================================

.cpp.o:
	g++ -c $(COMPILE_FLAGS) -o $@ $(INCLUDES) $<

# ===== Macros ===============================================================

OBJECTS = $(SOURCES:.cpp=.o)

# ===== Targets / Cibles =====================================================

$(OUTPUT) : $(OBJECTS)
	ar r $@ $^
	ranlib $@

depend:
	makedepend -Y $(INCLUDES) $(SOURCES)
	
# DO NOT DELETE

Device_Impl.o: Component.h ../Import/Includes/KmsBase.h
Device_Impl.o: ../Import/Includes/SafeAPI.h
Device_Impl.o: ../Import/Includes/WindowsToLinux.h
Device_Impl.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN_Protocol.h
Device_Impl.o: ../Includes/EthCAN/Display.h ../Includes/EthCAN_Types.h
Device_Impl.o: ../Includes/EthCAN/System.h ../Includes/EthCAN/Object.h
Device_Impl.o: Serial.h IMessageReceiver.h Thread.h UDPSocket.h Device_Impl.h
Device_Impl.o: ../Includes/EthCAN/Device.h
Device.o: Component.h ../Import/Includes/KmsBase.h
Device.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Device.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN/Device.h
Device.o: ../Includes/EthCAN_Types.h ../Includes/EthCAN/Object.h
Display.o: Component.h ../Import/Includes/KmsBase.h
Display.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Display.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN/Display.h
Display.o: ../Includes/EthCAN_Protocol.h ../Includes/EthCAN_Types.h
File.o: Component.h ../Import/Includes/KmsBase.h ../Import/Includes/SafeAPI.h
File.o: ../Import/Includes/WindowsToLinux.h ../Includes/EthCAN_Result.h
File.o: ../Includes/EthCAN/File.h ../Includes/EthCAN_Types.h
L_Serial.o: Component.h ../Import/Includes/KmsBase.h
L_Serial.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
L_Serial.o: ../Includes/EthCAN_Result.h Serial.h IMessageReceiver.h
L_System_Impl.o: Component.h ../Import/Includes/KmsBase.h
L_System_Impl.o: ../Import/Includes/SafeAPI.h
L_System_Impl.o: ../Import/Includes/WindowsToLinux.h
L_System_Impl.o: ../Includes/EthCAN_Result.h Serial.h IMessageReceiver.h
L_System_Impl.o: System_Impl.h ../Includes/EthCAN/System.h
L_System_Impl.o: ../Includes/EthCAN_Protocol.h ../Includes/EthCAN_Types.h
L_System_Impl.o: ../Includes/EthCAN/Object.h
L_Thread.o: Component.h ../Import/Includes/KmsBase.h
L_Thread.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
L_Thread.o: ../Includes/EthCAN_Result.h Thread.h IMessageReceiver.h
L_UDPSocket.o: Component.h ../Import/Includes/KmsBase.h
L_UDPSocket.o: ../Import/Includes/SafeAPI.h
L_UDPSocket.o: ../Import/Includes/WindowsToLinux.h
L_UDPSocket.o: ../Includes/EthCAN_Result.h UDPSocket.h
Object.o: Component.h ../Import/Includes/KmsBase.h
Object.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Object.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN/Object.h
Serial.o: Component.h ../Import/Includes/KmsBase.h
Serial.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Serial.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN_Protocol.h Thread.h
Serial.o: IMessageReceiver.h Serial.h
System_Impl.o: Component.h ../Import/Includes/KmsBase.h
System_Impl.o: ../Import/Includes/SafeAPI.h
System_Impl.o: ../Import/Includes/WindowsToLinux.h
System_Impl.o: ../Includes/EthCAN_Result.h Device_Impl.h
System_Impl.o: ../Includes/EthCAN_Protocol.h ../Includes/EthCAN/Device.h
System_Impl.o: ../Includes/EthCAN_Types.h ../Includes/EthCAN/Object.h
System_Impl.o: IMessageReceiver.h Serial.h Thread.h UDPSocket.h System_Impl.h
System_Impl.o: ../Includes/EthCAN/System.h
System.o: Component.h ../Import/Includes/KmsBase.h
System.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
System.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN/Device.h
System.o: ../Includes/EthCAN_Types.h ../Includes/EthCAN/Object.h
System.o: ../Includes/EthCAN/System.h ../Includes/EthCAN_Protocol.h
System.o: ../Common/Version.h System_Impl.h IMessageReceiver.h
Thread.o: Component.h ../Import/Includes/KmsBase.h
Thread.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Thread.o: ../Includes/EthCAN_Result.h Thread.h IMessageReceiver.h
UDPSocket.o: Component.h ../Import/Includes/KmsBase.h
UDPSocket.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
UDPSocket.o: ../Includes/EthCAN_Result.h ../Includes/EthCAN_Protocol.h
UDPSocket.o: UDPSocket.h
