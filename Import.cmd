@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Import.cmd
rem Usage     .\Import.cmd

rem CODE REVIEW

echo Executing  Import.cmd  ...

rem ===== Configuration =====================================================

set EXPORT_FOLDER=K:\Export

rem ===== Dependencies ======================================================

set KMS_BASE="%EXPORT_FOLDER%\KmsBase\3.0.30_Windows"

rem ===== Constants =========================================================

set DST_FOLDER="%CD%\Import"

rem ===== Verification ======================================================

if not exist %KMS_BASE% (
	echo FATAL ERROR  %KMS_BASE%  does not exist
	pause
	exit /B 10
)

rem  ===== Execution ========================================================

if not exist %DST_FOLDER% mkdir %DST_FOLDER%

pushd %KMS_BASE%
    call Import.cmd %DST_FOLDER%
popd

if ERRORLEVEL 1 (
	echo ERROR  call Import.cmd %DST_FOLDER%  failed - %ERRORLEVEL%
	pause
	exit /B 20
)

rem  ===== End ==============================================================

echo OK
