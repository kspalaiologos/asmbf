#!/usr/bin/perl

use Const::Fast;

const my $match_mp => '(\>+|[0-9]+\>|\<+|[0-9]+\<)';
const my $match_value => '(\++|[0-9]+\+|\-+|[0-9]+\-)';

print "#include <stdio.h>\n";
print "#include <stdlib.h>\n";
print "#include <stdint.h>\n";
print "#define OFF(x) ((x) - 'a')"
print "int main(void) {\n";
print "uint16_t tape = malloc(sizeof(uint16_t) * 65536), mp;\n";

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
    };
    
    $match =~ s/(\<+)//g and do {
        print "mp -= " . length($1) . "\n";
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
    };
    
    $match =~ s/(\-+)//g and do {
        print "tape[mp] -= " . length($1) . "\n";
    };
}

while(<STDIN>) {
    while(length $_ > 0) {
        # These don't really change that much.
        # They for sure tweak the `mp'. All the
        # instructions are about to get inlined,
        # therefore we don't need this much setup code.
        
        s/^AA//g and do {
            print "mp=OFF('b');\n"
        };
        
        s/^AB//g and do {
            print "mp=OFF('b');\n"
        };
        
        s/^AC//g and do {
            print "mp=OFF('a');\n"
        };
        
        s/^AD//g and do {
            print "mp=OFF('c');\n"
        };
        
        s/^AE$match_mp\|$match_value//g and do {
            tweak_mp($1);
            tweak_value($2);
        }
    }
}

print "}\n";
