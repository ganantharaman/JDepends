/* alloc.h - control which allocator is used.
 *
 * This header file is included by programs that want to test
 * against various memory allocators. Such programs will call
 * the macros MALLOC() and FREE() rather than a specific allocator.
 * This header file expands those macros based on how you set
 * two command-line options:
 * 
 * /DALLOCATOR=1   <- use compiler's standard malloc()/free()
 * /DALLOCATOR=2   <- use compiler's standard new/delete
 * /DALLOCATOR=3   <- use OLE's IMalloc() interface
 * /DALLOCATOR=4   <- use Win32's Heap management functions
 *
 * To use this header file, you simply #include it and then make
 * sure to call AllocInit() before performing any calls to MALLOC()
 * and FREE(). A HEAPMIN() function is also provided, though it
 * expands to -1 (failure) for those environments that don't support
 * it. You must also link with alloc.obj, which contains AllocInit().
 * MODIFICATIONS : 
 * A.G 
 *  CHECKTOOL :: Modified for analysing JvmMemAlloc and JvmMemFree();
 */

#include <stdlib.h>
#include <objbase.h>    /* for IMalloc interface */
#include <windows.h>

int     AllocInit(void);
DWORD   MemStats(DWORD*);
DWORD   MemAvail(void);
void*   GetMem(DWORD Size);

void    DummyFree(void* Foo);
void*   DummyMalloc(size_t Size);

extern  HANDLE      MyHeap;
extern  IMalloc*    OleAlloc;

#define ALLOCATOR  1

#if !defined(ALLOCATOR)
#   error   Set ALLOCATOR to a value defined in alloc.h
#elif ALLOCATOR==1
#   define ALLOCINIT()      (1)
#   define MALLOC_NAME      "malloc()"
#   define MALLOC           malloc
#   define FREE             free
#   if defined(__BORLANDC__)    /* Borland doesn't need a heapmin() */
#       define HEAPMIN()        (0)
#   elif defined(__WATCOMC__)   /* Watcom uses a different name     */
#       define HEAPMIN()        (_heapshrink())
#   else
#       define HEAPMIN()        (_heapmin())
#   endif
#elif ALLOCATOR==2
#   define ALLOCINIT()      (1)
#   define MALLOC_NAME      "operator new"
#   define MALLOC(size)     ((void*)new char[size])
#   define FREE(p)          (delete[] p)
#   define HEAPMIN()        (_heapmin())
#elif ALLOCATOR==3
#   define ALLOCINIT()      AllocInit()
#   define MALLOC_NAME      "OLE IMalloc"
#   if defined __cplusplus
#       define MALLOC(size) OleAlloc->Alloc(size)
#       define FREE(p)      OleAlloc->Free(p)
#       define HEAPMIN()    (OleAlloc->HeapMinimize(), 0)
#   else
#       define MALLOC(size) OleAlloc->lpVtbl->Alloc(OleAlloc, size)
#       define FREE(p)      OleAlloc->lpVtbl->Free(OleAlloc, p)
#       define HEAPMIN()    (OleAlloc->lpVtbl->HeapMinimize(OleAlloc), 0)
#   endif
#elif ALLOCATOR==4
#   define ALLOCINIT()      AllocInit()
#   define MALLOC_NAME      "Win32 HeapAlloc()"
#   define MALLOC(size)     (HeapAlloc(MyHeap, \
                                HEAP_NO_SERIALIZE, size))
#   define FREE(p)          (HeapFree(MyHeap, HEAP_NO_SERIALIZE, p))
#   define HEAPMIN()        (HeapCompact(MyHeap, HEAP_NO_SERIALIZE))
/** A.G : Added for JVM Memory Manager Tests **/
#elif ALLOCATOR==5
#   define ALLOCINIT()         JvmMemInit()
#   define MALLOC_NAME         "JvmMemAlloc()"
#   define MALLOC           JvmMemAlloc
#   define FREE             JvmMemFree
#   if defined(__BORLANDC__)    /* Borland doesn't need a heapmin() */
#       define HEAPMIN()        (0)
#   elif defined(__WATCOMC__)   /* Watcom uses a different name     */
#       define HEAPMIN()        (_heapshrink())
#   elif defined(__JVM_CHECKTOOL_TEST_ENV__)
#       define HEAPMIN()        (JvmMemExit())
#   else
#       define HEAPMIN()        (_heapmin())
#   endif
#elif ALLOCATOR==6
#   define ALLOCINIT()         mem_init()
#   define MALLOC_NAME         "mem_alloc()"
#   define MALLOC           mem_alloc
#   define FREE             mem_free
#   if defined(__BORLANDC__)    /* Borland doesn't need a heapmin() */
#       define HEAPMIN()        (0)
#   elif defined(__WATCOMC__)   /* Watcom uses a different name     */
#       define HEAPMIN()        (_heapshrink())
#   elif defined(__JVM_CHECKTOOL_TEST_ENV__)
#       define HEAPMIN()        (mem_exit())
#   else
#       define HEAPMIN()        (_heapmin())
#   endif
#else
#   error   Set ALLOCATOR to a value defined in alloc.h
#endif




#if defined(__SC__)
#   define COMPILER_NAME "Symantec"
#elif defined(__BORLANDC__)
#   define COMPILER_NAME "Borland"
#elif defined(_MSC_VER)
#   define COMPILER_NAME "Microsoft"
#elif defined(__WATCOMC__)
#   define COMPILER_NAME "Watcom"
#else
#   define COMPILER_NAME "Unknown"
#endif

