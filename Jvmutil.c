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

#include <jvmutil.h>

#define STRING_H "Includes string header"
#include <jvmhdrs.h>

JVM_STRING JvmStrDup( JVM_STRING jsStr )
{
  return  (JVM_STRING)strdup( jsStr );
}

/** Assumptions jsStr1 is big enough **/
JVM_STRING JvmStrCat( JVM_STRING jsStr1,JVM_STRING jsStr2 )
{
   return (JVM_STRING)strcat( jsStr1,jsStr2 );
}

JVM_STRING JvmStrCpy( JVM_STRING jsStr1,JVM_STRING jsStr2 )
{
   return (JVM_STRING)strcpy( jsStr1,jsStr2 );
}   

JVM_INT JvmStrCmp( JVM_STRING jsStr1,JVM_STRING jsStr2 )
{
	return stricmp(jsStr1,jsStr2);
}

JVM_INT JvmStrLen( JVM_STRING jsStr )
{
	return strlen( jsStr );
}

JVM_VOID JvmDisplay(JVM_STRING message )
{
   JVM_DISPLAY( message );
}

JVM_PVOID JvmMemCpy( JVM_PVOID pjvDest,JVM_PVOID pjvSrc,JVM_UINT juSize )
{
	return memcpy( pjvDest,pjvSrc,juSize );
}

JVM_PVOID JvmMemMove( JVM_PVOID pjvDest,JVM_PVOID pjvSrc,JVM_UINT juSize )
{
	return memmove( pjvDest,pjvSrc,juSize );
}

JVM_PVOID JvmMemSet( JVM_PVOID pjvDest,JVM_UINT juChar,JVM_UINT juLength )
{
	return memset( pjvDest,juChar,juLength);
}

JVM_INT JvmMemCmp( JVM_PVOID pjvBuf1,JVM_PVOID pjvBuf2,JVM_UINT juSize )
{
   return memcmp( pjvBuf1, pjvBuf2, juSize );
}


/* returns the index of the set bit */
/* Input assumed to be a number which is 2^(n) */
JVM_UINT JvmBitIndex( JVM_ULONG julValue )
{
  JVM_UINT juCount = 0;
  JVM_ULONG ONE = 1;

  for ( ; ; )
  {
	 if ( ( ONE << juCount ) == julValue )
		 break;

	 juCount++;
  }

  return juCount + 1;
}