@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Scripts/Set_Arduino.cmd

echo Executing  Scripts/Set_Arduino  ...

rem ===== Initialisation ====================================================

set ARDUINO_FOLDER=C:\Program Files (x86)\Arduino
set BUILD_FOLDER=Build
set PACKAGES_FOLDER=%USERPROFILE%\AppData\Local\Arduino15\packages

rem ===== Verification ======================================================

if not exist "%ARDUINO_FOLDER%" (
    echo FATAL ERROR  "%ARDUINO_FOLDER%"  does not exist
    echo Install Arduino IDE
    pause
    exit /B 10
)

if not exist %PACKAGES_FOLDER% (
    echo FATAL ERROR  %PACKAGES_FOLDER%  does not existe
    echo See the appropriate DevComputer.txt
    pause
    exit /B 20
)

rem ===== End ===============================================================

echo OK
