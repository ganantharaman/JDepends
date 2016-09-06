#include <assert.h>
#include <stdio.h>
#include "alloc.h"
#include <stdlib.h>
#include <jvmmem.h>

void    DumpStats(void)
    {
    DWORD Reserved, Committed;

    Reserved    = MemStats(&Committed);
    printf("Reserved=0x%08X, Committed=0x%08x\n", Reserved, Committed);
    }

double  CalcIntFrag(int Interval)
    {
    DWORD   High, Low, Lowest, BytesAllocated, Used, Wasted;
    DWORD   Granularity, NAllocs;
    DWORD   Reserved0, Committed0, Reserved, LastCommitted, Committed;
    DWORD   Guess;

    /* High == highest amount of memory available in this run   */
    High    = MemAvail();
    /* reserve most virtual memory to speed later calculations  */
    while(High > 0x00FFFFFF)
        {
        if(!GetMem(High/2))
            break;
        High    = MemAvail();
        }
    printf("Now all but 0x%08x of memory is reserved\n",
        High);

//    MALLOC(3*1024*1024);    /* for those 4Mb-at-a-time mallocs! */

    /* use up most every byte in currently-allocated pages           */
    do  {
        MALLOC(1);
        }
        while(High == MemAvail());

    Reserved0   = MemStats(&Committed0);
    Granularity = High - MemAvail();
    DumpStats();
    printf("%s reserves chunks of 0x%08X\n", MALLOC_NAME, Granularity);

    /* keep track of # of bytes we've allocated                 */
    BytesAllocated  = 1;

    /* alloc Interval bytes at a time, until another VirtualAlloc() */
    Low             = MemAvail();
    Lowest          = Low;
    if(Interval < 1000)
        Guess           = Granularity /
            (32 + ((Interval+15)/16) *16);
    else
        Guess           = Granularity / (Interval *4);
    Guess   -= 256/Interval;
    for(NAllocs=0; NAllocs < Guess; ++NAllocs)
        MALLOC(Interval);
    BytesAllocated  += Guess * Interval;

    printf("Approaching slow part\n");
    /* make sure we didn't overshoot */
    assert(MemAvail() == Low);

    for(; ; ++NAllocs)
        {
        MALLOC(Interval);
        Low         = MemAvail();

        if((High - Low) > Granularity)
            /* don't count the MALLOC() that pushed us over */
            break;
        else
            Lowest          = Low;
        BytesAllocated  += Interval;
        }
    Reserved    = MemStats(&Committed);
    printf("Reserved increased 0x%08X, Committed increased 0x%08X\n",
        Reserved-Reserved0, Committed-Committed0);
    
    Used    = High - Lowest;
    Wasted  = Used - BytesAllocated;

DumpStats();
printf("Used=%d, Wasted=%d, BytesAllocated=%d\n", Used, Wasted, BytesAllocated);
printf("Bytes wasted per allocation = %f\n", (float)Wasted/(float)NAllocs);


#if 0
    while(NAllocs-- > 5)
        MALLOC(Interval);
    High    = MemAvail();
    do  {
        MemStats(&LastCommitted);
        assert(MALLOC(Interval));
        }
        while(High == MemAvail());
    MemStats(&Committed);
    printf("Reserving new block of 0x%08X causes commit of 0x%08X\n",
        High-MemAvail(), Committed-LastCommitted);
#endif

    return ((double)Wasted*100.0)/(double)Used;
    }

void    main(int argc, char** argv)
    {
    int     Size;
    double  Frag;

#ifdef __WATCOMC__
    _amblksiz = 64*1024;
#endif

    ALLOCINIT();
    if(argc < 2 || (Size=atoi(argv[1]))<=0)
        printf("Usage: intfrag AllocSize\n");
    else
        {
        printf("Compiler '%s', using allocation size of %d\n",
            COMPILER_NAME, Size);
        Frag    = CalcIntFrag(Size);
        printf("'%s' internal fragmentation = %f%%\n", MALLOC_NAME, Frag);
        }
    }
