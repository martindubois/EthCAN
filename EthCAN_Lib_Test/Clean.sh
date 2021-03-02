#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      EthCAN_Lib_Test/Clean.sh
# Usage     ./Clean.sh

echo Executing  EthCAN_Lib_Test/Clean.sh  ...

# ===== Execution ===========================================================

rm -f ../Binaries/EthCAN_Lib_Test

rm -f *.o

# ===== End =================================================================

echo OK
exit 0
