// MRCA_hash_changed_test.cpp
// I want to design a nlogn changed-pair detection algorithm, but have not finished yet


#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include "MRCA_hash_changed.h"
#include <unordered_map>
#include "Global.h"
#include "Parser.h"
#include "basic.h"
#include <string>
#include "Prepare.h"


//========================== global variable definition ==========================
// the following are the default value of all the global variables
double PRECISION;
double NUMERIC;
double TOLERANCE = 0.01;
long int SAMPLE;
long int TREELEN;
int THREADS = 1;
long int CUTOFF = 0;
long int FILTER;
long int DISCRETIZATION = 0;
long int CLENGTH = 100000000;
int FORMAT = 0;
int INPUTSRC = 0;

// about multi-threading
pthread_t thread[10];
pthread_mutex_t mut_read;
pthread_mutex_t mut_write;
vector<block_boundary_package> boundary_block;

// output filehandle
FILE * file_out;

// input filename
char filename[50];

int BLOCK_NUM;
int BLOCK_SIZE;
int BLOCK_SIZE_LAST;
unordered_map<string, unordered_map<string, int>> MODE_BLOCK;
unordered_map<string, unordered_map<string, int>> MODE_BLOCK_LAST;
//================================================================================


using namespace std;


int main()
{
    //============================================================================================
    // set all the global parameters when entering the program
    // if there is a need, we changed the following variables: (according to the input parameters)
    // some of them are necessary to initialize
    // TOLERANCE; THREADS; CUTOFF; DISCRETIZATION; CLENGTH; FORMAT; INPUTSRC; filename[50]
    //
    //
    //
    //
    sprintf(filename, "%s", "test_1000.trees");

    // perform prepare() at last
    prepare();
    //============================================================================================


    //================ testing ================
    FILE * file = fopen(filename, "r");

    char tree1[TREELEN];
    char tree2[TREELEN];

    // get the first tree
    fgets(tree1, TREELEN, file);
    while(!tree_judge(tree1))
        fgets(tree1, TREELEN, file);

    // get the second tree
    fgets(tree2, TREELEN, file);
    while(!tree_judge(tree2))
        fgets(tree2, TREELEN, file);


    printf("start and end coordinate:\n");
    cout << get_coordinate(tree1) << " " << get_coordinate(tree2) << endl;
    cout << "now enter the operation loop..." << endl;


    //============================ testing the nlogn algorithm ================================
    struct timeval start;
    struct timeval end;
    double diff;
    double time1 = 0;
    double time2 = 0;

    // one testing loop
    //===============================================
    // processing with timing; 20 times average
    for(int i = 1; i <= 20; i++)
    {
        unordered_map<long int, double> result1;
        unordered_map<long int, double> result2;

        gettimeofday(&start, NULL);
        changed_pairs_hash(tree1, tree2, &result1);
        gettimeofday(&end, NULL);
        diff = (double)(end.tv_sec-start.tv_sec)*1000 + (double)(end.tv_usec-start.tv_usec)/1000;
        printf("Time used totally is %f milliseconds (n^2 algorithm).\n", diff);
        time1 += diff;
        printf("Time used totally (overall) is %f milliseconds (n^2 algorithm).\n", time1/i);

        // processing with timing
        gettimeofday(&start, NULL);
        IBD_hash(tree1, tree2, &result2);
        gettimeofday(&end, NULL);
        diff = (double)(end.tv_sec-start.tv_sec)*1000 + (double)(end.tv_usec-start.tv_usec)/1000;
        printf("Time used totally is %f milliseconds (nlogn algorithm).\n", diff);
        time2 += diff;
        printf("Time used totally (overall) is %f milliseconds (nlogn algorithm).\n", time2/i);


        printf("the size of the two results: (n^2 and nlogn)\n");
        cout << result1.size() << " " << result2.size() << endl;

        // test the correctness
        for(auto itr = result2.begin(); itr != result2.end(); itr ++)
        {
            std::unordered_map<long int, double>::const_iterator got = result1.find((*itr).first);
            if(got != result1.end())
            {
                result1.erase((*itr).first);
            }
        }

        cout << "the # of different elements: " << result1.size() << endl;

        /*
        // who they are:
        long int sample1, sample2, name;
        for(auto itr = result1.begin(); itr != result1.end(); itr ++)
        {
            cout << (*itr).first;
            name = (*itr).first + 1;
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
            printf("(%ld %ld) ", sample1, sample2);
        }
        cout << endl;
        */
    }


    /*
    // another testing loop
    //===============================================
    // get the second tree
    char tree3[TREELEN];
    fgets(tree3, TREELEN, file);
    while(get_coordinate(tree3) < 1000000)
        fgets(tree3, TREELEN, file);

    unordered_map<long int, double> result3;
    unordered_map<long int, double> result4;

    // processing with timing
    gettimeofday(&start, NULL);
    unchanged_pairs_hash(tree2, tree3, &result3);
    gettimeofday(&end, NULL);
    diff = (double)(end.tv_sec-start.tv_sec)+ (double)(end.tv_usec-start.tv_usec)/1000000;
    printf("Time used totally is %f seconds (n^2 algorithm).\n", diff);

    // processing with timing
    gettimeofday(&start, NULL);
    IBD_hash(tree2, tree3, &result4);
    gettimeofday(&end, NULL);
    diff = (double)(end.tv_sec-start.tv_sec)+ (double)(end.tv_usec-start.tv_usec)/1000000;
    printf("Time used totally is %f seconds (nlogn algorithm).\n", diff);

    printf("the size of the two results: (n^2 and nlogn)\n");
    cout << result3.size() << " " << result4.size() << endl;


    for(auto itr = result4.begin(); itr != result4.end(); itr ++)
    {
        std::unordered_map<long int, double>::const_iterator got = result3.find((*itr).first);
        if(got != result3.end())
        {
            result3.erase((*itr).first);
        }
    }

    cout << "the difference: " << result3.size() << endl;
    //===============================================
    */

    fclose(file);

    return 0;
}

// end of MRCA_hash_changed_test.cpp