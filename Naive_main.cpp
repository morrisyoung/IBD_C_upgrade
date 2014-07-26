// Naive_main.cpp
// function: the Naive_xxx are all prepared for the previous n^2 naive algorithm
// project includes the following files:
// Naive_global.h
// Naive_main.cpp Naive_main.h
// Naive_chunk.cpp Naive_chunk.h
// Naive_parser.cpp Naive_parser.h
// Parser.cpp Parser.h
// List.cpp List.h
// Basic.cpp Basic.h

// Jul 24
// now this is a workable but better version of the n^2 naive algorithm's implementation
// we don't use c++11 libraries, so the compilation will have no problems even in the clusters
// we have not wrapped it up because we are only testing the algorithm now

#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "Naive_global.h"
#include "Naive_main.h"
#include "Naive_chunk.h"
#include "Naive_parser.h"
#include "Naive_basic.h"
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */


//========================== global variable definition ==========================
// the following are the default value of all the global variables
double PRECISION;
double NUMERIC;
double TOLERANCE = 0.01;
long int SAMPLE;
long int TREELEN;
int THREADS = 4;
long int CUTOFF = 1000000;
long int FILTER;
long int DISCRETIZATION = 0;
long int CLENGTH = 100000000;
int FORMAT = 0;
int INPUTSRC = 0;

// about multi-threading
pthread_t thread[10];
pthread_mutex_t mut_read;
pthread_mutex_t mut_write;
//vector<block_boundary_package> boundary_block;

// output filehandle
FILE * file_out;

// input filename
char filename[50];

/*
int BLOCK_NUM;
int BLOCK_SIZE;
int BLOCK_SIZE_LAST;
unordered_map<string, unordered_map<string, int>> MODE_BLOCK;
unordered_map<string, unordered_map<string, int>> MODE_BLOCK_LAST;
*/
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
    sprintf(filename, "%s", "test_1000G_50I_100Million.trees");
    // test.trees has 1000 samples in

    // the output filehandle
    char buffer[50];
    sprintf(buffer, "%s%s", "result_naive_", filename);
    file_out = fopen(buffer, "w");

    // fill other necessary variables
    FILE * file_in = fopen(filename, "r");
    TREELEN = 200000;
    char tree[TREELEN];
    fgets(tree, TREELEN, file_in);
    while(!tree_judge(tree))
        fgets(tree, TREELEN, file_in);
    fclose(file_in);

    PRECISION = 0.1;    // although it seems that we can still move back, but things may not be that optimistic
    NUMERIC = 0.00001;  // this is for hashing safe; this problem results from the float number precision issue
    FILTER = CUTOFF/2;
    if(FILTER == 0)
        FILTER = 1;
    SAMPLE = parser_sample(tree);
    TREELEN = (long int)(strlen(tree) * 1.2);  // I think coefficient 1.2 is enough for a tree
    // judge the format: (FORMAT)
    //
    //
    //

    //============================================================================================

    //=============================== program sumarization ==============================
    printf("The following are your running parameters:\n");
    printf("========================================================================\n");
    cout << "epsilon(tolerance for tMRCA comparison): " << TOLERANCE << endl;
    cout << "sample size(automatically detected from the trees): " << SAMPLE << endl;
    cout << "number of working threads: " << THREADS << endl;
    cout << "cutoff value for true IBD segment: " << CUTOFF << endl;
    cout << "filter value for IBD candidates(basically half of cutoff value): " << FILTER << endl;
    cout << "discretization value(sample trees every xxx bp): " << DISCRETIZATION << endl;
    cout << "chromosome length: " << CLENGTH << endl;
    printf("tree format: %s\n", FORMAT?"Newick":"Nexus");
    printf("input source: %s\n", INPUTSRC?"stdin":"tree file");
    if(!INPUTSRC)
    {
        printf("tree file name: %s\n", filename);
    }
    printf("result saved as: result_naive_%s\n", filename);
    printf("========================================================================\n");
    printf("Now the program begins...\n");


    //=========== normal work begins from here: sequential or multi-threaded =============
    //================== timing ===================
    struct timeval start;
    struct timeval end;
    double diff;
    gettimeofday(&start, NULL);


    if(THREADS == 1)
    {
        // initialize the only working threads (not a new thread from this one)

        //================== allocate the global space for chunks =================
        chunk_package package[THREADS];  // we put all the parameters a working thread will need into the package
        for(int i = 0; i < THREADS; i++)
        {
            package[i].seq = i+1;
            // later for the start;
            package[i].start_exp = (long int)(CLENGTH*i/THREADS);
            package[i].end_exp = (long int)(CLENGTH*(i+1)/THREADS);
            package[i].table_coordinate = (long int *)malloc(sizeof(long int) * SAMPLE * SAMPLE);
        }
        // set the package for thread #0, but don't initialize that
        package[0].start = 0;

        //=================== start the first (and the only here) chunk here ====================
        chunk_level((void *)&package[0]);

        // we should also merge in this case, because we have not yet provided the potential first
        // segment and the last segment for one specific pair
        chunk_merge(package, 0);

        cout << "The sequential work ends here!" << endl;

        //=================== free all the global space for chunks ====================
        for(int i = 0; i < THREADS; i++)
        {
            free(package[i].table_coordinate);
        }
    }
    else
    {
        //================== initialize the locker and ... =====================
        pthread_mutex_init(&mut_read, NULL);
        pthread_mutex_init(&mut_write, NULL);
        memset(&thread, 0, sizeof(thread));
        chunk_package package[THREADS];  // we put all the parameters a working thread will need into the package

        //================== allocate the global space =================
        for(int i = 0; i < THREADS; i++)
        {
            package[i].seq = i+1;
            // later for the start;
            package[i].start_exp = (long int)(CLENGTH*i/THREADS);
            package[i].end_exp = (long int)(CLENGTH*(i+1)/THREADS);
            package[i].table_coordinate = (long int *)malloc(sizeof(long int) * SAMPLE * SAMPLE);
        }

        // set the package for thread #0, but don't initialize that
        package[0].start = 0;

        //===================== new design: immediately open all the threads ====================
        // we don't perform further works here; we just initialize several working threads
        int ret;
        for(int i = 1; i < THREADS; i++)
        {
            if((ret = pthread_create(&thread[i], NULL, chunk_level, (void *)&package[i])) != 0)
                printf("separate working thread for chunk #%d is not successfully initialized!\n", i+1);
            else
                printf("separate working thread for chunk #%d is successfully initialized!\n", i+1);
        }

        //=================== start the first chunk here ====================
        printf("Now the program works for all the chunks simutaneously...\n");
        chunk_level((void *)&package[0]);

        //============= waiting for the threads to terminate, then combine ===========
        chunk_merge(package, 0);
        int i = 1;
        while(i < THREADS)
        {
            pthread_join(thread[i], NULL);
            chunk_merge(package, i);
            i++;
        }

        cout << "All the working threads are finished by now!! Program terminate..." << endl;

        //=================== free all the global space ====================
        for(int i = 0; i < THREADS; i++)
        {
            free(package[i].table_coordinate);
        }
    }

    //================== timing ===================
    gettimeofday(&end, NULL);
    diff = (double)(end.tv_sec-start.tv_sec)+ (double)(end.tv_usec-start.tv_usec)/1000000;
    printf("Time used totally is %f seconds.\n", diff);


    // release the output filehandle for security
    fclose(file_out);
    return 0;
}