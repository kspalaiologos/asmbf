#!/usr/bin/perl

use Term::ANSIColor;

my @proc;

foreach my $file(@ARGV) {
    my $pid = fork();
    push @proc, $pid;

    if ($pid == 0) {
        system("$ENV{'HOME'}/.asmbf/bfmake $file");

        $file  =~ s{\.[^.]+$}{};

        `timeout 20s $ENV{'HOME'}/.asmbf/bfi $file.b < $file.in > $file.aout`;
        $diff = `diff $file.aout $file.out`;

        if(length($diff) > 0) {
            printf "%-40s", $file;
            print color('bold red');
            print " *** TEST FAILED!\tOutput diff:\n";
            print $diff;
            print color('reset');
            exit length($diff);
        } else {
            printf "%-40s", $file;
            print color('bold green');
            print " *** TEST PASS.";
            print color('reset');
            print "\n";
        }
        
        exit 0;
    }
}

while(($pid = wait()) > 0){
    if($? > 0) {
        kill 'TERM', @proc;
        exit 1;
    }
}

exit 0;
