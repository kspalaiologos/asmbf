
#include "lexer.h"

int main(int argc, char * argv[]) {
    if(argc == 2)
        process_file(argv[1]);
    else
        process_stream(stdin);
    
    flush_code(stdout);
}
