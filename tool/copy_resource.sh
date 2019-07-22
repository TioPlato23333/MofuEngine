#!/bin/bash

ROOT_DIR=$(cd `dirname $0`; pwd)
mkdir -p $1
cp -r "${ROOT_DIR}/../demo/resource" $1
