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

#include <jvmparse.h>
#include <jvmmem.h>
#include <jvmerr.h>
#include <jvmutil.h>
#include <jvmsprng.h>

#define ASSERT_H
#include <jvmhdrs.h>


/* Parses a method descriptor and fills up relevant data */
JVM_BOOL JvmParseMethodDescriptor( 
   IN  OUT   JVM_PBYTE * ppjbByteStream ,
   OUT      _JVM_Method_Info *pjmiMethod
)
{
  JVM_DWORD jdwStatus = JVM_STATUS_OK;

  JVM_ASSERT_VALID(  pjmiMethod );
  pjmiMethod->jdwStatus = ERR_JVM_PARSE_INCORRECTMETHODDESCRIPTOR;

  if ( **ppjbByteStream != '(' )
  {
  	  JvmErrSetErrorCode( pjmiMethod->jdwStatus, S_JVM_PARSE_METHODDESCRIPTOR );
      return JVM_FALSE;
  }

  (*ppjbByteStream)++;

  /* Any number of  parameters */
  jdwStatus = JvmParseParameterDescriptor( 
	             ppjbByteStream ,
	             &pjmiMethod->pjfiParameterFields ,
	             &pjmiMethod->jiParameterCount
	          );

  if ( jdwStatus != JVM_STATUS_OK )
  {
     pjmiMethod->jdwStatus = jdwStatus;
	 JvmParseCleanupFields(
		 &pjmiMethod->pjfiParameterFields,
		 pjmiMethod->jiParameterCount
 	 );
	 return JVM_FALSE;
  }

  if ( **ppjbByteStream != ')' )
  {
	  JvmParseCleanupFields(
	     &pjmiMethod->pjfiParameterFields,
		 pjmiMethod->jiParameterCount
	  );
	  JvmErrSetErrorCode( pjmiMethod->jdwStatus,S_JVM_PARSE_METHODDESCRIPTOR);
      return JVM_FALSE;
  }

  (*ppjbByteStream)++;

  /* One return field */
  jdwStatus = JvmParseReturnDescriptor( 
	  ppjbByteStream, 
	  &pjmiMethod->jbReturn,
	  &pjmiMethod->jfiReturnField
    );

  if ( jdwStatus != JVM_STATUS_OK  )
  {
		JvmParseCleanupFields( 
			&pjmiMethod->pjfiParameterFields,
			 pjmiMethod->jiParameterCount
		);
   	    JvmErrSetErrorCode( pjmiMethod->jdwStatus,S_JVM_PARSE_METHODDESCRIPTOR);
		return JVM_FALSE;
  }

  pjmiMethod->jdwStatus = jdwStatus;

  JvmStoreParametersSize( pjmiMethod );

  return JVM_TRUE; 

}

/* Stores the parameter size in number of JAVA_WORDS */
JVM_VOID JvmStoreParametersSize( 
 IN		_JVM_Method_Info * pjmMethod 
)
{
    JVM_INT  jiParameterCount;
   _JVM_Field_Info * pjfField;

	JVM_ASSERT_VALID( pjmMethod );

	pjmMethod->jiParamsWidthInWords = 0;
	jiParameterCount = pjmMethod->jiParameterCount;

	for ( ;jiParameterCount; )
	{
   	   pjfField =  pjmMethod->pjfiParameterFields[ --jiParameterCount ];
	   pjmMethod->jiParamsWidthInWords +=  pjfField->jiWidthInWords;
	}
}

/** Assumptions : The bytestream has ')' in it .It is 
    quite important since we use it as an anchor 
    value to get out .
    It is the verifiers job to ensure the presence of ')'
 **/

