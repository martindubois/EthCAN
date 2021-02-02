#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) KMS 2021
# Product   EthCAN
# File      RunDoxygen.sh
# Usage     ./RunDoxygen.sh

echo Executing  RunDoxygen.sh  ...

# ===== Execution ===========================================================

doxygen DoxyFile_en.txt
if [ 0 != $? ] ; then
    echo ERROR  doxygen DoxyFile_en.txt  failed
    exit 10
fi

# ===== End =================================================================

echo OK
