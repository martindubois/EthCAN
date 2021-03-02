#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      EthCAN_Lib/Clean.sh
# Usage     ./Clean.sh

echo Executing  EthCAN_Lib/Clean.sh  ...

# ===== Execution ===========================================================

rm -f ../Libraries/EthCAN_Lib.a

rm -f *.o

# ===== End =================================================================

echo OK
exit 0
