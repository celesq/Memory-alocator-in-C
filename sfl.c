//Celescu Rares

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

#define MAX_STRING_SIZE 650

typedef struct date_t {
	void *data, *start, *end;
	int size, write;
} date_t;

typedef struct dll_node_t {
	struct dll_node_t *next, *prev;
	struct date_t data;

} dll_node_t;

typedef struct doubly_linked_list_t {
	dll_node_t *head;
	int size, block_size, nr;
	size_t data_size;

} doubly_linked_list_t;

typedef struct dump_mem {
	int liststart, bytes, malloc, fragmentations, free, lists, exit;
} dump_mem;

doubly_linked_list_t **INIT_HEAP(int *address, int nr, int bytes, int type);

void MALLOC(doubly_linked_list_t ***list, doubly_linked_list_t **mylist,
			int bytes, dump_mem *mem);

void FREE(doubly_linked_list_t ***list, doubly_linked_list_t *mylist,
		  int address, dump_mem *mem);

void READ(doubly_linked_list_t **list, doubly_linked_list_t *mylist,
		  int address, int bytes, dump_mem *mem);

void WRITE(doubly_linked_list_t **list, doubly_linked_list_t *mylist,
		   int address, char *data, int bytes, dump_mem *mem);

void DUMP_MEMORY(doubly_linked_list_t **list, doubly_linked_list_t *mylist,
				 dump_mem *mem);

void DESTROY_HEAP(doubly_linked_list_t ***list, doubly_linked_list_t **mylist);

dll_node_t *removenode(doubly_linked_list_t *list);

void print(doubly_linked_list_t **list, doubly_linked_list_t *mylist);

void addtolist(doubly_linked_list_t *mylist, dll_node_t *malloc_node, int type);

int main(void)
{
	doubly_linked_list_t **list, *mylist;
	dump_mem mem;
	mem.bytes = 0, mem.fragmentations = 0, mem.free = 0, mem.lists = 0;
	mem.liststart = 0, mem.malloc = 0, mem.exit = 0;
	mylist = malloc(sizeof(doubly_linked_list_t));
	DIE(!mylist, "Malloc failed\n");
	mylist->head = NULL, mylist->size = 0, mylist->nr = 0;
	mylist->data_size = sizeof(int);
	while (1) {
		if (mem.exit == 1) {
			DESTROY_HEAP(&list, &mylist);
			return 0;
		}
		char command[MAX_STRING_SIZE];
		fgets(command, sizeof(command), stdin);
		fprintf(stderr, "%s\n", command);
		char *ptr = strchr(command, '\n');
		if (ptr)
			*ptr = '\0';
		char *p = strtok(command, " ");
		if (strcmp(p, "INIT_HEAP") == 0) {
			int address, nr, bytes, type;
			p = strtok(NULL, " ");
			if (sscanf(p, "%x", &address) != 1)
				return 0;
			p = strtok(NULL, " ");
			if (sscanf(p, "%u", &nr) != 1)
				return 0;
			p = strtok(NULL, " ");
			if (sscanf(p, "%u", &bytes) != 1)
				return 0;
			p = strtok(NULL, " ");
			if (sscanf(p, "%u", &type) != 1)
				return 0;
			mem.liststart = nr, mem.bytes = bytes;
			list = INIT_HEAP(&address, nr, bytes, type);
		} else if (strcmp(p, "MALLOC") == 0) {
			p = strtok(NULL, " ");
			int bytes;
			if (sscanf(p, "%u", &bytes) != 1)
				return 0;
			MALLOC(&list, &mylist, bytes, &mem);
		} else if (strcmp(p, "FREE") == 0) {
			p = strtok(NULL, " ");
			int address;
			if (sscanf(p, "%x", &address) != 1)
				return 0;
			FREE(&list, mylist, address, &mem);
		} else if (strcmp(p, "READ") == 0) {
			p = strtok(NULL, " ");
			int address, bytes;
			if (sscanf(p, "%x", &address) != 1)
				return 0;
			p = strtok(NULL, " ");
			if (sscanf(p, "%u", &bytes) != 1)
				return 0;
			READ(list, mylist, address, bytes, &mem);
		} else if (strcmp(p, "WRITE") == 0) {
			p = strtok(NULL, " ");
			char *data = malloc(MAX_STRING_SIZE);
			int address, bytes;
			if (sscanf(p, "%x", &address) != 1)
				return 0;
			p = strtok(NULL, "\"");
			strcpy(data, p);
			p = strtok(NULL, " ");
			if (sscanf(p, "%d", &bytes) != 1)
				return 0;
			WRITE(list, mylist, address, data, bytes, &mem);
			free(data);
		} else if (strcmp(p, "DUMP_MEMORY") == 0) {
			DUMP_MEMORY(list, mylist, &mem);
		} else if (strcmp(p, "DESTROY_HEAP") == 0) {
			DESTROY_HEAP(&list, &mylist);
			break;
		}
	}
	return 0;
}

