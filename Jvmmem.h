/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*  This code and any software associated is covered by                     */
/*  GNU Lesser Public License (LGPL)                                        */
/*                                                                          */
/*  Author: Gopal Ananthraman                                               */
/*                                                                          */
/*  Desc  :                                                                 */
/*                                                                          */
/****************************************************************************/

#ifndef __JVM_MEMORY_INCLUDE__
#define __JVM_MEMORY_INCLUDE__

#include <jvmmcros.h>

JVM_PVOID JvmMemAlloc( 
  IN  JVM_ULONG julSize         
);

JVM_PVOID JvmMemCalloc( 
  IN  JVM_ULONG julSize         
);


JVM_VOID  JvmMemFree(
  IN  JVM_PVOID pjpvBlockToFree
);

JVM_PVOID JvmMemRealloc( 
  IN  JVM_PVOID pjvBlock ,
  IN  JVM_ULONG julNewSize
);

JVM_BOOL  JvmMemIsValidHeapPtr( 
 IN  JVM_PVOID pjvAddress 
);

JVM_VOID  JvmMemDump();
JVM_BOOL  JvmMemInit(G_STATE_P state);
JVM_VOID  JvmMemExit();


#endif /* __JVM_MEMORY_INCLUDE__ */


