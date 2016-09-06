/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*  This code and any software associated is covered by                     */
/*  GNU Lesser General Public License (LGPL)                                        */
/*                                                                          */
/*  Author: Gopal Ananthraman                                               */
/*                                                                          */
/*  Desc  :                                                                 */
/*                                                                          */
/****************************************************************************/

/***************************************************
 *      	 PLATFORM SPECIFIC MACROS
 *  PLATFORM : WIN32 ( Win95 & NT )
 *  Supported Macros : 
 *         JVM Data Types,Debugging,Threads,
 *               Input/Output,Common Includes
 *
 ***************************************************/  
/* WIN32 specific */

#ifndef __JVM_WIN32_INCLUDE__
#define __JVM_WIN32_INCLUDE__

#include <windows.h>
#include <osapi.h>

#ifdef WIN32

/* JVM Data Types */
#define JVM_CHAR       char
#define JVM_BYTE       unsigned char
#define JVM_SIGNED16   short
#define JVM_SHORT      short
#define JVM_WORD       unsigned short 
#define JVM_INT        int
#define JVM_SIGNED32   int
#define JVM_BOOL       unsigned int
#define JVM_UINT       unsigned int
#define JVM_UNSIGNED16 JVM_WORD
#define JVM_UNSIGNED32 unsigned int
#define JVM_U64        unsigned __int64
/* sizeof(long) == sizeof(int) in WIN32 */
#define JVM_LONG         long
/* sizeof(unsigned long) ==
   sizeof(unsigned int) in WIN32 */
#define JVM_UNSIGNEDLONG unsigned long
#define JVM_ULONG  unsigned long
#define JVM_DWORD  JVM_ULONG
#define JVM_FLOAT   float
#define JVM_DOUBLE  double
#define JVM_VOID         void
#define JVM_HANDLE       HANDLE
#define JVM_FILE         HANDLE

/** JAVA base types **/
#define JAVA_CHAR       unsigned short
#define JAVA_BYTE       char
#define JAVA_SHORT      short
#define JAVA_INT        int
#define JAVA_LONG     __int64
#define JAVA_FLOAT      float 
#define JAVA_DOUBLE     double
#define JAVA_BOOL       int

#define JAVA_WORD       unsigned int

/** 1JAVA base values **/
#define JAVA_NULL       NULL
#define JAVA_TRUE       1
#define JAVA_FALSE      0

#define JVM_WORDS_TO_LONG(HI, LO) (((JVM_U64)(HI) << 32)|(JVM_U64)(JVM_ULONG)(LO))


#define WORD_LEN_CHAR   1
#define WORD_LEN_BYTE   1
#define WORD_LEN_SHORT  1
#define WORD_LEN_INT    1
#define WORD_LEN_LONG   2
#define WORD_LEN_FLOAT  1
#define WORD_LEN_DOUBLE 2
#define WORD_LEN_BOOL   1

#define WORD_LEN_REF    1


/** Bits Per Byte in Win32**/
#define JVM_BITS_PER_BYTE 8

/** Boolean macros **/
#define JVM_TRUE  TRUE
#define JVM_FALSE FALSE

/** NULL macros **/
#define JVM_NULL  NULL
#define JVM_NULL_TYPE LPVOID


/* Debugging macros */
#define JVM_ASSERT(x) assert((x)) 
#define JVM_ASSERT_VALID(x) assert((x)) 
#define JVM_ASSERT_WARN(x,y) { printf("WARNING: %s\r\n",(y));assert((x)); }

/* Thread Specific */
#define	 JVM_THREAD  os_thread_t* 
#define  JVM_MUTEX   lock_t
#define  JVM_THD_CLB_RETURN    void 
#define  JVM_THD_CLB_PREFIX    
#define  JVM_THD_CLB_PARAM     JVM_ULONG

/** Error handling specific **/
#define JVM_ERROR DWORD

/* Input/Output specific */
#if 0
#define JVM_DISPLAY( x )  MessageBox(NULL, x,"Sniper MicroVM",MB_OK);
#endif
#define JVM_DISPLAY( x )
#define JVM_INVALID_HANDLE   INVALID_HANDLE_VALUE


/* Native function call */
#ifdef __I386__

extern WORD  _os_i86_load_word( BYTE * pBytes );
extern DWORD _os_i86_load_doubleword( BYTE * pBytes );

#define LD_WORD(x)	  _os_i86_load_word( (BYTE *)x )
#define LD_DWORD(x)   _os_i86_load_doubleword((BYTE *)x)

#define JVM_NATIVE_CALL( nArgs, dwArgs, lpfnFunc, dwReturn, bIsFP, bIsJLong, pjvSaveEdx ) \
__asm    \
   {			 \
	   __asm push eax  \
     __asm push ebx  \
	   __asm push ecx  \
     __asm lea  eax,dwArgs \
	   __asm mov ecx,0 \
	   __asm __L1_##dwReturn:          \
     __asm cmp ecx, nArgs  \
	   __asm je near __L2_##dwReturn \
     __asm mov ebx,dword ptr[eax+4*ecx] \
	   __asm push ebx  \
	   __asm inc ecx   \
	   __asm jmp near __L1_##dwReturn \
	   __asm __L2_##dwReturn:   \
	   __asm call lpfnFunc   \
     __asm mov ebx, eax    \
     __asm mov dword ptr[pjvSaveEdx], edx    \
	   __asm mov eax, nArgs  \
	   __asm mov ecx, 4	   \
	   __asm mul ecx		   \
     __asm add esp, eax	   \
	   __asm mov ecx, bIsFP  \
	   __asm cmp ecx,1	   \
	   __asm jne  __L3_##dwReturn  \
	   __asm mov ecx, bIsJLong	  \
     __asm cmp ecx,1			  \
	   __asm je  __L41_##dwReturn  \
	   __asm fstp dword ptr[dwReturn] \
	   __asm jmp near __L51_##dwReturn \
	   __asm __L41_##dwReturn:         \
	   __asm fstp qword ptr[dwReturn] \
     __asm jmp  __L51_##dwReturn  \
	   __asm __L3_##dwReturn:          \
	   __asm mov ecx, bIsJLong	   \
     __asm cmp ecx,1			   \
	   __asm je  __L4_##dwReturn  \
	   __asm mov dwReturn, ebx \
	   __asm jmp near __L51_##dwReturn \
	   __asm __L4_##dwReturn: \
	   __asm mov eax,dwReturn  \
	   __asm mov dword ptr[eax], ebx \
	   __asm mov edx, dword ptr[pjvSaveEdx]\
	   __asm mov dword ptr[eax + 4],edx \
       __asm __L51_##dwReturn: \
	   __asm pop ecx		   \
	   __asm pop ebx		   \
	   __asm pop eax		   \
   }				   

#endif /* __I386__ */
#endif /*  WIN32 */
#endif /* __JVM_WIN32_INCLUDE__ */
