@echo off

rem Author 	  KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      Export.cmd

rem CODE REVIEW

echo Executing  Export.cmd %1 %2  ...

rem ===== Initialization ====================================================

set IMPORT_SCRIPT_FOLDER=Import\Scripts

set EXPORT_CMD="%IMPORT_SCRIPT_FOLDER%\KmsBase_Export.cmd"

rem ===== Execution =========================================================

call %EXPORT_CMD% EthCAN %1 %2
if ERRORLEVEL 1 (
    echo ERROR  call %EXPORT_CMD% EthCAN %1 %2  failed - %ERRORLEVEL%
	pause
	exit /B 10
)

rem ===== End ===============================================================

echo OK