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
            case TEXT:
            *((int*)root->data) = strings_add((char*)root->data);
            break;

            /*Cases som må lage nye scope*/
            case FUNCTION_LIST:
            /*code*/
            break;

            //Sounds like function is a huge special case maybe?
            case FUNCTION:
            /*Probably
            plenty
            of
            code*/
            scope_add();
            break;

            case: VARIABLE:
            //How to differ between variable lookup and declaration/initialization?

            /*Case som BARE skal lage ny scope*/
            case BLOCK:
            scope_add();
            break;
        }
        if(root->n_children > 0){
            for (int i = 0; i < root->n_children; i++){
                bind_names(root->children[i]);
            }
        }
    }
    return;
}
