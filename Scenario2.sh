#!/bin/bash

rm -f *.txt

./host 1 2 25 1 1 1 1 2 2 "Message from (1,2) to (2,2)"&

./bridge 1 4&

./router 1 1 1 2 19 2 5 2 2 17&

./bridge 2 4&

./host 2 2 87 2 5 2 1&
