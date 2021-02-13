#include "Types.h" 


void configure(int argc, char **argv, char **origindir, char **destdir) {		// Function for -i argument

	if(argc > 1) {

		/*
			argv choices:
				-v	verbose
				-d	deleted
				-l	links
		*/
		// Go through all arguments
		for (int i = 1 ; i < argc ; i++) {
			// Check if we have -c as argument
			if(!strcmp("-v", argv[i])) { /* if we have -c */

				verboseFlag = 1;


			} else if(!strcmp("-d", argv[i])) { /* if we have -c */

				deleteFlag = 1;


			} else if(!strcmp("-l", argv[i])) { /* if we have -c */

				linksFlag = 1;


			} else {

				if(*origindir == NULL) {

					*origindir = (char *) malloc(sizeof(strlen(argv[i]) + 1));
					strcpy(*origindir, argv[i]);

				} else {

					*destdir = (char *) malloc(sizeof(strlen(argv[i]) + 1));
					strcpy(*destdir, argv[i]);
				
				}

			}

		}

	}

}

statistics parseDir(char *srcPath, char *destPath, clock_t programStart, iList *list) {

	DIR *dir_ptr;
	struct dirent *direntp;
	statistics stats = {0, 0, 0};

	if((dir_ptr = opendir(srcPath)) == NULL) {

		fprintf(stderr, "Error while opening %s.", srcPath);

		exit(-1);

	}

	while((direntp = readdir(dir_ptr)) != NULL) {
		
		if(direntp->d_name[0] == '.')	// dont mind these
			continue;
		// up total entities counter
		stats.entityCount++;
		// build path
        char path[1024];	// path is source path
        strcpy(path, srcPath);
        strcat(path, "/");
        strcat(path, direntp->d_name);

        char path2[1024];	// path2 is destionation path
        strcpy(path2, destPath);
        strcat(path2, "/");
        strcat(path2, direntp->d_name);

        struct stat st, st2;
        lstat(path, &st);
        // Loop
		if(st.st_mtime > programStart)// if modification time is after program start means we have a cycle, so skip to prevent infinite looping
			continue;
		// Directories
		if(lstat(path, &st) == 0 && S_ISDIR(st.st_mode))	{	// src is dir

			if(lstat(path2, &st2) == 0) {	// directory already exists in dest
				
				if(!S_ISDIR(st2.st_mode)) {	// entity in destination folder is not dir
				
					remove(path2);	// remove the false type entity

					if(verboseFlag) {	// log removal if needed

						if(S_ISREG(st.st_mode))
							printf("Removed file %s.\n", path2);
						else if(S_ISLNK(st.st_mode))
							printf("Removed soft link %s.\n", path2);
						else
							printf("Removed soft link %s.\n", path2);

					}

				}

			}

			if(lstat(path2, &st2) == -1) {

				if(errno == ENOENT) { // dir in destination folder does not exist

					mkdir(path2, S_IRWXU);	// make the directory

					struct stat st;
					lstat(path, &st);
					chmod(path2, st.st_mode);

					stats.copiedCount++;	// up respective counter

		    		if(verboseFlag)	// log if needed
		    			printf("Created directory %s\n", path2);
		    	
		    	} else {

		    		exit(-1);	// error occured

		    	}

			}
				
			statistics temp = parseDir(path, path2, programStart, list);	// call recursively for next depth
			// Increment counters based on values returned
			stats.entityCount += temp.entityCount;
			stats.copiedCount += temp.copiedCount;
			stats.bytesCopied += temp.bytesCopied;
		
		} else if(lstat(path, &st) == 0 && S_ISREG(st.st_mode)) { // src is file / hard link
			// Hard Links
			if(linksFlag) {

				if(manageHardLinks(path, path2, list)) {	// if we ended up creating a hard link for this entity
					
					lstat(path2, &st2);
					stats.bytesCopied += st2.st_size; // add site of link to bytes copied counter
					stats.copiedCount++;	// up the entities copied counter

					continue;	// skip rest of loop

				}

			}

			if(lstat(path2, &st2) == 0) { // file in destination folder exists

				if(!S_ISREG(st2.st_mode)) { // file in destination folder is not file
				
					remove(path2);	// so we remove false type entity

					if(verboseFlag) {	// if needed log removal

						if(S_ISDIR(st2.st_mode))
							printf("Removed directory %s.\n", path2);
						else if(S_ISLNK(st2.st_mode))
							printf("Removed soft link %s.\n", path2);
						else
							printf("Removed entity %s.\n", path2);

					}

				} else if((st.st_size != st2.st_size) || (st.st_mtime > st2.st_mtime)) {	// size of src and dest is not the same so we overwrite
																							// OR dest has more recent modification time
					remove(path2);	// remove old dest file

					if(verboseFlag)	 // log if needed
						printf("Removed file %s.\n", path2);

					stats.bytesCopied += copy(path, path2);	// copy src to dest path

					stats.copiedCount++;	// up copied entities counter

	    			if(verboseFlag)	// log if needed
	    				printf("Created file %s\n", path2);

				}

			}

			if(lstat(path2, &st2) == -1) {

				if(errno == ENOENT) { // file in destination folder does not exist

					stats.bytesCopied += copy(path, path2);	// copy src to dest path

					stats.copiedCount++;	// up copied entities counter

		    		if(verboseFlag)	// log if needed
		    			printf("Created file %s\n", path2);

		    	} else {

		    		exit(-1);	// error

		    	}

			}

		} else if(linksFlag) {	// if we are managing links

			if(lstat(path, &st) == 0 && S_ISLNK(st.st_mode)) {	// src is soft link

				if(lstat(path2, &st2) == 0) { // entity in destination folder exists

					if(!S_ISLNK(st2.st_mode)) { // entity in destination folder is not link
					
						remove(path2);	// remove false entity

						if(verboseFlag)	{ // log if needed
							
							if(!S_ISREG(st2.st_mode))
								printf("Removed file %s.\n", path2);
							else if(!S_ISDIR(st2.st_mode))
								printf("Removed directory %s.\n", path2);
							else
								printf("Removed entity %s.\n", path2);

						}

					} else if((st.st_size != st2.st_size) || (st.st_mtime > st2.st_mtime)) { // size of src and dest is not the same so we overwrite
																							// OR dest has more recent modification time
						remove(path2);	// remove old file

						if(verboseFlag) // log if needed
							printf("Removed soft link %s.\n", path2);

						char buf[PATH_MAX]; 
					    ssize_t len = readlink(path, buf, sizeof(buf) - 1);	// get value of soft link
					    buf[len] = '\0';

					    if(len == -1)
					    	exit(-1);	// error

					    symlink(buf, path2);	// create soft link

					    lstat(path2, &st2);
					    stats.bytesCopied += st2.st_size; // add site of link to bytes copied counter
						stats.copiedCount++;	// up entity creation counter

			    		if(verboseFlag)	// log if needed
			    			printf("Created link %s\n", path2);


					}

				}

				if(lstat(path2, &st2) == -1) {

					if(errno == ENOENT) { // file in destination folder does not exist

						char buf[PATH_MAX]; 
					    ssize_t len = readlink(path, buf, sizeof(buf) - 1); // get value of soft link
					    buf[len] = '\0';

					    if(len == -1)
					    	exit(-1);	// bad :(

					    symlink(buf, path2);	// create soft link

					    lstat(path2, &st2);
					    stats.bytesCopied += st2.st_size; // add site of link to bytes copied counter
						stats.copiedCount++;	// up entity creation counter

			    		if(verboseFlag)	// log if needed
			    			printf("Created link %s\n", path2);

			    	} else {

			    		exit(-1);	// error

			    	}

				}

			}

		}

    }

    if(closedir(dir_ptr) == - 1) {	// close dir we parsed -- error handle

    	fprintf(stderr, "Error while closing directory %s\n", srcPath);

    	exit(-1);

    }

    return stats;	// return statistics

}

