#!/bin/bash

# Creating folders
cd $PROJECT_DIR && \
echo "gta5view build version is $APPLICATION_VERSION" && \
mkdir build && \
mkdir assets && \
cd build && \
qmake-static DEFINES+=GTA5SYNC_BUILDTYPE_DEV "DEFINES+=GTA5SYNC_APPVER=\\\\\\\"$APPLICATION_VERSION\\\\\\\"" ../gta5view.pro && \
make -j 4 && \
cp -Rf release/*.exe $PROJECT_DIR/assets/
