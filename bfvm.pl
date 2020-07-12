#!/usr/bin/perl

use Const::Fast;

const my $match_mp => '(\>*|[0-9]+\>|\<*|[0-9]+\<)';
const my $match_value => '(\+*|[0-9]+\+|\-*|[0-9]+\-)';

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
        return;
    };
    
    # Match some normal brainfucky movements.
    $match =~ s/(\>+)//g and do {
        print "mp += " . length($1) . "\n";
        return;
    };
    
    $match =~ s/(\<+)//g and do {
        print "mp -= " . length($1) . "\n";
        return;
    };
}

sub tweak_value {
    my $match = shift;
    
    # If we have any digits inside the match
    # this probably means, we've got a RLE-d string.
    $match =~ s/([0-9]+)(\+|\-)//g and do {
        print "tape[mp] += $1;\n" if $2 eq "+";
        print "tape[mp] -= $1;\n" if $2 eq "-";
        return;
    };
    
    # Match some normal brainfucky movements.
    $match =~ s/(\++)//g and do {
        print "tape[mp] += " . length($1) . "\n";
        return;
    };
    
    $match =~ s/(\-+)//g and do {
        print "tape[mp] -= " . length($1) . "\n";
        return;
    };
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
        # These don't really change that much.
        # They for sure tweak the `mp'. All the
        # instructions are about to get inlined,
        # therefore we don't need this much setup code.
        
        s/^AA//g;
        s/^AB//g;
        s/^AC//g;
        s/^AD//g;
        
        s/^AE$match_mp\|$match_value//g and do {
            tweak_mp($1);
            tweak_value($2);
        };
        
        s/^AF$match_mp\|//g and do {
            tweak_mp($1);
            print "tape[mp]=0;\n";
        };
        
        s/^AG$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]+=t0;\n";
        };
        
        s/^AH$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=tape[mp]&&t0;\n";
        };
        
        s/^AI$match_mp\|//g and do {
            tweak_mp($1);
            print "tape[mp]--;\n";
        };
        
        s/^AJ$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]/=t0;\n";
        };
        
        s/^AK$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=tape[mp]==t0;\n";
        };
        
        s/^AL$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=tape[mp]>=t0;\n";
        };
        
        s/^AM$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=tape[mp]>t0;\n";
        };
        
        s/^AN$match_mp\|//g and do {
            tweak_mp($1);
            print "tape[mp]=inchar();\n";
        };
        
        s/^BZ$match_mp\|//g and do {
            tweak_mp($1);
            print "tape[mp]++;\n";
        };
        
        s/^AO$match_mp\|//g and do {
            tweak_mp($1);
            print "ip=tape[mp];\ng=0;\nbreak;\n";
        };
        
        s/^AP$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "if(t){ip=tape[mp];g=0;break;}\n";
        };
        
        s/^AQ$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "if(!t){ip=tape[mp];g=0;break;}\n";
        };
        
        s/^$match_value|AR\|//g and do {
            print "case " . suck_constant($1) . ":\n";
        };
        
        s/^AS$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=tape[mp]<=t0;\n";
        };
        
        s/^AT$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=tape[mp]<t0;\n";
        };
        
        s/^AU$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=tape[mp]%t0;\n";
        };
        
        s/^AV$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=t0;\n";
        };
        
        s/^AW$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]*=t0;\n";
        };
        
        s/^AX$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=tape[mp]!=t0;\n";
        };
        
        s/^AY$match_mp\|//g and do {
            tweak_mp($1);
            print "tape[mp]=-tape[mp];\n";
        };
        
        s/^AZ$match_mp\|//g and do {
            tweak_mp($1);
            print "tape[mp]=!tape[mp];\n";
        };
        
        s/^BA$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]=tape[mp]||t0;\n";
        };
        
        s/^BB$match_mp\|//g and do {
            tweak_mp($1);
            print "putchar(tape[mp]);\n";
        };
        
        s/^BC$match_mp\|$match_mp\|//g and do {
            tweak_mp($2);
            print "t0=tape[mp+2*sp];sp--;\n";
            tweak_mp($1);
            print "tape[mp]=t0;\n";
        };
        
        s/^BD$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp+2*t0]=t0;sp++;\n";
        };
        
        s/^BE$match_mp\|$match_mp\|$match_mp\|//g and do {
            tweak_mp($2);
            print "t0=tape[mp];\n";
            tweak_mp($3);
            print "t1=tape[mp+2*t0];\n";
            tweak_mp($1);
            print "tape[mp]=t1;\n";
        };
        
        s/^BF$match_mp\|$match_mp\|$match_mp\|//g and do {
            tweak_mp($1);
            print "t0=tape[mp];\n";
            tweak_mp($2);
            print "t1=tape[mp];\n";
            tweak_mp($3);
            print "tape[mp+2*t0]=t1;\n";
        };
        
        s/^BG$match_mp\|$match_mp\|//g and do {
            emit_compound($2, $1);
            print "tape[mp]-=t0;\n";
        };
        
        s/^BH$match_mp\|$match_mp\|//g and do {
            tweak_mp($1);
            print "t0=tape[mp];\n";
            tweak_mp($2);
            print "t1=tape[mp];\n";
            tweak_mp($1);
            print "tape[mp]=t0;\n";
            tweak_mp($2);
            print "tape[mp]=t1;\n";
        };
        
        s/^BI$match_mp\|//g and do {
            tweak_mp($1);
            print "tape[mp]=0;\n";
        };
        
        s/^BJ$match_mp//g and do {
            tweak_mp($1);
            print "ip=tape[mp+2*sp];sp--;break;\n";
        };
        
        s/^BK//g and do {
            print "return 0;\n";
        };
    }
}

print "return 0;}}\n";
