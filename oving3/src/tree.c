#include "tree.h"


#ifdef DUMP_TREES
void
node_print ( FILE *output, node_t *root, uint32_t nesting ){
    if ( root != NULL ){
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

void finalize_node(node_t *discard){
	free(discard->data);
	free(discard->entry);
	free(discard->children);
	free(discard);
}

node_t* simplify_tree ( node_t* node ){
    if ( node != NULL ){
        // Recursively simplify the children of the current node
        for ( uint32_t i=0; i<node->n_children; i++ ){
            node->children[i] = simplify_tree ( node->children[i] );
        }

        // After the children have been simplified, we look at the current node
        // What we do depend upon the type of node
        switch ( node->type.index ){
            // These are lists which needs to be flattened. Their structure
            // is the same, so they can be treated the same way.
            case FUNCTION_LIST: case STATEMENT_LIST: case PRINT_LIST:
            case EXPRESSION_LIST: case VARIABLE_LIST:
				if(node->n_children == 2){
					node_t *current; 
					current = node;
					node = current->children[0];
					node->n_children++;
					node->children = (node_t**) realloc(node->children, node->n_children*sizeof(node_t*));
					node->children[node->n_children-1] = current->children[1];
					finalize_node(current);
				}
                break;

            // Declaration lists should also be flattened, but their stucture is sligthly
            // different, so they need their own case
            
			case DECLARATION_LIST:
				if(node->n_children == 2){
					if(node->children[0] == NULL){
						node_t *child;
						child = node->children[1];
						free(node->children);
						node->children = (node_t**) malloc(sizeof(node_t*));
						node->n_children--;
						node->children[0] = child;
					} else {
						//Else do the same as the case above...
						node_t *current; 
						current = node;
						node = current->children[0];
						node->n_children++;
						node->children = (node_t**) realloc(node->children, node->n_children*sizeof(node_t*));
						node->children[node->n_children-1] = current->children[1];
						finalize_node(current);
					}
				}
                break;

            // These have only one child, so they are not needed
            case STATEMENT: case PARAMETER_LIST: case ARGUMENT_LIST:
				if(node->n_children == 1){
					node_t *current;
					current = node;
					node = current->children[0];
					finalize_node(current);
				}
                break;
			
            // Expressions where both children are integers can be evaluated (and replaced with
            // integer nodes). Expressions whith just one child can be removed (like statements etc above)
            case EXPRESSION:
				//Mistenker at feilen avhenger av min forståelse av hvilke cases som skal dekkes/hvordan case'ene fungerer.
				//liker ikke at vi selv må finne ut av hvilke cases vi skal dekke når dette ikke er skrevet eksplisitt i oppgaveteksten (PDF'en).
				//Sliter med å finne ut hvilken av if-testene nedenfor som forårsaker feilen. Mistenker det har noe å gjøre med den
				//nederste, men klarer ikke å tenke ut hva som er feil med den, eller hva som faktisk går galt...
				if(node->n_children == 2 &&
					node->children[0]->type.index == INTEGER &&
					node->children[1]->type.index == INTEGER){
					node_t *current = node;
					node = node->children[0];
					if(strcmp(current->data, "+") == 0){
						*((int*)node->data) = *((int*)node->data) + 
							(*(int*)current->children[1]->data);
					} else if(strcmp(current->data, "-") == 0){
						*((int*)node->data) = *((int*)node->data) -
							(*(int*)current->children[1]->data);
					} else if (strcmp(current->data, "*") == 0){
						*((int*)node->data) = *((int*)node->data) *
							(*(int*)current->children[1]->data);
					} else if (strcmp(current->data, "/") == 0){
						*((int*)node->data) = *((int*)node->data) /
							(*(int*)current->children[1]->data);
					} else if (strcmp(current->data, "==") == 0){
						*((int*)node->data) = *((int*)node->data) ==
							(*(int*)current->children[1]->data);
					} else if (strcmp(current->data, "!=") == 0){
						*((int*)node->data) = *((int*)node->data) !=
							(*(int*)current->children[1]->data);
					} else if (strcmp(current->data, "<=") == 0){
						*((int*)node->data) = *((int*)node->data) <=
							(*(int*)current->children[1]->data);
					} else if (strcmp(current->data, ">=") == 0){
						*((int*)node->data) = *((int*)node->data) >=
							(*(int*)current->children[1]->data);
					} else if (strcmp(current->data, "<") == 0){
						*((int*)node->data) = *((int*)node->data) <
							(*(int*)current->children[1]->data);
					} else if (strcmp(current->data, ">") == 0){
						*((int*)node->data) = *((int*)node->data) >
							(*(int*)current->children[1]->data);
					}
					finalize_node(current->children[1]);
					finalize_node(current);
				} else if (node->n_children == 1){
					node_t *other = node;
					node = node->children[0];
					if(other->data == NULL){
						free(other);
					} else if(strcmp(other->data, "-") &&
						node->children[0]->type.index == INTEGER){
						*((int*)node->data) = *((int*)node->data) -
							2*(*((int*)node->data));
						node->type = other->type;
						finalize_node(other);
					}
				}
				break;
		}
	}
	return node;
}
