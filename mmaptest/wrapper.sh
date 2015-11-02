#!/bin/bash

echo "Creating 8 kB random file testfile"
dd if=/dev/urandom of=testfile bs=8192 count=1

echo "Creating 8193 B random file nonPagedfile"
dd if=/dev/urandom of=nonPagedfile bs=8193 count=1

echo "Creating 10 B random file smallfile"
dd if=/dev/urandom of=smallfile bs=10 count=1

./a.out
./b.out
./c.out
./d.out
./e.out
./f.out # Will "Bus error (core dumped)" here.