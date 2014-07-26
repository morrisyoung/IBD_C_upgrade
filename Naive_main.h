// Naive_main.h
// function: providing data types for the naive algorithm implementation

#ifndef NAIVE_MAIN_H
#define NAIVE_MAIN_H


using namespace std;


// the variable space used for chunks
typedef struct chunk_package
{
	int seq;
	long int start;		// the start coordinate of the chromosome for this chunk
	long int start_exp;	// the expected start coordinate for this chunk
	long int end_exp;	// the expected end coordinate for this chunk
	long int * table_coordinate;	// the record table for the coordinates (the first changed point and the last changed point)
}chunk_package;

#endif

// end of Naive_main.h