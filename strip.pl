#!/usr/bin/perl

my ($style) = @ARGV;
$_ = do { local $/; <> };
do {
	$l = length;
	s/[\n\r\t ]+//g
	s/[^\+\-\<\>\[\]\.\,]+//g if $style eq "--generic" or $style == undef;
	s/[^\+\-\<\>\[\]\.\,0-9]+//g if $style eq "--keep-rle" or $style == undef;
	s/<>//g;
	s/><//g;
	s/\+-//g;
	s/-\+//g;
} while($l != length);

print;
