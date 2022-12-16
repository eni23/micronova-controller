#!/bin/bash


if [ -z $1 ]; then
  echo "error: value defined"
  exit 1
fi

if [ -z $2 ]; then
  CONFIG_FILE="src/config.h"
else
  CONFIG_FILE=$2
fi


grep "#define ${1}" "${CONFIG_FILE}" | tr -s ' ' | cut -d" " -f4
