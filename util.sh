#!/usr/bin/env sh
LIST=`pkg-config --static --libs-only-l gdlib | sed s/-l//g`
PRESENT=0

for i in $LIST; do
  if test "$i" = "$1"; then
    PRESENT=1
  fi
done

if test $PRESENT -eq 0; then
  echo false
else
  echo true
fi
