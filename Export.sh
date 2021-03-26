#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      Export.sh
# Usage     ./Export.sh

# CODE REVIEW

# TODO Export
#      Use KmsCopy

echo Executing  Export.sh $1 $2  ...

# ===== Initialisation ======================================================

DST_FOLDER=~/Export/EthCAN/$1_$2_Linux

# ===== Execution ===========================================================

mkdir $DST_FOLDER
if [ 0 != $? ] ; then
    echo ERROR  mkdir $DST_FOLDER  failed
    exit 10
fi

mkdir $DST_FOLDER/Binaries
mkdir $DST_FOLDER/Includes
mkdir $DST_FOLDER/Includes/EthCAN
mkdir $DST_FOLDER/Libraries
mkdir $DST_FOLDER/Packages

cp _DocUser/EthCAN.ReadMe.txt                         $DST_FOLDER
cp Binaries/EthCAN_Test                               $DST_FOLDER/Binaries
cp Binaries/EthCAN_Tool                               $DST_FOLDER/Binaries
cp Import/Binaries/KmsCopy                            $DST_FOLDER
cp Import/KmsBase.KmsLib.ReadMe.txt                   $DST_FOLDER
cp Includes/*.h                                       $DST_FOLDER/Includes
cp Includes/EthCAN/*.h                                $DST_FOLDER/Includes/EthCAN
cp Libraries/EthCAN_Lib.a                             $DST_FOLDER/Libraries
cp EthCAN_Lib/_DocUser/EthCAN.EthCAN_Lib.ReadMe.txt   $DST_FOLDER
cp EthCAN_Tool/_DocUser/EthCAN.EthCAN_Tool.ReadMe.txt $DST_FOLDER
cp Firmware0/_DocUser/EthCAN.Firmware0.ReadMe.txt     $DST_FOLDER
cp Firmware1/_DocUser/EthCAN.Firmware1.ReadMe.txt     $DST_FOLDER
# KmsVersion "kms-ethcan_" ".deb" 13
cp Packages/kms-ethcan_1.0-2.deb                      $DST_FOLDER/Packages
cp Scripts/Import.sh                                  $DST_FOLDER
cp Scripts/Import.sh.txt                              $DST_FOLDER
cp Scripts/Import.txt                                 $DST_FOLDER
cp DoxyFile_en.txt                                    $DST_FOLDER
cp RunDoxygen.sh                                      $DST_FOLDER

# ===== End =================================================================

echo OK
