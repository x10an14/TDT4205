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
	scopes = (hash_t*)calloc(scopes_size, sizeof(hash_t*));
	values = (symbol_t*)calloc(values_index, sizeof(symbol_t*));
	strings = (char*)calloc(strings_size, sizeof(char*));
}

void symtab_finalize (void){
	for (int i = scopes_index; i >= 0; i--){
		//Delete current scope and all it holds
		//scope_remove()?
		//ght_finalize(scopes[i]);?
		//Delete corresponding values symbol I suppose?
	}
	free(scopes);
	free(values);
	free(strings);
}

int32_t strings_add (char *str){
	if(strings_index == strings_size){
		strings_size = 2*strings_size;
		strings = realloc(strings, strings_size*sizeof(char*));
	}
	strings_index++;
	strings[strings_index] = str;
	return strings_index;
}

void strings_output (FILE *stream){//USE fprintf!!!! (Ikke bry deg om plass sa studass og undass...)
	//Usage of fprintf
	fprintf(*stream, ".data\n.INTEGER: .string\"%%d\"\n"); //Print first two lines
	for (int i = 0; i < strings_index; i++){
		fprintf(*stream, ".STRING%d: .string \"%s\"\n",i,strings[i]);//Every string line
	}
	fprintf(*stream, ".globl main\n");//Last line. (Took the liberty of adding "\n"...)
	/*//Below is the hacky code I think I got working before I was told I could use fprintf.
	//This was based on googling and stack overflow of "how to concatenate C strings" av
	//forskjellige variasjoner...
	//First count space needed
	int len[strings_index+3];
	len[0] = 7; //strlen(".data\n")
	len[1] = 27; //strlen(".INTEGER: .string \"%%d\"\n")
	int tot_len = 34;
	for (int i = 0; i < strings_index; i++){
		int cur = 23 + strlen((char*)i) + strlen(strings[i]) -2; //-2 to ignore "\0"
		len[i+2] = cur;
		tot_len += cur;
		//len += 7 + 12 + 4; //strlen(".STRING") + strlen(": .string \"") + strlen("\"\n")
	}
	len[strings_index+2] = 12;
	tot_len += 12;
	// len += 12; //strlen(".globl main")+"\0"

	//Then allocate space needed
	char *out = (char*)malloc(tot_len);
	char *str = out;
	//Then concatenate
	int pos = 0;
	for (int i = -1; i < strings_index+1; i++){ //Will this work?
		char buffer[tot_len];
		if(i > -1 && i < strings_index){
			snprintf(buffer, tot_len, ".STRING%d: .string \"%s\"\n",i,strings[i]);
		} else if(i == -1){
			snprintf(buffer, tot_len, ".data\n.INTEGER: .string \"%%d\"\n");
		} else{
			snprintf(buffer, tot_len, ".globl main");
		}
		int cur = 0;
		while(*buffer[cur] != (char*)'\0'){
			*(str+pos+cur)= *(buffer+cur);
			cur++;
		}
		pos += cur;
	}

	//Finishing the stringstream:
	*(str + ++pos) = (char*)'\0';
	//Is the below line correct? Or should I replace str with stream from the start?
	stream = str;
	//Should I remove this line??
	free(out);*/
}

void scope_add (void){
	//Whenever we call Function_List, Functions, or Blocks, add a new scope to stack
	if (scopes_index == scopes_size){
		scopes_size *= 2;
		scopes = realloc(scopes, scopes_size*sizeof(hash_t*));
	}
	//Add scope
	hash_t *ptr = malloc(sizeof(hash_t));
	ptr = ght_create(8);
	scopes_index++;
	scopes[scopes_index] = ptr;
}

void scope_remove (void){
	if (scopes_index > -1){
		ght_finalize(scopes[scopes_index]);
		scopes_index--;
	}
}

void symbol_insert (char *key, symbol_t *value){
	if(values_index == values_size){
		values_size *= 2;
		values = realloc(values, values_size*sizeof(symbol_t*));
	}
	symbol_t *ptr = (symbol_t*)malloc(sizeof(symbol_t));
	ptr = *value;
	values_index++;
	values[values_index] = ptr;
	// ptr->...???
	// Keep this for debugging/testing
	#ifdef DUMP_SYMTAB
	fprintf ( stderr, "Inserting (%s,%d)\n", key, value->stack_offset );
	#endif
}

symbol_t *symbol_get (char *key){
	symbol_t* result = NULL;
	// Keep this for debugging/testing
	#ifdef DUMP_SYMTAB
		if ( result != NULL )
			fprintf ( stderr, "Retrieving (%s,%d)\n", key, result->stack_offset );
	#endif
}
