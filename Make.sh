#!/bin/sh

# Author    KMS - Martin Dubois, P. Eng.
# Copyright (C) 2021-2022 KMS
# Product   EthCAN
# File      Make.sh
# Usage     ./Make.sh

# CODE REVIEW

echo Excuting  Make.sh  ...

# ===== Functions ===========================================================

Make()
{
    cd $1
    echo Building $1 ...
    make
    if [ 0 != $? ] ; then
        echo ERROR  $1 - make  failed
        exit 10
    fi
    cd ..
}

# ===== Execution ===========================================================

if [ ! -d Binaries ]
then
    mkdir Binaries
fi

if [ ! -d Libraries ]
then
    mkdir Libraries
fi

Make EthCAN_Lib
Make EthCAN_Lib_Test
Make EthCAN_Tool

# ===== End =================================================================

echo OK
