#!/bin/bash

gcc -E -I. -Iinclude "$1""$2""$3""$4""$5" > "$1.i"
sed '/^#/ d' < "$1.i" > "$1.p"
rm -f "$1.i"
