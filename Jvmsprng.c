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

#include <jvmmcros.h>
#include <jvmrt.h>
#include <jvmmem.h>
#include <jvmsys.h>
#include <hashtabl.h>
#include <jvmsprng.h>
#include <jvmutil.h>
#include <jvmexcmn.h>
#include <jvmexcpt.h>

#define ASSERT_H
#define STDIO_H
#include <jvmhdrs.h>


extern Hash_Table * _pjhtGSpringTable;
extern _JVM_Mutex_Object  _jmoGSpringMutex;



/** NOTE ////////////////////////////////////////////////////////////
/   On Kinds of Springs 
/  There are 3 kinds of Springs in our world . 
/  1) UTF8 Spring    
/       Pure UTF8 string each char of 2 byte size and can have values
/	   from 0x0000 to 0xffff.
/  2) C    Spring 
/       Pure 'C' string with each char of 1 byte and null terminated. 
/  3) Mutant C Spring
/        These are the characters in 'C' Spring represented using the
/    		2 byte format with the leading byte 0x00.
/		 These are Meaningful only with 'C' springs. 
/   e.g   the C Spring containing "Sniper\0"
/         in the mutant representation would become 
/           "'\0''S''\0''n''\0''i''\0''p''\0'e''\0''r' 
/          Note that there is no  '\0' character at the end unlike a 
/           normal 'C' String.
/        This representation is for convenience. 
/
/
/////////////////////////////////////////////////////////////////////**/

JVM_BOOL JvmSpringInit(G_STATE_P state)
{
   Hash_Type htinvalid;

   if (IS_EXCL_READY( state ) )
   {

   htinvalid.u.pv = JVM_NULL;
#ifdef __DEBUG__
   if ( !CreateHashTable(
						 "SpringHtbl",
	    &_pjhtGSpringTable ,
	    htinvalid,
	    HASH_TYPE_PTR,
	    HASH_TYPE_PTR,
	    JvmSpringHashRundown
	   ) 
	  )
#else
   if ( !CreateHashTable(
	    &_pjhtGSpringTable ,
	    htinvalid,
	    HASH_TYPE_PTR,
	    HASH_TYPE_PTR,
	    JvmSpringHashRundown
	   ) 
	  )
#endif
     return JVM_FALSE;

     /* Assumptions that the exception system is up by now */

    JVM_SPRING_CREATE("java/lang/Object");
	JVM_SPRING_CREATE("java/lang/Throwable");
    JVM_SPRING_CREATE("java/lang/String");
    JVM_SPRING_CREATE("java/lang/Class");
    JVM_SPRING_CREATE("java/lang/Thread");
    JVM_SPRING_CREATE("Code");
    JVM_SPRING_CREATE("ConstantValue");
    JVM_SPRING_CREATE("Exceptions");
    JVM_SPRING_CREATE("LineNumberTable");
    JVM_SPRING_CREATE("LocalVariableTable");
    JVM_SPRING_CREATE("SourceFile");  
	JVM_SPRING_CREATE("JavaByte");
	JVM_SPRING_CREATE("JavaChar");
	JVM_SPRING_CREATE("JavaInt");
	JVM_SPRING_CREATE("JavaShort");
	JVM_SPRING_CREATE("JavaLong");
	JVM_SPRING_CREATE("JavaDouble");
	JVM_SPRING_CREATE("JavaFloat");


	/** TODO :  Create a Spring Thread that will clean up read only C strings **/

    SPGL_READY( state );

	return JVM_TRUE;
   }


   return JVM_FALSE;



}


/** Assumes that this is called by the last thread **/
JVM_VOID JvmSpringExit()
{

    if ( JVM_VALID(_pjhtGSpringTable ) )
	{
	  JvmSysAcquireMutex( _jmoGSpringMutex );
      DestroyHashTable( _pjhtGSpringTable );
	  _pjhtGSpringTable = JVM_NULL;
	  JvmSysReleaseMutex( _jmoGSpringMutex );
	}
}



/* CHECK,CHECK for correctness **/
/** Makes jvm internal Strings called SniperStrings or Springs            **/
/** A Spring in its raw essence is just a utf8, double null terminated
    stream of bytes **/
