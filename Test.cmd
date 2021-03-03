@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Test.cmd

echo Executing  Test.cmd  ...

rem ===== Initialization ====================================================

set BASE_TXT="EthCAN_Tool\Tests\Base.txt"

set FIRMWARE1_BASE_TXT="Firmware1_Tool\Tests\Base.txt"

set DEBUG=x64\Debug

set RELEASE=x64\Release

rem ===== Verification ======================================================

if not exist "%DEBUG%" (
    echo FATAL ERROR  "%DEBUG%"  does not exist
    echo Compile the product
    pause
    exit /B 10
)

if not exist "%RELEASE%" (
    echo FATAL ERROR  "%RELEASE%"  does not exist
    echo Compile the product
    pause
    exit /B 20
)

rem ===== Execution =========================================================

%DEBUG%\EthCAN_Lib_Test.exe
if ERRORLEVEL 1 (
    echo ERROR  %DEBUG%\EthCAN_Lib_Test.exe  failed - %ERRORLEVEL%
    pause
    exit /B 30
)

%DEBUG%\EthCAN_Tool.exe "Execute=ExecuteScript %BASE_TXT%"
if  ERRORLEVEL 1 (
	echo ERROR  %DEBUG%\EthCAN_Tool.exe "Execute=ExecuteScript %BASE_TXT%"  failed - %ERRORLEVEL%
	pause
	exit /B 40
)

%DEBUG%\Firmware1_Tool.exe "Execute=ExecuteScript %FIRMWARE1_BASE_TXT%"
if  ERRORLEVEL 1 (
	echo ERROR  %DEBUG%\Firmware1_Tool.exe "Execute=ExecuteScript %FIRMWARE1_BASE_TXT%"  failed - %ERRORLEVEL%
	pause
	exit /B 45
)

%RELEASE%\EthCAN_Lib_Test.exe
if ERRORLEVEL 1 (
    echo ERROR  %RELEASE%\EthCAN_Lib_Test.exe  failed - %ERRORLEVEL%
    pause
    exit /B 50
)

%RELEASE%\EthCAN_Tool.exe "Execute=ExecuteScript %BASE_TXT%"
if  ERRORLEVEL 1 (
	echo ERROR  %RELEASE%\EthCAN_Tool.exe "Execute=ExecuteScript %BASE_TXT%"  failed - %ERRORLEVEL%
	pause
	exit /B 60
)

%RELEASE%\Firmware1_Tool.exe "Execute=ExecuteScript %FIRMWARE1_BASE_TXT%"
if  ERRORLEVEL 1 (
	echo ERROR  %RELEASE%\Firmware1_Tool.exe "Execute=ExecuteScript %FIRMWARE1_BASE_TXT%"  failed - %ERRORLEVEL%
	pause
	exit /B 70
)

rem ===== End ===============================================================

echo OK
