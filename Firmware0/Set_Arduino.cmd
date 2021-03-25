@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copywrite (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware0/Set_Arduino.cmd

echo Executing  Firmware0/Set_Arduino.cmd  ...

rem ===== Initialistion =====================================================

call ../Scripts/Set_Arduino.cmd
if ERRORLEVEL 1 (
    echo FATAL ERROR  Set_Arduino.cmd  failed - %ERRORLEVEL%
    pause
    exit /B 10
)

set COMPONENT=Firmware0

set BIN_FILE_0=%BUILD_FOLDER%\%COMPONENT%.ino.bin
set BIN_FILE_1=%BUILD_FOLDER%\%COMPONENT%.ino.partitions.bin

set PACKAGE_FOLDER=%PACKAGES_FOLDER%\esp32

set TOOLS_FOLDER=%PACKAGE_FOLDER%\hardware\esp32\1.0.4\tools

set BOOT_APP="%TOOLS_FOLDER%\partitions\boot_app0.bin"
set BOOT_LOADER="%TOOLS_FOLDER%\sdk\bin\bootloader_dio_80m.bin"

set PROG_TOOL="%PACKAGE_FOLDER%\tools\esptool_py\2.6.1\esptool.exe"

rem ===== Verification ======================================================

if not exist %BIN_FILE_0% (
    echo FATA ERROR  %BIN_FILE_0%  does not exist
    echo Execute .\Make.cmd
    pause
    exit /B 20
)

if not exist %BIN_FILE_1% (
    echo FATA ERROR  %BIN_FILE_1%  does not exist
    pause
    exit /B 30
)

if not exist %BOOT_APP% (
    echo FATAL ERROR  %BOOT_APP%  does not exist
    pause
    exit /B 40
)

if not exist %BOOT_LOADER% (
    echo FATAL ERROR  %BOOT_LOADER%  does not exist
    pause
    exit /B 50
)

if not exist %PROG_TOOL% (
    echo FATAL ERROR  %PROG_TOOL%  does not exist
    pause
    exit /B 70
)

rem ===== End ===============================================================

echo OK