doubly_linked_list_t **INIT_HEAP(int *address, int nr, int bytes, int type)
{
	doubly_linked_list_t **list;
	int power = 8;
	list = calloc(nr, sizeof(doubly_linked_list_t *));
	DIE(!list, "Malloc failed\n");
	for (int i = 0; i < nr; i++) {
		int nrblocks = bytes / power;
		list[i] = calloc(1, sizeof(doubly_linked_list_t));
		DIE(!list[i], "Malloc failed\n");
		list[i]->nr = nr;
		list[i]->head = NULL;
		list[i]->size = 0;
		list[i]->data_size = sizeof(int);
		list[i]->block_size = power;
		for (int j = 0; j < nrblocks; j++) {
			dll_node_t *new_node = calloc(1, sizeof(dll_node_t));
			new_node->data.start = calloc(1, list[i]->data_size);
			new_node->data.end = calloc(1, list[i]->data_size);
			new_node->data.size = power;
			*(int *)new_node->data.start = *address;
			*address = *address + power;
			*(int *)new_node->data.end = *address;
			new_node->data.size = power;
			addtolist(list[i], new_node, 0);
		}
		power = power * 2;
	}
	return list;
}

void MALLOC(doubly_linked_list_t ***list, doubly_linked_list_t **mylist,
			int bytes, dump_mem *mem)
{
	int done = 0;
	mem->malloc++;
	for (int i = 0; i < (**list)->nr; i++) {
		if (bytes <= (*list)[i]->block_size && (*list)[i]->size > 0) {
			if (done == 1)
				return;
			done = 1;
			dll_node_t *malloc_node;
			malloc_node = removenode((*list)[i]);
			malloc_node->data.size = bytes;
			addtolist(*mylist, malloc_node, 1);
			if (bytes < (*list)[i]->block_size && (*list)[i]->head) {
				int f = 0;
				mem->fragmentations++;
				dll_node_t *list_node = calloc(1, sizeof(dll_node_t));
				list_node->data.start = calloc(1, (*list)[i]->data_size);
				list_node->data.end = calloc(1, (*list)[i]->data_size);
				list_node->data.size = (*list)[i]->block_size - bytes;
				int end = *(int *)malloc_node->data.start + bytes;
				*(int *)malloc_node->data.end = end;
				*(int *)list_node->data.start = *(int *)malloc_node->data.end;
				*(int *)list_node->data.end = *(int *)list_node->data.start;
				*(int *)list_node->data.end += (*list)[i]->block_size - bytes;
				int size = list_node->data.size;
				for (int j = 0; j < (**list)->nr; j++)
					if ((*list)[j]->block_size == size && f == 0) {
						f = 1;
						addtolist((*list)[j], list_node, 1);
					}
				if (f == 0) {
					int nr = (**list)->nr + 1;
					*list = realloc(*list, nr * sizeof(*list));
					(*list)[nr - 1] = calloc(1, sizeof(doubly_linked_list_t));
					(*list)[nr - 1]->data_size = (*list)[0]->data_size;
					(**list)->nr++;
					nr--;
					int nodesize = list_node->data.size;
					while (nr > 0 && (*list)[nr - 1]->block_size > nodesize) {
						(*list)[nr]->head = (*list)[nr - 1]->head;
						(*list)[nr]->block_size = (*list)[nr - 1]->block_size;
						(*list)[nr]->size = (*list)[nr - 1]->size;
						nr--;
					}
					(*list)[nr]->size = 0;
					(*list)[nr]->block_size = list_node->data.size;
					addtolist((*list)[nr], list_node, 0);
				}
			}
		}
	}
	if (done == 0) {
		printf("Out of memory\n");
		mem->malloc--;
	}
}