int copy(char *src, char *dest) {

	int n, from, to, bytesCopied = 0; 
	char buf[BUFFSIZE];
	mode_t fdmode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	if((from = open(src, O_RDONLY)) < 0) {	// open src location -- error handling

		fprintf(stderr, "Error while opening entity %s\n", src);

		return -1; 
	
	}

	if((to = open(dest, O_WRONLY | O_CREAT, fdmode )) < 0 ) {	// open dest location -- error handling
	
		fprintf(stderr, "Error while opening entity %s\n", dest);
		
		return -1;

	}

	while((n = read(from, buf, sizeof(buf))) > 0)	// copy until there is no more to copy
		bytesCopied += write(to, buf, n);


	struct stat st;
	lstat(src, &st);
	chmod(dest, st.st_mode);	// copy over permissions

	close(from); 
	close(to);

	return bytesCopied;

}

int manageHardLinks(char *path, char *path2, iList *list) {

	struct stat st, st2;

	if(lstat(path, &st) == 0 && S_ISREG(st.st_mode)) {	// if src entity is reg file

		char *inodeExists = NULL;

		if(*list == NULL) {	// if this is the first entry, create the list

			*list = iListCreate(st.st_ino, path2);	

		} else {

			inodeExists = iListInsert(*list, st.st_ino, path2);

		}

		if(lstat(path2, &st2) == 0) { // entity in destination folder exists

			if(!S_ISREG(st2.st_mode) || (inodeExists != NULL)) { // entity in destination folder is not reg -- or entity exists but is hard copy
			
				remove(path2);	// remove false entity

				if(verboseFlag)	{ // log if needed
					
					if(S_ISLNK(st2.st_mode))
						printf("Removed soft link %s.\n", path2);
					else if(S_ISDIR(st2.st_mode))
						printf("Removed directory %s.\n", path2);
					else if(S_ISREG(st2.st_mode))
						printf("Removed file %s.\n", path2);
					else
						printf("Removed entity %s.\n", path2);

				}

			} else if((st.st_size != st2.st_size) || (st.st_mtime > st2.st_mtime)) { // size of src and dest is not the same so we overwrite
																					// OR dest has more recent modification time
				if(inodeExists != NULL) {	// if i node exists already in map this means, our file can be made a hard link

					if(lstat(path2, &st2) == 0 && S_ISREG(st2.st_mode))	{ // if deep copy exists from before -- remove before makign hard link

						remove(path2);

						if(verboseFlag)
							printf("Removed file %s.\n", path2);

					}

					link(inodeExists, path2);

					if(verboseFlag)
						printf("Created hard link %s.\n", path2);

					return 1;	// Skip rest

				}

			}

		}

		if(lstat(path2, &st2) == -1) {

			if(errno == ENOENT) { // file in destination folder does not exist

				if(inodeExists != NULL) {	// if i node exists already in map this means, our file can be made a hard link

					if(lstat(path2, &st2) == 0 && S_ISREG(st2.st_mode))	{ // if deep copy exists from before -- remove before makign hard link

						remove(path2);

						if(verboseFlag)
							printf("Removed file %s.\n", path2);

					}

					link(inodeExists, path2);

					if(verboseFlag)
						printf("Created hard link %s.\n", path2);

					return 1;	// Skip rest

				}

			} else {

				exit(-1); //error

			}

		}

	}

	return 0;

}

