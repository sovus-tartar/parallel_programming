#!/bin/bash

for i in $(seq 1 16)
do
	./a.out $i 16
	echo -----------------------------------------------------------
done
