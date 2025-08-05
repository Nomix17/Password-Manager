#!/bin/bash

set -e

BASE_NAME=$(basename *.pro .pro)

if [ ! -d "../bin" ]; then
  mkdir ../bin
fi

cd ../bin

qmake6 ../src/*.pro
make clean
make

mkdir -p /opt/PasswordManager
cp -r ../bin /opt/PasswordManager/

cp ../run.sh /usr/bin/PasswordManager

cp ../PasswordManager.desktop /usr/share/applications/
