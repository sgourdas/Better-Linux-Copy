#include "Types.h"

int verboseFlag = 0; 
int deleteFlag = 0; 
int linksFlag = 0;

int main(int argc, char **argv) {

	char *origindir = NULL, *destdir = NULL;

	configure(argc, argv, &origindir, &destdir);

	if(origindir == NULL || destdir == NULL) {

		fprintf(stderr, "Correct usage: ./quic [-v][-d][-l] origindir destdir\n");

		return -1;

	}

	struct stat st;

	if(lstat(origindir, &st) == - 1) {

		fprintf(stderr, "Source provided does not exist.\n");

		return -1;

	} else if(lstat(origindir, &st) == 0 && !S_ISDIR(st.st_mode)) {

		fprintf(stderr, "Source provided is not a directory.\n");

		return -1;

	}

	int madeDir = 0;

	if(lstat(destdir, &st) == -1) {

		if(errno == ENOENT) {

		    mkdir(destdir, S_IRWXU);

		    struct stat st;
			lstat(origindir, &st);
			chmod(destdir, st.st_mode);
			madeDir = st.st_size;

		    if(verboseFlag)
		   		printf("Created directory %s\n", destdir);

		} else {

			return -1;

		}

	
	}

	clock_t t = clock();
	// Parse src dir
	iList list = NULL;
	statistics stats = parseDir(origindir, destdir, time(NULL), &list);

	if(madeDir != 0) {

		stats.entityCount++;
		stats.copiedCount++;
		stats.bytesCopied += madeDir;

	}

	if(deleteFlag) {
		
		int removedEntities = cleanup(origindir, destdir);

		printf("\n");

		printf("number of entities removed is %d\n", removedEntities);
	
	} else {

		printf("\n");

	}

	printf("there are %d files/directories in the hierarchy.\n", stats.entityCount);
	printf("number of enities copied is %d\n", stats.copiedCount);
	double time_taken = ((double) (clock() - t) / CLOCKS_PER_SEC);
	printf("copied %d bytes in %fsec at %f bytes/sec\n", stats.bytesCopied, time_taken, stats.bytesCopied / time_taken);

	if(list != NULL)
		iListDestroy(list);
    
    free(origindir);
	free(destdir);

	return 0;

}