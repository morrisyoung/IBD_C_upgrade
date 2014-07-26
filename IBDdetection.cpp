// IBDdetection.cpp
// function: divide all the trees into several working chunks, then initialize them into threads

// note:
// 1. the result file is named as "result_xxx" where xxx is your tree file
// 2. if the input src is stdin, then we can only use the sequential version

#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>
#include <pthread.h>
#include "MRCA_hash.h"
#include "Global.h"
#include "Parser.h"
#include "RMQmode.h"
#include "LCA.h"
#include "Basic.h"
#include "IBDdetection.h"
#include "Chunk_level.h"
#include <vector>


// define the global variables and give them the default values (default value)
double PRECISION = 0;       // used for tMRCA's partial hashing
double NUMERIC = 0;         // used for the tMRCA's hashing related numeric problem
double TOLERANCE = 0;       // the tMRCA change tolerance
long int SAMPLE = 0;        // the sample size
long int TREELEN = 0;       // the string lenth of a Nexus/Newick tree
int THREADS = 1;            // the number of threads we used
long int CUTOFF = 1000000;        // the IBD segment cutoff value
long int FILTER = CUTOFF/2;        // the filter value used for candidate extraction; half of the CUTOFF
long int DISCRETIZATION = 0;// process trees every "discretization" base pairs
long int CLENGTH = 100000000;// the length of the chromosome; we don't know it in Nexus
int FORMAT = 0;             // the format of the tree: Nexus(0) or Newick(1)
int INPUTSRC = 0;           // the source of the trees: tree file(0) or stdin(1)

// used for multi-threads
pthread_t thread[10];  // we support at most 10 threads, because even a working station will
                            // always have at most about 10 cores
pthread_mutex_t mut_read;  // mutex used by all the threads; this is actually used for speedup, because blocking makes things slow
pthread_mutex_t mut_write;  // mutex used by all the threads

FILE * file_out;

char filename[50];

vector<block_boundary_package> boundary_block;


using namespace std;


int main()
{
    // set all the global parameters when entering the program (if needed to change)
    // the following are default one
    PRECISION = 0.1;
    NUMERIC = 0.00001;
    TOLERANCE = 0.01;
    SAMPLE = 2000;
    TREELEN = 120000;  // averagely 1000 sample will have 20,000 characters in the tree string
    THREADS = 1;
    CUTOFF = 1000000;
    FILTER = CUTOFF/2;
    DISCRETIZATION = 0;
    CLENGTH = 100000000;
    FORMAT = 0;
    INPUTSRC = 0;

    // provide the initial filehandle before processing
    sprintf(filename, "%s", "test.trees");
    char tree[TREELEN];

    //==================== provide the result file filehandle ====================
    char buffer[50];
    sprintf(buffer, "%s%s", "result_", filename);
    FILE * file_out = fopen(buffer, "w");


    //============================= get the SAMPLE/TREELEN size ==============================
    // we temporarily use this filehandle to get the SAMPLE size
    FILE * filehandle = fopen(filename, "r");
    fgets(tree, TREELEN, filehandle);
    while(!tree_judge(tree))
    {
        fgets(tree, TREELEN, filehandle);
    }
    fclose(filehandle);
    SAMPLE = parser_sample(tree);
    TREELEN = (long int)(strlen(tree) * 1.2);  // I think coefficient 1.2 is enough for a tree


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
    printf("result saved as: result_%s\n", filename);
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
        // using filehandle0 to process all the trees; sequentially

        //================== allocate the global space =================
        chunk_package package[THREADS];  // we put all the parameters a working thread will need into the package
        for(int i = 0; i < THREADS; i++)
        {
            package[i].seq = i+1;
            // later for the start;
            package[i].start_exp = (long int)(CLENGTH*i/THREADS);
            package[i].end_exp = (long int)(CLENGTH*(i+1)/THREADS);
            package[i].table = (long int *)malloc(sizeof(long int) * SAMPLE * SAMPLE);
        }

        // set the package for thread #0, but don't initialize that
        package[0].start = 0;

        //=================== start the first (and the only) chunk here ====================
        chunk_level((void *)&package[0]);

        cout << "The sequential work ends here!" << endl;

        //=================== free all the global space ====================
        for(int i = 0; i < THREADS; i++)
        {
            free(package[i].table);
        }

        fclose(file_out);
    }
    else
    {
        // chunk_division algorithm: (actually this has been changed)
        // 1. divide;
        // 2. if not the first chunk, initialize a new thread;
        // 3. finally begin the works for the first thread;
        // 4. when the first chunk finishes, waiting and merging the results.

        //================== initialize the locker and ... =====================
        pthread_mutex_init(&mut_read, NULL);
        pthread_mutex_init(&mut_write, NULL);
        memset(&thread, 0, sizeof(thread));
        chunk_package package[THREADS];  // we put all the parameters a working thread will need into the package
        // this is used to fill the following boundary_block vector
        for(int i = 0; i < THREADS - 1; i++)
        {
            block_boundary_package p;
            p.table = (double *)malloc(sizeof(double) * SAMPLE * SAMPLE);
            boundary_block.push_back(p);
        }

        //block_boundary_package pointer_temp;
        //for(int i = 0; i < THREADS - 1; i++)boundary_block.push_back(pointer_temp);

        //================== allocate the global space =================
        for(int i = 0; i < THREADS; i++)
        {
            package[i].seq = i+1;
            // later for the start;
            package[i].start_exp = (long int)(CLENGTH*i/THREADS);
            package[i].end_exp = (long int)(CLENGTH*(i+1)/THREADS);
            package[i].table = (long int *)malloc(sizeof(long int) * SAMPLE * SAMPLE);
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
            free(package[i].table);
        }
        for(auto itr = boundary_block.begin(); itr != boundary_block.end(); itr ++)
        {
            free(itr->table);
            //free(*itr);  // the struct itself needs not be released
        }

        //=================== close all the filehandles ====================
        fclose(file_out);
    }

    //================== timing ===================
    gettimeofday(&end, NULL);
    diff = (double)(end.tv_sec-start.tv_sec)+ (double)(end.tv_usec-start.tv_usec)/1000000;
    printf("Time used totally is %f seconds.\n", diff);

    return 0;
}

// end of IBDdetection.cpp