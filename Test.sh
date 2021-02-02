#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      Test.sh
# Usage     ./Test.sh

echo Excuting  Test.sh  ...

# ===== Initialisation ======================================================

BASE_TXT=EthCAN_Tool/Tests/Base.txt

BINARIES=./Binaries

# ===== Functions ===========================================================

Test()
{
    $BINARIES/$1 "$2"
    if [ 0 != $? ] ; then
        echo ERROR  $BINARIES/$1 "$2"  failed
        exit 10
    fi
}

# ===== Execution ===========================================================

Test EthCAN_Lib_Test
Test EthCAN_Tool "Execute=ExecuteScript $BASE_TXT"

# ===== End =================================================================

echo OK
