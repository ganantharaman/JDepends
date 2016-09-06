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


#include <jvmexcmn.h>
#include <jvmrt.h>
#include <jvmutil.h>
#include <jvmanstr.h>
#include <jvmsys.h>
#include <jvmsprng.h>
#include <jvmldr.h>


/* Forward declarations 
  ,Declared here to avoid circular references 
*/

extern
JVM_BOOL JvmLdrLoadNewClass(
  IN     JVM_STRING  jsClassFileName,
  IN     JVM_PBYTE   pjbyClassBytes,
  IN     JVM_OBJREF  joClassLoader, 
  OUT    JVM_OBJREF  * pjoClassObject
);


extern 
_JVM_Class_Class * JvmLdrGetExceptionClass( 
  IN    eJVMExceptionType ejetException,
  IN    JVM_OBJREF  joClassLoader
);

extern
JVM_OBJREF JvmRtCreateNewInstance(
 IN   _JVM_Class_Class *pjccTheClass 
);

extern 
JVM_VOID JvmRtDestroyStackFrame();

extern
JVM_BOOL  IsKindOf( 
  IN       JVM_STRING  jsSubjectClassName,
  IN       JVM_STRING  jsTargetClassName,
  IN       JVM_OBJREF  joClassLoader
);

extern
JVM_STRING JvmGetClassName(
  IN  _JVM_Class_Class * pjccTheClass, 
  IN   JVM_WORD jwCPIndex,
  OUT  JVM_WORD * pjwClassNameLen
);



JVM_VOID JvmExcCallDefaultHandler( 
  IN     JAVA_OBJREF joExcObject, 
  IN     eJVMExceptionType ejetException 
);

JVM_BOOL JvmCatchException( 
  IN    eJVMExceptionType ejetException
);
JVM_VOID JvmThrowException( 
  IN    eJVMExceptionType ejetException
);
JVM_INT  JvmDefaultExceptHandler(
  IN    eJVMExceptionType ejetException
);
JVM_STRING JvmExcToStr(
  IN    eJVMExceptionType ejetException
);
JVM_VOID JvmThrowUnhandledException(
);
JVM_BOOL JvmCatchAllExceptions(
);
JVM_VOID JvmThrowJavaException(
  IN    eJVMExceptionType ejetException
);
JVM_VOID JvmThrowRealJavaException( 
  IN     JAVA_OBJREF joExcObject, 
  IN     eJVMExceptionType ejetException 
);

JVM_VOID JvmExcPrepareToExecExcHandler(
  IN   JAVA_WORD jawPC, 
  IN   JAVA_OBJREF joExcObject 
);



JVM_BOOL JvmExceptInit(G_STATE_P state )
{
	EXCL_READY( state );
	return JVM_TRUE;
}

JVM_STRING JvmExcToStr(
  IN     eJVMExceptionType ejetException 
) 
{  return _aajbExcToStr[ ejetException ];   }


JVM_VOID JvmThrowException(
  IN  eJVMExceptionType ejetException
)
{
  /* Get the Current Thread state */
  _JVM_Thread_State *pjtsThreadState = JvmGetThreadState();   
  _JVM_Except_Frame *pjefFrame;

  /** Don't throw if it's the Special value( used internally) **/
  if ( EXC_NOEXCEPTION == ejetException )
	  return;
 
  /* Validate */
  JVM_ASSERT( pjtsThreadState != NULL );
  /* Set the exception of the top exception frame
     in exception stack */
  pjefFrame = pjtsThreadState->pjefTopFrame;
  /* Call the default handler if not caught */
  if ( pjefFrame ) {
	  pjefFrame->ejetException =  ejetException;
      /* long jump */
      longjmp(pjefFrame->jbCallStack,1);
      /* Should never be reached */
      JVM_ASSERT( FALSE );
  }
  else
  {
   	 /* Call the default handler */
	  JVM_ASSERT( pjtsThreadState->pjdehDefExHandler != NULL );
	 (*pjtsThreadState->pjdehDefExHandler)(ejetException);  
  }
}

JVM_VOID JvmThrowUnhandledException()
{
  /* Get the Current Thread state */
  _JVM_Thread_State *pjtsThreadState = JvmGetThreadState();   
  _JVM_Except_Frame *pjefFrame;
  
  /* Validate */
  JVM_ASSERT( pjtsThreadState != NULL );
  /* Set the exception of the top exception frame
     in exception stack */
  pjefFrame = pjtsThreadState->pjefTopFrame;
  JVM_ASSERT( pjefFrame != NULL );

  /* Propagate the exception to a higher level handler,
     if present	. If not,call the default */
  if ( pjefFrame->pjefPrevFrame )
  {
	  pjefFrame->pjefPrevFrame->ejetException = 
		  pjefFrame->ejetException;
	  pjtsThreadState->pjefTopFrame = pjefFrame->pjefPrevFrame;
	  longjmp(pjtsThreadState->pjefTopFrame->jbCallStack,1);
	  /* should never be reached */
	  JVM_ASSERT( FALSE );
  }
  else
  {
	 /* Call the default handler */
	  JVM_ASSERT( pjtsThreadState->pjdehDefExHandler != NULL );
	 (*pjtsThreadState->pjdehDefExHandler)(pjefFrame->ejetException);  
  }
  
}

