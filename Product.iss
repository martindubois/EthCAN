
; Author    KMS - Martin Dubois, P.Eng.
; Copyright (C) 2021 KMS
; Product   EthCAN
; File      Product.iss

; CODE REVIEW

[Setup]
AllowNetworkDrive=no
AllowUNCPath=no
AppCopyright=Copyright (C) 2021 KMS.
AppName=EthCAN
AppPublisher=KMS
AppPublisherURL=http://www.kms-quebec.com
AppSupportURL=http://www.kms-quebec.com
AppVersion=1.0.5
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
DefaultDirName={pf}\EthCAN
LicenseFile=License.txt
MinVersion=10.0
OutputBaseFilename=EthCAN_1.0.5
OutputDir=Installer

[Files]
Source: "_DocUser\EthCAN.ReadMe.txt"                        ; DestDir: "{app}"; Flags: isreadme
Source: "EthCAN_Lib\_DocUser\EthCAN.EthCAN_Lib.ReadMe.txt"  ; DestDir: "{app}"
Source: "EthCAN_Tool\_DocUser\EthCAN.EthCAN_Tool.ReadMe.txt"; DestDir: "{app}"
Source: "Firmware0\_DocUser\EthCAN.Firmware0.ReadMe.txt"    ; DestDir: "{app}"
Source: "Firmware1\_DocUser\EthCAN.Firmware1.ReadMe.txt"    ; DestDir: "{app}"
Source: "Import\KmsBase.KmsLib.ReadMe.txt"                  ; DestDir: "{app}"
Source: "Import\Libraries\Debug_64_DLL\KmsLib.lib"          ; DestDir: "{app}\Libraries\Debug_64"
Source: "Import\Libraries\Release_64_DLL\KmsLib.lib"        ; DestDir: "{app}\Libraries\Release_64"
Source: "Includes\*.h"                                      ; DestDir: "{app}\Includes"
Source: "x64\Debug\EthCAN_Lib.lib"                          ; DestDir: "{app}\Libraries\Debug_64"
Source: "x64\Debug\EthCAN_Lib.pdb"                          ; DestDir: "{app}\Libraries\Debug_64"
Source: "x64\Release\EthCAN_Lib.lib"                        ; DestDir: "{app}\Libraries\Release_64"
Source: "x64\Release\EthCAN_Tool.exe"                       ; DestDir: "{app}"
