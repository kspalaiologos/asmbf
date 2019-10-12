#!/usr/bin/perl

# asm2bf label preprocessor.
# C'mon, who doesn't love regexes?

# Released to the public domain by Krzysztof Szewczyk.

$n=0;%o=();$_=do{local$/;<>};s/^[ \t]*@([A-Za-z_]+).*$/$o{$1}=++$n;'lbl '.$n."\n";/gem;
s/(%[A-Za-z_]+|"[^"\n]*%[A-Za-z_]+)/substr($1,0,1)eq'"'?$1:%o{substr$1,1}/ge;print;
