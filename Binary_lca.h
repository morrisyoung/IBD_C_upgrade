// Binary_lca.h
// Bender&Colton's LCA algorithm; The LCA Problem Revisited (2000);
// start from parsing the Nexus/Newick tree

#ifndef BINARY_LCA_H
#define BINARY_LCA_H

#include <vector>
#include <string>
#include <unordered_map>


using namespace std;


typedef struct LCA_package
{
	long int coordinate;

	vector<int> E;  // used as the list of labels; not initialized
	vector<int> L;  // used as the list of levels; not initialized
	vector<double> r;  // used for the internal nodes; not initialized
	vector<int> R;  // used for the leaves; use the names of nodes to index; initialized

	vector<int> list_up_val;	// 5000: 714, so it won't go beyond 1000
	vector<int> list_up_pos;	// the same with above
	vector<string> list_up_type;
	unordered_map<string, int> upResult_val;
	unordered_map<string, int> upResult_pos;
}LCA_package;


void LCA_list_construction(char *);  //Euler tour for a tree to get the E[1,...,2n-1], L[1,...,2n-1], R[1,...,n]

void Dynamic();

// RMQ pre-processing from here
void LCA_RMQ();

// the entrance to the pre-processing
void LCA_preprocess(char *, LCA_package *);

// tMRCA query
double getMRCA(long int, long int, LCA_package *);

#endif

// end of Binary_lca.h