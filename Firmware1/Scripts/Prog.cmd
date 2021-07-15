@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware1/Scripts/Prog.cmd

echo Executing  Firmware1/Scripts/Prog.cmd %1  ...

rem ===== Initialisation ====================================================

set AVR_FOLDER=%ARDUINO_FOLDER%\hardware\tools\avr

set COMPONENT=Firmware1

set CONF_FILE=avrdude.conf

set HEX_FILE="%COMPONENT%.ino.hex"

set PORT=%1

set PROG_TOOL="avrdude.exe"

rem ===== Verification ======================================================

if ""=="%PORT%" (
    echo USER ERROR  Invalid command line
    echo Usage  .\Prog.cmd {COMx}
    pause
    exit /B 10
)

rem ===== Execution =========================================================

%PROG_TOOL% "-C%CONF_FILE%" -v -patmega168 -carduino -P%PORT% -b19200 -D -Uflash:w:%HEX_FILE%:i
if ERRORLEVEL 1 (
    echo ERROR  %PROG_TOOL%  failed - %ERRORLEVEL%
    pause
    exit /B 20
)

rem ===== End ===============================================================

echo OK
