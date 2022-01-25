#!/bin/sh

# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      CreatePackages.sh
# Usage     ./CreatePackages.sh

# CODE REVIEW

echo Excuting  CreatePackages.sh  ...

# ===== Version =============================================================

OS=`uname`

# KmsVersion "PACKAGE_VERSION=" "\n" 13
PACKAGE_VERSION=1.1-5

# KmsVersion "VERSION=" "\n" 2
VERSION=1.1

# ===== Execution ===========================================================

echo Creating  kms-ethcan  ...

PACKAGE_NAME=kms-ethcan_$PACKAGE_VERSION

mkdir Packages
mkdir Packages/$PACKAGE_NAME
mkdir Packages/$PACKAGE_NAME/usr
mkdir Packages/$PACKAGE_NAME/usr/local
mkdir Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
mkdir Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION/Binaries
mkdir Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION/Includes
mkdir Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION/Includes/EthCAN
mkdir Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION/libraries

cp _DocUser/EthCAN.ReadMe.txt                         Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
cp Binaries/EthCAN_Tool                               Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION/Binaries
cp DoxyFile_en.txt                                    Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
cp EthCAN_Lib/_DocUser/EthCAN.EthCAN_Lib.ReadMe.txt   Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
cp EthCAN_Tool/_DocUser/EthCAN.EthCAN_Tool.ReadMe.txt Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
cp Firmware0/_DocUser/EthCAN.Firmware0.ReadMe.txt     Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
cp Firmware1/_DocUser/EthCAN.Firmware1.ReadMe.txt     Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
cp Import/KmsBase.KmsLib.ReadMe.txt                   Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
cp Import/Libraries/KmsLib.a                          Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION/Libraries
cp Includes/*.h                                       Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION/Includes
cp Includes/EthCAN/*.h                                Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION/Includes/EthCAN
cp Libraries/EthCAN_Lib.a                             Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION/libraries
cp Scripts/Import.sh                                  Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
cp Scripts/Import.sh.txt                              Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION
cp Scripts/Import.txt                                 Packages/$PACKAGE_NAME/usr/local/EthCAN-$VERSION

if [ "Darwin" = "$OS" ]
then
    productbuild --identifier "com.kms-quebec.ethcan" --root Packages/$PACKAGE_NAME/usr/local /Applications Packages/$PACKAGE_NAME.pkg

    if [ 0 != $? ] ; then
        echo ERROR  productbuild --identifier "com.kms-quebec.ethcan" --root Packages/$PACKAGE_NAME/usr/local /Applications Packages/$PACKAGE_NAME.pkg  failed
        exit 10
    fi
fi

if [ "Linux" = "$OS" ]
then
    mkdir Packages/$PACKAGE_NAME/DEBIAN

    cp Scripts/control Packages/$PACKAGE_NAME/DEBIAN/control

    dpkg-deb --build Packages/$PACKAGE_NAME

    if [ 0 != $? ] ; then
        echo ERROR  dpkg-deb --build Packages/$PACKAGE_NAME  failed
        exit 10
    fi
fi

rm -r Packages/$PACKAGE_NAME

# ===== End =================================================================

echo OK
