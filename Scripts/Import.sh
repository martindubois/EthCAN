#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      Scripts/Import.sh
# Usage     ./Import.sh {Destination}

echo Executing  Import.sh $1  ...

# ===== Initialisation =======================================================

KMS_COPY=./KmsCopy

# ===== Verification =========================================================

if [ ! -d $1 ]
then
    echo ERROR  $1  does not exist
    exit 10
fi

# ===== Execution ============================================================

$KMS_COPY . $1 Import.txt
if [ 0 != $? ] ; then
    echo ERROR  $KMS_COPY . $1 Import.txt  failed
    exit 20
fi

$KMS_COPY . $1 Import.sh.txt
if [ 0 != $? ] ; then
    echo ERROR  $KMS_COPY . $1 Import.sh.txt  failed
    exit 30
fi

# ===== End ==================================================================

echo OK