JVM_DWORD JvmParseParameterDescriptor( 
   IN OUT     JVM_PBYTE  * ppjbByteStream ,
   OUT       _JVM_Field_Info *** pppjfiParamFields, 
   OUT        JVM_INT    * pjiParamCount
)
{
   JVM_DWORD jdwStatus  = JVM_STATUS_OK;
   _JVM_Field_Info jfiField = {0,0,0,0,JVM_NULL,0,JVM_STATUS_OK };
   _JVM_Field_Info *pjfiFieldInfo = JVM_NULL;

  if ( **ppjbByteStream == ')')
	   return jdwStatus;

  (*pjiParamCount)++;

  if ( !JvmParseFieldDescriptor( ppjbByteStream , &jfiField ) )
  {
  	   jdwStatus =  ERR_JVM_PARSE_ERRORINPARAMETER;
       JvmErrSetErrorMessage( jdwStatus ,
		   S_JVM_PARSE_PARAMETERDESCRIPTOR,
		   TYPE_INTEGER,
		   *pjiParamCount
	   );
	   return jdwStatus;
  }
  

  /* found a valid parameter */
  *pppjfiParamFields =
	  (_JVM_Field_Info **) JvmMemRealloc(
	     *pppjfiParamFields,
		(*pjiParamCount) * sizeof( _JVM_Field_Info *)
	  );

  (*pppjfiParamFields)[ *pjiParamCount - 1 ]   = 
	   ( _JVM_Field_Info *) JvmMemCalloc( sizeof( _JVM_Field_Info ) );

  /* Copy the information about the parameter */
   *((*pppjfiParamFields)[ *pjiParamCount - 1 ]) = jfiField;

  jdwStatus = JvmParseParameterDescriptor( 
	  ppjbByteStream,
	  pppjfiParamFields,
	  pjiParamCount
	  );
  
  return jdwStatus;
}


/* Return type could be 'V' or a BaseType or an ObjectType */
JVM_DWORD JvmParseReturnDescriptor(
   IN OUT   JVM_PBYTE      * ppjbByteStream ,
   OUT      JVM_BOOL       * pjbReturnExistFlag,
   OUT     _JVM_Field_Info * pjfiField
)
{
	JVM_DWORD jdwStatus = JVM_STATUS_OK;

	*pjbReturnExistFlag = JVM_TRUE;

	if ( **ppjbByteStream == 'V' )
	{
		*pjbReturnExistFlag = JVM_FALSE;
		return jdwStatus;
	}

    if ( !JvmParseFieldDescriptor(ppjbByteStream ,pjfiField ) )
	{
	  jdwStatus = ERR_JVM_PARSE_ERRORINRETURNTYPE;
	  JvmErrSetErrorCode( jdwStatus,S_JVM_PARSE_RETURNDESCRIPTOR);
	}

	return jdwStatus;
}

							  

/* Parses a field descriptor and updates Field info object */
JVM_BOOL JvmParseFieldDescriptor( 
   IN OUT    JVM_PBYTE      * ppjbByteStream ,
   OUT      _JVM_Field_Info * pjfiField
)
{
	JVM_CHAR   jcBaseType      = 0;
    JVM_STRING jsObjectTypeName    = JVM_NULL;
	JVM_INT    jiObjectTypeNameLen = 0;

	JVM_ASSERT_VALID( pjfiField );

	if ( **ppjbByteStream == '\0' )
		return JVM_FALSE;

   /* BaseType */
   if ( IsJvmBaseType( ppjbByteStream , &jcBaseType, &pjfiField->jiWidthInWords ) )
   {
	   pjfiField->jwFieldType |= JVM_BASE_TYPE;
	   pjfiField->jcBaseType   = jcBaseType;
   }
   else
   {
	   /* ObjectType */
	   if ( IsJvmObjectType( ppjbByteStream , &jsObjectTypeName , &jiObjectTypeNameLen ) )
	   {
		   pjfiField->jwFieldType       |=  JVM_OBJECT_TYPE;
		   pjfiField->jcBaseType         =  JAVA_TYPE_REF;
		   pjfiField->jsObjectName       =  JVM_SPRING_CREATE( jsObjectTypeName );
		   pjfiField->jiObjectNameLen    =  JVM_SPRING_LEN( pjfiField->jsObjectName );

		   if ( jiObjectTypeNameLen > JVM_MAX_NAME ) 
		   {
			   pjfiField->jdwStatus = ERR_JVM_PARSE_OBJECTTYPENAMETOOLONG;
  		   	   JvmErrSetErrorCode( pjfiField->jdwStatus,S_JVM_PARSE_FIELDDESCRIPTOR); 
			   return JVM_FALSE;
		   }

	   }
	   else
	   {
		   /* Array,could be of an object or base types */
		   if ( IsJvmArrayType( ppjbByteStream ) )
		   {
		   	   pjfiField->jwFieldType  |= JVM_ARRAY_TYPE;
			   pjfiField->jwArrayDimension ++;
		
			   if ( !JvmParseFieldDescriptor( ppjbByteStream, pjfiField ) )
					   return JVM_FALSE;
		   }
		   else
		   {
			   pjfiField->jdwStatus = ERR_JVM_PARSE_INCORRECTFIELDDESCRIPTOR;
  		   	   JvmErrSetErrorCode( pjfiField->jdwStatus,S_JVM_PARSE_FIELDDESCRIPTOR);
   		       return JVM_FALSE;
		   }
	   }

	   pjfiField->jiWidthInWords = WORD_LEN_REF;
   }


   return JVM_TRUE;
}


