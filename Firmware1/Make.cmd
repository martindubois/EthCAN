@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copywrite (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware1/Arduino.cmd
rem Usage     ./Arduino.cmd [COMx]

echo Executing  Firmware0/Make.cmd %1  ...

rem ===== Initialistion =====================================================

set PORT=%1

rem ===== Execution =========================================================

call ..\Scripts\Make_Arduino.cmd Firmware1
if ERRORLEVEL 1 (
    echo ERROR  call ..\Scripts\Make_Arduino.cmd Firmware1  failed - %ERRORLEVEL%
    pause
    exit 30
)

set AVR_FOLDER=%ARDUINO_FOLDER%\hardware\tools\avr

set PROG_TOOL="%AVR_FOLDER%\bin\avrdude.exe"

if not ""=="%PORT%" (
    %PROG_TOOL% "-C%AVR_FOLDER%\etc\avrdude.conf" -v -patmega168 -carduino -P%PORT% -b19200 -D -Uflash:w:%BUILD_FOLDER%\%COMPONENT%.ino.hex:i
)

rem ===== End ===============================================================

echo OK
