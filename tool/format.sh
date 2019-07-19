#!/bin/bash

function format_dir() {
  working_dir=$1
  echo "Formatting $working_dir..."
  clang-format -i $(find "${working_dir}" | grep -E "${working_dir}/[^/]*\.(cpp|cc|h|mm|m)")
}

function format_dir_recursive() {
  working_dir=$1
  echo "Formatting $working_dir..."
  clang-format -i $(find "${working_dir}" | grep -E ".*\.(cpp|cc|h|mm|m)")
}


base_dir=$(cd `dirname $0`; pwd)

if [ -z "$(which clang-format)" ]; then
  echo "No clang-format found. Do nothing."
  exit 0
fi

format_dir "${base_dir}/../MofuEngine/core"
format_dir "${base_dir}/../MofuEngine/video"
format_dir "${base_dir}/../MofuEngine/audio"
format_dir_recursive "${base_dir}/../demo"
