#!/usr/bin/perl

foreach my $file(@ARGV) {
	system("$ENV{'HOME'}/.asmbf/bfmake $file");

	print "$file\t";

	$file  =~ s{\.[^.]+$}{};

	`timeout 20s $ENV{'HOME'}/.asmbf/bfi $file.b < $file.in > $file.aout`;
	$diff = `diff $file.aout $file.out`;

	if(length($diff) > 0) {
		print " *** TEST FAILED!\tOutput diff:\n";
		print $diff;
		exit length($diff);
	} else {
		print " *** TEST PASS.\n";
	}
}

exit 0;