/** That explains the JVM_PBYTE return type.                              **/
/** Assumptions : 
      if ju32Len is 0, pjbyRawBytes is considered a constant ANSI String 
	  that cannot be changed and length becomes strlen and the node 
	  itself is stored in a hash table.
	  Caller MUST specify read only state
 **/

JVM_PBYTE JvmSpringFromBytes(
   IN JVM_PBYTE  pjbyRawBytes,
   IN JVM_U32    ju32InputLen,
   IN JVM_BOOL   jbIsCString,
   IN JVM_BOOL   jbIsReadOnly
)
{
  _JVM_Spring_Node * pjsnNode = JVM_NULL; 
   JVM_STRING      jsCString;
   register JVM_U32         ju32Len   = ju32InputLen;
   register JVM_PBYTE  pjbyBytes      = JVM_NULL;
   JVM_PBYTE   pjbySpringBytes = JVM_NULL;
   Hash_Type   htkey, htval;

   if ( !pjbyRawBytes )
	   JVM_THROW_EXCEPTION( EXC_JVMZEROBYTESPRING );
  
   /** If string is ANSI **/
   if ( jbIsCString )
   {
  	 jsCString = (JVM_STRING)pjbyRawBytes; 
	 if ( ju32InputLen == 0 )
       ju32Len = JvmStrLen( jsCString ) + 1;

	 /** If it is read only store it in a hash table **/
	 if ( jbIsReadOnly )
	 {
		JvmSysAcquireMutex( _jmoGSpringMutex );
		htkey.u.pv = jsCString;
		htval = GetFromHashTable( _pjhtGSpringTable, htkey ); 
		pjbyBytes = ( JVM_PBYTE )htval.u.pv;
	 }
   }

   /** If we found a match return it **/
   if ( !JVM_VALID(pjbyBytes) )
   {
      
	  /****/        
	  /** Jvm Internal String ( Spring )representation is **/
	  /** <------- sizeof( _JVM_Spring_Node ) bytes --->|<---string length  bytes--->|<-2 byte->|   
	  /** ----------------------------------------------------------------------------------
	  /**|           Spring node object                 |   string bytes             |   0000   |
      /**-----------------------------------------------------------------------------------  
	  /****/ 

	  /** Must do a calloc or do memset to 0 **/
	  pjbyBytes =  ( JVM_PBYTE )JvmMemCalloc( sizeof( _JVM_Spring_Node ) + ju32Len + UTF8_BYTES_PER_CHAR);
           JVM_ALLOC_CHECK( pjbyBytes );
	  pjsnNode = (  _JVM_Spring_Node * )pjbyBytes;

      pjbySpringBytes  = pjbyBytes + sizeof( _JVM_Spring_Node );
 
	  /** Actual length of string + 1 for '\0' **/
      pjsnNode->ju32Len      = ju32Len;
	  /** Hash value **/
      pjsnNode->ju32Hash     = hash( pjbyRawBytes, ju32Len );
	  pjsnNode->ju32RefCount = 0;
	  /** Default it to UTF8 , non-readonly **/
	  pjsnNode->jbyStringType = 0;

	  if ( jbIsCString )
    	 pjsnNode->jbyStringType |= C_STRING;
	  if ( jbIsReadOnly )
		 pjsnNode->jbyStringType  |= READ_ONLY_STRING;

	  /** Initially NULL for ANSI and points to UTF8 bytes for non-ANSI **/
	  pjsnNode->pjbyUtf8OrC = jbIsCString ? JVM_NULL : pjbySpringBytes;
		/** ANSI String's '\0' is copied here too**/
      JvmMemCpy( pjbySpringBytes, pjbyRawBytes, ju32Len );

	  /** Add it to the hash table , if its an ANSI read only string**/
	  if ( jbIsCString && jbIsReadOnly )
	  {
		  htkey.u.pv = pjbyRawBytes;
		  htval.u.pv = pjsnNode;
		  AddToHashTable( _pjhtGSpringTable, htkey, htval);
	  }
   }

   ((  _JVM_Spring_Node * )pjbyBytes)->ju32RefCount++;
	   
   /** Release the mutex only if ANSI **/
   if ( jbIsCString && jbIsReadOnly )
      JvmSysReleaseMutex( _jmoGSpringMutex );

   return ((JVM_PBYTE)pjbyBytes) + sizeof( _JVM_Spring_Node );
}


