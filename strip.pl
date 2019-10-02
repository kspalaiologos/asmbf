#!/usr/bin/perl

$_ = do { local $/; <> };
s/[\n\r\t ]+//g;
s/<>//g;
s/><//g;
s/\+-//g;
s/-\+//g;
s/[^\+\-\<\>\[\]\,\.]+//g;
print;