void FREE(doubly_linked_list_t ***list, doubly_linked_list_t *mylist,
		  int address, dump_mem *mem)
{
	int done = 0, ok = 0;
	mem->free++;
	dll_node_t *current_node = mylist->head, *previous_node;
	for (int i = 0; i < mylist->size; i++) {
		if (*(int *)current_node->data.start == address && done == 0) {
			done = 1;
			dll_node_t *free_node;
			free_node = current_node;
			if (free_node == mylist->head) {
				free_node = removenode(mylist);
			} else {
				current_node->next->prev = current_node->prev;
				current_node->prev->next = current_node->next;
				current_node->next = NULL;
				current_node->prev = NULL;
				mylist->size--;
			}
			for (int j = 0; j < (**list)->nr; j++)
				if ((*list)[j]->block_size == free_node->data.size) {
					addtolist((*list)[j], free_node, 1);
					ok = 1;
					break;
				}
			if (ok == 0) {
				int nrliste = (**list)->nr + 1;
				*list = realloc(*list, nrliste * sizeof(*list));
				(*list)[nrliste - 1] = calloc(1, sizeof(doubly_linked_list_t));
				(*list)[nrliste - 1]->data_size = (*list)[0]->data_size;
				(**list)->nr++;
				int j = nrliste - 1;
				int size = free_node->data.size;
				while (j > 0 && (*list)[j - 1]->block_size > size) {
					(*list)[j]->head = (*list)[j - 1]->head;
					(*list)[j]->block_size = (*list)[j - 1]->block_size;
					(*list)[j]->size = (*list)[j - 1]->size;
					j--;
				}
				(*list)[j]->size = 0;
				(*list)[j]->block_size = free_node->data.size;
				addtolist((*list)[j], free_node, 1);
			}
		}
		current_node = current_node->next;
	}
	if (done == 0) {
		printf("Invalid free\n");
		mem->free--;
	}
}

void READ(doubly_linked_list_t **list, doubly_linked_list_t *mylist,
		  int address, int bytes, dump_mem *mem)
{
	int nealoc = 0;
	for (int i = 0; i < (*list)->nr; i++) {
		dll_node_t *current_node = list[i]->head;
		for (int j = 0; j < list[i]->size; j++) {
			if (*(int *)current_node->data.start == address)
				nealoc = 1;
			current_node = current_node->next;
		}
	}
	dll_node_t *current_node = mylist->head;
	int start, end = -1;
	for (int i = 0; i < mylist->size; i++) {
		if (*(int *)current_node->data.start == address) {
			start = *(int *)current_node->data.start;
			end = *(int *)current_node->data.end;
		}
		if (end == *(int *)current_node->data.start)
			end = *(int *)current_node->data.end;
		current_node = current_node->next;
	}
	if (end < address + bytes)
		nealoc = 1;
	if (nealoc == 1) {
		printf("Segmentation fault (core dumped)\n");
		DUMP_MEMORY(list, mylist, mem);
		mem->exit = 1;
		return;
	}
	current_node = mylist->head;
	for (int i = 0; i < mylist->size; i++) {
		if (*(int *)current_node->data.start == start && bytes > 0) {
			int nrbytes = *(int *)current_node->data.end - start;
			char s[bytes];
			if (bytes > current_node->data.write) {
				int w = current_node->data.write;
				strncpy(s, (char *)current_node->data.data, w);
				for (int k = 0; k < current_node->data.write; k++)
					printf("%c", s[k]);
			} else {
				strncpy(s, (char *)current_node->data.data, bytes);
				for (int k = 0; k < bytes; k++)
					printf("%c", s[k]);
			}
			start = start + nrbytes;
			bytes = bytes - current_node->data.write;
		}
		current_node = current_node->next;
	}
	printf("\n");
}

