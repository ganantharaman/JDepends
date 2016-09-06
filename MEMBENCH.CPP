#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include "alloc.h"
#include <time.h>
#include <jvmmem.h>

#define CPS ((int)CLOCKS_PER_SEC)

long NIterations;
int NBlocks;        /* total number of memory blocks to allocate    */
int MinBlockSize;   /* minimum block size to allocate               */
int MaxBlockSize;   /* maximum block size to allocate               */

int     DummyCount; // so no one can optimize my dummies away

void    BenchMark(void)
    {
    clock_t     StartTime, StopTime, LoopTime, TotalTime;
    long    i;
    void**  Pointers;
    int*    Sizes;
    /* first, create array of pointers */
    Pointers    = (void**)calloc(NBlocks, sizeof(void*));
    Sizes       = (int*)calloc(NBlocks, sizeof(int));

    /* second, initialize sizes to random values */
    for(i=0; i < NBlocks; ++i)
        {
        int Size    = rand() % ((MaxBlockSize-MinBlockSize) + 1);
        Size       += MinBlockSize;
        Sizes[i]    = Size;
        }

    /* third, measure basic loop time */
    StartTime   = clock();
    for(i = 0; i < NIterations; ++i)
        {
        int iBlock  = i % NBlocks;
        if(Sizes[iBlock]&1) // randomly call dummy free or malloc
            {
            DummyFree(Pointers[iBlock]);
            Pointers[iBlock]    = 0;
            }
        else
            Pointers[iBlock]    = DummyMalloc(Sizes[iBlock]);
        }

    StopTime    = clock();
    LoopTime    = StopTime - StartTime;

    /* fourth, allocate a random set of blocks */
    for(i=0; i < NBlocks; ++i)
        if(rand() & 1)
            Pointers[i]    = MALLOC(Sizes[i]);


    /* finally, perform benchmark */
    StartTime   = clock();
    for(i = 0; i < NIterations; ++i)
        {
        int iBlock  = i % NBlocks;
        if(Pointers[iBlock])
            {
            FREE(Pointers[iBlock]);
            Pointers[iBlock]    = 0;
            }
        else
            Pointers[iBlock]    = MALLOC(Sizes[iBlock]);
        }
    StopTime    = clock();
    TotalTime   = (StopTime - StartTime) - LoopTime;

    printf("NIterations=%ld, NBlocks=%d, MinBlockSize=%d, MaxBlockSize=%d\n",
        NIterations, NBlocks, MinBlockSize, MaxBlockSize);
    {
    int Secs = TotalTime / CPS;
    int Decs = (TotalTime-(Secs*CPS)) / (CPS/10);

    printf( "Time = %d.%d\n", Secs, Decs);
    printf( "memory time=%ld, loop time=%ld\n", TotalTime, LoopTime);
    }
    }

void    main(int argc, char**argv)
    {
    ALLOCINIT();

#ifdef __WATCOMC__
    _amblksiz   = 64*1024;
#endif

    // assume at least hundredths of a second resolution
    assert(CPS >= 100);

    MinBlockSize    = 4;
    NIterations     = 1000L*1000L*5;
    if(argc == 3)
        {
        NBlocks         = atoi(argv[1]);
        MaxBlockSize    = atoi(argv[2]);
        }
    if(NBlocks < 1 || MaxBlockSize < MinBlockSize)
        fprintf(stderr, "Usage: membench <#of blocks> <max block size>\n");
    else
        {
        printf("Benchmarking '%s'\n", MALLOC_NAME);
        BenchMark();
        }
    exit(DummyCount?EXIT_SUCCESS:EXIT_FAILURE);
    }

