#include "node.h"

void display_line(pnode ptr, int indent) {
    switch (ptr->type) {
        case A:
            printf("%*c%s (%d)\n", indent, ' ', ptr->name, ptr->line);
            break;
        case B: 
            printf("%*c%s\n", indent, ' ', ptr->name);
            break;
        case C: 
            if (!strcmp(ptr->name, "INT"))
                printf("%*c%s: %d\n", indent, ' ', ptr->name, ptr->type_int);
            else if (!strcmp(ptr->name, "FLOAT"))
                printf("%*c%s: %f\n", indent, ' ', ptr->name, ptr->type_float);
            else if (!strcmp(ptr->name, "ID"))
                printf("%*c%s: %s\n", indent, ' ', ptr->name, ptr->type_id);
            else
                printf("%*c%s: %s\n", indent, ' ', ptr->name, ptr->type_id);
            break;
        default:
            break;
    }
}

void display(pnode ptr, int indent) {
    pnode cur;
    display_line(ptr, indent);
    indent += 4;
    cur = ptr->l;
    while (cur) {
        display(cur, indent);
        cur = cur->r;
    }
}

pnode const_B(char *name, int line) {
    pnode ptr = (pnode)malloc(sizeof(node));
    ptr->line = line;
    strcpy(ptr->name, name);
    ptr->type = B;
    ptr->l = NULL;
    return ptr;
}

pnode const_C(char *name, int line, char *con, int base) {
    pnode ptr = (pnode)malloc(sizeof(node));
    ptr->line = line;
    strcpy(ptr->name, name);
    ptr->type = C;
    if (!strcmp(ptr->name, "INT"))
        ptr->type_int = strtol(con, NULL, base);
    else if (!strcmp(ptr->name, "FLOAT"))
        ptr->type_float = atof(con);
    else
        strcpy(ptr->type_id, con);
    ptr->l = NULL;
    return ptr;
}

