#!/usr/bin/env sh

LIST=`gdlib-config --features 2>/dev/null`
PRESENT=0

for i in $LIST; do
  if test $i = $1; then
    PRESENT=1
  fi
done

if test $PRESENT -eq 0; then
  echo false
else
  echo true
fi
