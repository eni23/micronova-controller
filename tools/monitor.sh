#!/bin/bash

if [ -z "${MONITOR_PIDFILE}" ]; then
  MONITOR_PIDFILE=".monitorpid"
fi

if [ -z "${MONITOR_UPLOADNOTIFY_FILE}"]; then
  MONITOR_UPLOADNOTIFY_FILE=".upload-done"
fi


show_help() {
  echo "Usage: $0 ACTION [PORT] [BAUDRATE]"
}


if [ -z $1 ]; then
  show_help
  exit 1;
fi


if [[ $1 == "open" ]]; then
  if [ -z $2 ] || [ -z $3 ]; then
    show_help
    exit 1;
  fi
  echo $$ > "${MONITOR_PIDFILE}"
  screen $2 $3
  rm $MONITOR_PIDFILE
  exit 0
fi


if [[ $1 == "loop" ]]; then
  if [ -z $2 ] || [ -z $3 ]; then
    show_help
    exit 1;
  fi
  while true; do
    echo $$ > "${MONITOR_PIDFILE}"
    screen $2 $3
    rm $MONITOR_PIDFILE
    echo "wait for upload (press ctrl+c or disconnect device to exit)"
    while [ ! -f "${MONITOR_UPLOADNOTIFY_FILE}" ]; do
      if [ ! -c $2 ]; then
        echo "disconnected"
        exit 0
      fi
      echo -n "."
      sleep .5
    done
    rm "${MONITOR_UPLOADNOTIFY_FILE}"
    sleep .5
  done
  exit 0
fi


if [[ $1 == "close" ]]; then
  if [ -f "${MONITOR_PIDFILE}" ]; then
    SCREEN_PID=$(pgrep -P $(cat "${MONITOR_PIDFILE}"))
    TTY_PID=$(pgrep -P "${SCREEN_PID}")
    if [ -n "${SCREEN_PID}" ] && [ -n "${TTY_PID}" ]; then
      kill "${TTY_PID}" "${SCREEN_PID}"
    fi
  fi
  exit 0
fi


if [[ $1 == "notify-done" ]]; then
  touch "${MONITOR_UPLOADNOTIFY_FILE}"
  exit 0
fi


echo "Unkown action $1"
exit 1
