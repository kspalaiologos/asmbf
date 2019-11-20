#!/usr/bin/perl -p

my ($style) = @ARGV;

s/\d+(.)/$1x$&/ge if $style eq "postfix" or $style == undef;
s/(.)\d+/$1x$&/ge if $style eq "prefix";
