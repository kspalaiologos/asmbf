#!/usr/bin/perl

$file = $ARGV[0];
`$file`;

print "$file\t";

$file  =~ s{\.[^.]+$}{};

`bfi $file.b < $file.in > $file.aout`;
$diff = `diff $file.aout $file.out`;

if(length($diff) > 0) {
	print " *** TEST FAILED!\tOutput diff:\n";
	print $diff;
} else {
	print " *** TEST PASS.\n";
}

exit length($diff);
