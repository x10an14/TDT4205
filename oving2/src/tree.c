#include "tree.h"


#ifdef DUMP_TREES
void
node_print ( FILE *output, node_t *root, uint32_t nesting )
{
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


node_t* node_init ( node_t *nd, nodetype_t type, void *data, uint32_t n_children, ... ) {

	//Tackling members that are known (AKA all except children)	
	nd->type = type;
	nd->data = malloc(sizeof *data);
	nd->data = data;
	nd->n_children = n_children;
	nd->children = malloc((sizeof *nd)*n_children);

	//Tackling kids/children
	va_list kids;
	va_start(kids, n_children);
	for(int i = 0; i < n_children; i++){
		nd->children[i] =(node_t*) va_arg(kids, int);
	}
	va_end(kids);

	return nd;
}


void node_finalize ( node_t *discard ) {
	free(discard->children);
	free(discard->data);
	free(discard);
	return;
}


void destroy_subtree ( node_t *discard ){

	if(discard->n_children == 0){
	//If node has no children (originally)
		node_finalize(discard);
	} else{
	//If node has children, call this method recursively for each kid, 
	//and then destroy node.
		for(int i = 0; i < discard->n_children; i++){
			destroy_subtree(discard->children[i]);
		}
		node_finalize(discard);
	}
	return;
}


