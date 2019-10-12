#!/usr/bin/perl

$file = $ARGV[0];
`$file`;

$file  =~ s{\.[^.]+$}{};

`bfi $file.b < $file.in > $file.aout`;
$diff = `diff $file.aout $file.out`;

exit length($diff);
