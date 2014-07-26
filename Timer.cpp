// Timer.cpp
// there are several timer functions which can be used by other processes

#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include "MRCA_hash.h"
#include <unordered_map>
#include "Global.h"
#include "Parser.h"
#include "RMQmode.h"
#include "LCA.h"
#include "Basic.h"
#include <string>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */


double PRECISION = 0;
double NUMERIC = 0;
double TOLERANCE = 0;
long int SAMPLE = 0;
long int TREELEN = 0;
long int CUTOFF;
pthread_mutex_t mut_write;  // mutex used by all the threads; used in saving the IBD segments into file


// the filehandle for the output file; global, to permit all the threads access it; using the mut above to protect
FILE * file_out;


using namespace std;


int main()
{
    // set all the global parameters when entering the program
    PRECISION = 0.1;    // although it seems that we can still move back, but things may not be that optimistic
    NUMERIC = 0.00001;  // this is for hashing safe; this problem results from the float number precision issue
    TOLERANCE = 0.01;
    SAMPLE = 2000;
    TREELEN = 200000;

    // for testing the n^2 algorithm and the nlogn algorithm, change the SAMPLE value and the tree file name

    char filename[] = "test_2000.trees";
    //char filename[] = "tree_5000.trees";
    //char filename[] = "twotrees.txt";
    FILE * file = fopen(filename, "r");

    char tree1[TREELEN];
    //char tree2[TREELEN];

    // get the first tree
    fgets(tree1, TREELEN, file);
    while(!tree_judge(tree1))
        fgets(tree1, TREELEN, file);


    // tree with 20 samples in
    //char tree1[] = "    tree NumGen_tree_1_1_pos_0 = [&U] ((((12.1:25.088, 3.1:25.088):425.908, ((20.1:108.422, (16.1:3.251, 2.1:3.251):105.170):26.295, 17.1:134.717):316.279):517.064, ((6.1:158.176, 8.1:158.176):301.232, 18.1:459.408):508.651):11.239, ((((5.1:8.879, 7.1:8.879):126.897, 14.1:135.776):156.553, 9.1:292.329):2.762, ((((1.1:65.607, 4.1:65.607):33.919, (10.1:6.591, 11.1:6.591):92.936):172.922, (13.1:14.976, 19.1:14.976):257.472):8.811, 15.1:281.259):13.833):684.207);";



    /*
    // timing model: (we should use the CPU clock ticks instead of using the actual running time)
    //===================================================================================
    struct timeval start;
    struct timeval end;
    double diff;
    gettimeofday(&start, NULL);

    // tested function

    gettimeofday(&end, NULL);
    diff = (double)(end.tv_sec-start.tv_sec)+ (double)(end.tv_usec-start.tv_usec)/1000000;
    printf("Time used is %f seconds.\n", diff);
    //===================================================================================
    */


    /*
    // timing using CPU clock ticks
    //===================================================================================
    clock_t t_start, t_end, t_diff;
    t_start = clock();

    // tested function

    t_end = clock();
    t_diff = t_start - t_end;
    printf("It took me %ld clicks (%f seconds).\n", t_diff, ((float)t_diff)/CLOCKS_PER_SEC);
    //===================================================================================
    */


    fclose(file);
    return 0;
}

// end of Timer.cpp