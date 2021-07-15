@echo off

rem Author    KMS - Martin Dubois, P.Eng.
rem Copyright (C) 2021 KMS
rem Product   EthCAN
rem File      CreateLinks.cmd
rem Usage     .\CreateLinks.cmd

echo Executing  CreateLinks.cmd  ...

rem ===== Execution =========================================================

cd Firmware0

mklink /D Common ..\Common
mklink /D Includes ..\Includes

cd ..

cd Firmware1

mklink /D Common ..\Common
mklink /D Includes ..\Includes

cd ..

rem ===== End ===============================================================

echo OK
