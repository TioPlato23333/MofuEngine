#! /bin/bash

ROOT_DIR=$(cd "$(dirname $0)"; pwd)
DEBUG=ON
GL_ES=OFF

mkdir "${ROOT_DIR}/build"
rm -rf "${ROOT_DIR}/build/cmake_install"
mkdir "${ROOT_DIR}/build/cmake_install"
cd "${ROOT_DIR}/build/cmake_install"
cmake "../.." -DDEBUG=${DEBUG} -DGL_ES=${GL_ES}
make -j8
