@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copywrite (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware0/Arduino.cmd
rem Usage     ./Arduino.cmd [COMx]

echo Executing  Firmware0/Make.cmd %1  ...

rem ===== Initialistion =====================================================

set PORT=%1

rem ===== Execution =========================================================

call ..\Scripts\Make_Arduino.cmd Firmware0
if ERRORLEVEL 1 (
    echo ERROR  call ..\Scripts\Make_Arduino.cmd Firmware0  failed - %ERRORLEVEL%
    pause
    exit 30
)

set PACKAGE_FOLDER=%PACKAGES_FOLDER%\esp32
set TOOLS_FOLDER=%PACKAGE_FOLDER%\hardware\esp32\1.0.4\tools

set BOOT_APP="%TOOLS_FOLDER%\partitions\boot_app0.bin"
set BOOT_LOADER="%TOOLS_FOLDER%\sdk\bin\bootloader_dio_80m.bin"

set PROG_TOOL="%PACKAGE_FOLDER%\tools\esptool_py\2.6.1\esptool.exe"

if not ""=="%PORT%" (
    %PROG_TOOL% --chip esp32 --port %PORT% --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 %BOOT_APP% 0x1000 %BOOT_LOADER% 0x10000 %BUILD_FOLDER%\%COMPONENT%.ino.bin 0x8000 %BUILD_FOLDER%\%COMPONENT%.ino.partitions.bin
)

rem ===== End ===============================================================

echo OK
