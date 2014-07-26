// Binary_chunk.h

#ifndef BINARY_CHUNK_H
#define BINARY_CHUNK_H

#include "Package.h"
#include <stdio.h>
#include <list>


using namespace std;


long int get_tree(FILE *, list<char *> *);

char * block_finding(block_package *, list<char *> *, FILE *, char *, long int);

void * chunk_level(void *);

void chunk_merge(chunk_package *, int);

#endif

// end of Binary_chunk.h