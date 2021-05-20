#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*effect)(void*);

typedef struct list_node{
  struct list_node *next;
  void *data;
  char *key;
} list_node;

typedef struct list{
  struct list_node *head;
  struct list_node **tail;
  
} list;

/* list_init must be called before a list is used */
void list_init(list *subject)
{
  subject->head = 0;
  subject->tail = &(subject->head);
}



/* 
 * LIST IMPLEMENTATIONS
 */



void list_insert(list *collection,char *key, void *data)
{
  list_node *new_node = malloc(sizeof(list_node));
  new_node->next = 0;
  new_node->key = key;
  new_node->data = data;
  *(collection->tail) = new_node;
  collection->tail = &(new_node->next);
}


static
void list_apply_(list_node *cur, effect fun)
{
  if(0 == cur) return;
  fun(cur->data);
  list_apply_(cur->next, fun);
}


void list_apply(list *collection, effect fun)
{  list_apply_(collection->head, fun);  }


static
void list_destroy(list_node *cur)
{
  if(0 == cur) return;
  list_destroy(cur->next);
  free(cur);
}


void list_destruct(list *collection)
{
  list_destroy(collection->head);
  list_init(collection); /* setup so it can be reused */
}


static
void list_destroy_all(list_node *cur)
{
  if(0 == cur) return;
  list_destroy_all(cur->next);
  free(cur->data);
  free(cur);
}


void list_destruct_all(list *collection)
{
  list_destroy_all(collection->head);
  list_init(collection); /* setup so it can be reused */
}


void list_print_data(void *lst_node)
{ printf("%s", (char*)((list_node*)lst_node)->data); }

void print_voidstar(void *string)
{ if(0 != string)printf("%s\n", (const char *)string); }


void print_voidstar_int(void *i)
{ if(0 != i)printf("%d\n", *((int *)i)); }










typedef struct hash_tbl
{
	list **tbl;
	size_t size;
} hash_tbl;

void init_hash_tbl(hash_tbl *tbl, size_t size)
{
	tbl->tbl = malloc(sizeof(list*)*size);
	tbl->size = size;
	for(int i = 0; i < size; i++){
		tbl->tbl[i] = 0;
	}
}

int hash(char *str)
{
	size_t pos = 0;
	int obj = 0;
	while(str[pos] != '\0'){
		obj += str[pos];
		pos++;
	}
	obj /= strlen(str);
	return obj;
}

hash_tbl* realloc_hash_tbl(hash_tbl *tbl, size_t min)
{
	while(tbl->size >= min){
		tbl->size *= 2;	
	}
	realloc(tbl->tbl, sizeof(list*) * tbl->size);
	tbl->size = min;
	return tbl;
}


void insert_hash_tbl(hash_tbl *tbl, char *key, void *datum)
{
	int khash = hash(key);
	
	if(khash >= tbl->size){
		realloc_hash_tbl(tbl, khash);
	}
	
	if(tbl->tbl[khash] == 0){
		list *chain = (list*)malloc(sizeof(list));
		tbl->tbl[khash] = chain;
		list_init(chain);
	}
	
	list_insert(tbl->tbl[khash],key, datum);
}

void * search_hash_tbl(hash_tbl *tbl, char *key)
{
	int khash = hash(key);
	if(khash >= tbl->size) return 0;
	
	list *chain = tbl->tbl[khash];
	list_node *cur = chain->head; 
	for(;cur != 0; cur = cur->next){
		if(strcmp(cur->key, key) == 0){
			return cur->data;
		}
	}
	
	return 0;
}



int has_functions(char *line){
	int len = strlen(line);
	int count = 0;
	for(int i = 0; i < len; i++){
		switch(line[i]){
			case '!': ++count;
			break;
		}
	}
	return count;
}

int scan_for_functions(FILE *scriptFile)
{
	char line[4096];
	int functionCount = 0;
	while(!feof(scriptFile)){
		fgets(line, 4096, scriptFile);
		functionCount += has_functions(line);
		printf("%s", line);
	}
	return functionCount;
}


int main(int argc, char **argv)
{
	char *script = argv[1];
	FILE *scriptFile = fopen(script, "r");
	scan_for_functions(scriptFile);
	fclose(scriptFile);
	hash_tbl t;
	init_hash_tbl(&t, 100000);
	insert_hash_tbl(&t, "key", &t);
    void *got = search_hash_tbl(&t, "keyh");
	if(got == &t) puts("found");
	puts("didn't crash");
	return 0;
}