/** NON - Re entrant **/
JVM_VOID JvmSpringHashRundown( 
 IN 	 Hash_Type   htype
 )
{
   _JVM_Spring_Node * pjsnNode;
   JVM_ASSERT_VALID( ppjvNode );
   pjsnNode = (_JVM_Spring_Node *)htype.u.pv;

   if (pjsnNode->ju32RefCount == 0 )
   {
      if ( pjsnNode->jbyStringType & C_STRING &&
		   pjsnNode->jbyStringType & READ_ONLY_STRING
		 )
	  {
	      /** Free the Utf8OrC pointer, if any **/
		  if (JVM_VALID(pjsnNode->pjbyUtf8OrC))
		   JvmMemFree( pjsnNode->pjbyUtf8OrC );
     	  /** And the node itself **/
		   JvmMemFree( pjsnNode );
	  }
   }
}


JVM_VOID JvmSpringFree(
 IN 	 JVM_PVOID  * ppjvNode 
)
{
  _JVM_Spring_Node * pjsnNode;
   JVM_ASSERT_VALID( ppjvNode );
	
   if ( JVM_VALID(*ppjvNode) )
   {
	  pjsnNode = (_JVM_Spring_Node * )
		  ((JVM_PBYTE)(*ppjvNode) - sizeof( _JVM_Spring_Node )); 
	  pjsnNode->ju32RefCount --;

	  /** Read only 'C' strings will be freed elsewhere ,we just decrement
	      the count **/
	  if ( pjsnNode->jbyStringType & C_STRING   &&
	   	   pjsnNode->jbyStringType & READ_ONLY_STRING )
	  {
       JVM_SYS_POSTTHREADMESSAGE("Jvm_Thd_Spring", ptr, pjsnNode );
			 return;
	  }	

	  JVM_ASSERT( pjsnNode->ju32RefCount == 0 );

      /** Free the Utf8OrC pointer, if any **/
	  if (JVM_VALID(pjsnNode->pjbyUtf8OrC))
		   JvmMemFree( pjsnNode->pjbyUtf8OrC );

	  /** And the node itself **/
	  JvmMemFree( pjsnNode );

      *ppjvNode = JVM_NULL;
   }
}



JVM_PBYTE JvmSpringToMutantCString(
 IN   JVM_PBYTE  pjbySpring
)
{
  _JVM_Spring_Node * pjsnNode;

  JVM_ASSERT_VALID( pjbySpring );
  pjsnNode = (_JVM_Spring_Node * )(pjbySpring - sizeof( _JVM_Spring_Node )); 

  if ( pjsnNode->jbyStringType & C_STRING )
  {
	 if ( pjsnNode->pjbyUtf8OrC == JVM_NULL )
	 {
		 /** Calloc is important **/
		 pjsnNode->pjbyUtf8OrC = JvmMemCalloc( 
			 (pjsnNode->ju32Len - 1) * UTF8_BYTES_PER_CHAR );
		 JVM_ALLOC_CHECK( pjsnNode->pjbyUtf8OrC );


		 /** For characters c1c2c3...cN in ANSI 
		     Create 00c100c2...00cN **/ 
		 {
			JVM_U32 ju32Index;

  			for ( ju32Index = 0; 
			ju32Index < pjsnNode->ju32Len; ju32Index ++ )
			{
			  pjsnNode->pjbyUtf8OrC[ ju32Index * 
				  UTF8_BYTES_PER_CHAR + 1] = pjbySpring[ ju32Index ] ;
			}
		 }
	 }
  }

  return pjsnNode->pjbyUtf8OrC;
}



JVM_STRING JvmSpringToCString( 
 IN   JVM_PBYTE  pjbySpring
)
{
  _JVM_Spring_Node * pjsnNode;
   
  JVM_ASSERT_VALID( pjbySpring );

  pjsnNode = (_JVM_Spring_Node * )(pjbySpring - sizeof( _JVM_Spring_Node )); 

  if ( !pjsnNode->jbyStringType & C_STRING )
  {
    JVM_ASSERT_WARN(pjsnNode->jbyStringType & C_STRING,
		"UTF8 Characters are converted to C String");
  }

  /** For now just return the pjbyUtf8Str itself ,it is
      0000 terminated anyway **/
   return (JVM_STRING)pjbySpring;
}



