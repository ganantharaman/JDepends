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

#ifndef __JVM_UTIL_INCLUDE__
#define __JVM_UTIL_INCLUDE__

#include <jvmmcros.h>


void JvmDisplay(JVM_STRING str );
JVM_STRING JvmStrDup( JVM_STRING jsStr );
JVM_STRING JvmStrCat(JVM_STRING str1,JVM_STRING str2 );
JVM_STRING JvmStrCpy(JVM_STRING str1,JVM_STRING str2 );
JVM_INT    JvmStrCmp( JVM_STRING jsStr1,JVM_STRING jsStr2 );
JVM_INT    JvmStrLen( JVM_STRING jsStr );
JVM_PVOID  JvmMemSet( JVM_PVOID pjvDest,JVM_UINT juChar,JVM_UINT juLength );
JVM_PVOID  JvmMemCpy( JVM_PVOID pjvDest,JVM_PVOID pjvSrc,JVM_UINT juSize );
JVM_PVOID  JvmMemMove( JVM_PVOID pjvDest,JVM_PVOID pjvSrc,JVM_UINT juSize );
JVM_INT    JvmMemCmp( JVM_PVOID pjvBuf1,JVM_PVOID pjvBuf2,JVM_UINT juSize );
JVM_UINT   JvmBitIndex( JVM_ULONG julValue );

#endif /* __JVM_UTIL_INCLUDE__ */