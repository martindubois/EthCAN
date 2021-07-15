@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Firmware0/Build.cmd
rem Usage     .\Build.cmd

echo Executing  Build.cmd  ...

rem ===== Initialization ====================================================

set EXPORT_CMD=Export.cmd

set KMS_VERSION_EXE=..\Import\Binaries\Release_32\KmsVersion.exe

set VERSION_H=..\Common\Version.h

rem ===== Verification ======================================================

if not exist %KMS_VERSION_EXE% (
	echo FATAL ERROR  %KMS_VERSION%  does not exist
	echo Execute Import.cmd in the product folder
	pause
	exit /B 10
)

rem ===== Execution =========================================================

call .\Make.cmd
if ERRORLEVEL 1 (
	echo ERROR  call .\Make.cmd  failed - %ERRORLEVEL%
	pause
	exit /B 20 
)

%KMS_VERSION_EXE% -S %VERSION_H% %EXPORT_CMD%
if ERRORLEVEL 1 (
    echo ERROR  %KMS_VERSION_EXE% -S %VERSION_H% %EXPORT_CMD%  failed - %ERRORLEVEL%
	pause
	exit /B 30
)

rem ===== End ===============================================================

echo OK
