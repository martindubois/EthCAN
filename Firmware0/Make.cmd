@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copywrite (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware0/Arduino.cmd
rem Usage     ./Arduino.cmd [COMx]

echo Executing  Firmware0/Arduino.cmd  ...

rem ===== Initialistion =====================================================

set COMPONENT=Firmware0

set BUILD_FOLDER=Build
set PACKAGE_FOLDER=C:\Users\mdubois.KMS-QUEBEC\AppData\Local\Arduino15\packages\esp32

set TOOLS_FOLDER=%PACKAGE_FOLDER%\hardware\esp32\1.0.4\tools

set BOOT_APP="%TOOLS_FOLDER%\partitions\boot_app0.bin"
set BOOT_LOADER="%TOOLS_FOLDER%\sdk\bin\bootloader_dio_80m.bin"

set BUILDER="C:\Program Files (x86)\Arduino\arduino-builder.exe"

set PROG_TOOL="%PACKAGE_FOLDER%\tools\esptool_py\2.6.1\esptool.exe"

set PORT=%1

rem ===== Verification ======================================================

if not exist %BUILDER% (
    echo FATAL ERROR  %BUILDER%  does not exist
    echo See _DocDev/DevComputer.txt
    pause
    exit /B 10
)

if not exist %PROG_TOOL% (
    echo FATAL ERROR  %PROG_TOOL%  does not exist
    echo See _DocDev/DevComputer.txt
    pause
    exit /B 20
)

rem ===== Execution =========================================================

if not exist %BUILD_FOLDER% mkdir %BUILD_FOLDER%

%BUILDER% -build-options-file build.options.json -build-path %BUILD_FOLDER% -compile %COMPONENT%.ino

if ERRORLEVEL 1 (
    echo ERROR  %BUILDER% -build-options-file build.options.json -build-path %BUILD_FOLDER% -compile %COMPONENT%.ino  failed
    pause
    exit 30
)

if not ""=="%PORT%" (
    %PROG_TOOL% --chip esp32 --port %PORT% --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 %BOOT_APP% 0x1000 %BOOT_LOADER% 0x10000 %BUILD_FOLDER%\%COMPONENT%.ino.bin 0x8000 %BUILD_FOLDER%\%COMPONENT%.ino.partitions.bin
)

rem ===== End ===============================================================

echo OK
