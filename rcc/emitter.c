
#include "symtab.h"
#include "emitter.h"
#include <string.h>
#include <stdlib.h>

static struct code_chunk * code = NULL, * code_head = NULL;
static struct data_chunk * data = NULL, * data_head = NULL;

void emit(char * instruction, char * param1, char * param2) {
    struct code_chunk * instance = malloc(sizeof(struct code_chunk));

    if(!code) {
        code = code_head = instance;
        code->instruction = instruction;
        code->param1 = param1;
        code->param2 = param2;
        code->next = NULL;
    } else {
        code_head->next = instance;
        code_head->next->instruction = instruction;
        code_head->next->param1 = param1;
        code_head->next->param2 = param2;
        code_head->next->next = NULL;
        code_head = code_head->next;
    }
}

void emit_txt(unsigned char linkage, char * identifier, char * string) {
    struct data_chunk * instance = malloc(sizeof(struct data_chunk));

    if(!data) {
        data = data_head = instance;
        data->next = NULL;
        data->type = Txt;
        data->linkage = linkage;
        data->identifier = identifier;
        data->value.char_sequence = string;
    } else {
        data_head->next = instance;
        data_head->next->next = NULL;
        data_head->next->type = Txt;
        data_head->next->linkage = linkage;
        data_head->next->identifier = identifier;
        data_head->next->value.char_sequence = string;
        data_head = data_head->next;
    }
}

void emit_data(unsigned char linkage, char * identifier, int type, int n) {
    struct data_chunk * instance = malloc(sizeof(struct data_chunk));

    if(!data) {
        data = data_head = instance;
        data->next = NULL;
        data->type = type;
        data->identifier = identifier;
        data->linkage = linkage;
        data->value.constant = n;
    } else {
        data_head->next = instance;
        data_head->next->next = NULL;
        data_head->next->type = type;
        data_head->next->linkage = linkage;
        data_head->next->identifier = identifier;
        data_head->next->value.constant = n;
        data_head = data_head->next;
    }
}

void flush_code(FILE * f) {
    {
        struct data_chunk * iterator = data;

        while(iterator) {
            switch(iterator->type) {
                case Txt:
                    if(iterator->identifier) {
                        fprintf(f,
                            iterator->linkage == ExternalLinkage ?
                                "&%s\n" : "&_internal_%s\n"
                            , iterator->identifier
                        );
                    }

                    fprintf(f, "txt %s\n", iterator->value.char_sequence);
                    break;
                case Db:
                    if(iterator->identifier) {
                        fprintf(f,
                            iterator->linkage == ExternalLinkage ?
                                "&%s\n" : "&_internal_%s\n"
                            , iterator->identifier
                        );
                    }

                    fprintf(f, "db %d\n", iterator->value.constant);
                    break;
                case Seg:
                    fprintf(f, "seg %d\n", iterator->value.constant);
                    break;
                case Off:
                    fprintf(f, "org %d\n", iterator->value.constant);
                    break;
                case Stk:
                    fprintf(f, "stk %d\n", iterator->value.constant);
                    break;
            }

            iterator = iterator->next;
        }
    }
    {
        struct code_chunk * iterator = code;
        
        while(iterator) {
            if(!strcmp(iterator->instruction, "mov")
            && iterator->param1 && iterator->param2
            && !strcmp(iterator->param1, iterator->param2)) {
                iterator = iterator->next;
                continue;
            }

            fprintf(f, "%s", iterator->instruction);

            if(iterator->param1 != NULL) {
                fprintf(f, " %s", iterator->param1);
            }

            if(iterator->param2 != NULL) {
                fprintf(f, ", %s\n", iterator->param2);
            } else
                fprintf(f, "\n");

            iterator = iterator->next;
        }
    }
}
