@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copywrite (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware1/Set_Arduino.cmd

echo Executing  Firmware1/Set_Arduino.cmd  ...

rem ===== Initialistion =====================================================

call ../Scripts/Set_Arduino.cmd
if ERRORLEVEL 1 (
    echo FATAL ERROR  Set_Arduino.cmd  failed - %ERRORLEVEL%
    pause
    exit /B 10
)

set AVR_FOLDER=%ARDUINO_FOLDER%\hardware\tools\avr

set COMPONENT=Firmware1

set CONF_FILE=%AVR_FOLDER%\etc\avrdude.conf

set HEX_FILE=%BUILD_FOLDER%\%COMPONENT%.ino.hex

set LIB_USB_DLL="%AVR_FOLDER%\bin\libusb0.dll"

set PROG_TOOL="%AVR_FOLDER%\bin\avrdude.exe"

rem ===== Verification ======================================================

if not exist "%CONF_FILE%" (
    echo FATA ERROR  "%CONF_FILE%"  does not exist
    echo See the appropriate DevComputer.txt
    pause
    exit /B 15
)

if not exist %HEX_FILE% (
    echo FATA ERROR  %HEX_FILE%  does not exist
    echo Execute .\Make.cmd
    pause
    exit /B 20
)

if not exist %PROG_TOOL% (
    echo FATAL ERROR  %PROG_TOOL%  does not exist
    pause
    exit /B 70
)

rem ===== End ===============================================================

echo OK
