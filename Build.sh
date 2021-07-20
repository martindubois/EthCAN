#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      Build.sh
# Usage     ./Build.sh

# CODE REVIEW

echo Excuting  Build.sh  ...

# ===== Initialisation ======================================================

KMS_VERSION=Import/Binaries/KmsVersion

VERSION_H=Common/Version.h

# ===== Execution ===========================================================

./Clean.sh

./Make.sh
if [ 0 != $? ] ; then
    echo ERROR  ./Make.sh  failed
    exit 10
fi

./Test.sh
if [ 0 != $? ] ; then
    echo ERROR  ./Test.sh  failed
    exit 20
fi

$KMS_VERSION $VERSION_H CreatePackages.sh Export.sh
if [ 0 != $? ] ; then
    echo ERROR  $KMS_VERSION $VERSION_H CreatePackages.sh Export.sh  failed
    exit 30
fi

./CreatePackages.sh
if [ 0 != $? ] ; then
    echo ERROR  ./CreatePackages.sh  failed
    exit 40
fi

$KMS_VERSION -S $VERSION_H ./Export.sh
if [ 0 != $? ] ; then
    echo ERROR  $KMS_VERSION -S $VERSION_H ./Export.sh  failed
    exit 50
fi

# ===== End =================================================================

echo OK
exit 0
