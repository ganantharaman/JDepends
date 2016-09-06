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

#ifndef __JVM_EXCEPT_INCLUDE__
#define __JVM_EXCEPT_INCLUDE__

#include <jvmexcmn.h>
#include <jvmrt.h>

#define SETJMP_H "Includes setjmp header"
#include <jvmhdrs.h>

/* Extern functions */
extern JVM_BOOL JvmCatchException( eJVMExceptionType ejetException );
extern JVM_VOID JvmTerminate();
extern JVM_VOID JvmThrowException( eJVMExceptionType ejetException );
extern JVM_BOOL JvmCatchAllExceptions();
extern eJVMExceptionType JvmGetExceptionType();

extern JVM_VOID JvmThrowJavaException( eJVMExceptionType ejetException );
extern JVM_VOID JvmThrowUnhandledException();

JVM_INT JvmDefaultExceptHandler(
  IN    eJVMExceptionType ejetException
);

JVM_BOOL JvmExceptInit(G_STATE_P state );


#define JVM_TRY  \
do {\
   _JVM_Except_Frame jefThisFrame;\
   _JVM_Thread_State *pjtsThreadState = JvmGetThreadState();\
   jefThisFrame.ejetException  = EXC_NOEXCEPTION;\
   jefThisFrame.pjefPrevFrame = pjtsThreadState->pjefTopFrame;\
   pjtsThreadState->pjefTopFrame = &jefThisFrame;\
   if ( setjmp(jefThisFrame.jbCallStack) == 0) {

/* Catches upto three different exceptions 
   Out-of-line implementation of JvmCatchException */ 
#define JVM_CATCH_EXCEPTION(x) \
	} else if (  JvmCatchException(x) ) {

#define JVM_CATCH_TWO_EXCEPTIONS(x,y) \
	} else if (( JvmCatchException(x) ) || (JvmCatchException(y) )) {

#define JVM_CATCH_THREE_EXCEPTIONS(x,y,z) \
	} else if ( JvmCatchException(x) ||\
	         JvmCatchException(y) ||\
			 JvmCatchException(z) ) {

/* All exceptions */
#define JVM_CATCH_ALL_EXCEPTIONS()	\
	} else if ( JvmCatchAllExceptions() ) {

#define JVM_END_TRY \
	} else \
      JvmThrowUnhandledException(); \
 pjtsThreadState->pjefTopFrame = jefThisFrame.pjefPrevFrame;\
}while(0);

#define JVM_CATCH_ALL  JVM_CATCH_ALL_EXCEPTIONS

  
/* Out-of-line implementation of JvmThrowException */         
#define JVM_THROW_EXCEPTION(x)	    JvmThrowException(x);
#define JAVA_THROW_EXCEPTION(x)     JvmThrowJavaException(x);
#define JVM_GET_EXCEPTION_TYPE()    JvmGetExceptionType()

#endif /* __JVM_EXCEPT_INCLUDE__ */