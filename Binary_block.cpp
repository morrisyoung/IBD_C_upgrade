// Binary_block.cpp
// function: this is the Block_level functions;
// algorithm:
// 1. find candidates;
// 2. binary search;


// special notes:
// we don't provide multi-threads for this implementation (all-verification algorithm -- block finding part; verification part)


// Jul 25
// we have two tables, one for storing all the ongoing IBD segments' start coordinate, and one for their tMRCA
// we store the first tMRCA for further comparison because we want to make things comparable
// (a tMRCA value within the tolerant range of the first tMRCA value will be regarded as the same with the first tMRCA value)


// Jul 26 0:04
// has not yet start coding this block
// this block is where the binary search actually exists


#include "Binary_block.h"
#include "Package.h"
#include "Binary_lca.h"
#include "Global.h"
#include "Parser.h"
#include "Basic.h"
#include <unordered_map>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <vector>


using namespace std;


// input:
//	table: table for the start of the IBD segment
// 	table_tMRCA: table for the tMRCA of the start of a IBD segment
//	list: previous/present/next block packages
//	pool: the pool (address) storing all the trees in present block and next block (doubly block)
// output:
//	saved IBD segment during the process
//	updated tables (A and B)
void block_level(long int * table, double * table_tMRCA, block_package * previous, block_package * present, block_package * next, vector<LCA_package> * lca_env_rep, vector<char *> pool_env)
{
	// algorithm:
	// 1. go through all the hashing tables, and get the candidates waiting to be verified
	// 2. for each of the candidates (alpha or beta), binary search its terminate point

	//========================= construct the candidate lists ============================
	// the following two are local, which means they will be dropped after this function
	unordered_map<long int, double> can_list1;	// alpha
	unordered_map<long int, double> can_list2;	// beta

	//==============
	// alpha filling
	//==============
	if(present->first != 1)	// otherwise there will be no alphas
	{
		for(auto itr = previous->hashtable.begin(); itr != previous->hashtable.end(); itr ++)
		{
			std::unordered_map<long int, double>::const_iterator got = present->hashtable.find((*itr).first);
  			if(got == present->hashtable.end())  // this is alpha
  			{
  				can_list1[(*itr).first] = table_tMRCA[(*itr).first];
  			}
		}
	}
	//=================
	// beta and filling
	//=================
	if(present->last != 1)
	{
		for(auto itr = next->hashtable.begin(); itr != next->hashtable.end(); itr ++)
		{
			std::unordered_map<long int, double>::const_iterator got = present->hashtable.find((*itr).first);
  			if(got == present->hashtable.end())  // this is beta
  			{
  				// the reference tMRCA should be further considered
  				can_list2[(*itr).first] = next->hashtable[(*itr).first];
  			}
		}
	}

	//================================== binary search ==================================
	double tMRCA, tMRCA1;
	long int coordinate;
	long int sample1, sample2, name;
	char * tree;
	LCA_package * lca_package;
	LCA_package * lca_package_target;
	int pointer;
	int direction;

	//========================== search alpha ============================
	for(auto itr = can_list1.begin(); itr != can_list1.end(); itr ++)
	{
		name = (*itr).first;
		//tMRCA = (*itr).second;
		direction = (int)((*lca_env_rep).size()/2);
		pointer = -1;
		while(1)
		{
			// get the new lca_package
			pointer += direction;
			lca_package = &(*lca_env_rep)[pointer];

			//============ get the sample pair =============
			name += 1;
			if(name % SAMPLE == 0)
			{
				sample1 = name / SAMPLE;
				sample2 = SAMPLE;
			}
			else
			{
				sample1 = name / SAMPLE + 1;
				sample2 = name % SAMPLE;
			}
			name -= 1;
			//==============================================
			//tMRCA = tMRCA_find(pool_env[pointer], sample1, sample2);
			tMRCA = getMRCA(sample1, sample2, lca_package);
			// DEBUG
			/*
			tMRCA1 = getMRCA(sample1, sample2, lca_package);
			tMRCA = tMRCA_find(pool_env[pointer], sample1, sample2);

			if(tMRCA1 != tMRCA)
				cout << tMRCA1 << " " << tMRCA << endl;
			*/

			//======== determine the direction of moving ========
			if(tMRCA - (*itr).second > TOLERANCE || tMRCA - (*itr).second < -TOLERANCE)
			{
				// backward search
				direction = -(int)(abs(direction)/2);
				lca_package_target = lca_package;
			}
			else
			{
				// forward search
				direction = (int)(abs(direction)/2);
				lca_package_target = &(*lca_env_rep)[pointer+1];
			}

			// judge
			if(direction == 0)	// if the interval of new package and old package is too small, terminate
				break;
		}

		IBDreport(sample1, sample2, table[name], lca_package_target->coordinate);
	}

	//========================== search beta ============================
	for(auto itr = can_list2.begin(); itr != can_list2.end(); itr ++)
	{
		name = (*itr).first;
		//tMRCA = (*itr).second;
		direction = (int)((*lca_env_rep).size()/2);
		pointer = -1;
		while(1)
		{
			// get the new lca_package
			pointer += direction;
			lca_package = &(*lca_env_rep)[pointer];

			//============ get the sample pair =============
			name += 1;
			if(name % SAMPLE == 0)
			{
				sample1 = name / SAMPLE;
				sample2 = SAMPLE;
			}
			else
			{
				sample1 = name / SAMPLE + 1;
				sample2 = name % SAMPLE;
			}
			name -= 1;
			//==============================================
			//tMRCA = tMRCA_find(pool_env[pointer], sample1, sample2);

			//printf("@@\n");


			//cout << pointer << " " << tMRCA << endl;
			//cout << lca_package->r.size() << endl;



			/*
			printf("@@\n");
			cout << lca_package->r.size() << endl;
			for(auto itr = lca_package->r.begin(); itr != lca_package->r.end(); itr ++)
				cout << (*itr) << " ";
			cout << endl;
			*/


			tMRCA = getMRCA(sample1, sample2, lca_package);

			//printf("@@@\n");


			// DEBUG
			/*
			tMRCA1 = getMRCA(sample1, sample2, lca_package);
			tMRCA = tMRCA_find(pool_env[pointer], sample1, sample2);

			if(tMRCA1 != tMRCA)
				cout << tMRCA1 << " " << tMRCA << endl;
			*/


			//======== determine the direction of moving ========
			if(tMRCA - (*itr).second > TOLERANCE || tMRCA - (*itr).second < -TOLERANCE)
			{
				// forward search
				direction = (int)(abs(direction)/2);
				lca_package_target = &(*lca_env_rep)[pointer+1];
			}
			else
			{
				// backward search
				direction = -(int)(abs(direction)/2);
				lca_package_target = lca_package;
			}

			// judge
			if(direction == 0)	// if the interval of new package and old package is too small, terminate
				break;
		}

		table[name] = lca_package_target->coordinate;
		table_tMRCA[name] = tMRCA;
	}
	//================================== binary search done ==================================


	//========= if this is the last block, terminate all deltas and betas ==========
	if(present->last == 1)
	{
		// for delta/epsilon (there are only possibly this two kinds of segments)
		for(auto itr = present->hashtable.begin(); itr != present->hashtable.end(); itr ++)
		{
			name = (*itr).first;	// this is the pair
			//(*itr2).second	// this is the candidate

			//============ get the sample pair =============
			name += 1;
			if(name % SAMPLE == 0)
			{
				sample1 = name / SAMPLE;
				sample2 = SAMPLE;
			}
			else
			{
				sample1 = name / SAMPLE + 1;
				sample2 = name % SAMPLE;
			}
			name -= 1;
			//==============================================
			// no matter this is a delta or epsilon, we shuold always perform termination for it
			IBDreport(sample1, sample2, table[name], CLENGTH);
		}
	}

	return;
}

// end of Binary_block.cpp