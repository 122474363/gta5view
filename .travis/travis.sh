#!/bin/bash

if [ `git name-rev --tags --name-only $(git rev-parse HEAD)` == "undefined" ]; then export APPLICATION_VERSION="$PACKAGE_VERSION-$TRAVIS_BUILD_NUMBER"; else export APPLICATION_VERSION=`git name-rev --tags --name-only $(git rev-parse HEAD)`; fi
export PROJECT_DIR=$(pwd)

.travis/$BUILD_SCRIPT