JVM_BOOL JvmCatchException(
  IN     eJVMExceptionType ejetException
)
{
  _JVM_Thread_State *pjtsThreadState = JvmGetThreadState();   
  _JVM_Except_Frame *pjefFrame;
  
  /* Validate */
  JVM_ASSERT( pjtsThreadState != NULL );
  /* Set the exception of the top exception frame
     in exception stack */
  pjefFrame = pjtsThreadState->pjefTopFrame;
  JVM_ASSERT( pjefFrame != NULL );

  /* return true if the exception is similar */
  return ( pjefFrame->ejetException == ejetException ) ?
			JVM_TRUE : JVM_FALSE;
}

JVM_BOOL JvmCatchAllExceptions()
{
  _JVM_Thread_State *pjtsThreadState = JvmGetThreadState();   
  _JVM_Except_Frame *pjefFrame;
  
  /* Validate */
  JVM_ASSERT( pjtsThreadState != NULL );
  /* Set the exception of the top exception frame
     in exception stack */
  pjefFrame = pjtsThreadState->pjefTopFrame;
  JVM_ASSERT( pjefFrame != NULL );

  if ( pjefFrame->ejetException >= MIN_JVMEXCEPTIONVAL  &&
	   pjefFrame->ejetException <= MAX_JVMEXCEPTIONVAL  )
    return JVM_TRUE;

  /* Should never be reached */
  JVM_ASSERT( FALSE );
  
  return JVM_FALSE;
}


eJVMExceptionType JvmGetExceptionType()
{
  _JVM_Thread_State *pjtsThreadState = JvmGetThreadState();   
  _JVM_Except_Frame *pjefFrame;
  
  /* Validate */
  JVM_ASSERT( pjtsThreadState != NULL );
  /* Set the exception of the top exception frame
     in exception stack */
  pjefFrame = pjtsThreadState->pjefTopFrame;
  JVM_ASSERT( pjefFrame != NULL );

  return pjefFrame->ejetException;

}


JVM_INT JvmDefaultExceptHandler(
  IN    eJVMExceptionType ejetException
)
{
	JVM_BUFFER  jbTempBuf = {0};  
   JvmDisplay( JvmStrCat (
			     JvmStrCpy(jbTempBuf,"Uncaught Exception "),  
	             JvmExcToStr(ejetException)
			     )
	           );

  return JVM_TRUE;
}

JVM_VOID JvmThrowJavaException(
  IN     eJVMExceptionType ejetException
)
{
	/** Assumes that the class is loaded **/
	JVM_OBJREF joExcObject;
	register _JVM_Stack_Frame  * pjsfStackFrame = JVM_NULL;
	register _JVM_Method       * pjmCurrMethod      = JVM_NULL;

	_JVM_Thread_State * pjtsThreadState = JvmGetThreadState();   
	_JVM_Class_Class  * pjccExcClass;
	_JVM_Class_Class  * pjccCurrClass;

	
	pjsfStackFrame = pjtsThreadState->pjsfCurrFrame;
	pjmCurrMethod  = pjsfStackFrame->pjmCurrMethod;
	pjccCurrClass = pjtsThreadState->pjsfCurrFrame->pjccCurrClass;

	pjccExcClass = JvmLdrGetExceptionClass( ejetException, pjccCurrClass->joClassLoaderObject);

	JVM_ASSERT_VALID( pjccExcClass );
	JVM_ASSERT_VALID( pjtsThreadState );

    joExcObject     = JvmRtCreateNewInstance( pjccExcClass );
	

	/** Check this stuff lad **/
	if ( pjmCurrMethod->jwMethodAccessFlags & ACC_NATIVE )
	{
#ifndef __JDEPENDS__
		JvmRtDestroyStackFrame();
		pjtsThreadState->pjsfCurrFrame = 
	       pjtsThreadState->pjsfCurrFrame->pjsfPrevFrame;
#endif /* __JDEPENDS__ */
	}

     JvmThrowRealJavaException(  joExcObject, ejetException );
}


/** CHECK,CHECK - code correctness **/
/** DO NOT ASSUME ANYTHING ABOUT THE STATE OF THE THREAD'S OPERAND STACK!! **/
/** IT IS UNDEFINED **/

