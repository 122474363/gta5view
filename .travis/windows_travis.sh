#!/bin/bash

QT_VERSION=5.6.3
DOCKER_IMAGE=syping/qt5-static-mingw:$QT_VERSION
PROJECT_DIR_DOCKER=/gta5view/

cd $PROJECT_DIR && \
docker pull $DOCKER_IMAGE && \
docker run --rm \
	-v "${PROJECT_DIR}:${PROJECT_DIR_DOCKER}" \
	$DOCKER_IMAGE \
	/bin/bash -c "export PROJECT_DIR=${PROJECT_DIR_DOCKER} && export QT_SELECT=${QT_SELECT} && export PACKAGE_VERSION=${PACKAGE_VERSION} && export APPLICATION_VERSION=${APPLICATION_VERSION} && cd ${PROJECT_DIR_DOCKER} && .travis/windows_build.sh"
