@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      UploadDocumentation.cmd

echo  Executing  OploadDocumentation.cmd  ...

rem  ===== Execution ========================================================

ftp.exe -i -s:UploadDocumentation.cmd.txt ftp.kms-quebec.com
if ERRORLEVEL 1 (
    echo  ERROR  ftp -s:UploadDocumentation.txt ftp.kms-quebec.com  failed - %ERRORLEVEL%
    pause
    exit /B 10
)

rem  ===== Fin ==============================================================

echo  OK
