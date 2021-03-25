@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware0/Scripts/Prog.cmd

echo Executing  Firmware0/Scripts/Proc.cmd %1  ...

rem ===== Initialisation ====================================================

set COMPONENT=Firmware0

set BIN_FILE_0="%COMPONENT%.ino.bin"
set BIN_FILE_1="%COMPONENT%.ino.partitions.bin"

set BOOT_APP="boot_app0.bin"
set BOOT_LOADER="bootloader_dio_80m.bin"

set PORT=%1

set PROG_TOOL="esptool.exe"

rem ===== Verification ======================================================

if ""=="%PORT%" (
    echo USER ERROR  Invalid command line
    echo Usage  .\Prog.cmd {COMx}
    pause
    exit /B 10
)

rem ===== Execution =========================================================

%PROG_TOOL% --chip esp32 --port %PORT% --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 %BOOT_APP% 0x1000 %BOOT_LOADER% 0x10000 %BIN_FILE_0% 0x8000 %BIN_FILE_1%
if ERRORLEVEL 1 (
    echo ERROR  %PROG_TOOL%  failed - %ERRORLEVEL%
    pause
    exit /B 20
)

rem ===== End ===============================================================

echo OK