JVM_VOID JvmThrowRealJavaException( 
  IN     JAVA_OBJREF joExcObject, 
  IN     eJVMExceptionType ejetException 
)
{
	register _JVM_Thread_State * pjtsThreadState = JvmGetThreadState();   
	register  JAVA_WORD jawPCVal = 0;
	register  JVM_WORD jwExcTableCount = 0;
	register _JVM_EXC_Table_Info * pjetiExcTblInfo = JVM_NULL;
	register _JVM_Method       * pjmCurrMethod      = JVM_NULL;
	register _JVM_Class_Class  * pjccCurrClass      = JVM_NULL;
	JVM_WORD  jwCount  = 0;

	JVM_ASSERT_VALID( pjtsThreadState );

	if ( pjtsThreadState->pjsfCurrFrame == JVM_NULL )
	{
		/** Call ThreadGroup.uncaughtException() **/
		JvmExcCallDefaultHandler(joExcObject,ejetException);

		/** Finally kill ourselves **/
		JvmSysExitThread();

		/** Should never be reached **/
		JVM_ASSERT( FALSE );
	}

	jawPCVal = pjtsThreadState->pjsfCurrFrame->jawPC;
	jawPCVal--;
	JVM_ASSERT( jawPCVal != 0);

	pjmCurrMethod = pjtsThreadState->pjsfCurrFrame->pjmCurrMethod;
	pjccCurrClass = pjtsThreadState->pjsfCurrFrame->pjccCurrClass;

	JVM_ASSERT_VALID( pjmCurrMethod );
	JVM_ASSERT_VALID( pjmCurrMethod->pjciCodeInfo );

	jwExcTableCount = pjmCurrMethod->pjciCodeInfo->jwExceptionTableCount;


	if ( jwExcTableCount )
	{
      /** A-Ha! We have handlers **/
        for ( jwCount = 0; jwCount < jwExcTableCount; jwCount++ )
		{
			pjetiExcTblInfo = 
				pjmCurrMethod->pjciCodeInfo->ppjetiExceptionTable[ jwCount ];
			JVM_ASSERT_VALID( pjetiExcTblInfo );
	
			if ( jawPCVal >= pjetiExcTblInfo->jwStartPC &&
			     jawPCVal <= pjetiExcTblInfo->jwEndPC ) 
			{
				if ( ( 0 == pjetiExcTblInfo->jwCatchType ) ||
					 ( IsKindOf(  JvmGetClassName( pjccCurrClass, 
	    			pjetiExcTblInfo->jwCatchType, JVM_NULL  ),
    				(((_JVM_Object *)joExcObject)->joiObjectID.pjccClass)->jsClassName, 
					pjccCurrClass->joClassLoaderObject  )
					 )
				   ) 
				{
					/** Jump with Same Frame retained **/
		    		JvmExcPrepareToExecExcHandler( pjetiExcTblInfo->jwHandlerPC, joExcObject );
					/** Should never come back **/
                    return; 
				}
	
			}
		}
	}


	/** No exception handlers in this method or            **/
	/** None of the handlers handle this kind of exception **/
	/** Restore caller's frame **/
#ifndef __JDEPENDS__
	JvmRtDestroyStackFrame();
	pjtsThreadState->pjsfCurrFrame = 
	       pjtsThreadState->pjsfCurrFrame->pjsfPrevFrame;
#endif /*  __JDEPENDS__ */

    JvmThrowRealJavaException( joExcObject, ejetException );

	JVM_ASSERT( FALSE );
}


JVM_VOID JvmExcPrepareToExecExcHandler( JAVA_WORD jawPC, JAVA_OBJREF joExcObject )
{
  register _JVM_Thread_State * pjtsThreadState = JvmGetThreadState();   
  /** Setup PC */
  pjtsThreadState->pjsfCurrFrame->jawPC = jawPC;

  /** Setup Operand Stack **/
  pjtsThreadState->pjsfCurrFrame->pjawOpStackElements[0] = (JAVA_WORD)joExcObject;
  pjtsThreadState->pjsfCurrFrame->ju32TopElementIndex	   = 0;
  pjtsThreadState->pjeeCurrExecEnv->ju32OpStackTop = 0;

  /** Should continue bytecode execution **/

}


JVM_VOID JvmExcCallDefaultHandler( 
  IN     JAVA_OBJREF joExcObject, 
  IN     eJVMExceptionType ejetException 
)
{
   _JVM_Object * joClassThreadGroup; 
   register _JVM_Class_Class  * pjccCurrClass      = JVM_NULL;
   register _JVM_Thread_State * pjtsThreadState = JvmGetThreadState();   
    _JVM_Object * pjoExceptionObject = joExcObject;
   pjccCurrClass = pjoExceptionObject->joiObjectID.pjccClass;

   if (!JvmLdrLoadNewClass(S_JAVA_LANG_THREADGROUP, JVM_NULL, 
	    pjccCurrClass->joClassLoaderObject, &joClassThreadGroup ))
   {
	   JvmThrowException( ejetException );
	   JVM_ASSERT( JVM_FALSE );
   }
   
  /* JvmRtInvokeMethod(JVM_SPRING_CREATE("UnCaughtException"), 
                     JVM_SPRING_CREATE("(Ljava/lang/Thread;Ljava/lang/Throwable;)V"),
                     joClassObject, JVM_NULL, 0, JVM_NULL, JVM_NULL);      
  */
   
   
   

   /** TODO transfer control to method UnCaughtException of ThreadGroup **/
}



















