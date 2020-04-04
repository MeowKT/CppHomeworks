#!/bin/bash

nasm -f elf64 -o MultLongLong.o MultLongLong.asm && ld -o mul MultLongLong.o
i=0
while [ "$i" -le 100 ]; do
  python3 gen.py $i > in
  python3 stupid.py  < in > out-stupid
  ./mul  < in > out

  if ! cmp -s out out-stupid
  then
    echo 'Found failing test!'
    break
  fi
  echo 'OK'
  i=$(( i + 1 ))
  rm out in out-stupid
done
rm mul MultLongLong.o
