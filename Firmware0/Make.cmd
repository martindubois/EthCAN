@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copywrite (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware0/Make.cmd
rem Usage     ./Make.cmd

echo Executing  Firmware0/Make.cmd  ...

rem ===== Execution =========================================================

call ..\Scripts\Make_Arduino.cmd Firmware0
if ERRORLEVEL 1 (
    echo ERROR  call ..\Scripts\Make_Arduino.cmd Firmware0  failed - %ERRORLEVEL%
    pause
    exit 30
)

rem ===== End ===============================================================

echo OK
