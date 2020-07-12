#!/usr/bin/perl

use Const::Fast;

const my ${match_mp} => '(\>*|[0-9]+\>|\<*|[0-9]+\<)';
const my ${match_value} => '(\+*|[0-9]+\+|\-*|[0-9]+\-)';

print "#include <stdio.h>\n";
print "#include <stdlib.h>\n";
print "#include <stdint.h>\n";
print "#define OFF(x) ((x) - 'a')\n";
print "uint8_t inchar(void) { uint8_t v = getchar(); return v < 0 ? 0 : v; }\n";
print "int main(void) {\n";
print "uint16_t tape=malloc(sizeof(uint16_t)*65536), mp, t0, sp, ip=0, g=1;\n";
print "while(1) switch(ip) {\n";
print "case 0:\n";
print "if(!g) return 0;\n";

sub tweak_mp {
    my $match = shift;
    
    # If we have any digits inside the match
    # this probably means, we've got a RLE-d string.
    $match =~ s/([0-9]+)(\>|\<)//g and do {
        print "mp += $1;\n" if $2 eq ">";
        print "mp -= $1;\n" if $2 eq "<";
        return 1;
    };
    
    # Match some normal brainfucky movements.
    $match =~ s/(\>+)//g and do {
        print "mp += " . length($1) . "\n";
        return 1;
    };
    
    $match =~ s/(\<+)//g and do {
        print "mp -= " . length($1) . "\n";
        return 1;
    };
    
    return 0;
}

sub tweak_value {
    my $match = shift;
    
    # If we have any digits inside the match
    # this probably means, we've got a RLE-d string.
    $match =~ s/([0-9]+)(\+|\-)//g and do {
        print "tape[mp] += $1;\n" if $2 eq "+";
        print "tape[mp] -= $1;\n" if $2 eq "-";
        return 1;
    };
    
    # Match some normal brainfucky movements.
    $match =~ s/(\++)//g and do {
        print "tape[mp] += " . length($1) . "\n";
        return 1;
    };
    
    $match =~ s/(\-+)//g and do {
        print "tape[mp] -= " . length($1) . "\n";
        return 1;
    };
    
    return 0;
}

sub suck_constant {
    my $match = shift;
    
    $match =~ s/([0-9]+)(\+|\-)//g and do {
        return  int($1) if $2 eq "+";
        return -int($1) if $2 eq "-";
    };
    
    $match =~ s/(\++)//g and return length($1);
    $match =~ s/(\-+)//g and return -length($1);
    
    return 0;
}

sub emit_compound {
    my ($a, $b) = @_;
    tweak_mp($b);
    print "t0=tape[mp];\n";
    tweak_mp($a);
}

