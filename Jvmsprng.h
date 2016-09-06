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

#ifndef __JVM_SPRING_INCLUDE__
#define __JVM_SPRING_INCLUDE__

#define UTF8_BYTES_PER_CHAR  2

#define C_STRING					 0x01
#define UTF8_STRING                  0x02
#define READ_ONLY_STRING             0x04

/** Based on this structure **/
typedef struct _JVM_Spring_Node
{
	  JVM_U32  ju32Len;        /** Length **/
	  JVM_U32  ju32Hash;	   /** Hash   **/
	  JVM_U32  ju32RefCount;   /** Reference Count **/
	  JVM_BYTE jbyStringType;  /** Is C String or Is read only etc.?? **/
	  JVM_PBYTE pjbyUtf8OrC;   /** For storing 'UTF8' format string for 'C' strings but
	                               now not for 'C' strings for UTF8 **/
} _JVM_Spring_Node;


JVM_BOOL JvmSpringInit( 
 IN OUT G_STATE_P state 
);

JVM_VOID JvmSpringExit(
);

JVM_PBYTE JvmSpringFromBytes(
   IN JVM_PBYTE  pjbyRawBytes,
   IN JVM_U32    ju32InputLen,
   IN JVM_BOOL   jbIsCString,
   IN JVM_BOOL   jbIsReadOnly
);


JVM_VOID JvmSpringHashRundown( 
 IN 	 Hash_Type   htype
);


JVM_VOID JvmSpringFree(
 IN 	 JVM_PVOID pjvNode 
);

JVM_PBYTE JvmSpringClone( 
 IN       JVM_PBYTE pjbyOriginal
);

JVM_PBYTE JvmSpringToMutantCString(
 IN   JVM_PBYTE  pjbySpring
);

JVM_STRING JvmSpringToCString( 
 IN   JVM_PBYTE  pjbySpring
);

JVM_INT  JvmSpringStrCmp(
 IN JVM_PBYTE  pjbyStrOne,
 IN JVM_PBYTE  pjbyStrTwo
);

JVM_U32 JvmSpringGetLen( 
 IN		JVM_STRING jsName
);

JVM_U32 JvmSpringGetMutantCStringLen( 
 IN		JVM_STRING jsName
);

/** Create a read only 'C' format Spring **/
#define JVM_SPRING_CREATE( x )    JvmSpringFromBytes(x,0,1,1)
#define JVM_SPRING_TO_CSTR(x)     JvmSpringToCString(x)
#define JVM_SPRING_LEN(x)	      JvmSpringGetLen(x)


#endif /** __JVM_Spring_INCLUDE__ **/