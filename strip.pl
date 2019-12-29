#!/usr/bin/perl

$_ = do { local $/; <> };
do {
	$l = length;
	s/[\n\r\t ]+//g;
	s/<>//g;
	s/><//g;
	s/\+-//g;
	s/-\+//g;
} while($l != length);
print;
