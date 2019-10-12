#!/usr/bin/perl

$file = $ARGV[0];
`$file`;

$file  =~ s{\.[^.]+$}{};

`bfi $file.b < $file.in > $file.aout`;
$diff = `diff $file.aout $file.out`;

if(length($diff) > 0) {
	print " *** TEST FAILED! Output diff:\n";
	print $diff;
}

exit length($diff);
