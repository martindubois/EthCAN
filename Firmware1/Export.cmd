@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware1/Export.cmd

echo Executing  Firmware1/Export.cmd %1 %2  ...

rem ===== Initialisation ====================================================

call Set_Arduino.cmd
if ERRORLEVEL 1 (
    echo FATAL ERROR  call Set_Arduino.cmd  failed - %ERRORLEVEL%
    pause
    exit /B 10
)

set VERSION_MMB=%1
set VERSION_TYPE=%2

set EXPORT_CMD_TXT="Export.cmd.txt"

set EXPORT_FOLDER=K:\Export\EthCAN

if ""=="%VERSION_TYPE%" (
    set DST=%EXPORT_FOLDER%\%VERSION_MMB%_%COMPONENT%
) else (
    set DST=%EXPORT_FOLDER%\%VERSION_MMB%_%VERSION_TYPE%_%COMPONENT%
)

set KMS_COPY_EXE=..\Import\Binaries\Release_32\KmsCopy.exe

rem ===== Verification ======================================================

if ""=="%VERSION_MMB%" (
    echo FATAL ERROR  Invalid command line
    echo Usage  .\Export.cmd {Version} {Type}
    pause
    exit /B 20
)

if exist %DST% (
    echo FATAL ERROR  %DST%  exist
    pause
    exit /B 30
)

if not exist %KMS_COPY_EXE% (
    echo FATAL ERROR  %KMS_COPY_EXE%  does not exist
    echo Execute Import.cmd in the product folder
    pause
    exit /B 40
)

rem ===== Execution =========================================================

%KMS_COPY_EXE% . %DST% %EXPORT_CMD_TXT%
if ERRORLEVEL 1 (
    echo ERROR  %KMS_COPY_EXE% . %DST% %EXPORT_CMD_TXT%  - %ERRORLEVEL%
    pause
    exit /B 50
)

copy "%CONF_FILE%"  %DST%
copy  %LIB_USB_DLL% %DST%
copy  %PROG_TOOL%   %DST%

rem ===== End ===============================================================

echo OK
