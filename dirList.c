#include "Types.h"


dirList dirListCreate(char *dir) {	// List create
	// Allocate space and assign values
	dirList l = (dirList) malloc(sizeof(dirListNode));
	strcpy(l->dir, dir);
	l->link = NULL;

	return l;

}

void dirListInsert(dirList list, char *dir) {	// List insertion

	// Allocate new space and assign values
	dirList l = dirListCreate(dir);
	// Linkage -- put in 2nd pos
	l->link = list->link;
	list->link = l;

}

void dirListPrint(dirList list) {	// List printing for debugging

	while(list != NULL) { // Parse all nodes

		printf("%s -> ", list->dir);

		list = list->link;
	
	}

}

void dirListDestroy(dirList list) {	// Free space of a List 
	// Recursively free extra nodes
	if(list->link != NULL)
		dirListDestroy(list->link);
	// Free current node
	free(list);

}