#include "Types.h"


iList iListCreate(long unsigned int inode, char *dir) {	// List create
	// Allocate space and assign values
	iList l = (iList) malloc(sizeof(ListNode));

	l->inode = inode;
	l->subList = dirListCreate(dir);
	l->link = NULL;

	return l;

}

char *iListInsert(iList list, long unsigned int inode, char *dir) {	// List insertion -- returns not NULL if the inode exists

	iList prev = NULL;

	while(list != NULL) { // Parse all nodes in search for inode's listnode

		if(list->inode == inode) {

			dirListInsert(list->subList, dir);
		
			return list->subList->dir;

		}

		prev = list;
		list = list->link;
	
	}
	// Allocate new space, assign values and link
	prev->link = iListCreate(inode, dir);

	return NULL;

}

void iListPrint(iList list) {	// List printing for debugging

	while(list != NULL) { // Parse all nodes

		printf("[%ld: ", list->inode);

		dirListPrint(list->subList);

		printf("] -> ");

		list = list->link;
	
	}

	printf("\n");

}

void iListDestroy(iList list) {	// Free space of a List 
	// Recursively free extra nodes
	if(list->link != NULL)
		iListDestroy(list->link);

	dirListDestroy(list->subList);
	// Free current node
	free(list);

}