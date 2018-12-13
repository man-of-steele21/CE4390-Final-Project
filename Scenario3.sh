#!/bin/bash

rm -f *.txt

./host 1 5 25 1 9 1 1 1 6 "(1,5) talks to (1,6)"&

./host 1 6 75 1 9 1 3 1 5 "This is (1,6)"&

./host 1 8 18 1 9 1 2 1 6 "(1,8) chats with (1,6)"&

./bridge 1 5&
