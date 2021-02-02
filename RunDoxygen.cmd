@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      RunDoxygen.cmd

echo Executing  RunDoxygen.cmd  ...

rem ===== Initialisation ====================================================

set DOXYFILE_EN="DoxyFile_en.txt"

set DOXYGEN="C:\Program Files\doxygen\bin\doxygen.exe"

rem ===== Verification ======================================================

if not exist %DOXYGEN% (
    echo FATAL ERROR  %DOXYGEN%  does not exist
	echo Install doxygen
	pause
	exit /B 10
)

rem ===== Execution =========================================================

%DOXYGEN% %DOXYFILE_EN%
if ERRORLEVEL 1 (
    echo ERROR  %DOXYGEN% %DOXYFILE_EN%  failed - %ERRORLEVEL%
	pause
	exit /B 20
)

rem ===== End ===============================================================

echo OK
