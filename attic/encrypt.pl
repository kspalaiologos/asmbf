#!/usr/bin/perl

use warnings;
use strict;

use File::Basename;
use Term::ReadKey;

sub switch_ext {
    my $path = shift @_;
    my $text = shift @_;
    my ($name,$dir,$ext) = fileparse($path,'\..*');
    return $dir . $name . $text;
}

my $schema = shift(@ARGV);

print "Password: ";
ReadMode 'noecho';
my $password = ReadLine 0;
chomp $password;
ReadMode 'normal';

print "\nRetype: ";
ReadMode 'noecho';
my $password2 = ReadLine 0;
chomp $password2;
ReadMode 'normal';

print "\n";

if($password ne $password2) {
    print "Passwords don't match.\n";
    exit 1;
}

foreach my $file(@ARGV) {
    if($schema =~ /d/) {
        my $out = switch_ext($file, '.txt');
        system("openssl aes-128-cbc -d -salt -in $file -out $out -pass 'pass:$password'");
        unlink($file) if $? == 0;
    } else {
        my $out = switch_ext($file, '.aes');
        system("openssl aes-128-cbc -salt -in $file -out $out -pass 'pass:$password'");
        unlink($file) if $? == 0;
    }
}
