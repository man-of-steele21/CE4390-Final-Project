#!/bin/bash

rm -f *.txt

./host 1 3 59 1 9 1 1 1 2 "Hi to (1,2)"&

./host 1 2 27 1 9 7 1&

./bridge 1 4 2 3&

./bridge 2 4 1 4 5&

./bridge 3 4 1 6 7&

./bridge 4 4 2&

./bridge 5 4 2&

./bridge 6 4 3&

./bridge 7 4 3&


