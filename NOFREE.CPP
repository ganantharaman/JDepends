#include <malloc.h>
#include "alloc.h"

#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <jvmmem.h>


void    main()
    {
    void*   Ptrs[1024];
    DWORD   StartRes, StartCom;
    DWORD   BigRes, BigCom;
    int     i;

    StartRes    = MemStats(&StartCom);

    printf("Start: 0x%08X Reserved, 0x%08X Committed\n",
        StartRes, StartCom);

    // now use up at least BIGALLOC Mb of memory
    for(i = 0; i < 1024; ++i)
        assert((Ptrs[i] = MALLOC(1024*4+rand()%512)) != 0);

    BigRes  = MemStats(&BigCom);

    printf("Middle: 0x%08X Reserved, 0x%08X Committed\n",
        BigRes, BigCom);
    assert(BigRes > StartRes);

    for(i = 0; i < 1024; ++i)
        FREE(Ptrs[i]);

    BigRes  = MemStats(&BigCom);

    printf("End: 0x%08X Reserved, 0x%08X Committed\n",
        BigRes, BigCom);
    if(BigRes <= StartRes && BigCom <= StartCom)
        printf("%s implicitly frees memory\n", COMPILER_NAME);
    HEAPMIN();
    BigRes  = MemStats(&BigCom);

    printf("Final: 0x%08X Reserved, 0x%08X Committed\n",
        BigRes, BigCom);
    if(BigCom > StartCom)
        printf("%s does not decommit memory!\n", COMPILER_NAME);
    if(BigRes > StartRes)
        printf("%s does not free memory!\n", COMPILER_NAME);
    }