/* Clean up routine */
JVM_VOID JvmParseCleanupFields(
   IN  _JVM_Field_Info *** pppjfiFields,
   IN	JVM_INT jiParameterCount
)
{
  JVM_ASSERT_VALID( pppjfiFields );

  if ( !JVM_VALID( *pppjfiFields ) )
	  return;

  /* Free up all the parameter fields ,if any */
  for ( ; jiParameterCount ; )
  {
     --jiParameterCount;
    if ( (*pppjfiFields)[ jiParameterCount ] )
	{
		JvmParseCleanAFieldInfo((*pppjfiFields)[ jiParameterCount ]);
	 	JvmMemFree( (*pppjfiFields)[ jiParameterCount ] );
	}
  }
  /* Free the field pointer */
  JvmMemFree( *pppjfiFields );

  *pppjfiFields = JVM_NULL;

}

JVM_VOID JvmParseCleanAFieldInfo( 
 IN     _JVM_Field_Info * pjfiFieldInfo
)
{
   if (!pjfiFieldInfo ) return;
   if ( pjfiFieldInfo->jsObjectName )
   	   JvmSpringFree(&pjfiFieldInfo->jsObjectName);
}



/** Returns JVM_TRUE  : If Stream has a base type 
            JVM_FALSE : Otherwise 
 **/
JVM_BOOL IsJvmBaseType( 
   IN OUT  JVM_PBYTE * ppjpbStream,
   OUT     JVM_PBYTE pjpbType,
   OUT     JVM_INT  * pjiWidthInWords
)
{
  JVM_INT  ajiBaseTypes[]  =  { 'B','C','D','F','I','J','S','Z' };
  JVM_INT  jiNoOfBaseTypes = sizeof( ajiBaseTypes ) / sizeof( ajiBaseTypes[0] ) ;
  JVM_INT  ajiBaseSizes[]  =  {
		WORD_LEN_BYTE,   WORD_LEN_CHAR,  
		WORD_LEN_DOUBLE, WORD_LEN_FLOAT,
		WORD_LEN_INT,    WORD_LEN_LONG,
		WORD_LEN_SHORT,  WORD_LEN_BOOL
	};


  for ( ;jiNoOfBaseTypes; )
  {
     if ( ajiBaseTypes[ --jiNoOfBaseTypes ] == **ppjpbStream )
     {
	   *pjpbType = *((*ppjpbStream)++) ;
	   *pjiWidthInWords = ajiBaseSizes[ jiNoOfBaseTypes ];
	   return  JVM_TRUE;
     }
  }

  return  JVM_FALSE;
}

/** Returns JVM_TRUE  : If Stream is an object type (  class name )
            JVM_FALSE : Otherwise 
 **/
JVM_BOOL IsJvmObjectType(
   IN OUT  JVM_PBYTE  * ppjbStream,
   OUT     JVM_STRING * pjsObjectTypeName,
   OUT     JVM_INT    * pjiObjectTypeNameLen
)
{
   JVM_BYTE  jbByte = **ppjbStream;
   JVM_INT   jiLen  = 0;
   JVM_PBYTE pjbBeginStream = *ppjbStream;

   /* Must begin with 'L' */
   if ( jbByte != 'L' )
     return JVM_FALSE;

   while (( ++jiLen <= JVM_MAX_NAME ) &&
 	     ( *(++(*ppjbStream)) != ';' ) );
  	  
   if ( jiLen > 	JVM_MAX_NAME )
   {
 	*pjiObjectTypeNameLen = jiLen;
 	return JVM_FALSE;
   }

   *pjiObjectTypeNameLen = *ppjbStream - pjbBeginStream;
   *pjsObjectTypeName = ( JVM_STRING )JvmMemCalloc( *pjiObjectTypeNameLen );

   JvmMemCpy(*pjsObjectTypeName , pjbBeginStream + 1,*pjiObjectTypeNameLen - 1 );
  
   (*pjsObjectTypeName)[ *pjiObjectTypeNameLen - 1 ] = '\0';

   ++(*ppjbStream);

   return JVM_TRUE; 	  
}

/** Returns JVM_TRUE  : If Stream has an array type 
            JVM_FALSE : Otherwise 
 **/
JVM_BOOL IsJvmArrayType(
   IN OUT JVM_PBYTE  * ppjbStream 
)
{
   return (**ppjbStream == '[' ) ? ((*ppjbStream)++,JVM_TRUE) 
       	     : JVM_FALSE;
}
















