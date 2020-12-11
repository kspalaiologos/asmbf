
; Build me with `bfmake -t 8ball.asm'!
; This is an example of _heavily_ optimized asm2bf code.

inc r1
raw .[
in r1
add r2, r1
nav r1
raw .]

mod r2, 20

#function gencase(text)
#   emit("<")
#   gen_text(text)
#   emit(">>")
#end

#function build_case(text)
#   gencase(text)
#   emit("]<]>[-")
#end

nav r2

#emit(">+<")
#times("raw .[\nraw .-", 19)
#emit(">[-")
#build_case("It is certain")
#build_case("As I see it, yes.")
#build_case("Reply hazy, try again.")
#build_case("Don't count on it.")
#build_case("It is decidedly so.")
#build_case("Most likely.")
#build_case("Ask again later.")
#build_case("My reply is no.")
#build_case("Without a doubt.")
#build_case("Outlook good.")
#build_case("Better not tell you now.")
#build_case("My sources say no.")
#build_case("Outlook not so good.")
#build_case("Cannot predict now.")
#build_case("Yes.")
#build_case("Yes - definitely.")
#build_case("You may rely on it.")
#build_case("Signs point to yes.")
#build_case("Concentrate and ask again.")
#gencase("Very doubtful.")
#emit("]")
