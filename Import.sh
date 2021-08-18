#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      Import.sh
# Usage     ./Import.sh

# CODE REVIEW

echo Excuting  Import.sh  ...

# ===== Configuration ========================================================

EXPORT_FOLDER=~/Export

# ===== Dependencies =========================================================

OS=`uname`
echo $OS

PROCESSOR=`uname -p`
echo $PROCESSOR

if [ "Darwin" = "$OS" ]
then
    KMS_BASE=$EXPORT_FOLDER/KmsBase/3.0.32_Darwin
fi

if [ "Linux" = "$OS" ]
then
    if [ "aarch64" = "$PROCESSOR" ]
    then
        KMS_BASE=$EXPORT_FOLDER/KmsBase/3.0.31_Linux
    fi

    if [ "x86_64" = "$PROCESSOR" ]
    then
        KMS_BASE=$EXPORT_FOLDER/KmsBase/3.0.28_Linux
    fi
fi

if [ ! -d $KMS_BASE ]
then
    if [ "Darwin" = "$OS" ]
    then
        KMS_BASE=/Applications/KmsBase-3.0
    fi

    if [ "Linux" = "$OS" ]
    then
        KMS_BASE=/usr/local/KmsBase-3.0
    fi
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

if [ ! -d Binaries ]
then
    mkdir Binaries
fi

if [ ! -d Libraries ]
then
    mkdir Libraries
fi

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
