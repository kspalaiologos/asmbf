#!/usr/bin/perl

my ($segment_policy) = @ARGV;

%labels = ();
$segment = 0;
$offset = 0;

while(<STDIN>) {
	$repl = $segment if $segment_policy eq "--keep-segments" or $segment_policy == undef;
	$repl = 0 if $segment_policy eq "--discard-segments";
	
	s/^[ \t]*&([A-Za-z_]+[A-Za-z0-9_]*).*$/$labels{$1}=$segment+$offset;"";/ge;
	s/(\*([A-Za-z_]+[A-Za-z0-9_]*)|"[^"\n]*\*([A-Za-z_]+[A-Za-z0-9_]*))/substr($1,0,1)eq'"'?$1:%labels{substr$1,1}/ge;
	$offset++ if $_ =~ m/^[ \t]*db_/;
	$offset += length($1) if $_ =~ m/^[ \t]*txt[ \t]*"(.*)"/;
	$segment = int($1) if $_ =~ m/^[ \t]*seg[ \t]*([0-9]+)/;
	$offset = int($1) if $_ =~ m/^[ \t]*org[ \t]*([0-9]+)/;
	print $_;
}
