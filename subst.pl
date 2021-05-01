#!/usr/bin/perl

# subst.pl: convert to/from trivial brainfuck subsitutions.
# golfed purely for fun.

use Generator::Object;

@i = split //, "><+-.,[]";

# Note: Most of these substitutions have been taken from the esolang wiki. I didn't create any of these.
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
    "GERMAN" => [ "LINKS", "RECHTS", "ADDITION", "SUBTRAKTION", "EINGABE", "AUSGABE", "SCHLEIFENANFANG", "SCHLEIFENENDE" ],
    "HMHSWS" => [ "Move the pointer to the right", "Move the pointer to the left", "Increment the memory cell under the pointer", "Decrement the memory cell under the pointer", "Output the character signified by the cell at the pointer", "Input the character signified by the cell at the pointer", "Jump past the matching right bracket if the cell under the pointer is 0", "Jump back to the matching left bracket if the cell under the pointer is 0" ],
    "K-on fuck" => [ "うんうんうん", "うんうんたん", "うんたんうん", "うんたんたん", "たんうんうん", "たんうんたん", "たんたんうん", "たんたんたん" ],
    "Morsefuck" => [ ".--", "--.", "..-", "-..", "-.-", ".-.", "---", "..." ],
    "Omam" => [ "hold your horses now", "sleep until the sun goes down", "through the woods we ran", "deep into the mountain sound", "don't listen to a word i say", "the screams all sound the same", "though the truth may vary", "this ship will carry" ],
    "PenisScript" => [ "8=D", "8==D", "8===D", "8====D", "8=====D", "8======D", "8=======D", "8========D" ],
    "Pewlang" => [ "pew", "Pew", "pEw", "peW", "PEw", "pEW", "PeW", "PEW" ],
    "Pikalang" => [ "pipi", "pichu", "pi", "ka", "pikachu", "pikapi", "pika", "chu" ],
    "ReverseFuck" => [ "<", ">", "-", "+", ",", ".", "]", "[" ],
    "Revolution9" => [ "It's alright", "turn me on, dead man", "Number 9", "if you become naked", "The Beatles", "Paul is dead", "Revolution 1", "Revolution 9" ],
    "Roadrunner" => [ "meeP", "Meep", "mEEp", "MeeP", "MEEP", "meep", "mEEP", "MEEp" ],
    "Screamcode" => [ "AAAH", "AAAAGH", "FUCK", "SHIT", "!!!!!!", "WHAT?!", "OW", "OWIE" ],
    "Ternary" => [ "01", "00", "11", "10", "21", "20", "02", "12" ],
    "Triplet" => [ "001", "100", "111", "000", "010", "101", "110", "011" ],
    "UwU" => [ "OwO", "°w°", "UwU", "QwQ", "\@w\@", ">w<", "~w~", "¯w¯" ],
    "ZZZ" => [ "zz", "-zz", "z", "-z", "zzz", "-zzz", "z+z", "z-z" ],
    "{}" => [ "{{}{}}", "{{}{}{}}", "{}", "{{}}", "{{}{}{}{}{}}", "{{}{}{}{}}", "{{", "}}" ], # note: ambiguous - & []
    "!!Fuck" => [ "!!!!!#", "!!!!!!#", "!!!!!!!#", "!!!!!!!!#", "!!!!!!!!!#", "!!!!!!!!!!#", "!!!!!!!!!!!#", "!!!!!!!!!!!!#" ],
    "Fuckfuck" => [ "fuck", "shag", "boob", "tits", "cock", "knob", "arse", "butt" ], # what the fuck
    "oOo" => [ "ooo", "ooO", "oOo", "oOO", "Ooo", "OoO", "OOo", "OOO" ],
    "NOT" => [ "IT NEVER WAS", "IT WILL NEVER BE", "THIS IS NOT A BRAINFUCK DERIVATIVE", "IT HAS NOTHING TO DO WITH BRAINFUCK", "TO ANYBODY WHO SAYS THIS IS A BRAINFUCK DERVIATIVE:", "FUCK YOU", "SHUT UP YOU LITTLE BITCH", "SHIT THE FUCK UP" ]
);

sub u{print"usage: subst [from/to] [language].\nlanguage list:\n";print"* ".$_.$/foreach keys%s;exit 1;}($d,$l)=@ARGV;u()
if!defined$l;@t=@{$s{$l}};@v;if("to"eq$d){@v=map{[$i[$_],$t[$_]]}0..$#i;}else{@v=reverse sort{length@$a[0]<=>length@$b[0]
}map{[$t[$_],$i[$_]]}0..$#t;}while(<STDIN>){foreach$e(@v){($k,$v)=@$e;s/\Q$k\E/$v\n/g;}s/\n//g if"from"eq$d;print;}
