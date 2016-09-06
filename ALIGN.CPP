/* align.c - figure out how this allocator aligns its memory
 */

#include <stdio.h>
#include <stddef.h> /* for offsetof */
#include "alloc.h"
#include <windows.h>
#include <jvmmem.h>

/* biggest alignment any sane allocator would enforce */
#define BIGGEST_ALIGN   (4096)

#define DEFINEALIGN(type) \
typedef struct  type##ALIGN \
    {                       \
    char    Foo1[1];        \
    type    Member;         \
    }   type##ALIGN;

DEFINEALIGN(int); DEFINEALIGN(long);
DEFINEALIGN(float); DEFINEALIGN(double);

struct  Tiniest {
    char    PeeWee[1];
    };

int     Alignment(void* P)
    {
    long    Target  = (long)P;

    int i;
    for(i = BIGGEST_ALIGN; i > 0; i/=2)
        {
        if(!(Target % i))
            return i;
        }
    return BIGGEST_ALIGN;
    }

int     HowIsAligned(int Size)
    {
    void*   Ptrs[1024*2];
    int     i, Align, NewAlign;

    Align   = BIGGEST_ALIGN;
    for(i=0; i < 1024; ++i)
        {
        Ptrs[i*2]   = MALLOC(Size);
        NewAlign    = Alignment(Ptrs[i*2]);
        Ptrs[i*2+1] = MALLOC(i);
        if(NewAlign < Align)
            Align   = NewAlign;
        }
    for(i=0; i < 1024; ++i)
        {
        FREE(Ptrs[i*2]);
        FREE(Ptrs[i*2+1]);
        }
    return Align;
    }

void main()
    {
    int IntAlign, LongAlign, FloatAlign, DoubleAlign;
    int Align   = BIGGEST_ALIGN;
    int OldSize = 1;
    int WorstAlign;

    int Size;

    ALLOCINIT();
    WorstAlign = Align   = HowIsAligned(OldSize);
    for(Size=2; Size <= 1024; ++Size)
        {
        int NewAlign    = HowIsAligned(Size);
        printf(".");    /* let me know it's alive... */
        if(NewAlign != Align || Size == 1024)
            {
            if(Size > OldSize+1)
                printf("\n%d through %d -byte segments aligned at %d\n",
                    OldSize, Size-1, Align);
            else
                printf("\n%d-byte segments aligned at %d\n",
                    OldSize, Align);
            OldSize = Size;
            Align   = NewAlign;
            if(NewAlign < WorstAlign)
                WorstAlign  = NewAlign;
            }
        }
    printf("'%s' Aligns on %d-byte boundaries\n",
        MALLOC_NAME, WorstAlign);
    IntAlign    = Alignment((void*)offsetof(intALIGN, Member));
    LongAlign   = Alignment((void*)offsetof(longALIGN, Member));
    FloatAlign  = Alignment((void*)offsetof(floatALIGN, Member));
    DoubleAlign = Alignment((void*)offsetof(doubleALIGN, Member));

    if(IntAlign > Align)
        printf("But ints are aligned at %d!\n", IntAlign);
    if(LongAlign > Align)
        printf("But longs are aligned at %d!\n", LongAlign);
    if(FloatAlign > Align)
        printf("But floats are aligned at %d!\n", FloatAlign);
    if(DoubleAlign > Align)
        printf("But doubles are aligned at %d!\n", DoubleAlign);
    if(sizeof(struct Tiniest) > Align)
        printf("But the minimum struct size is %d!\n", DoubleAlign);
    printf("This test compiled with '%s'\n", COMPILER_NAME);
    }
