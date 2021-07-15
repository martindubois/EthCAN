#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      Clean.sh
# Usage     ./Clean.sh

# CODE REVIEW

echo Excuting  Clean.sh  ...

# ===== Functions ===========================================================

Clean()
{
    cd $1
    ./Clean.sh
    cd ..
}

# ===== Execution ===========================================================

Clean EthCAN_Lib
Clean EthCAN_Lib_Test
Clean EthCAN_Tool

# ===== End =================================================================

echo OK
exit 0
