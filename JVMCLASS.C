

#include <jvmrt.h>
#include <jvmexcmn.h>
#include <jvmexcpt.h>
#include <jvmsys.h>
#include <jvmutil.h>
#include <hashtabl.h>

extern _JVM_Mutex_Object  _jmoGGlobalDataMutex;




/* Static method Class.forName implementation */
_JVM_Class_Class * JvmClassForName(
   IN   JVM_STRING jsClassName
)
{
   /** TODO **/
  return JVM_NULL;
  
}

/* Class.newInstance */
_JVM_Object * JvmClassNewInstance(
   IN   _JVM_Class_Class * pjccThisClass 
)
{
	/** TODO **/
  return JVM_NULL;
}

/* Class.isInstance */
JVM_BOOL JvmClassIsInstance(
  IN  _JVM_Object * pjoThisObject,
  IN  _JVM_Object * pjoAnObject 
)
{
   JVM_ASSERT_VALID ( pjoThisObject );
   JVM_ASSERT_VALID ( pjoAnObject );
   JVM_ASSERT_VALID ( pjoThisObject->pjccClass );
   JVM_ASSERT_VALID ( pjoAnObject->pjccClass );


   return ( JvmStrCmp(
	   pjoThisObject->pjoiObjectID->pjccClass->jsClassName,
	   pjoAnObject->pjoiObjectID->pjccClass->jsClassName 
	   ) == 0 );
}

/* Class.isAssignableFrom */
JVM_BOOL JvmClassIsAssignableFrom(
  IN   _JVM_Object * pjoThisObject,
  IN   _JVM_Class_Class * pjccAClass
)
{
  _JVM_Class_Class *pjccThisClass = JVM_NULL;

  JVM_ASSERT_VALID( pjoThisObject );
  JVM_ASSERT_VALID( pjccAClass );

  pjccThisClass =  pjoThisObject->pjoiObjectID->pjccClass;
  JVM_ASSERT_VALID( pjccThisClass );

  if ( pjccThisClass->jdwClassFlags & JAVA_TYPE_PRIMITIVE )
    return pjccThisClass == pjccAClass;

  do
  {
    if ( JvmStrCmp(pjccThisClass->jsClassName,pjccAClass->jsClassName ) == 0 )
		 return JVM_TRUE;

	pjccThisClass = pjccAClass->joSuperClassObject->pjoiObjectID->pjccClass;

  } while ( pjccThisClass );

  return JVM_FALSE;
  
}


JVM_BOOL JvmClassIsInterface(
  IN   _JVM_Object * pjoThisObject
)
{
	JVM_ASSERT_VALID( pjoThisObject );
	JVM_ASSERT_VALID( pjoThisObject->pjccClass );

    return ( pjoThisObject->pjoiObjectID->pjccClass->jdwClassFlags &  JAVA_TYPE_INTERFACE );
}

JVM_BOOL JvmClassIsArray(
  IN   _JVM_Object * pjoThisObject
)
{
	JVM_ASSERT_VALID( pjoThisObject );
	JVM_ASSERT_VALID( pjoThisObject->pjccClass );

    return ( pjoThisObject->pjoiObjectID->pjccClass->jdwClassFlags &  JAVA_TYPE_ARRAY );
}

JVM_BOOL JvmClassIsPrimitive(
  IN   _JVM_Object * pjoThisObject
)
{
	JVM_ASSERT_VALID( pjoThisObject );
	JVM_ASSERT_VALID( pjoThisObject->pjccClass );

    return ( pjoThisObject->pjoiObjectID->pjccClass->jdwClassFlags &  JAVA_TYPE_PRIMITIVE );
}


JVM_STRING JvmClassGetName(
  IN   _JVM_Object * pjoThisObject
)
{
	JVM_ASSERT_VALID( pjoThisObject );
	JVM_ASSERT_VALID( pjoThisObject->pjccClass );

    return ( pjoThisObject->pjoiObjectID->pjccClass->jsClassName );
}


_JVM_Object * JvmClassGetClassLoader(
  IN  _JVM_Object * pjoThisObject
)
{
	JVM_ASSERT_VALID( pjoThisObject );
	JVM_ASSERT_VALID( pjoThisObject->pjccClass );

    return ( pjoThisObject->pjoiObjectID->pjccClass->joClassLoaderObject );
}

_JVM_Object  * JvmGetSuperClass(
  IN  _JVM_Object * pjoThisObject
)
{
	JVM_ASSERT_VALID( pjoThisObject );
	JVM_ASSERT_VALID( pjoThisObject->pjccClass );

	return pjoThisObject->pjoiObjectID->pjccClass->joSuperClassObject;
}


_JVM_Object ** JvmGetInterfaces(
  IN  _JVM_Object * pjoThisObject
)
{
	JVM_ASSERT_VALID( pjoThisObject );
	JVM_ASSERT_VALID( pjoThisObject->pjccClass );

    return pjoThisObject->pjoiObjectID->pjccClass->ppjoInterfaces;
}


_JVM_Class_Class * JvmGetComponentType(
  IN  _JVM_Object * pjoThisObject
)
{
	JVM_ASSERT_VALID( pjoThisObject );
	JVM_ASSERT_VALID( pjoThisObject->pjccClass );

   if ( pjoThisObject->pjoiObjectID->pjccClass->jdwClassFlags & JAVA_TYPE_ARRAY )
     return   pjoThisObject->pjoiObjectID->pjccClass;

   return JVM_NULL;
}

/* Note that we return a DWORD which is 2 words == 32 bit unsigned int
   Since we don't use the sign bit it can as well be treated as a
   JAVA int type
*/
JVM_DWORD  JvmGetModifiers(
  IN  _JVM_Object * pjoThisObject
)
{
   JVM_ASSERT_VALID( pjoThisObject );
   JVM_ASSERT_VALID( pjoThisObject->pjccClass );

   return pjoThisObject->pjoiObjectID->pjccClass->jdwClassFlags; 
}





				 


























