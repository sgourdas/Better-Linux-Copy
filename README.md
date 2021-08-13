# Better-Linux-Copy
An improvement of the Linux "cp" command

## Information

Goal of this project is to create a program that copies hierarchies of directories/files fast. The program is called quick incremental copy (quic for short). To do this we have to 
take advantage of the fact that usually when we copy a hierarchy a big part of it might already exist unmodified. The program identifies and manages different types of entities to
make the exact same copy of a specified directory to a destination path. Each entity is copied over using read write syscalls. Also, a "map", that maps all links, is created for their correct recreation.

## Using the database

### Compilation

Enter the ``` make ``` command while in the */Better-Linux-Copy/* directory.

### Execution 

Type ``` ./quic ``` while in the project's directory

### Command line arguments
You can give the following arguments in the command line:

``` origindir ``` : The source directory.

``` destdir ``` : The destination directory.

``` -v ``` : Verbose flag that provides logging of the program actions.

``` -d ``` : Delete flag that deletes entities that exist in the destination but not in the source directory.

``` -l ``` : Links flag that determines whether links will be handled.

## About
- This is a fifth semester assignment, by professor [A. Delis](https://www.alexdelis.eu/), for the Department of Informatics and Telecommunications, UoA. It was a part of the Operating Systems course and was given as the fourth and final assignment.
