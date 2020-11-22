#!/usr/bin/perl

use Term::ANSIColor;

my $blocking = 0;

if($ARGV[0] eq '--blocking') {
    $blocking = 1;
    shift @ARGV;
}

my $max = scalar @ARGV;
my $current = 0;

my @proc;

foreach my $file(@ARGV) {
    $current++;

    my $pid;

    if(!$blocking) {
        $pid = fork();
        push @proc, $pid;
    } else {
        $pid = 0;
    }

    if ($pid == 0) {
        my $myc = $current;

        open my $fh, '<', "$file"; 
        my $firstLine = <$fh>; 
        close $fh;

        my $makeflags = '';

        if($firstLine =~ m/;flags=(.*)$/) {
            $makeflags = $1;
        }

        my $code = system("$ENV{'HOME'}/.asmbf/bfmake $makeflags $file > /dev/null");

        if($file =~ /invalid/) {
            die " *** TEST FAILED: $file shouldn't build. Code: $code" if($code == 0);
        } else {
            die " *** TEST FAILED: $file should build." if($code != 0);
        }

        $file  =~ s{\.[^.]+$}{};

        my $code = system("timeout 20s $ENV{'HOME'}/.asmbf/bfi $file.b < $file.in > $file.aout");

        if($code != 0) {
            die " *** TEST FAILED: $file.asm, interpreter crashed.";
        }

        $diff = `diff $file.aout $file.out`;

        if(length($diff) > 0) {
            print color('bold yellow');
            printf "[%03d/%03d] ", $current, $max;
            print color('reset');
            printf "%-30s", $file;
            print color('bold red');
            print " *** TEST FAILED!\tOutput diff:\n";
            print $diff;
            print color('reset');
            exit length($diff);
        } else {
            print color('bold yellow');
            printf "[%03d/%03d] ", $current, $max;
            print color('reset');
            printf "%-30s", $file;
            print color('bold green');
            print " *** TEST PASS.";
            print color('reset');
            print "\n";
        }
        
        if(!$blocking) {
            exit 0;
        }
    }
}

if(!$blocking) {
    while(($pid = wait()) > 0){
        if($? > 0) {
            kill 'TERM', @proc;
            exit 1;
        }
    }
}

exit 0;
