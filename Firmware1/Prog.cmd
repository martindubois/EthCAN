@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copywrite (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware1/Prog.cmd

echo Executing  Firmware1/Prog.cmd %1  ...

rem ===== Initialistion =====================================================

call .\Set_Arduino.cmd
if ERRORLEVEL 1 (
    echo FATAL ERROR  .\Set_Arduino.cmd  failed - %ERRORLEVEL%
    pause
    exit /B 10
)

set PORT=%1

rem ===== Verification ======================================================

if not exist %HEX_FILE% (
    echo FATAL ERROR  %HEX_FILE%  does not exist
    echo Execute .\Make.cmd
    pause
    exit /B 20
)

if "%PORT%"=="" (
    echo USER ERROR  Invalid command line
    echo Usage  ./Prog.cmd {COMx}
    pause
    exit /B 30
)

if not exist %PROG_TOOL% (
    echo FATAL ERROR  %PROG_TOOL%  does not exist
    pause
    exit /B 40
)

rem ===== Execution =========================================================

%PROG_TOOL% "-C%CONF_FILE%" -v -patmega168 -carduino -P%PORT% -b19200 -D -Uflash:w:%HEX_FILE%:i
if ERRORLEVEL 1 (
    echo ERROR  %PROG_TOOL%  failed - %ERRORLEVEL%
    pause
    exit /B 50
)

rem ===== End ===============================================================

echo OK
