#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      Import.sh
# Usage     ./Import.sh

# CODE REVIEW

echo Excuting  Import.sh  ...

# ===== Dependencies =========================================================

PROCESSOR=`uname -p`
echo $PROCESSOR

if [ "aarch64" = "$PROCESSOR" ]
then
    KMS_BASE=~/Export/KmsBase/3.0.31_Linux
fi

if [ "x86_64" = "$PROCESSOR" ]
then
    KMS_BASE=~/Export/KmsBase/3.0.28_Linux
fi

if [ ! -d $KMS_BASE ]
then
    KMS_BASE=/usr/local/KmsBase-3.0
fi

# ===== Constants ============================================================

DST_FOLDER=$PWD/Import

# ===== Verification =========================================================

if [ ! -d $KMS_BASE ]
then
    echo FATAL ERROR  $KMS_BASE  does not exist
    exit 10
fi

# ===== Execution ============================================================

if [ ! -d $DST_FOLDER ]
then
    mkdir $DST_FOLDER
fi

cd $KMS_BASE

./Import.sh $DST_FOLDER

if [ 0 -ne $? ]
then
    echo ERROR  ./Import.sh $DST_FOLDER  failed - $?
    exit 20
fi

# ===== End ==================================================================

echo OK