int cleanup(char *srcPath, char *destPath) {

	DIR *dir_ptr;
	struct dirent *direntp;
	int removals = 0;

	if((dir_ptr = opendir(destPath)) == NULL) {

		fprintf(stderr, "Error while opening %s.", srcPath);

		exit(-1);

	}

	while((direntp = readdir(dir_ptr)) != NULL) {

		// dont mind these
		if(direntp->d_name[0] == '.')
			continue;
		// build path
        char path[1024];
        strcpy(path, srcPath);
        strcat(path, "/");
        strcat(path, direntp->d_name);

        char path2[1024];
        strcpy(path2, destPath);
        strcat(path2, "/");
        strcat(path2, direntp->d_name);

        struct stat st, st2;

		if(lstat(path2, &st2) == 0 && S_ISDIR(st2.st_mode))	{	// if current entity is a dir

			if(lstat(path, &st) == 0) {	

				removals += cleanup(path, path2);


			} else if(lstat(path, &st) == -1) {	

				if(errno == ENOENT) {	// and it does not exist in the relative path of source 

					rmdir(path2);	// remove it

					removals++;
					
					if(verboseFlag)
						printf("Removed directory %s.\n", path2);

				} else {

					exit(-1);

				}

			}
				
		
		} else if(lstat(path2, &st2) == 0 && (S_ISREG(st2.st_mode) || S_ISLNK(st2.st_mode))) { // if current entity is a file

			if(lstat(path, &st) == -1) {

				if(errno == ENOENT) { // file in destination folder does not exist

					remove(path2);

					removals++;

					if(verboseFlag) {

						if(S_ISLNK(st2.st_mode))
							printf("Removed soft link %s.\n", path2);
						else if S_ISREG(st2.st_mode)
							printf("Removed file %s.\n", path2);
						else
							printf("Removed entity %s.\n", path2);
					}

		    	} else {

		    		exit(-1);

		    	}

			}

		}

    }

    if(closedir(dir_ptr) == - 1) {

    	fprintf(stderr, "Error while closing directory %s\n", srcPath);

    	exit(-1);

    }

    return removals;

}