void WRITE(doubly_linked_list_t **list, doubly_linked_list_t *mylist,
		   int address, char *data, int bytes, dump_mem *mem)
{
	if (bytes > (int)strlen(data))
		bytes = strlen(data);
	int nealoc = 0;
	for (int i = 0; i < (*list)->nr; i++) {
		dll_node_t *current_node = list[i]->head;
		for (int j = 0; j < list[i]->size; j++) {
			if (*(int *)current_node->data.start == address)
				nealoc = 1;
			current_node = current_node->next;
		}
	}
	dll_node_t *current_node = mylist->head;
	int start, end = -1;
	for (int i = 0; i < mylist->size; i++) {
		if (*(int *)current_node->data.start == address) {
			start = *(int *)current_node->data.start;
			end = *(int *)current_node->data.end;
		}
		int start = *(int *)current_node->data.start;
		if (end == *(int *)current_node->data.start)
			if (*(int *)current_node->prev->data.end == start)
				end = *(int *)current_node->data.end;
		current_node = current_node->next;
	}
	if (end < address + bytes)
		nealoc = 1;
	if (nealoc == 1) {
		printf("Segmentation fault (core dumped)\n");
		DUMP_MEMORY(list, mylist, mem);
		mem->exit = 1;
		return;
	}
	current_node = mylist->head;
	for (int i = 0; i < mylist->size; i++) {
		if (*(int *)current_node->data.start == start && bytes > 0) {
			int nrbytes = *(int *)current_node->data.end - start;
			if (!current_node->data.data) {
				if (nrbytes > strlen(data))
					nrbytes = strlen(data);
				current_node->data.data = (char *)calloc(1, MAX_STRING_SIZE);
				strncpy(current_node->data.data, data, nrbytes);
			} else {
				char string[MAX_STRING_SIZE];
				strcpy(string, current_node->data.data);
				free(current_node->data.data);
				current_node->data.data = (char *)calloc(1, MAX_STRING_SIZE);
				int oldlen = strlen(string), len;
				if (nrbytes > strlen(data))
					len = strlen(data);
				else
					len = bytes;
				strncat(current_node->data.data, data, len);
				if (oldlen > len) {
					char aux[MAX_STRING_SIZE];
					for (int k = 0; k < len; k++) {
						strcpy(aux, string + 1);
						strcpy(string, aux);
					}
					strncat(current_node->data.data, string, oldlen - len);
					nrbytes = oldlen;
				}
			}
			current_node->data.write = nrbytes;
			data = data + nrbytes;
			start = start + nrbytes;
			bytes = bytes - nrbytes;
		}
		current_node = current_node->next;
	}
}

void DUMP_MEMORY(doubly_linked_list_t **list, doubly_linked_list_t *mylist,
				 dump_mem *mem)
{
	dll_node_t *current_node;
	int total_allocated = 0, allocated_blocks = 0, free_blocks = 0;
	printf("+++++DUMP+++++\n");
	printf("Total memory: %u bytes\n", mem->liststart * mem->bytes);
	if (mylist->size > 0)
		current_node = mylist->head;
	for (int i = 0; i < mylist->size; i++) {
		total_allocated = total_allocated + current_node->data.size;
		allocated_blocks++;
		current_node = current_node->next;
	}
	printf("Total allocated memory: %u bytes\n", total_allocated);
	int total_mem = mem->liststart * mem->bytes - total_allocated;
	printf("Total free memory: %u bytes\n", total_mem);
	for (int i = 0; i < (*list)->nr; i++)
		free_blocks = free_blocks + list[i]->size;
	printf("Free blocks: %u\n", free_blocks);
	printf("Number of allocated blocks: %u\n", allocated_blocks);
	printf("Number of malloc calls: %u\n", mem->malloc);
	printf("Number of fragmentations: %u\n", mem->fragmentations);
	printf("Number of free calls: %u\n", mem->free);
	for (int i = 0; i < (*list)->nr; i++) {
		if (list[i]->size > 0) {
			int size = list[i]->size, sizeb = list[i]->block_size;
			printf("Blocks with %u bytes - %u free block(s) :", sizeb, size);
			current_node = list[i]->head;
			for (int j = 0; j < list[i]->size; j++) {
				printf(" 0x%x", *(int *)current_node->data.start);
				current_node = current_node->next;
			}
			printf("\n");
		}
	}
	current_node = mylist->head;
	printf("Allocated blocks :");
	for (int i = 0; i < mylist->size; i++) {
		int size = current_node->data.size;
		printf(" (0x%x - %u)", *(int *)current_node->data.start, size);
		current_node = current_node->next;
	}
	printf("\n");
	printf("-----DUMP-----\n");
}

