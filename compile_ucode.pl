#!/usr/bin/perl -p

s/^[Rr][Ee][Mm].*$//;
s/^.{1,48}[\t ]*//;

if(m/^\*\*\*/) {
    # And the only way out is to...
    die;
}

s/[\n\r]//;
