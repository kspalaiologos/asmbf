#!/usr/bin/perl

use Term::ANSIColor;

foreach my $file(@ARGV) {
    system("$ENV{'HOME'}/.asmbf/bfmake $file");

    printf "%-40s", $file;

    $file  =~ s{\.[^.]+$}{};

    `timeout 20s $ENV{'HOME'}/.asmbf/bfi $file.b < $file.in > $file.aout`;
    $diff = `diff $file.aout $file.out`;

    if(length($diff) > 0) {
        print color('bold red');
        print " *** TEST FAILED!\tOutput diff:\n";
        print $diff;
        exit length($diff);
    } else {
        print color('bold green');
        print " *** TEST PASS.\n";
    }
    
    print color('reset');
}

exit 0;
