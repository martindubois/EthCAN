@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Build.cmd
rem Usage     .\Build.cmd

rem CODE REVIEW

echo Executing  Build.cmd  ...

rem ===== Initialization ====================================================

set IMPORT_SCRIPT_FOLDER=Import\Scripts

set BUILD_BEGIN_CMD="%IMPORT_SCRIPT_FOLDER%\KmsBase_Build_Begin.cmd"
set BUILD_INSTALLER_CMD="%IMPORT_SCRIPT_FOLDER%\KmsBase_Build_Installer.cmd"
set SIGN_CMD="%IMPORT_SCRIPT_FOLDER%\KmsBase_Sign.cmd"

rem ===== Verification ======================================================

if not exist %BUILD_BEGIN_CMD% (
	echo FATAL ERROR  %BUILD_BEGIN_CMD%  does not exist
	echo Execute .\Import.cmd
	pause
	exit /B 10
)

if not exist %BUILD_INSTALLER_CMD% (
	echo FATAL ERROR  %BUILD_INSTALLER_CMD%  does not exist
	pause
	exit /B 20
)

if not exist %SIGN_CMD% (
	echo FATAL ERROR  %SIGN_CMD%  does not exist
	pause
	exit /B 30
)

rem ===== Execution =========================================================

call %BUILD_BEGIN_CMD% EthCAN
if ERRORLEVEL 1 (
	echo ERROR  call %BUILD_BEGIN_CMD% EthCAN  failed - %ERRORLEVEL%
	pause
	exit /B 40
)

%MSBUILD% %OPTIONS% /property:Configuration=Debug /property:Platform=x64
if ERRORLEVEL 1 (
	echo ERROR  %MSBUILD% %OPTIONS% /property:Configuration=Debug /property:Platform=x64  failed - %ERRORLEVEL%
	pause
	exit /B 50
)

%MSBUILD% %OPTIONS% /property:Configuration=Release /property:Platform=x64
if ERRORLEVEL 1 (
	echo ERROR  %MSBUILD% %OPTIONS% /property:Configuration=Release /property:Platform=x64  failed - %ERRORLEVEL%
	pause
	exit /B 60
)

call %TEST_CMD%
if ERRORLEVEL 1 (
    echo ERROR  %TEST_CMD%  failed - %ERRORLEVEL%
	pause
	exit /B 70
)

%KMS_VERSION_EXE% %VERSION_H% %EXPORT_CMD_TXT%
if ERRORLEVEL 1 (
	echo ERROR  %KMS_VERSION_EXE% %VERSION_H% %EXPORT_CMD_TXT%  failed - %ERRORLEVEL%
	pause
	exit /B 90
)

call %BUILD_INSTALLER_CMD% %PRODUCT_ISS%
if ERRORLEVEL 1 (
	echo ERROR  call %BUILD_INSTALLER_CMD% %PRODUCT_ISS%  failed - %ERRORLEVEL%
	pause
	exit /B 100
)

call %SIGN_CMD% Installer/%PRODUCT%_*.exe
if ERRORLEVEL 1 (
	echo ERROR  call %SIGN_CMD% Installer/%PRODUCT%_*.exe  failed - %ERRORLEVEL%
	pause
	exit /B 110
)

%KMS_VERSION_EXE% -S %VERSION_H% %EXPORT_CMD%
if ERRORLEVEL 1 (
    echo ERROR  %KMS_VERSION_EXE% -S %VERSION_H% %EXPORT_CMD%  failed - %ERRORLEVEL%
	pause
	exit /B 120
)

rem ===== End ===============================================================

echo OK
