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

/* 
   Dependencies : 1) JVM Data Type header  
                  2) setjmp header    
*/

#ifndef __JVM_EXCEPT_CMN_INCLUDE__
#define __JVM_EXCEPT_CMN_INCLUDE__

#include <jvmmcros.h>

#define  SETJMP_H 	"Include setjmp header"
#include <jvmhdrs.h>

typedef enum eJVMExceptionType{
	EXC_NOEXCEPTION = 0,
	EXC_JVMNOMEMORY,
	EXC_JVMFILEIOERROR,
	EXC_JVMVERSIONNOTSUPPORTED,
	EXC_JVMZEROBYTESPRING,
	EXC_CLASSNOTPRESENT,
	EXC_CLASSINCORRECTSYNTAX,
  EXC_CLASSGENERALERROR,
	EXC_JAVAARITHMETIC,
	EXC_JAVAARRAYSTORE,
	EXC_JAVACLASSCAST,
	EXC_JAVAILLEGALMONITORSTATE,
	EXC_JAVAARRAYINDEXOUTOFBOUNDS,
	EXC_JAVANEGATIVEARRAYSIZE,
	EXC_JAVANULLPOINTER,
	EXC_JAVASECURITY,
	EXC_JAVACLASSFORMATERROR,
	EXC_JAVACLASSCIRCULARITYERROR,
	EXC_JAVANOCLASSDEFFOUNDERROR,
	EXC_JAVASTACKOVERFLOWERROR,
	EXC_JAVAINSTANTIATIONERROR,
	EXC_JAVAILLEGALACCESSERROR,
	EXC_JAVAINCOMPATIBLECLASSCHANGEERROR,
	EXC_JAVAABSTRACTMETHODERROR,
	EXC_JAVAUNSATISFIEDLINKERROR,
	EXC_JAVAVIRTUALMACHINEERROR,
	EXC_ILLEGALACCESSERROR,
	EXC_ALLEXCEPTIONS
}eJVMExceptionType;


typedef JVM_INT (* _JVM_PFN_Default_Except_Handler)( enum eJVMExceptionType );

/* Exception Frame storage goes here */
struct _JVM_Except_Frame
{
	struct _JVM_Except_Frame   * pjefPrevFrame;
	enum eJVMExceptionType   ejetException;
	jmp_buf jbCallStack;
};
typedef struct _JVM_Except_Frame _JVM_Except_Frame;


#endif /* __JVM_EXCEPT_CMN_INCLUDE__*/
