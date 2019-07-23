#!/bin/bash

function format_dir() {
  WORKING_DIR=$1
  echo "Formatting $WORKING_DIR..."
  clang-format -i $(find "${WORKING_DIR}" | grep -E "${WORKING_DIR}/[^/]*\.(cpp|cc|h|mm|m)")
}

function format_dir_recursive() {
  WORKING_DIR=$1
  echo "Formatting $WORKING_DIR..."
  clang-format -i $(find "${WORKING_DIR}" | grep -E ".*\.(cpp|cc|h|mm|m)")
}

ROOT_DIR=$(cd `dirname $0`; pwd)
if [ -z "$(which clang-format)" ]; then
  echo "No clang-format found. Do nothing."
  exit 0
fi
format_dir "${ROOT_DIR}/../MofuEngine/core"
format_dir "${ROOT_DIR}/../MofuEngine/video"
format_dir "${ROOT_DIR}/../MofuEngine/audio"
format_dir "${ROOT_DIR}/../MofuEngine/util"
format_dir_recursive "${ROOT_DIR}/../MofuEngine/template"
format_dir_recursive "${ROOT_DIR}/../demo"
