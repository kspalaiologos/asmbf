
#ifndef NODE_H
#define NODE_H

enum {
	ProgramDeclaration, StackSize, ProgramOrigin, Address,
    Segment, Offset, ModuleDeclaration, DataDeclaration, DataName,
    ExternalDataDeclaration,

    InternalFunctionDeclaration, FunctionArityNode, ImperativeReturn,
    ImperativeAssign, RegisterReference, ConstantWrapper, ImperativeAssignAdd,
    MinusExpression, PlusExpression, RegisterList, Preserve, ImperativeExit,
    ExternalFunctionDeclaration
};

struct node {
    struct node * child, * brother;
    char * value;
    unsigned int type;
	unsigned int line, col;
};

struct node *new_node(
	unsigned int type,
	struct node *child, struct node *brother,
	char *value,
	unsigned int line, unsigned int col);
struct node * append_brother(struct node * parent, struct node * brother);

#endif