while(<STDIN>) {
    while(length $_ > 0) {
        for($_) {
            # These don't really change that much.
            # They for sure tweak the `mp'. All the
            # instructions are about to get inlined,
            # therefore we don't need this much setup code.
            
            s/^AA//g and do last;
            s/^AB//g and do last;
            s/^AC//g and do last;
            s/^AD//g and do last;
            
            s/^AE${match_mp}Z${match_value}//g and do {
                tweak_mp($1);
                tweak_value($2);
                last;
            };
            
            s/^AF${match_mp}Z//g and do {
                tweak_mp($1);
                print "tape[mp]=0;\n";
                last;
            };
            
            s/^AG${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]+=t0;\n";
                last;
            };
            
            s/^AH${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=tape[mp]&&t0;\n";
                last;
            };
            
            s/^AI${match_mp}Z//g and do {
                tweak_mp($1);
                print "tape[mp]--;\n";
                last;
            };
            
            s/^AJ${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]/=t0;\n";
                last;
            };
            
            s/^AK${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=tape[mp]==t0;\n";
                last;
            };
            
            s/^AL${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=tape[mp]>=t0;\n";
                last;
            };
            
            s/^AM${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=tape[mp]>t0;\n";
                last;
            };
            
            s/^AN${match_mp}Z//g and do {
                tweak_mp($1);
                print "tape[mp]=inchar();\n";
                last;
            };
            
            s/^BZ${match_mp}Z//g and do {
                tweak_mp($1);
                print "tape[mp]++;\n";
                last;
            };
            
            s/^AO${match_mp}Z//g and do {
                tweak_mp($1);
                print "ip=tape[mp];\ng=0;\nbreak;\n";
                last;
            };
            
            s/^AP${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "if(t){ip=tape[mp];g=0;break;}\n";
                last;
            };
            
            s/^AQ${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "if(!t){ip=tape[mp];g=0;break;}\n";
                last;
            };
            
            s/^${match_value}ZARZ//g and do {
                print "case " . suck_constant($1) . ":\n";
                last;
            };
            
            s/^AS${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=tape[mp]<=t0;\n";
                last;
            };
            
            s/^AT${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=tape[mp]<t0;\n";
                last;
            };
            
            s/^AU${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=tape[mp]%t0;\n";
                last;
            };
            
            s/^AV${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=t0;\n";
                last;
            };
            
            s/^AW${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]*=t0;\n";
                last;
            };
            
            s/^AX${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=tape[mp]!=t0;\n";
                last;
            };
            
            s/^AY${match_mp}Z//g and do {
                tweak_mp($1);
                print "tape[mp]=-tape[mp];\n";
                last;
            };
            
            s/^AZ${match_mp}Z//g and do {
                tweak_mp($1);
                print "tape[mp]=!tape[mp];\n";
                last;
            };
            
            s/^BA${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]=tape[mp]||t0;\n";
                last;
            };
            
            s/^BB${match_mp}Z//g and do {
                tweak_mp($1);
                print "putchar(tape[mp]);\n";
                last;
            };
            
            s/^BC${match_mp}Z${match_mp}Z//g and do {
                tweak_mp($2);
                print "t0=tape[mp+2*sp];sp--;\n";
                tweak_mp($1);
                print "tape[mp]=t0;\n";
                last;
            };
            
            s/^BD${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp+2*t0]=t0;sp++;\n";
                last;
            };
            
            s/^BE${match_mp}Z${match_mp}Z${match_mp}Z//g and do {
                tweak_mp($2);
                print "t0=tape[mp];\n";
                tweak_mp($3);
                print "t1=tape[mp+2*t0];\n";
                tweak_mp($1);
                print "tape[mp]=t1;\n";
                last;
            };
            
            s/^BF${match_mp}Z${match_mp}Z${match_mp}Z//g and do {
                tweak_mp($1);
                print "t0=tape[mp];\n";
                tweak_mp($2);
                print "t1=tape[mp];\n";
                tweak_mp($3);
                print "tape[mp+2*t0]=t1;\n";
                last;
            };
            
            s/^BG${match_mp}Z${match_mp}Z//g and do {
                emit_compound($2, $1);
                print "tape[mp]-=t0;\n";
                last;
            };
            
            s/^BH${match_mp}Z${match_mp}Z//g and do {
                tweak_mp($1);
                print "t0=tape[mp];\n";
                tweak_mp($2);
                print "t1=tape[mp];\n";
                tweak_mp($1);
                print "tape[mp]=t0;\n";
                tweak_mp($2);
                print "tape[mp]=t1;\n";
                last;
            };
            
            s/^BI${match_mp}Z//g and do {
                tweak_mp($1);
                print "tape[mp]=0;\n";
                last;
            };
            
            s/^BJ${match_mp}Z//g and do {
                tweak_mp($1);
                print "ip=tape[mp+2*sp];sp--;break;\n";
                last;
            };
            
            s/^BK//g and do {
                print "return 0;\n";
                last;
            };
            
            print "FUUUUUUUUUCK\n";
            print;
            die;
        }
    }
}

print "return 0;}}\n";