void DESTROY_HEAP(doubly_linked_list_t ***list, doubly_linked_list_t **mylist)
{
	int nr = (**list)->nr;
	for (int i = 0; i < nr; i++) {
		if ((*list)[i]->size == 0) {
			free((*list)[i]);
		} else {
			dll_node_t *previous_node = NULL, *current_node = (*list)[i]->head;
			for (int j = 0; j < (*list)[i]->size - 1; j++) {
				if (previous_node) {
					free(previous_node->data.start);
					free(previous_node->data.end);
					if (previous_node->data.data)
						free(previous_node->data.data);
				}
				free(previous_node);
				previous_node = current_node;
				current_node = current_node->next;
			}
			free(current_node->data.start);
			free(current_node->data.end);
			if (current_node->data.data)
				free(current_node->data.data);
			free(current_node);
			if (previous_node) {
				free(previous_node->data.start);
				free(previous_node->data.end);
				if (previous_node->data.data)
					free(previous_node->data.data);
			}
			free(previous_node);
			free((*list)[i]);
		}
	}
	free(*list);
	*list = NULL;
	if ((*mylist)->size > 0) {
		dll_node_t *current_node = (*mylist)->head, *previous_node = NULL;
		for (int i = 0; i < (*mylist)->size - 1; i++) {
			if (previous_node) {
				if (previous_node->data.start)
					free(previous_node->data.start);
				free(previous_node->data.end);
				if (previous_node->data.data)
					free(previous_node->data.data);
				free(previous_node);
			}
			previous_node = current_node;
			current_node = current_node->next;
		}
		free(current_node->data.start);
		free(current_node->data.end);
		if (current_node->data.data)
			free(current_node->data.data);
		free(current_node);
		if (previous_node) {
			free(previous_node->data.start);
			free(previous_node->data.end);
			if (previous_node->data.data)
				free(previous_node->data.data);
			free(previous_node);
		}
	}
	free(*mylist);
	*mylist = NULL;
}

dll_node_t *removenode(doubly_linked_list_t *list)
{
	dll_node_t *current_node;
	current_node = list->head;
	if (list->size == 1) {
		list->size--;
		current_node->prev = NULL;
		current_node->next = NULL;
		return current_node;
	}
	list->head = list->head->next;
	current_node->next->prev = current_node->prev;
	current_node->prev->next = current_node->next;
	list->size--;
	current_node->next = NULL;
	current_node->prev = NULL;
	return current_node;
}

void print(doubly_linked_list_t **list, doubly_linked_list_t *mylist)
{
	for (int i = 0; i < (*list)->nr; i++) {
		dll_node_t *current_node;
		current_node = list[i]->head;
		for (int j = 0; j < list[i]->size; j++) {
			printf("0x%x ", *(int *)current_node->data.start);
			int block_size = list[i]->block_size;
			printf("0x%x %d\n", *(int *)current_node->data.end, block_size);
			current_node = current_node->next;
		}
		printf("\n");
	}
	dll_node_t *current_node;
	if (mylist->size != 0) {
		current_node = mylist->head;
		for (int i = 0; i < mylist->size; i++) {
			printf("0x%x ", *(int *)current_node->data.start);
			int size = current_node->data.size;
			printf("0x%x %d\n", *(int *)current_node->data.end, size);
			if (current_node->next)
				current_node = current_node->next;
		}
	}
}

void addtolist(doubly_linked_list_t *mylist, dll_node_t *malloc_node, int type)
{
	dll_node_t *current_node = mylist->head;
	current_node = mylist->head;
	if (mylist->size == 0) {
		mylist->head = malloc_node;
		malloc_node->next = malloc_node;
		malloc_node->prev = malloc_node;
		mylist->size++;
		return;
	}
	if (type == 0) {
		for (int j = 0; j < mylist->size - 1; j++)
			current_node = current_node->next;
		current_node->next = malloc_node;
		malloc_node->prev = current_node;
		malloc_node->next = mylist->head;
		mylist->head->prev = malloc_node;
		mylist->size++;
	} else {
		current_node = mylist->head;
		int poz = 1;
		while (poz < mylist->size) {
			int start = *(int *)current_node->next->data.start;
			int end = *(int *)current_node->data.end;
			if (*(int *)malloc_node->data.start >= end)
				if (*(int *)malloc_node->data.end <= start)
					break;
			poz++;
			current_node = current_node->next;
		}
		if (*(int *)malloc_node->data.end <= *(int *)mylist->head->data.start)
			poz = 0;
		dll_node_t *previous_node = NULL;
		current_node = mylist->head;
		for (int i = 0; i < poz; i++) {
			previous_node = current_node;
			current_node = current_node->next;
		}
		malloc_node->next = current_node;
		if (!previous_node && mylist->size > 0) {
			malloc_node->prev = mylist->head->prev;
			mylist->head->prev->next = malloc_node;
			malloc_node->next->prev = malloc_node;
			mylist->head = malloc_node;
		} else {
			current_node->prev = malloc_node;
			previous_node->next = malloc_node;
			malloc_node->prev = previous_node;
		}
		mylist->size++;
	}
}
