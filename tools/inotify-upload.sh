#!/bin/bash

if [ -z $1 ]; then
  echo "error: no file specified"
  exit 1
fi

while true; do
  inotifywait -e close_write $@
  make upload
done
