#include "tree.h"
#include "symtab.h"


#ifdef DUMP_TREES
void node_print ( FILE *output, node_t *root, uint32_t nesting ){
    if ( root != NULL )
    {
        fprintf ( output, "%*c%s", nesting, ' ', root->type.text );
        if ( root->type.index == INTEGER )
            fprintf ( output, "(%d)", *((int32_t *)root->data) );
        if ( root->type.index == VARIABLE || root->type.index == EXPRESSION )
        {
            if ( root->data != NULL )
                fprintf ( output, "(\"%s\")", (char *)root->data );
            else
                fprintf ( output, "%p", root->data );
        }
        fputc ( '\n', output );
        for ( int32_t i=0; i<root->n_children; i++ )
            node_print ( output, root->children[i], nesting+1 );
    }
    else
        fprintf ( output, "%*c%p\n", nesting, ' ', root );
}
#endif

node_t * node_init ( node_t *nd, nodetype_t type, void *data, uint32_t n_children, ... ){
    va_list child_list;
    *nd = (node_t) { type, data, NULL, n_children,
        (node_t **) malloc ( n_children * sizeof(node_t *) )
    };
    va_start ( child_list, n_children );
    for ( uint32_t i=0; i<n_children; i++ )
        nd->children[i] = va_arg ( child_list, node_t * );
    va_end ( child_list );
    return nd;
}

void node_finalize ( node_t *discard ){
    if ( discard != NULL ){
        free ( discard->data );
        free ( discard->children );
        free ( discard );
    }
}

void destroy_subtree ( node_t *discard ){
    if ( discard != NULL ){
        for ( uint32_t i=0; i<discard->n_children; i++ )
            destroy_subtree ( discard->children[i] );
        node_finalize ( discard );
    }
}

void bind_names ( node_t *root ){
    if (root != NULL){
        switch(root->type.index){
            case FUNCTION_LIST:
                {scope_add();
                for(int i = 0; i < root->n_children; i++){
                    symbol_t *value = (symbol_t*) malloc(sizeof(symbol_t));
                    value->stack_offset = 0;
                    symbol_insert(root->children[i]->children[0]->data,value);
                }
                for(int i = 0; i < root->n_children; i++){
                    bind_names(root->children[i]);
                }
                scope_remove();}
            break;

            case FUNCTION:
                /*Alltid tre barn: Navn, parameterlist/variablelist, og block
                Ignore child[0] (name has already been handled in FUNCTION_LIST)*/
                scope_add();
                /*Iterate over all the parameters which are the children of child[1]*/
                if(root->children[1] != NULL){
                    node_t *current = root->children[1];
                    for(int i = 0; i < current->n_children; i++){
                        symbol_t *value = (symbol_t*) malloc(sizeof(symbol_t));
                        value->stack_offset = 8+4*(current->n_children-i-1);
                        symbol_insert((char*)current->children[i]->data,value);
                    }
                }
                bind_names(root->children[2]);
                scope_remove();
            break;

            case BLOCK:
                scope_add();
                /*children[0] will be a declaration list (if it exists). It will (after simplify tree) only have zero or more children of which all will be variables.*/
                /*Iterate over all the declarations and variable children of block*/
                if(root->children[0] != NULL){
                    node_t *declaration = root->children[0];
                    int cntr = 1;
                    for(int i = 0; i < declaration->n_children; i++){
                        /*Now we're iterating over all the variable children of the declaration node in this block*/
                        printf("Declaration children[%d] type: %s\n", i, declaration->children[i]->type->text);
                        node_t *variableList = declaration->children[i]->children[0];
                        for(int j = 0; j < variableList->n_children; i++){
                            symbol_t *value = (symbol_t*) malloc(sizeof(symbol_t));
                            value->stack_offset = -4*cntr;
                            symbol_insert((char*) variableList->children[j]->data, value);
                            cntr++;
                        }
                    }
                }
                bind_names(root->children[1]);
                scope_remove();
            break;

            case VARIABLE:
                {root->entry = symbol_get((char*)root->data);}
            break;

            case TEXT:
                {int *ptr = (int*) malloc(sizeof(int));
                *ptr = (int) strings_add((char*)root->data);
                root->data = ptr;}
            break;

            default:
                if(root->n_children > 0){
                   for (int i = 0; i < root->n_children; i++){
                        bind_names(root->children[i]);
                    }
                }
            break;
        }
    }
    return;
}
