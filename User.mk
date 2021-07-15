
# Author    KMS - Martin Dubois, P.Eng.
# Copyright (C) 2021 KMS
# Product   EthCAN
# File      User.mk

IMPORT_FOLDER = ../Import

COMPILE_FLAGS = -fpic -ggdb -O2
# COMPILE_FLAGS = -DNDEBUG -fpic -ggdb -O2

INCLUDE_IMPORT = -I $(IMPORT_FOLDER)/Includes
KMS_LIB_A = $(IMPORT_FOLDER)/Libraries/KmsLib.a
