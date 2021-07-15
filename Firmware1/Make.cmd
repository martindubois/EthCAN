@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copywrite (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware1/Make.cmd
rem Usage     ./Make.cmd

echo Executing  Firmware1/Make.cmd  ...

rem ===== Execution =========================================================

call ..\Scripts\Make_Arduino.cmd Firmware1
if ERRORLEVEL 1 (
    echo ERROR  call ..\Scripts\Make_Arduino.cmd Firmware1  failed - %ERRORLEVEL%
    pause
    exit 30
)

rem ===== End ===============================================================

echo OK
