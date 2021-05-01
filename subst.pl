#!/usr/bin/perl

# subst.pl: convert to/from trivial brainfuck subsitutions.
# golfed purely for fun.

use Generator::Object;

@i = split //, "><+-.,[]";
%s = (
    "Ook!" => [ "Ook. Ook?", "Ook? Ook.", "Ook. Ook.", "Ook! Ook!", "Ook! Ook.", "Ook. Ook!", "Ook! Ook?", "Ook? Ook!" ],
    "Alphuck" => [ "a", "c", "e", "i", "j", "o", "p", "s" ],
    "Btjzxgquartfrqifjlv" => [ "f", "rqi", "qua", "rtf", "lv", "j", "btj", "zxg" ],
    "Binaryfuck" => [ "010", "011", "000", "001", "100", "101", "110", "111" ],
    "Blub" => [ "Blub. Blub?", "Blub? Blub.", "Blub. Blub.", "Blub! Blub!", "Blub! Blub.", "Blub. Blub!", "Blub! Blub?", "Blub? Blub!" ],
    "Brainfuck²" => [ "Ook!", "Alphuck", "Fuckfuck", "POGAACK", "Unibrain", "Wordfuck", "Brainfuck²", "ZZZ" ],
    "Colonoscopy" => [ ";};", ";{;", ";;};", ";;{;", ";;;};", ";;;{;", "{{;", "}};" ],
    "DetailedFuck" => [ "MOVE THE MEMORY POINTER ONE CELL TO THE RIGHT", "MOVE THE MEMORY POINTER ONE CELL TO THE LEFT", "INCREMENT THE CELL UNDER THE MEMORY POINTER BY ONE", "DECREMENT THE CELL UNDER THE MEMORY POINTER BY ONE", "REPLACE THE CELL UNDER THE MEMORY POINTER'S VALUE WITH THE ASCII CHARACTER CODE OF USER INPUT", "PRINT THE CELL UNDER THE MEMORY POINTER'S VALUE AS AN ASCII CHARACTER", "IF THE CELL UNDER THE MEMORY POINTER'S VALUE IS ZERO INSTEAD OF READING THE NEXT COMMAND IN THE PROGRAM JUMP TO THE CORRESPONDING COMMAND EQUIVALENT TO THE ] COMMAND IN BRAINFUCK", "IF THE CELL UNDER THE MEMORY POINTER'S VALUE IS NOT ZERO INSTEAD OF READING THE NEXT COMMAND IN THE PROGRAM JUMP TO THE CORRESPONDING COMMAND EQUIVALENT TO THE [ COMMAND IN BRAINFUCK" ],
    "Fluffle Puff" => [ "b", "t", "bf", "pl", "!", "?", "*gasp*", "*pomf*" ],
    "fuckbeEs" => [ "f", "u", "c", "k", "b", "e", "E", "s" ],
    "GERMAN" => [ "LINKS", "RECHTS", "ADDITION", "SUBTRAKTION", "EINGABE", "AUSGABE", "SCHLEIFENANFANG", "SCHLEIFENENDE" ]
);

sub u{print"usage: subst [from/to] [language].\nlanguage list:\n";print"* ".$_.$/foreach keys%s;exit 1;}($d,$l)=@ARGV;u()
if!defined$l;@t=@{$s{$l}};@v;if("to"eq$d){@v=map{[$i[$_],$t[$_]]}0..$#i;}else{@v=reverse sort{length@$a[0]<=>length@$b[0]
}map{[$t[$_],$i[$_]]}0..$#t;}while(<STDIN>){foreach$e(@v){($k,$v)=@$e;s/\Q$k\E/$v\n/g;}s/\n//g if"from"eq$d;print;}
