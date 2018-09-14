#!/usr/bin/env bash

./echo_arg.out csc209 > echo_out.txt
./echo_stdin.out < ./echo_stdin.c
./count.out 210 | wc
ls -S | ./echo_stdin.out

