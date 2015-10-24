#!/bin/bash

echo "Creating 8 kB random file testfile"
dd if=/dev/urandom of=testfile bs=8k count=1

./a.out