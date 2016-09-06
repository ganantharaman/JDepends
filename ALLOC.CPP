#include <assert.h>
#include "alloc.h"
#include <jvmmem.h>

#if defined(__BORLANDC__)
#   pragma argsused
#endif
void    DummyFree(void* Foo)
    {
    }

#if defined(__BORLANDC__)
#   pragma argsused
#endif
void*   DummyMalloc(size_t Size)
    {
    return 0;
    }

HANDLE      MyHeap;
IMalloc*    OleAlloc;


#if ALLOCATOR==3 || ALLOCATOR==4
int     AllocInit(void)
    {
    MyHeap  = HeapCreate(HEAP_NO_SERIALIZE, 1024, 0);
    assert(MyHeap != NULL);
    CoGetMalloc(1, &OleAlloc);
    assert(OleAlloc != NULL);
    return TRUE;
    }
#endif


void*  GetMem(DWORD n)    {
    return VirtualAlloc(0, n, MEM_RESERVE,
        PAGE_READWRITE);
    }
static void    FreeMem(LPVOID P)
    {  assert(VirtualFree(P, 0, MEM_RELEASE) != FALSE);  }

/* figure out how much memory is available */
DWORD    MemAvail()
    {
    int     i       = 0;
    DWORD   Avail   = 0;
    DWORD   Size    = 4096*0x40000;
    LPVOID  Ptrs[4096];
    
    memset(Ptrs, 0, sizeof(Ptrs));

    while(Size >= 4096)
        {
        while((Ptrs[i] = GetMem(Size)) != 0)
            {
            Avail   += Size;
            ++i;
            assert(i < 4095);
            }
        Size    /= 2;
        }

    for(i = 0; Ptrs[i]; ++i)
        FreeMem(Ptrs[i]);

    return Avail;
    }

/* figure out how much memory is available */
DWORD    MemStats(DWORD* PCommitted)
    {
    MEMORY_BASIC_INFORMATION    Info;
    DWORD                       Address     = 0;
    DWORD                       MaxAddr     = 0xFFFFFFFF;
    DWORD                       Reserved    = 0;
    DWORD                       Committed   = 0;

    for(;;)
        {
        VirtualQuery((LPVOID)Address, &Info, sizeof(Info));
        if(Info.State != MEM_FREE)
            {
            Reserved   += Info.RegionSize;
            if(Info.State == MEM_COMMIT)
                Committed  += Info.RegionSize;
            }
        if((MaxAddr - Address) <= Info.RegionSize)
            break;
        else
            Address += Info.RegionSize;
        }
    if(PCommitted)
        *PCommitted = Committed;
    return Reserved;
    }

