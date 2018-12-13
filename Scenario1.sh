#!/bin/bash

rm -f *.txt

./host 1 1 23 1 9 1 1 1 3 "Hi, how are you?"&

./host 1 3 54 1 9 1 2&

./bridge 1 4&
