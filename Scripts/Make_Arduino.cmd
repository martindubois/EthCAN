@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Scripts/Make_Arduino.cmd

echo Executing  Scripts/Make_Arduino.cmd %1  ...

rem ===== Initialisation ====================================================

call ../Scripts/Set_Arduino.cmd
if ERRORLEVEL 1 (
    echo FATAL ERROR  Set_Arduino.cmd  failed - %ERRORLEVEL%
    pause
    exit /B 5
)

set BUILDER="%ARDUINO_FOLDER%\arduino-builder.exe"

set COMPONENT=%1

rem ===== Verification ======================================================

if ""=="%1" (
    echo FATAL ERROR  Invalid command line
    echo Usage  call ../Scripts/Make_Arduino.cmd {Component}
    exit /B 10
)

if not exist %BUILDER% (
    echo FATAL ERROR  %BUILDER%  does not exist
    echo See %COMPONENT%/_DocDev/DevComputer.txt
    pause
    exit /B 20
)

rem ===== Execution =========================================================

if not exist %BUILD_FOLDER% mkdir %BUILD_FOLDER%

%BUILDER% -build-options-file build.options.json -build-path %BUILD_FOLDER% -compile %COMPONENT%.ino
if ERRORLEVEL 1 (
    echo ERROR  %BUILDER% -build-options-file build.options.json -build-path %BUILD_FOLDER% -compile %COMPONENT%.ino  failed - %ERRORLEVEL%
    pause
    exit /B 30
)

rem ===== End ===============================================================

echo OK
