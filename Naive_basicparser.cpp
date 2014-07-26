// Parser.cpp
// function: the stack-based parsing algorithm; using the self-defined List and related operations
// 1.
// input:	the string of a tree (an in-order traversal format of the tree)
// output:	the table in which we can find the all-pair tMRCAs
// 2.
// input:	two trees
// output:	whether or not one specific pair change their tMRCA

#include <stdio.h>
#include "List.h"
#include "Naive_basicparser.h"
#include "Naive_basic.h"
#include "Naive_global.h"
#include <string.h>
#include <unordered_map>


using namespace std;


// input: the tree
// output: updated all-pair tMRCAs table
long int parser(char * tree, double * table)
{
	// get the breakpoint of this tree (for Nexus, it is the start coordinate of this segment)
	long int coordinate;
	int i = 0;
	while(tree[i] != '\0')
	{
		if(tree[i] == 'p' && tree[i+1] == 'o' && tree[i+2] == 's' && tree[i+3] == '_')
		{
			char breakpoint[10];
			i = i+4;
			int j = 0;
			while(tree[i] != ' ')
			{
				breakpoint[j++] = tree[i++];
			}
			breakpoint[j] = '\0';
			coordinate = string_long(breakpoint);
			break;
		}
		i++;
	}

	// get the start of the tree from here
	while(tree[i] != '(')i++;

	List_list * l1 = createList_list();  // store the nodes and their existing list
	List_double * l2 = createList_double();  // store the MRCA of corresponding nodes

	int j = i;
	while(tree[j++] != '\0');
	tree[j-4] = '\0';  // for the "4": I guess there is a '\n' in the string befure '\0'

	// the following used only for IBD update in the while loop
	Node_list * node_list = NULL;
	Node_int * node_int = NULL;
	Node_double * node_double = NULL;
	double value;  // used by the tMRCA update

	while(tree[i] != '\0')
	{
		if(tree[i] == ',' || tree[i] == ' ' || tree[i] == ';')
		{
			i += 1;
			continue;
		}

		if(tree[i] == '(')
		{
			i += 1;
			appendList_list(l1);
			appendList_double(l2, 0);  // previous None in Python
			continue;
		}

		if(tree[i] >= 48 && tree[i] <= 57)
		{
			char node[5];
			int j = 0;
			while(tree[i] != '.')
			{
				node[j++] = tree[i++];
			}
			node[j] = '\0';
			long int node_value = string_long(node);
			while(tree[i++] != ':');
			char tmrca[20];  // I give this double type number 20 effective digits; it is just kidding of course
			j = 0;
			while(tree[i] != ',' && tree[i] != ')' && tree[i] != '\0')
			{
				tmrca[j++] = tree[i];
				i++;
			}
			tmrca[j] = '\0';
			double tmrca_value = string_double(tmrca);

			appendList_int(l1, node_value);  // we fix this kind of appending only to the last list in the list

			if((l2->end)->element == 0)  // two sub-nodes
			{
				l2->end->element = tmrca_value;
			}

			//=============== tMRCA update =================
			j = 0;
			node_list = l1->start;
			node_int = (node_list->start)->start;
			node_double = l2->start;

			for(j=1; j<=l1->length; j++)
			{
				do
				{
					if(node_int == NULL)  // there may be an empty list
					{
						break;
					}
					long int node1 = node_int->element;
					if(node1 != node_value)
					{
						long int name;
						if(node1 < node_value)
						{
							name = (node1 - 1) * SAMPLE + node_value - 1;  // here the name should be an long integer
						}
						else
						{
							name = (node_value - 1) * SAMPLE + node1 - 1;
						}


						// just simply record the all tMRCAs
						table[name] = node_double->element;


					}
					if(node_int->next == NULL){break;}
					else{node_int = node_int->next;}
				}
				while(1);
				if(j<l1->length)
				{
					node_list = node_list->next;
					node_int = (node_list->start)->start;
					node_double = node_double->next;
				}
			}
			continue;
		}

		if(tree[i] == ')')  // there must be a ":" following the ")"
		{

			// two possibilities: left leaf and right leaf
			extendList_list(l1);
			i += 2;
			char tmrca[20];  // I give this double type number 20 effective digits
			int j = 0;
			while(tree[i] != ',' && tree[i] != ')' && tree[i] != '\0')
			{
				tmrca[j++] = tree[i];
				i++;
			}
			tmrca[j] = '\0';

			value = string_double(tmrca);
			extendList_double(l2, value);
			continue;
		}
	}
	tree[i] = ')';  // retain the previous tree
	cleanList(l1, l2);

	return coordinate;
}


// input: tree
// output: the SAMPLE size of the tree
long int parser_sample(char * tree)
{
	// get the start of the tree from here
	int i = 0;
	while(tree[i] != '(')i++;
	long int sample = 0;

	while(tree[i] != ';')
	{
		if(tree[i] == ',' || tree[i] == ' ' || tree[i] == ';' || tree[i] == '(' || tree[i] == ')')
		{
			i += 1;
			continue;
		}

		if(tree[i] >= 48 && tree[i] <= 57)
		{
			sample++;
			while(tree[i] != ',' && tree[i] != ')')i++;
			continue;
		}

		if(tree[i] == ':')
		{
			while(tree[i] != ',' && tree[i] != ')')i++;
			continue;
		}
	}

	return sample;
}

// end of Naive_basicparser.cpp