#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <limits.h> //realpath

// int link(const char *oldpath, const char *newpath);
// realpath()


#define PERMS 0644
#define BUFFSIZE 1024

extern int verboseFlag; 
extern int deleteFlag; 
extern int linksFlag;

typedef struct dlst {

	char dir[PATH_MAX];
	struct dlst *link;

} dirListNode;

typedef dirListNode *dirList;

typedef struct lst {

	unsigned long int inode;
	dirList subList;
	struct lst *link;

} ListNode;

typedef ListNode *iList;

typedef struct {

	int entityCount;
	int copiedCount;
	int bytesCopied;

} statistics;
// util.c
void configure(int, char **, char **, char **);
int copy(char *src, char *dest);
statistics parseDir(char *, char *, clock_t, iList *);
int cleanup(char *, char *);
int manageHardLinks(char *, char *, iList *);
// dirList.c
dirList dirListCreate(char *);
void dirListInsert(dirList, char *);
void dirListPrint(dirList);
void dirListDestroy(dirList);
// iList.c
iList iListCreate(long unsigned int, char *);
char *iListInsert(iList list, long unsigned int, char *);
void iListPrint(iList list);
void iListDestroy(iList list);