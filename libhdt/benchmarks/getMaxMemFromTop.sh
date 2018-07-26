#!/bin/bash

file=$1

[ -z ${file} ] && echo "need top log file as arg" && exit 1;

maxMem=0
while read p; do
  topNotation=`echo ${p} | awk '{print toupper($6)}'`

  if [[ $topNotation =~ ^[0-9]*$ ]] ; then
    memKb=${topNotation}
  else
    numBytes=`echo ${topNotation} | numfmt --from=iec`
    memKb=$((numBytes / 1024))
  fi
  ((memKb > maxMem)) && maxMem=$memKb
  # echo ${topNotation}
done <${file}
echo ${maxMem}