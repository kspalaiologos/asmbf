#!/bin/asmbf/bfmake

stk 2
org 0

db_ 48
amp r1, 9
rcl r2, r1
out r2

mov r1, 10
sto r1, 60
amp r1, 5
rcl r2, r1
out r2
