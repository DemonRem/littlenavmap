#!/bin/bash

set -e
set -x

cd ${APROJECTS}/atools

git pull --verbose  --tags

cd ${APROJECTS}/littlenavmap

git pull --verbose  --tags