JVM_INT  JvmSpringStrCmp(
 IN JVM_PBYTE  pjbyStrOne,
 IN JVM_PBYTE  pjbyStrTwo
)
{
  _JVM_Spring_Node * pjsnOne;
  _JVM_Spring_Node * pjsnTwo; 
 
  JVM_ASSERT_VALID( pjbyStrOne );
  JVM_ASSERT_VALID( pjbyStrTwo );

  pjsnOne = (_JVM_Spring_Node * )(pjbyStrOne - sizeof( _JVM_Spring_Node ));
  pjsnTwo = (_JVM_Spring_Node * )(pjbyStrTwo - sizeof( _JVM_Spring_Node )); 

  if ( pjsnOne->ju32Hash != pjsnTwo->ju32Hash )
     return -1;

  return (JvmMemCmp( pjbyStrOne,
	                  pjbyStrTwo,
                      min( pjsnOne->ju32Len, pjsnTwo->ju32Len ))
					);
}


JVM_U32 JvmSpringGetLen( 
 IN		JVM_STRING jsName
)
{
  JVM_ASSERT_VALID( jsName );

  return ((_JVM_Spring_Node * )(jsName - sizeof( _JVM_Spring_Node )))->ju32Len;
}


JVM_PBYTE JvmSpringClone( 
 IN       JVM_PBYTE pjbyOriginal
 )
{
  _JVM_Spring_Node * pjsnSpring, * pjsnCopy;
   JVM_PBYTE  pjbyClone,pjbySpringBytes;
   register JVM_BOOL jbIsCString;

  pjsnSpring = (_JVM_Spring_Node * )(pjbyOriginal - sizeof( _JVM_Spring_Node ));

  jbIsCString = pjsnSpring->jbyStringType & C_STRING ;

  if ( jbIsCString && pjsnSpring->jbyStringType & READ_ONLY_STRING )
  {
	 pjsnSpring->ju32RefCount ++;
	 return  pjbyOriginal;
  }

   pjbyClone =  ( JVM_PBYTE )JvmMemCalloc( 
	            sizeof( _JVM_Spring_Node ) + 
				pjsnSpring->ju32Len + 
				UTF8_BYTES_PER_CHAR
				);
                JVM_ALLOC_CHECK( pjbyClone );

    pjsnCopy = (  _JVM_Spring_Node * )pjbyClone;

    pjbySpringBytes  = pjbyClone + sizeof( _JVM_Spring_Node );
    /** Actual length of string + 1 for '\0' **/
    pjsnCopy->ju32Len      = pjsnSpring->ju32Len;
    /** Hash value **/
    pjsnCopy->ju32Hash     = pjsnSpring->ju32Hash;
    pjsnCopy->ju32RefCount = 1;
    /** Default it to UTF8 , non-readonly **/
    pjsnCopy->jbyStringType = pjsnCopy->jbyStringType;
    
    /** Initially NULL for ANSI and points to UTF8 bytes for non-ANSI **/
    pjsnCopy->pjbyUtf8OrC = jbIsCString ? JVM_NULL : pjbySpringBytes;
		/** ANSI String's '\0' is copied here too**/
    JvmMemCpy( pjbySpringBytes,pjbyOriginal, pjsnSpring->ju32Len );

	if ( JVM_VALID( pjsnSpring->pjbyUtf8OrC ))
		JvmSpringToMutantCString( pjbySpringBytes );

	return 	pjbySpringBytes;
}




JVM_U32 JvmSpringGetMutantCStringLen( 
 IN		JVM_STRING jsName
)
{
	_JVM_Spring_Node * pjsnNode;

	JVM_ASSERT_VALID( jsName );
	pjsnNode = (_JVM_Spring_Node * )(jsName - sizeof( _JVM_Spring_Node ));
	if ( JVM_VALID( pjsnNode->pjbyUtf8OrC ) )
    	return (pjsnNode->ju32Len - 1) * UTF8_BYTES_PER_CHAR;

	return  pjsnNode->ju32Len;
}


