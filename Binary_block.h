// Binary_block.h

#ifndef BINARY_BLOCK_H
#define BINARY_BLOCK_H

#include "Package.h"
#include <list>


using namespace std;


typedef struct candidate
{
	int type1;			// segment ends at this block/normal unchanged pair (alpha/delta)
	int type2;			// segment starts at this block/un-normal changed pair (beta/epsilon)
	//long int pair;	// no need, because we will use a hashtable other than vector, and the pair name
						// will be used as the hashing key
	double tMRCA;
}candidate;


void block_level(long int *, double *, block_package *, block_package *, block_package * , list<char *> *);


#endif

// end of Binary_block.h