#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      EthCAN_Tool/Clean.sh
# Usage     ./Clean.sh

echo Executing  EthCAN_Tool/Clean.sh  ...

# ===== Execution ===========================================================

rm -f ../Binaries/EthCAN_Tool

rm -f *.o

# ===== End =================================================================

echo OK
exit 0
