#include "symtab.h"

// static does not mean the same as in Java.
// For global variables, it means they are only visible in this file.

// Pointer to stack of hash tables
static hash_t **scopes;

// Pointer to array of values, to make it easier to free them
//(Each entry pointer should be on this "stack"/list... to avoid doublefree)
static symbol_t **values;

// Pointer to array of strings, should be able to dynamically expand as new strings
// are added.
static char **strings;

// Helper variables for manageing the stacks/arrays
static int32_t scopes_size = 16, scopes_index = -1;
static int32_t values_size = 16, values_index = -1;
static int32_t strings_size = 16, strings_index = -1;


void symtab_init (void){
	scopes = (hash_t**)calloc(scopes_size, sizeof(hash_t*));
	values = (symbol_t**)calloc(values_size, sizeof(symbol_t*));
	strings = (char**)calloc(strings_size, sizeof(char*));
}

void symtab_finalize (void){
	for (int i = 0; i < scopes_index; i++){
		scope_remove();
	}
	scopes_index = -1;
	for (int i = values_index; i >= 0; i++){
		// free(values[i]->label);
		free(values[i]);
	}
	values_index = -1;
	for (int i = strings_index; i >= 0; i++){
		free(strings[i]);
	}
	strings_index = -1;
	free(scopes);
	free(values);
	free(strings);
}

int32_t strings_add (char *str){
	strings_index++;
	if(strings_index == strings_size){
		strings_size = 2*strings_size;
		strings = realloc(strings, strings_size*sizeof(char*));
	}
	strings[strings_index] = str;
	return strings_index;
}

void strings_output (FILE *stream){
	//Usage of fprintf
	fprintf(stream, ".data\n.INTEGER: .string\"%%d\"\n"); //Print first two lines
	for (int i = 0; i < strings_index; i++){
		fprintf(stream, ".STRING%d: .string \"%s\"\n",i,strings[i]);//Every string line
	}
	fprintf(stream, ".globl main\n");//Last line. (Took the liberty of adding "\n"...)
}

void scope_add (void){
	//Whenever we call Function_List, Functions, or Blocks, add a new scope to stack
	scopes_index++;
	if (scopes_index == scopes_size){
		scopes_size *= 2;
		scopes = realloc(scopes, scopes_size*sizeof(hash_t*));
	}
	//Add scope
	hash_t *ptr = malloc(sizeof(hash_t));
	ptr = ght_create(8);
	scopes[scopes_index] = ptr;
}

void scope_remove (void){
	if (scopes_index > -1){
		ght_finalize(scopes[scopes_index]);
		scopes_index--;
	}
}

void symbol_insert (char *key, symbol_t *value){
	values_index++;
	if(values_index == values_size){
		values_size *= 2;
		values = realloc(values, values_size*sizeof(symbol_t*));
	}
	//Fix the rest of the symbol_t members:
	value->depth = scopes_index;
	value->label = STRDUP(key);
	values[values_index] = value;
	ght_insert(scopes[scopes_index], value, strlen(key), value->label);
	// Keep this for debugging/testing
	#ifdef DUMP_SYMTAB
	fprintf ( stderr, "Inserting (%s,%d)\n", key, value->stack_offset );
	#endif
}

symbol_t *symbol_get (char *key){
	symbol_t* result = NULL;
	int len = strlen(key);
	int found = 0;
	for (int i = scopes_index; i >= 0; i++){
		result = (symbol_t*) ght_get(scopes[i], len, key);
		if(result != NULL){
			found = 1;
			break;
		}
	}
	// Keep this for debugging/testing
	#ifdef DUMP_SYMTAB
		if ( result != NULL )
			fprintf ( stderr, "Retrieving (%s,%d)\n", key, result->stack_offset );
	#endif
	return result;
}
