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

#include <jvmrt.h>
#include <jvmmem.h>
#include <jvmsys.h>
#include <jvmexcpt.h>
#include <jvmconfg.h>
#include <jvmmcros.h>
#include <jvmutil.h>
#include <jvmldr.h>
#include <jvmexcmn.h>
#include <jvmrtcmn.h>
#include <jvmsprng.h>

#define JAVA_REF _JVM_Object *
#define JAVA_ARRAYREF _JVM_Array_Object *

#define WORDS_TO_LONG(HI, LO) (((JVM_U64)(HI) << 32)|(JVM_U64)(JVM_ULONG)(LO))


JVM_VOID JvmFieldStoreWordValue( 
 IN	    _JVM_Field * pjfiField, 
 IN      JAVA_WORD   jawWord
 );
 
JVM_VOID JvmFieldStoreLongDoubleValue(
 IN	    _JVM_Field * pjfiField, 
 IN      JAVA_WORD   jawWordHigh,
 IN      JAVA_WORD   jawWordLow
) ;

JVM_BOOL IsStrictValidFieldOf(
 IN      _JVM_Class_Class   * pjccClass,
 IN      JVM_STRING         jsFieldName,
 IN      JVM_STRING         jsFieldDesc, 
 OUT     _JVM_Field        ** ppjfField);
 
 
JVM_BOOL GetFieldOfObject( 
 IN      _JVM_Object * pjaoObject,
 IN      JVM_STRING         jsFieldName,
 IN      JVM_STRING         jsFieldDesc, 
 OUT     _JVM_Field        ** ppjfField
);

JVM_BOOL GetFieldOfClass( 
 IN      _JVM_Class_Class * pjccClass,
 IN      JVM_STRING         jsFieldName,
 IN      JVM_STRING         jsFieldDesc, 
 OUT     _JVM_Field        ** ppjfField
); 

extern JVM_BOOL IsSuperclassOf( 
 IN 	_JVM_Class_Class * pjccS,
 IN     _JVM_Class_Class * pjccT
);

extern JVM_BOOL IsSubclassOf( 
 IN 	_JVM_Class_Class * pjccS,
 IN     _JVM_Class_Class * pjccT
);

extern JVM_BOOL IsInterfaceImplementedBy(
 IN 	_JVM_Class_Class * pjccS,
 IN     _JVM_Class_Class * pjccT
);

extern JVM_BOOL  IsKindOfEx( 
  IN       _JVM_Class_Class * pjccSubjectClass,
  IN       _JVM_Class_Class * pjccTargetClass
);

extern JVM_BOOL HasInterface(
  IN     _JVM_Class_Class * pjccSubjectClass,
  IN     _JVM_Class_Class * pjccTargetClass
);

extern JVM_BOOL JvmLdrGetDeferredException( 
 IN     JVM_STRING jsClassName,
 IN     JVM_OBJREF joClassLoader,
 IN OUT eJVMExceptionType  * pejeException 
);



/**  For reference 
#define JAVA_TYPE_CHAR       0
#define JAVA_TYPE_BYTE       1
#define JAVA_TYPE_SHORT      2
#define JAVA_TYPE_INT        3
#define JAVA_TYPE_LONG       4
#define JAVA_TYPE_FLOAT      5
#define JAVA_TYPE_DOUBLE     6
#define JAVA_TYPE_BOOL       7
**/

/** NOTE :
//////////////////////////////////////////////////////////////////////
/   On Array Implementation 	                                     /
//////////////////////////////////////////////////////////////////////
/   1) All arrays are of type _JVM_Array_Object * 					 /
/   2) First data member of the _JVM_Object_Array is the same as	 /
/      _JVM_Object . So at runtime we could typecast and check		 /
/	  a given pointer value and determine its type info.			 /
/	  So we conform to 'strong typing'								 /
/   3) The second data member is a union of pointer values 			 /
/      to all the different java types. Each of the ptr values		 /
/	  will point to the location of the start array elements 		 /
/	  of appropriate type.											 /
/   4) The ptr that will point to an array of reference types		 /
/      is special. These ptrs will point to a memory location 		 /
/	  whose first element is itself another reference ptr.			 /
/	  This is for ease in >1 dimensional array implementation.		 /
/	  and is also useful for assigment compatibility testing.		 /
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////**/


/** TODO - Pending 
/ 1) WIDE instruction extension for RET
/ 2) IsSamePackage function implementation
**/


JVM_U16 * _ajuGSizes;




extern JVM_INT JvmDefaultExceptHandler(enum eJVMExceptionType ejetException);
extern JVM_BOOL JvmLdrGetDeferredException( 
 IN     JVM_STRING jsClassName,
 IN     JVM_OBJREF joClassLoader,
 IN OUT eJVMExceptionType  * pejeException 
 );
extern JVM_BOOL JvmGetUtf8Info(
  IN   JVM_PBYTE pjbyCPUtf8, 
  OUT  JVM_PBYTE * pjbyUtf8bytes,
  OUT  JVM_WORD  * pjwLen 
);
extern _JVM_Thread_State* _pjtsGStartThread;
extern _JVM_Mutex_Object _jmoGGlobalDataMutex;
extern _JVM_Mutex_Object _jmoGExclCodeMutex;
extern _JVM_Object       _jaoGPrimitiveLoader;


#ifndef __JDEPENDS__

typedef enum ifopenum {
  eq = 0,
  ne,
  lt,
  ge,
  gt,
  le
} ifopenum;

static PFN_BYTECODEHANDLER _apfnGByteCodeHandlers[] =
{
	JvmRtByteCode_nop__Handler,
	JvmRtByteCode_aconst_null__Handler,
	JvmRtByteCode_iconst_n__Handler,
	JvmRtByteCode_iconst_n__Handler,
	JvmRtByteCode_iconst_n__Handler,
	JvmRtByteCode_iconst_n__Handler,
	JvmRtByteCode_iconst_n__Handler,
	JvmRtByteCode_iconst_n__Handler,
	JvmRtByteCode_iconst_n__Handler,
	JvmRtByteCode_lconst_n__Handler,
	JvmRtByteCode_lconst_n__Handler,
	JvmRtByteCode_fconst_n__Handler,
	JvmRtByteCode_fconst_n__Handler,
	JvmRtByteCode_fconst_n__Handler,
	JvmRtByteCode_dconst_n__Handler,
	JvmRtByteCode_dconst_n__Handler,
	JvmRtByteCode_bipush__Handler,
	JvmRtByteCode_sipush__Handler,
	JvmRtByteCode_ldc_common__Handler,
	JvmRtByteCode_ldc_common__Handler,
	JvmRtByteCode_ldc2_w__Handler,
	JvmRtByteCode_ifload__Handler,
	JvmRtByteCode_ldload__Handler,
	JvmRtByteCode_ifload__Handler,
	JvmRtByteCode_ldload__Handler,
	JvmRtByteCode_aload__Handler,
	JvmRtByteCode_iload_n__Handler,
	JvmRtByteCode_iload_n__Handler,
	JvmRtByteCode_iload_n__Handler,
	JvmRtByteCode_iload_n__Handler,
	JvmRtByteCode_lload_n__Handler,
	JvmRtByteCode_lload_n__Handler,
	JvmRtByteCode_lload_n__Handler,
	JvmRtByteCode_lload_n__Handler,
	JvmRtByteCode_fload_n__Handler,
	JvmRtByteCode_fload_n__Handler,
	JvmRtByteCode_fload_n__Handler,
	JvmRtByteCode_fload_n__Handler,
	JvmRtByteCode_dload_n__Handler,
	JvmRtByteCode_dload_n__Handler,
	JvmRtByteCode_dload_n__Handler,
	JvmRtByteCode_dload_n__Handler,
	JvmRtByteCode_aload_n__Handler,
	JvmRtByteCode_aload_n__Handler,
	JvmRtByteCode_aload_n__Handler,
	JvmRtByteCode_aload_n__Handler,
	JvmRtByteCode_iaload__Handler,
	JvmRtByteCode_laload__Handler,
	JvmRtByteCode_faload__Handler,
	JvmRtByteCode_daload__Handler,
	JvmRtByteCode_aaload__Handler,
	JvmRtByteCode_baload__Handler,
	JvmRtByteCode_caload__Handler,
	JvmRtByteCode_saload__Handler,
	JvmRtByteCode_ifstore__Handler,
	JvmRtByteCode_ldstore__Handler,
	JvmRtByteCode_ifstore__Handler,
	JvmRtByteCode_ldstore__Handler,
	JvmRtByteCode_astore__Handler,
	JvmRtByteCode_istore_n__Handler,
	JvmRtByteCode_istore_n__Handler,
	JvmRtByteCode_istore_n__Handler,
	JvmRtByteCode_istore_n__Handler,
	JvmRtByteCode_lstore_n__Handler,
	JvmRtByteCode_lstore_n__Handler,
	JvmRtByteCode_lstore_n__Handler,
	JvmRtByteCode_lstore_n__Handler,
	JvmRtByteCode_fstore_n__Handler,
	JvmRtByteCode_fstore_n__Handler,
	JvmRtByteCode_fstore_n__Handler,
	JvmRtByteCode_fstore_n__Handler,
	JvmRtByteCode_dstore_n__Handler,
	JvmRtByteCode_dstore_n__Handler,
	JvmRtByteCode_dstore_n__Handler,
	JvmRtByteCode_dstore_n__Handler,
	JvmRtByteCode_astore_n__Handler,
	JvmRtByteCode_astore_n__Handler,
	JvmRtByteCode_astore_n__Handler,
	JvmRtByteCode_astore_n__Handler,
	JvmRtByteCode_iastore__Handler,
	JvmRtByteCode_lastore__Handler,
	JvmRtByteCode_fastore__Handler,
	JvmRtByteCode_dastore__Handler,
	JvmRtByteCode_aastore__Handler,
	JvmRtByteCode_bastore__Handler,
	JvmRtByteCode_castore__Handler,
	JvmRtByteCode_sastore__Handler,
	JvmRtByteCode_pop__Handler,
	JvmRtByteCode_pop2__Handler,
	JvmRtByteCode_dup__Handler,
	JvmRtByteCode_dup_x1__Handler,
	JvmRtByteCode_dup_x2__Handler,
	JvmRtByteCode_dup2__Handler,
	JvmRtByteCode_dup2_x1__Handler,
	JvmRtByteCode_dup2_x2__Handler,
	JvmRtByteCode_swap__Handler,
	JvmRtByteCode_iadd__Handler,
	JvmRtByteCode_ladd__Handler,
	JvmRtByteCode_fadd__Handler,
	JvmRtByteCode_dadd__Handler,
	JvmRtByteCode_isub__Handler,
	JvmRtByteCode_lsub__Handler,
	JvmRtByteCode_fsub__Handler,
	JvmRtByteCode_dsub__Handler,
	JvmRtByteCode_imul__Handler,  /** TODO from here **/
	JvmRtByteCode_lmul__Handler,
	JvmRtByteCode_fmul__Handler,
	JvmRtByteCode_dmul__Handler,
	JvmRtByteCode_idiv__Handler,
	JvmRtByteCode_ldiv__Handler,
	JvmRtByteCode_fdiv__Handler,
	JvmRtByteCode_ddiv__Handler,
	JvmRtByteCode_irem__Handler,
	JvmRtByteCode_lrem__Handler,
	JvmRtByteCode_frem__Handler,
	JvmRtByteCode_drem__Handler,
	JvmRtByteCode_ineg__Handler,
	JvmRtByteCode_lneg__Handler,
	JvmRtByteCode_fneg__Handler,
	JvmRtByteCode_dneg__Handler,
	JvmRtByteCode_ishl__Handler,
	JvmRtByteCode_lshl__Handler,
	JvmRtByteCode_ishr__Handler,
	JvmRtByteCode_lshr__Handler,
	JvmRtByteCode_iushr__Handler,
	JvmRtByteCode_lushr__Handler,
	JvmRtByteCode_iand__Handler,
	JvmRtByteCode_land__Handler,
	JvmRtByteCode_ior__Handler,
	JvmRtByteCode_lor__Handler,
	JvmRtByteCode_ixor__Handler,
	JvmRtByteCode_lxor__Handler,
	JvmRtByteCode_iinc__Handler,
	JvmRtByteCode_i2l__Handler,
	JvmRtByteCode_i2f__Handler,
	JvmRtByteCode_i2d__Handler,
	JvmRtByteCode_l2i__Handler,
	JvmRtByteCode_l2f__Handler,
	JvmRtByteCode_l2d__Handler,
	JvmRtByteCode_f2i__Handler,
	JvmRtByteCode_f2l__Handler,
	JvmRtByteCode_f2d__Handler,
	JvmRtByteCode_d2i__Handler,
	JvmRtByteCode_d2l__Handler,
	JvmRtByteCode_d2f__Handler,
	JvmRtByteCode_i2b__Handler,
	JvmRtByteCode_i2c__Handler,
	JvmRtByteCode_i2s__Handler,
	JvmRtByteCode_lcmp__Handler,
	JvmRtByteCode_fcmpl__Handler,
	JvmRtByteCode_fcmpg__Handler,
	JvmRtByteCode_dcmpl__Handler,
	JvmRtByteCode_dcmpg__Handler,
	JvmRtByteCode_ifeq__Handler,
	JvmRtByteCode_ifne__Handler,
	JvmRtByteCode_iflt__Handler,
	JvmRtByteCode_ifge__Handler,
	JvmRtByteCode_ifgt__Handler,
	JvmRtByteCode_ifle__Handler,
	JvmRtByteCode_if_icmpeq__Handler,
	JvmRtByteCode_if_icmpne__Handler,
	JvmRtByteCode_if_icmplt__Handler,
	JvmRtByteCode_if_icmpge__Handler,
	JvmRtByteCode_if_icmpgt__Handler,
	JvmRtByteCode_if_icmple__Handler,
	JvmRtByteCode_if_acmpeq__Handler,
	JvmRtByteCode_if_acmpne__Handler,
	JvmRtByteCode_goto__Handler,
	JvmRtByteCode_jsr__Handler,
	JvmRtByteCode_ret__Handler,
	JvmRtByteCode_tableswitch__Handler,
	JvmRtByteCode_lookupswitch__Handler,
	JvmRtByteCode_ireturn__Handler,
	JvmRtByteCode_lreturn__Handler,
	JvmRtByteCode_freturn__Handler,
	JvmRtByteCode_dreturn__Handler,
	JvmRtByteCode_areturn__Handler,
	JvmRtByteCode_return__Handler,
	JvmRtByteCode_getstatic__Handler,
	JvmRtByteCode_putstatic__Handler,
	JvmRtByteCode_getfield__Handler,
	JvmRtByteCode_putfield__Handler,
	JvmRtByteCode_invokevirtual__Handler,
	JvmRtByteCode_invokespecial__Handler,
  JvmRtByteCode_invokestatic__Handler,
	JvmRtByteCode_invokeinterface__Handler,
	JvmRtByteCode_DUMMY__Handler,
	JvmRtByteCode_new__Handler,
	JvmRtByteCode_newarray__Handler,
	JvmRtByteCode_anewarray__Handler,
	JvmRtByteCode_arraylength__Handler,
	JvmRtByteCode_athrow__Handler,
	JvmRtByteCode_checkcast__Handler,
	JvmRtByteCode_instanceof__Handler,
	JvmRtByteCode_moniterenter__Handler,
	JvmRtByteCode_moniterexit__Handler,
	JvmRtByteCode_wide__Handler,
  JvmRtByteCode_multianewarray__Handler,
	JvmRtByteCode_ifnull__Handler,
	JvmRtByteCode_ifnonnull__Handler,
	JvmRtByteCode_goto_w__Handler,
	JvmRtByteCode_jsr_w__Handler
};

#endif /*  __JDEPENDS__ */

JVM_BOOL JvmRtInit( G_STATE_P state)
{
  if ( IS_OSL_READY( state )  && IS_EXCL_READY( state ))
  {
     _ajuGSizes = JvmMemAlloc( (JAVA_TYPE_BOOL - JAVA_TYPE_BYTE)* sizeof( JVM_U16 ) );
     JVM_ALLOC_CHECK( _ajuGSizes );
     JvmMemSet( _ajuGSizes, 0 , (JAVA_TYPE_BOOL - JAVA_TYPE_BYTE)* sizeof( JVM_U16 ) );

	_ajuGSizes[ JAVA_TYPE_CHAR   - JAVA_TYPE_BYTE] =  sizeof( JAVA_CHAR ); 
	_ajuGSizes[ JAVA_TYPE_SHORT  - JAVA_TYPE_BYTE] =  sizeof( JAVA_SHORT ); 
	_ajuGSizes[ JAVA_TYPE_INT    - JAVA_TYPE_BYTE] =  sizeof( JAVA_INT ); 
	_ajuGSizes[ JAVA_TYPE_LONG   - JAVA_TYPE_BYTE] =  sizeof( JAVA_LONG ); 
	_ajuGSizes[ JAVA_TYPE_FLOAT  - JAVA_TYPE_BYTE] =  sizeof( JAVA_FLOAT ); 
	_ajuGSizes[ JAVA_TYPE_DOUBLE - JAVA_TYPE_BYTE] =  sizeof( JAVA_DOUBLE ); 
	_ajuGSizes[ JAVA_TYPE_REF    - JAVA_TYPE_BYTE] =  sizeof( JAVA_REF ); 

	 RTL_READY( state );
	 return JVM_TRUE;
  }

  return JVM_FALSE;
}


#ifndef __JDEPENDS__


JVM_VOID  JvmRtByteCode_nop__Handler( 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID  JvmRtByteCode_aconst_null__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;

  pjawOpStack[ ++ju32StackTop ] = (JAVA_WORD)JAVA_NULL;

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC++;
  
}

JVM_VOID  JvmRtByteCode_iconst_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  pjawOpStack[ ++ju32StackTop ] = ( *pjbyThisCode  == 0x2 ) ? (JAVA_INT) -1 :
                                    (JAVA_INT)(*pjbyThisCode - 0x3);

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC++;
 
}

JVM_VOID  JvmRtByteCode_lconst_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  pjawOpStack[ ++ju32StackTop ] = 0; 
  /** A small assumption here : if the opcode is not 9 then it is assumed to be 10 **/
  pjawOpStack[ ++ju32StackTop ] =  ( *pjbyThisCode == 0x9 ) ? 0 : 1;
  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC++;

}


JVM_VOID  JvmRtByteCode_fconst_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  pjawOpStack[ ++ju32StackTop ] =  ( *pjbyThisCode == 0xb ) ? (JAVA_WORD)(( JAVA_FLOAT )0.0): 
                                  ( *pjbyThisCode == 0xc ) ? (JAVA_WORD)(( JAVA_FLOAT )1.0):
					                             (JAVA_WORD)(( JAVA_FLOAT )2.0);

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC++;

}


JVM_VOID  JvmRtByteCode_dconst_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  JVM_U16   jiVal = sizeof( JAVA_DOUBLE );
  JAVA_WORD jawWord = JVM_BITS_PER_BYTE * ( jiVal >> 1 );

  ++ju32StackTop;
  if ( *pjbyThisCode == 0xe )
  {
     pjawOpStack[ ju32StackTop ] = (JAVA_WORD)((JVM_U64)(JAVA_DOUBLE)0.0  >> jawWord);
	 pjawOpStack[ ++ju32StackTop ] = (JAVA_WORD)(((JVM_U64)(JAVA_DOUBLE)0.0 << jawWord ) >>  jawWord); 
  }
  else
  {
     pjawOpStack[ ju32StackTop ] = (JAVA_WORD)((JVM_U64)(JAVA_DOUBLE)1.0  >> jawWord);
	 pjawOpStack[ ++ju32StackTop ] = (JAVA_WORD)(((JVM_U64)(JAVA_DOUBLE)0.0 << jawWord) >> jawWord); 
  }

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC++;

}

JVM_VOID  JvmRtByteCode_bipush__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  pjawOpStack[ ++ju32StackTop ] =  ( JAVA_WORD ) *(pjbyThisCode + 1);  

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC += 2;

}


JVM_VOID  JvmRtByteCode_sipush__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop     = pjeeExecEnv->ju32OpStackTop;
  register  JAVA_WORD  * pjawOpStack  = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode = pjeeExecEnv->pjbyCode;
  pjawOpStack[ ++ju32StackTop ] =  (( JAVA_INT )( pjbyThisCode[1] ) << 8 ) | pjbyThisCode[2];  
  pjeeExecEnv->ju32OpStackTop   = 	ju32StackTop;
  pjeeExecEnv->pjsfFrame->jawPC += 3;
}


JVM_VOID  JvmRtByteCode_ldc_common__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register  _JVM_Class_Class *pjccCurrClass = pjeeExecEnv->pjsfFrame->pjccCurrClass;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JVM_U16     ju16CPIndex      = *( pjbyThisCode + 1 ); 
  register  JVM_BYTE     jbyOffset       = 2;

  if (  *pjbyThisCode == 0x13 )
  {
      ju16CPIndex = (( *( pjbyThisCode + 1 ) << 8 ) | *( pjbyThisCode + 2 ));
	  jbyOffset  = 3;
  }

  ++ju32StackTop;

  if ( IsCPElementType( JVM_CP_TAG_INTEGER, pjccCurrClass, ju16CPIndex ) )
  {
	  {
		  JAVA_INT ji ;
          if ( !JvmLdrCVProviderInteger( pjccCurrClass, ju16CPIndex, &ji ) )
			  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

		  pjawOpStack[ ju32StackTop ] = ji;
	  }
  }
  else if  ( IsCPElementType( JVM_CP_TAG_FLOAT, pjccCurrClass, ju16CPIndex ) )
  {
	  {
		  JAVA_FLOAT jf;
		  if ( !JvmLdrCVProviderFloat( pjccCurrClass, ju16CPIndex, &jf ) )
			  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
		  pjawOpStack[ ju32StackTop ] = (JAVA_WORD)jf;
	  }
  }
  else if ( IsCPElementType(  JVM_CP_TAG_STRING, pjccCurrClass, ju16CPIndex ) )
  {
	  {
		  JAVA_OBJREF jo;
		  if ( !JvmLdrCVProviderString( pjccCurrClass, ju16CPIndex, &jo ) )
			  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

		  pjawOpStack[ ju32StackTop ] = (JAVA_WORD)jo;
	  }
  }
  else
  {
	  --ju32StackTop;
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  }

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC += jbyOffset;
}


JVM_VOID  JvmRtByteCode_ldc2_w__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Class_Class *pjccCurrClass = pjeeExecEnv->pjsfFrame->pjccCurrClass;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JVM_U16     ju16CPIndex      = (( *( pjbyThisCode + 1 ) << 8 ) | *( pjbyThisCode + 2 ));

  ++ju32StackTop;

  if ( IsCPElementType( JVM_CP_TAG_LONG, pjccCurrClass, ju16CPIndex ) )
  {
	{
	   JAVA_LONG jl ;
       if ( !JvmLdrCVProviderLong( pjccCurrClass, ju16CPIndex, &jl ) )
		  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

      pjawOpStack[ju32StackTop ] = (JAVA_WORD)(jl  >> (JVM_BITS_PER_BYTE * ( sizeof( JAVA_LONG ) >> 1 ))); 
	  pjawOpStack[++ju32StackTop ] = (JAVA_WORD)((jl << (JVM_BITS_PER_BYTE * ( sizeof( JAVA_LONG ) >> 1 )))
	                                     >> (JVM_BITS_PER_BYTE * ( sizeof( JAVA_LONG ) >> 1 ))); 
	}
  }
  else if ( IsCPElementType( JVM_CP_TAG_DOUBLE, pjccCurrClass, ju16CPIndex ) )
  {
	{
	  JAVA_DOUBLE jd;
	  if ( !JvmLdrCVProviderDouble( pjccCurrClass, ju16CPIndex, &jd ) )
		  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	  pjawOpStack[ ju32StackTop ] = (JAVA_WORD)((JVM_U64)jd  >> (JVM_BITS_PER_BYTE * ( sizeof( JAVA_DOUBLE ) >> 1 ))); 
	  pjawOpStack[ ++ju32StackTop ] = (JAVA_WORD)(((JVM_U64)jd << (JVM_BITS_PER_BYTE * ( sizeof( JAVA_DOUBLE ) >> 1 )))
	                                     >> (JVM_BITS_PER_BYTE * ( sizeof( JAVA_DOUBLE ) >> 1 ))); 
	  
	}
  }

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC += 3;

}


JVM_VOID  JvmRtByteCode_ifload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JVM_U16      ju16Index,jbyIndex1,jbyIndex2;

  if ( *pjbyThisCode == WIDE )
  {
	  jbyIndex1 = * ( pjbyThisCode + 2 );
	  jbyIndex2 = * ( pjbyThisCode + 3 );
	  ju16Index = (( JVM_U16 )jbyIndex1 << 8 ) | jbyIndex2;
  }
  else
  {
	  ju16Index = *( pjbyThisCode + 1 ); 
  }

  pjawOpStack[ ++ju32StackTop ] =  pjsfCurrFrame->pjawLocals[ju16Index];


  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC += ((*pjbyThisCode == WIDE )? 4 : 2);

}


JVM_VOID  JvmRtByteCode_ldload__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JVM_U16      ju16Index,jbyIndex1,jbyIndex2;

  if ( *pjbyThisCode == WIDE )
  {
	  jbyIndex1 = * ( pjbyThisCode + 2 );
	  jbyIndex2 = * ( pjbyThisCode + 3 );
	  ju16Index = (( JVM_U16 )jbyIndex1 << 8 ) | jbyIndex2;
  }
  else
  {
	  ju16Index = *( pjbyThisCode + 1 ); 
  }

  pjawOpStack[ ++ju32StackTop ] = (JAVA_WORD)
	  pjsfCurrFrame->pjawLocals[ ju16Index ];

  pjawOpStack[ ++ju32StackTop ] = (JAVA_WORD)
	  pjsfCurrFrame->pjawLocals[ ju16Index + 1];

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC += ((*pjbyThisCode == WIDE )? 4 : 2);

}


/** * * * * S A S T O R E * * * * **/

JVM_VOID JvmRtByteCode_sastore__Handler(
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
   JvmDisplay("JvmRtByteCode_sastore__Handler Not implemented yet");
   return;
}

/** * * * * B A S T O R E * * * * **/

JVM_VOID JvmRtByteCode_bastore__Handler(
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
   JvmDisplay("JvmRtByteCode_bastore__Handler Not implemented yet");
   return;
}

/** * * * * C A S T O R E * * * * **/

JVM_VOID JvmRtByteCode_castore__Handler(
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
   JvmDisplay("JvmRtByteCode_castore__Handler Not implemented yet");
   return;
}



JVM_VOID  JvmRtByteCode_aload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  register  JVM_U16      ju16Index,jbyIndex1,jbyIndex2;

  if ( *pjbyThisCode == WIDE )
  {
	  jbyIndex1 = * ( pjbyThisCode + 2 );
	  jbyIndex2 = * ( pjbyThisCode + 3 );
	  ju16Index = (( JVM_U16 )jbyIndex1 << 8 ) | jbyIndex2;
  }
  else
  {
	  ju16Index = *( pjbyThisCode + 1 ); 
  }

  if ( pjsfCurrFrame->pju32StackMap ) 
  {
      if ( IS_A_REFERENCE != pjsfCurrFrame->pju32StackMap[ ju16Index ]  )
	    JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  }

  pjawOpStack[ ++ju32StackTop ] = pjsfCurrFrame->pjawLocals[ ju16Index ];

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC +=  ((*pjbyThisCode == WIDE )? 4 : 2);

}

JVM_VOID  JvmRtByteCode_iload_n__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  pjawOpStack[ ++ju32StackTop ] =  pjsfCurrFrame->pjawLocals[ *pjbyThisCode -  ILOAD_0 ];

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;

}


JVM_VOID  JvmRtByteCode_lload_n__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  pjawOpStack[ ++ju32StackTop ] =  pjsfCurrFrame->pjawLocals[ *pjbyThisCode -  LLOAD_0 ];
  pjawOpStack[ ++ju32StackTop ] =  pjsfCurrFrame->pjawLocals[ *pjbyThisCode -  LLOAD_0  + 1];

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;

}


JVM_VOID  JvmRtByteCode_fload_n__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  pjawOpStack[ ++ju32StackTop ] =  pjsfCurrFrame->pjawLocals[ *pjbyThisCode -  FLOAD_0 ];

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;

}

JVM_VOID  JvmRtByteCode_dload_n__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  pjawOpStack[ ++ju32StackTop ] =  pjsfCurrFrame->pjawLocals[ *pjbyThisCode -  DLOAD_0 ];
  pjawOpStack[ ++ju32StackTop ] =  pjsfCurrFrame->pjawLocals[ *pjbyThisCode -  DLOAD_0  + 1];

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;

}


JVM_VOID  JvmRtByteCode_aload_n__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  if ( pjsfCurrFrame->pju32StackMap ) {
    if ( IS_A_REFERENCE != pjsfCurrFrame->pju32StackMap[ (JVM_INT)(*pjbyThisCode) - ALOAD_0 ]  )
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  }

  pjawOpStack[++ju32StackTop] = pjsfCurrFrame->pjawLocals[*pjbyThisCode - ALOAD_0];

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;

}

JVM_VOID  JvmRtByteCode_iaload__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawIndex,jawArrayRef;

  
  jawIndex    =  pjawOpStack[ ju32StackTop-- ];
  jawArrayRef =  pjawOpStack[ ju32StackTop ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjaiInt == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  pjawOpStack[ ju32StackTop ] =  
	  *((((_JVM_Array_Object *)jawArrayRef)->u.pjaiInt) + jawIndex);

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
}

JVM_VOID  JvmRtByteCode_laload__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawIndex,jawArrayRef;

  jawIndex    =  pjawOpStack[ ju32StackTop-- ];
  jawArrayRef =  pjawOpStack[ ju32StackTop ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjalLong == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  pjawOpStack[ju32StackTop] =  
	  HI_JWORD( *((((_JVM_Array_Object *)jawArrayRef)->u.pjalLong) + jawIndex));
  pjawOpStack[ ++ju32StackTop] =  
	  LO_JWORD( *((((_JVM_Array_Object *)jawArrayRef)->u.pjalLong) + jawIndex));


  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
}


JVM_VOID  JvmRtByteCode_faload__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawIndex,jawArrayRef;

  jawIndex    =  pjawOpStack[ ju32StackTop-- ];
  jawArrayRef =  pjawOpStack[ ju32StackTop ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjafFloat == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  pjawOpStack[ ju32StackTop] =(JAVA_WORD)  
	  *((((_JVM_Array_Object *)jawArrayRef)->u.pjafFloat) + jawIndex);

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
}


JVM_VOID  JvmRtByteCode_daload__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawIndex,jawArrayRef;

  jawIndex    =  pjawOpStack[ ju32StackTop-- ];
  jawArrayRef =  pjawOpStack[ ju32StackTop ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjarRef == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  pjawOpStack[ ju32StackTop] =  
	  HI32( *((((_JVM_Array_Object *)jawArrayRef)->u.pjadDouble) + jawIndex));
  pjawOpStack[ ++ju32StackTop] =  
	  LO32( *((((_JVM_Array_Object *)jawArrayRef)->u.pjadDouble) + jawIndex));


  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
}


JVM_VOID  JvmRtByteCode_aaload__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawIndex,jawArrayRef;

  jawIndex    =  pjawOpStack[ ju32StackTop-- ];
  jawArrayRef =  pjawOpStack[ ju32StackTop ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjarRef == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  if ( ((_JVM_Object_ID *)(((_JVM_Array_Object *)jawArrayRef)->u.pjarRef[0]))->
	    jbyID == REF_TYPE_ARRAY )
  {
    pjawOpStack[ ju32StackTop] =  (JAVA_WORD)
	  *((_JVM_Array_Object **)(((_JVM_Array_Object *)jawArrayRef)->u.pjarRef) + jawIndex);
  }
  else if ( ((_JVM_Object_ID *)(((_JVM_Array_Object *)jawArrayRef)->u.pjarRef[0]))->
	    jbyID == REF_TYPE_CLASS )
  {
    pjawOpStack[ ju32StackTop ] = (JAVA_WORD) 
	  *((_JVM_Array_Object **)(((_JVM_Array_Object *)jawArrayRef)->u.pjarRef) + jawIndex);
  }
  else
  {
	 /* Invalid Array reference */
	 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  }

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
}


JVM_VOID  JvmRtByteCode_baload__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawIndex,jawArrayRef;

  jawIndex    =  pjawOpStack[ ju32StackTop-- ];
  jawArrayRef =  pjawOpStack[ ju32StackTop ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjabyByte == JVM_NULL ))
	 JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  pjawOpStack[ ju32StackTop] = 
	   (JAVA_INT)(*(JAVA_BYTE *)((((_JVM_Array_Object *)jawArrayRef)->u.pjabyByte) + jawIndex));

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
}


JVM_VOID  JvmRtByteCode_caload__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawIndex,jawArrayRef;

  jawIndex    =  pjawOpStack[ ju32StackTop-- ];
  jawArrayRef =  pjawOpStack[ ju32StackTop ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjacChar == JVM_NULL ))
	 JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  pjawOpStack[ ju32StackTop ] = ((_JVM_Array_Object *)jawArrayRef)->u.pjacChar[ jawIndex ];

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
}


JVM_VOID  JvmRtByteCode_saload__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawIndex,jawArrayRef;

  jawIndex    =  pjawOpStack[ ju32StackTop-- ];
  jawArrayRef =  pjawOpStack[ ju32StackTop  ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjasShort == JVM_NULL ))
	 JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  pjawOpStack[ ju32StackTop ] = 
	   ((((_JVM_Array_Object *)jawArrayRef)->u.pjasShort)[jawIndex]);

  pjeeExecEnv->ju32OpStackTop = 	ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
}


JVM_VOID  JvmRtByteCode_ifstore__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
    register  JVM_U16      ju16Index,jbyIndex1,jbyIndex2;

  if ( *pjbyThisCode == WIDE )
  {
	  jbyIndex1 = * ( pjbyThisCode + 2 );
	  jbyIndex2 = * ( pjbyThisCode + 3 );
	  ju16Index = (( JVM_U16 )jbyIndex1 << 8 ) | jbyIndex2;
  }
  else
  {
	  ju16Index = *( pjbyThisCode + 1 ); 
  }
  
  pjsfCurrFrame->pjawLocals[ ju16Index ] = 
	  pjawOpStack[ ju32StackTop-- ];

  pjeeExecEnv->ju32OpStackTop = ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC += ((*pjbyThisCode == WIDE )? 4 : 2);
   
}


JVM_VOID  JvmRtByteCode_ldstore__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JVM_U16      ju16Index,jbyIndex1,jbyIndex2;

  if ( *pjbyThisCode == WIDE )
  {
	  jbyIndex1 = * ( pjbyThisCode + 2 );
	  jbyIndex2 = * ( pjbyThisCode + 3 );
	  ju16Index = (( JVM_U16 )jbyIndex1 << 8 ) | jbyIndex2;
  }
  else
  {
	  ju16Index = *( pjbyThisCode + 1 ); 
  }

  
  pjsfCurrFrame->pjawLocals[ ju16Index + 1] = 
	  pjawOpStack[ ju32StackTop-- ];
  pjsfCurrFrame->pjawLocals[ ju16Index ] = 
	  pjawOpStack[ ju32StackTop-- ];

  pjeeExecEnv->ju32OpStackTop = ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC += ((*pjbyThisCode == WIDE )? 4 : 2);
   
}


JVM_VOID  JvmRtByteCode_astore__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JVM_U16      ju16Index,jbyIndex1,jbyIndex2;

  if ( *pjbyThisCode == WIDE )
  {
	  jbyIndex1 = * ( pjbyThisCode + 2 );
	  jbyIndex2 = * ( pjbyThisCode + 3 );
	  ju16Index = (( JVM_U16 )jbyIndex1 << 8 ) | jbyIndex2;
  }
  else
  {
	  ju16Index = *( pjbyThisCode + 1 ); 
  }

  /** Check the destination type **/
  if ( pjsfCurrFrame->pju32StackMap ) {
    if ( IS_A_REFERENCE != pjsfCurrFrame->pju32StackMap[ ju16Index ]  )
	   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  }
    
  pjsfCurrFrame->pjawLocals[ ju16Index ] = 
	  pjawOpStack[ ju32StackTop-- ];
  
  pjeeExecEnv->ju32OpStackTop = ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC += ((*pjbyThisCode == WIDE )? 4 : 2);
   
}

JVM_VOID  JvmRtByteCode_istore_n__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  
  pjsfCurrFrame->pjawLocals[ *( pjbyThisCode + 1 ) - ISTORE_0 ] = 
	  pjawOpStack[ ju32StackTop-- ];

  pjeeExecEnv->ju32OpStackTop = ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
   
}

JVM_VOID  JvmRtByteCode_lstore_n__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  
  pjsfCurrFrame->pjawLocals[ *( pjbyThisCode + 1 ) - LSTORE_0 + 1] = 
	  pjawOpStack[ ju32StackTop-- ];
  pjsfCurrFrame->pjawLocals[ *( pjbyThisCode + 1 ) - LSTORE_0 ] = 
	  pjawOpStack[ ju32StackTop-- ];

  pjeeExecEnv->ju32OpStackTop = ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
   
}


JVM_VOID  JvmRtByteCode_fstore_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  
  pjsfCurrFrame->pjawLocals[ *( pjbyThisCode + 1 ) - FSTORE_0 ] = 
	  pjawOpStack[ ju32StackTop-- ];

  pjeeExecEnv->ju32OpStackTop = ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
   
}


JVM_VOID  JvmRtByteCode_dstore_n__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  
  pjsfCurrFrame->pjawLocals[ *( pjbyThisCode + 1 ) - DSTORE_0 + 1] = 
	  pjawOpStack[ ju32StackTop-- ];
  pjsfCurrFrame->pjawLocals[ *( pjbyThisCode + 1 ) - DSTORE_0 ] = 
	  pjawOpStack[ ju32StackTop-- ];
  
  pjeeExecEnv->ju32OpStackTop = ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
   
}


JVM_VOID  JvmRtByteCode_astore_n__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;

  /** Check the destination type **/
  if ( IS_A_REFERENCE != pjsfCurrFrame->pju32StackMap[ *( pjbyThisCode + 1 ) - ASTORE_0 ]  )
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  
  pjsfCurrFrame->pjawLocals[ *( pjbyThisCode + 1 ) - ASTORE_0 ] = 
  pjawOpStack[ ju32StackTop-- ];
    
  pjeeExecEnv->ju32OpStackTop = ju32StackTop;

  pjeeExecEnv->pjsfFrame->jawPC ++;
   
}


JVM_VOID  JvmRtByteCode_iastore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawArrayRef,jawIndex; 

  jawArrayRef = pjawOpStack[ ju32StackTop - 2 ];
  jawIndex    = pjawOpStack[ ju32StackTop - 1 ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjaiInt == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

    
  (((_JVM_Array_Object *)jawArrayRef)->u.pjaiInt)[jawIndex] 
	= (JAVA_INT)pjawOpStack[ ju32StackTop ];
  
  pjeeExecEnv->ju32OpStackTop =  (ju32StackTop -= 3);

  pjeeExecEnv->pjsfFrame->jawPC ++;
   
}

JVM_VOID  JvmRtByteCode_lastore__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop   = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawArrayRef,jawIndex; 

  jawArrayRef = pjawOpStack[ ju32StackTop - 3 ];
  jawIndex    = pjawOpStack[ ju32StackTop - 2 ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjalLong == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  *((((_JVM_Array_Object *)jawArrayRef)->u.pjalLong) + jawIndex)
	= MAKE_64( pjawOpStack[ ju32StackTop - 1], pjawOpStack[ ju32StackTop ] );
  
  pjeeExecEnv->ju32OpStackTop =  (ju32StackTop -= 4);

  pjeeExecEnv->pjsfFrame->jawPC ++;
   
}

JVM_VOID  JvmRtByteCode_fastore__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawArrayRef,jawIndex; 

  jawArrayRef = pjawOpStack[ ju32StackTop - 2 ];
  jawIndex    = pjawOpStack[ ju32StackTop - 1 ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjafFloat == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  *((((_JVM_Array_Object *)jawArrayRef)->u.pjafFloat) + jawIndex)
	= (JAVA_FLOAT)pjawOpStack[ ju32StackTop ];
  
  pjeeExecEnv->ju32OpStackTop =  (ju32StackTop -= 3);

  pjeeExecEnv->pjsfFrame->jawPC ++;
   
}


JVM_VOID  JvmRtByteCode_dastore__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawArrayRef,jawIndex; 

  jawArrayRef = pjawOpStack[ ju32StackTop - 3 ];
  jawIndex    = pjawOpStack[ ju32StackTop - 2 ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) || 
	 (((_JVM_Array_Object *)jawArrayRef)->u.pjadDouble == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

    
  ((_JVM_Array_Object *)jawArrayRef)->u.pjadDouble[jawIndex]
	= (JAVA_DOUBLE)(JAVA_LONG)MAKE_64( pjawOpStack[ ju32StackTop - 1], pjawOpStack[ ju32StackTop ] );
  
  pjeeExecEnv->ju32OpStackTop =  (ju32StackTop -= 4);

  pjeeExecEnv->pjsfFrame->jawPC ++;
   
}


JVM_VOID  JvmRtByteCode_aastore__Handler( 
  
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
  register _JVM_Array_Object * pjaoArrayObject = JVM_NULL;
  register  JAVA_WORD    jawArrayRef,jawIndex,jawValue;

  /* S */jawValue = pjawOpStack[ ju32StackTop ];
  jawIndex = pjawOpStack[ ju32StackTop - 1 ];
 /* T*/ jawArrayRef = pjawOpStack[ ju32StackTop - 2 ];

  if (( JAVA_NULL == (JAVA_REF)jawArrayRef ) ||
	  ( ((_JVM_Array_Object *)jawArrayRef)->u.pjarRef == JVM_NULL ))
	  JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

  if ( jawIndex < ((_JVM_Array_Object *)jawArrayRef)->jawLowerBound  ||
	   jawIndex > ((_JVM_Array_Object *)jawArrayRef)->jawUpperBound )
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYINDEXOUTOFBOUNDS );

  if ( !JvmMemIsValidHeapPtr( (JVM_PVOID)jawValue ) || !IsReference( (JVM_PVOID)jawValue ) )
	 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR);

  if ( !IsAssignmentCompatible( (JAVA_OBJREF)jawValue, ((_JVM_Array_Object *)jawArrayRef)->u.pjarRef ))
	 JVM_THROW_EXCEPTION( EXC_JAVAARRAYSTORE );

  ((_JVM_Array_Object *)jawArrayRef)->u.pjarRef[1 + jawIndex ] = (JVM_PVOID)jawValue;

  pjeeExecEnv->ju32OpStackTop =  ju32StackTop - 3;

  pjeeExecEnv->pjsfFrame->jawPC ++;
  
}


/** Stopped at the 'store's  may have to re-write portions **/
/** Starting from pop ( 0x87 ) **/


JVM_VOID JvmRtByteCode_pop__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  pjeeExecEnv->ju32OpStackTop --;
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_pop2__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  pjeeExecEnv->ju32OpStackTop -= 2;
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_dup__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_WORD    jawValue;
  jawValue = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];

  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop) ] = jawValue;
  pjeeExecEnv->pjsfFrame->jawPC++;
 
}

JVM_VOID JvmRtByteCode_dup_x1__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_WORD    jawWord1,jawWord2;
  jawWord1 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jawWord2 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];

  pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1] = jawWord1;
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop ]	= jawWord2;
  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop)]	= jawWord1;
  pjeeExecEnv->pjsfFrame->jawPC++;

}

JVM_VOID JvmRtByteCode_dup_x2__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_WORD    jawWord1,jawWord2,jawWord3;

  jawWord1 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jawWord2 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];
  jawWord3 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2];


  pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2] = jawWord1;
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1]	= jawWord3;
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop]	    = jawWord2;
  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop)] = jawWord1;
  pjeeExecEnv->pjsfFrame->jawPC++;

}

JVM_VOID JvmRtByteCode_dup2__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_WORD    jawWord1,jawWord2;

  jawWord1 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jawWord2 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];
  
  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop)]	= jawWord2;
  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop)]	= jawWord1;
  pjeeExecEnv->pjsfFrame->jawPC++;

}


JVM_VOID JvmRtByteCode_dup2_x1__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_WORD    jawWord1,jawWord2,jawWord3;

  jawWord1 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jawWord2 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];
  jawWord3 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2];
  
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2]	= jawWord2;
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1]	= jawWord1;
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop ]	= jawWord3;
  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop)]	= jawWord2;
  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop)]	= jawWord1;

  pjeeExecEnv->pjsfFrame->jawPC++;

}


JVM_VOID JvmRtByteCode_dup2_x2__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_WORD    jawWord1,jawWord2,jawWord3,jawWord4;

  jawWord1 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jawWord2 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];
  jawWord3 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2];
  jawWord4 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 3];
  
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 3]	= jawWord2;
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2]	= jawWord1;
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1]	= jawWord4;
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop ]	= jawWord3;
  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop)]	= jawWord2;
  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop)]	= jawWord1;

  pjeeExecEnv->pjsfFrame->jawPC++;

}

JVM_VOID JvmRtByteCode_swap__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_WORD    jawWord1,jawWord2;

  jawWord1 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jawWord2 = pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];

  pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1] = jawWord1;
  pjawOpStack[ pjeeExecEnv->ju32OpStackTop ]    = jawWord2;

  pjeeExecEnv->pjsfFrame->jawPC++;

}


JVM_VOID JvmRtByteCode_iadd__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_INT    jaiValue1,jaiValue2;

  jaiValue1 = (JAVA_INT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jaiValue2 = (JAVA_INT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];

  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  (JAVA_WORD)(jaiValue1 + jaiValue2);

  pjeeExecEnv->pjsfFrame->jawPC++;
}




JVM_VOID JvmRtByteCode_ladd__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_LONG   jalValue1,jalValue2;

  jalValue2 = ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1 ]) << 
	           ( JVM_BITS_PER_BYTE * sizeof( JAVA_WORD )))
			   | ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop ]));

  jalValue1 = ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 3 ]) << 
	           ( JVM_BITS_PER_BYTE * sizeof( JAVA_WORD )))
			   | ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2]));

  jalValue1 += jalValue2;

  --(pjeeExecEnv->ju32OpStackTop);
  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  LO_JWORD(jalValue1);
  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  HI_JWORD(jalValue1);

  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_dadd__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_LONG   jalValue1,jalValue2;

  jalValue2 = ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1 ]) << 
	           ( JVM_BITS_PER_BYTE * sizeof( JAVA_WORD )))
			   | ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop ]));

  jalValue1 = ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 3 ]) << 
	           ( JVM_BITS_PER_BYTE * sizeof( JAVA_WORD )))
			   | ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2]));

  if ( jalValue1 == IEEE754_D_NaN &&
	   jalValue2 == IEEE754_D_NaN )
  {
	  jalValue1 =  IEEE754_D_NaN;
  }
  else if ((jalValue1 == IEEE754_D_POS_INFINITY &&
	        jalValue2 == IEEE754_D_NEG_INFINITY ) ||
		   (jalValue1 == IEEE754_D_NEG_INFINITY &&
	        jalValue2 == IEEE754_D_POS_INFINITY	))
  {
	 jalValue1 = IEEE754_D_NaN;
  }
  else if (jalValue1 == IEEE754_D_POS_INFINITY &&
           jalValue2 == IEEE754_D_POS_INFINITY)
  {
	 jalValue1 = IEEE754_D_POS_INFINITY;
  }
  else if ( jalValue1 == IEEE754_D_NEG_INFINITY &&
	        jalValue2 == IEEE754_D_NEG_INFINITY )
  {
	  jalValue1 = IEEE754_D_NEG_INFINITY ;
  }
  else if ( jalValue1 == IEEE754_D_POS_INFINITY  ||
	      	jalValue2 == IEEE754_D_POS_INFINITY )
  {
	  jalValue1 = IEEE754_D_POS_INFINITY;
  }
  else if ( jalValue1 == IEEE754_D_NEG_INFINITY  ||
	      	jalValue2 == IEEE754_D_NEG_INFINITY )
  {
	  jalValue1 = IEEE754_D_NEG_INFINITY;
  }
  else
  {
	  (JAVA_DOUBLE)jalValue1 += jalValue2;
  }

  
  --(pjeeExecEnv->ju32OpStackTop);
  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  LO_JWORD(jalValue1);
  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  HI_JWORD(jalValue1);

  pjeeExecEnv->pjsfFrame->jawPC++;
}


JVM_VOID JvmRtByteCode_fadd__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  JAVA_FLOAT    jafValue1,jafValue2;

  jafValue1 = (JAVA_FLOAT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jafValue2 = (JAVA_FLOAT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];

  if ( jafValue1 == IEEE754_F_NaN &&
	   jafValue2 == IEEE754_F_NaN )
  {
	  jafValue1 =  (JAVA_FLOAT)IEEE754_F_NaN;
  }
  else if ((jafValue1 == IEEE754_F_POS_INFINITY &&
	        jafValue2 == IEEE754_F_NEG_INFINITY ) ||
		   (jafValue1 == IEEE754_F_NEG_INFINITY &&
	        jafValue2 == IEEE754_F_POS_INFINITY	))
  {
	 jafValue1 = (JAVA_FLOAT)IEEE754_D_NaN;
  }
  else if (jafValue1 == IEEE754_F_POS_INFINITY &&
           jafValue2 == IEEE754_F_POS_INFINITY)
  {
	 jafValue1 = IEEE754_F_POS_INFINITY;
  }
  else if ( jafValue1 == IEEE754_F_NEG_INFINITY &&
	        jafValue2 == IEEE754_F_NEG_INFINITY )
  {
	  jafValue1 = IEEE754_F_NEG_INFINITY;
  }
  else if ( jafValue1 == IEEE754_F_POS_INFINITY  ||
	      	jafValue2 == IEEE754_F_POS_INFINITY )
  {
	  jafValue1 = IEEE754_F_POS_INFINITY;
  }
  else if ( jafValue1 == IEEE754_F_NEG_INFINITY  ||
	      	jafValue2 == IEEE754_F_NEG_INFINITY )
  {
	  jafValue1 = IEEE754_F_NEG_INFINITY;
  }
  else
  {
	  (JAVA_FLOAT)jafValue1 += jafValue2;
  }

  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  (JAVA_WORD)jafValue1;

  pjeeExecEnv->pjsfFrame->jawPC++;
}


JVM_VOID JvmRtByteCode_isub__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_INT    jaiValue1,jaiValue2;

  jaiValue2 = (JAVA_INT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jaiValue1 = (JAVA_INT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];

  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  (JAVA_WORD)(jaiValue1 - jaiValue2);

  pjeeExecEnv->pjsfFrame->jawPC++;
}





JVM_VOID JvmRtByteCode_lsub__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_LONG   jalValue1,jalValue2;

  jalValue2 = ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1 ]) << 
	           ( JVM_BITS_PER_BYTE * sizeof( JAVA_WORD )))
			   | ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop ]));

  jalValue1 = ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 3 ]) << 
	           ( JVM_BITS_PER_BYTE * sizeof( JAVA_WORD )))
			   | ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2]));

  jalValue1 -= jalValue2;

  --(pjeeExecEnv->ju32OpStackTop);
  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  LO_JWORD(jalValue1);
  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  HI_JWORD(jalValue1);

  pjeeExecEnv->pjsfFrame->jawPC++;

}


JVM_VOID JvmRtByteCode_fsub__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_FLOAT    jafValue1,jafValue2;

  jafValue2 = (JAVA_FLOAT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  jafValue1 = (JAVA_FLOAT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];

  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  (JAVA_WORD)(jafValue1 - jafValue2);

  pjeeExecEnv->pjsfFrame->jawPC++;
}


JVM_VOID JvmRtByteCode_dsub__Handler(
 
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_DOUBLE   jadValue1,jadValue2;

  jadValue2 = ( JAVA_DOUBLE)(((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1 ]) << 
	           ( JVM_BITS_PER_BYTE * sizeof( JAVA_WORD )))
			   | ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop ])));

  jadValue1 = (JAVA_DOUBLE)(((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 3 ]) << 
	           ( JVM_BITS_PER_BYTE * sizeof( JAVA_WORD )))
			   | ((JAVA_LONG)(pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 2])));

  jadValue1 -= jadValue2;

  --(pjeeExecEnv->ju32OpStackTop);
  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  LO_JWORD((JAVA_LONG)jadValue1);
  pjawOpStack[ --(pjeeExecEnv->ju32OpStackTop) ] =  HI_JWORD((JAVA_LONG)jadValue1);

  pjeeExecEnv->pjsfFrame->jawPC++;

}


JVM_VOID JvmRtByteCode_imul__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_lmul__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}


JVM_VOID JvmRtByteCode_fmul__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_dmul__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_idiv__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_ldiv__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_fdiv__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_ddiv__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_irem__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_lrem__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_frem__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_drem__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_ineg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_lneg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_fneg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_dneg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_ishl__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_lshl__Handler(
  IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_ishr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_lshr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_iushr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_lushr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_iand__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_land__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_ior__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_lor__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_ixor__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_lxor__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}


JVM_VOID JvmRtByteCode_i2l__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register JAVA_INT jaiValue = pjawOpStack[pjeeExecEnv->ju32OpStackTop];
  
  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = (JAVA_WORD)0L;
  pjawOpStack[++pjeeExecEnv->ju32OpStackTop] = (JAVA_WORD)jaiValue;
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_i2f__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
   pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_i2d__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
  

}

JVM_VOID JvmRtByteCode_l2i__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_l2f__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_l2d__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_f2i__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_f2l__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_f2d__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_d2i__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_d2l__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_d2f__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_i2b__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register JAVA_INT jaiValue = pjawOpStack[pjeeExecEnv->ju32OpStackTop];
  
  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = (JAVA_WORD)(JAVA_BYTE)jaiValue;
  pjeeExecEnv->pjsfFrame->jawPC++;
}


JVM_VOID JvmRtByteCode_i2c__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register JAVA_INT jaiValue = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  
  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = (JAVA_WORD)(JAVA_CHAR)jaiValue;
  pjeeExecEnv->pjsfFrame->jawPC++;
 
}

JVM_VOID JvmRtByteCode_i2s__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register JAVA_INT jaiValue = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];
  
  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = (JAVA_WORD)(JAVA_SHORT)jaiValue;
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_lcmp__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_INT    jaiValueLow,jaiValueHigh;
  register  JAVA_LONG   jalValue1,jalValue2;

  jaiValueLow = (JAVA_INT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop-- ];
  jaiValueHigh = (JAVA_INT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop--];
  
  jalValue2 =  WORDS_TO_LONG(jaiValueHigh, jaiValueLow);

  jaiValueLow = (JAVA_INT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop-- ];
  jaiValueHigh = (JAVA_INT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop--];
  
  jalValue1 =  WORDS_TO_LONG(jaiValueHigh, jaiValueLow);
  
  if(jalValue1 > jalValue2) {
    pjawOpStack[ ++pjeeExecEnv->ju32OpStackTop ] = (JAVA_INT)1;
  } else if ( jalValue1 == jalValue2) {
    pjawOpStack[ ++pjeeExecEnv->ju32OpStackTop ] = (JAVA_INT)0;
  }else {
    pjawOpStack[ ++pjeeExecEnv->ju32OpStackTop ] = (JAVA_INT)-1;
  }
  
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_fcmpl__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_FLOAT    jafValue1,jafValue2;
  
  jafValue2 = (JAVA_FLOAT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop];
  jafValue1 = (JAVA_FLOAT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];
  pjeeExecEnv->ju32OpStackTop--;
  
  if ( jafValue1 == IEEE754_F_NaN ||
	   jafValue2 == IEEE754_F_NaN )
  {
	  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
  }
  else if ((jafValue1 == IEEE754_F_POS_INFINITY &&
	          jafValue2 == IEEE754_F_NEG_INFINITY )) {
     pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
  } else if((jafValue1 == IEEE754_F_NEG_INFINITY &&
	        jafValue2 == IEEE754_F_POS_INFINITY	)) {
	   pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
  } else if (jafValue1 == IEEE754_F_POS_INFINITY &&
             jafValue2 == IEEE754_F_POS_INFINITY) {
	   pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
  } else if ( jafValue1 == IEEE754_F_NEG_INFINITY &&
	            jafValue2 == IEEE754_F_NEG_INFINITY ) {
	  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
  } else {
      if(jafValue1 > jafValue2) {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
      } else if(jafValue1 == jafValue2) {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
      } else {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
      }
  }
  
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_fcmpg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_FLOAT    jafValue1,jafValue2;
  
  jafValue2 = (JAVA_FLOAT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop];
  jafValue1 = (JAVA_FLOAT) pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];
  pjeeExecEnv->ju32OpStackTop--;
  
  if ( jafValue1 == IEEE754_F_NaN ||
	   jafValue2 == IEEE754_F_NaN )
  {
	  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
  }
  else if ((jafValue1 == IEEE754_F_POS_INFINITY &&
	          jafValue2 == IEEE754_F_NEG_INFINITY )) {
     pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
  } else if((jafValue1 == IEEE754_F_NEG_INFINITY &&
	        jafValue2 == IEEE754_F_POS_INFINITY	)) {
	   pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
  } else if (jafValue1 == IEEE754_F_POS_INFINITY &&
             jafValue2 == IEEE754_F_POS_INFINITY) {
	   pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
  } else if ( jafValue1 == IEEE754_F_NEG_INFINITY &&
	            jafValue2 == IEEE754_F_NEG_INFINITY ) {
	  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
  } else {
      if(jafValue1 > jafValue2) {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
      } else if(jafValue1 == jafValue2) {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
      } else {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
      }
  }
  
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_dcmpl__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_DOUBLE    jadValue1,jadValue2;
  
  jadValue2 = (JAVA_DOUBLE) pjawOpStack[ pjeeExecEnv->ju32OpStackTop];
  jadValue1 = (JAVA_DOUBLE) pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];
  pjeeExecEnv->ju32OpStackTop--;
  
  if ( jadValue1 == IEEE754_D_NaN ||
	   jadValue2 == IEEE754_D_NaN )
  {
	  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
  }
  else if ((jadValue1 == IEEE754_D_POS_INFINITY &&
	          jadValue2 == IEEE754_D_NEG_INFINITY )) {
     pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
  } else if((jadValue1 == IEEE754_D_NEG_INFINITY &&
	        jadValue2 == IEEE754_D_POS_INFINITY	)) {
	   pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
  } else if (jadValue1 == IEEE754_D_POS_INFINITY &&
             jadValue2 == IEEE754_D_POS_INFINITY) {
	   pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
  } else if ( jadValue1 == IEEE754_D_NEG_INFINITY &&
	            jadValue2 == IEEE754_D_NEG_INFINITY ) {
	  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
  } else {
      if(jadValue1 > jadValue2) {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
      } else if(jadValue1 == jadValue2) {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
      } else {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
      }
  }
  
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID JvmRtByteCode_dcmpg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JAVA_DOUBLE    jadValue1,jadValue2;
  
  jadValue2 = (JAVA_DOUBLE) pjawOpStack[ pjeeExecEnv->ju32OpStackTop];
  jadValue1 = (JAVA_DOUBLE) pjawOpStack[ pjeeExecEnv->ju32OpStackTop - 1];
  pjeeExecEnv->ju32OpStackTop--;
  
  if ( jadValue1 == IEEE754_D_NaN ||
	   jadValue2 == IEEE754_D_NaN )
  {
	  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
  }
  else if ((jadValue1 == IEEE754_D_POS_INFINITY &&
	          jadValue2 == IEEE754_D_NEG_INFINITY )) {
     pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
  } else if((jadValue1 == IEEE754_D_NEG_INFINITY &&
	        jadValue2 == IEEE754_D_POS_INFINITY	)) {
	   pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
  } else if (jadValue1 == IEEE754_D_POS_INFINITY &&
             jadValue2 == IEEE754_D_POS_INFINITY) {
	   pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
  } else if ( jadValue1 == IEEE754_D_NEG_INFINITY &&
	            jadValue2 == IEEE754_D_NEG_INFINITY ) {
	  pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
  } else {
      if(jadValue1 > jadValue2) {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 1;
      } else if(jadValue1 == jadValue2) {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = 0;
      } else {
        pjawOpStack[pjeeExecEnv->ju32OpStackTop] = -1;
      }
  }
  
  pjeeExecEnv->pjsfFrame->jawPC++;
}

JVM_VOID ifopcode(
IN  OUT   _JVM_Exec_Env * pjeeExecEnv, ifopenum enVal) {
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;  
  JAVA_INT jaiValue;
 
  jaiValue = (JAVA_INT) pjawOpStack[pjeeExecEnv->ju32OpStackTop--];  
  if(((jaiValue == 0) && (enVal == eq)) || 
    ((jaiValue != 0) && (enVal == ne))  ||
    ((jaiValue < 0) && (enVal == lt))   ||
    ((jaiValue <= 0) && (enVal == le))  ||
    ((jaiValue > 0) && (enVal == gt)) ||
    ((jaiValue >= 0) && (enVal == ge))) {
     pjeeExecEnv->pjsfFrame->jawPC += 
      ((pjeeExecEnv->pjbyCode[1] << 8) | pjeeExecEnv->pjbyCode[2]);
  } else {
     pjeeExecEnv->pjsfFrame->jawPC += 3;
  }
}

JVM_VOID ificmpopcode(
IN  OUT   _JVM_Exec_Env * pjeeExecEnv, ifopenum enVal) {
  register  JAVA_WORD  * pjawOpStack = pjeeExecEnv->pjsfFrame->pjawOpStackElements;  
  JAVA_INT jaiValue1, jaiValue2;
 
  jaiValue1 = (JAVA_INT) pjawOpStack[pjeeExecEnv->ju32OpStackTop--];  
  jaiValue2 = (JAVA_INT) pjawOpStack[pjeeExecEnv->ju32OpStackTop--];  
  
  if(((jaiValue1 == jaiValue2) && (enVal == eq)) ||
    ((jaiValue1 != jaiValue2) && (enVal == ne))  ||
    ((jaiValue1 < jaiValue2) && (enVal == lt))   ||
    ((jaiValue1 <= jaiValue2) && (enVal == le))  ||
    ((jaiValue1 > jaiValue2) && (enVal == gt)) ||
    ((jaiValue1 >= jaiValue2) && (enVal == ge))) {
     pjeeExecEnv->pjsfFrame->jawPC = 
      (pjeeExecEnv->pjbyCode[1] << 8) | pjeeExecEnv->pjbyCode[2];
  } else {
     pjeeExecEnv->pjsfFrame->jawPC += 3;
  }
}

JVM_VOID JvmRtByteCode_ifeq__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){  ifopcode(pjeeExecEnv, eq); }


JVM_VOID JvmRtByteCode_ifne__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){  ifopcode(pjeeExecEnv, ne); }

JVM_VOID JvmRtByteCode_iflt__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){  ifopcode(pjeeExecEnv, lt); }

JVM_VOID JvmRtByteCode_ifge__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){  ifopcode(pjeeExecEnv, ge); }

JVM_VOID JvmRtByteCode_ifgt__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){  ifopcode(pjeeExecEnv, gt); }

JVM_VOID JvmRtByteCode_ifle__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){  ifopcode(pjeeExecEnv, le); }

JVM_VOID JvmRtByteCode_if_icmpeq__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ ificmpopcode(pjeeExecEnv, eq); }

JVM_VOID JvmRtByteCode_if_icmpne__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ ificmpopcode(pjeeExecEnv, ne);}

JVM_VOID JvmRtByteCode_if_icmplt__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ ificmpopcode(pjeeExecEnv, lt);}

JVM_VOID JvmRtByteCode_if_icmpge__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ ificmpopcode(pjeeExecEnv, ge);}

JVM_VOID JvmRtByteCode_if_icmpgt__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ ificmpopcode(pjeeExecEnv, gt); }

JVM_VOID JvmRtByteCode_if_icmple__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ ificmpopcode(pjeeExecEnv, le); }

JVM_VOID JvmRtByteCode_if_acmpeq__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_if_acmpne__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_goto__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_jsr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}


JVM_VOID JvmRtByteCode_tableswitch__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_lookupswitch__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_ireturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}


JVM_VOID JvmRtByteCode_lreturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_freturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

JVM_VOID JvmRtByteCode_dreturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
   register JAVA_WORD   jawValueHigh, jawValueLow;
   JAVA_OBJREF joClassObject = JVM_NULL;
   register  JAVA_WORD  * pjawOpStack  = 
     pjeeExecEnv->pjsfFrame->pjawOpStackElements;
   register _JVM_Method * pjmMethod = 
     pjeeExecEnv->pjsfFrame->pjmCurrMethod;
   
   
   if(!pjmMethod->pjmiMethodInfo->jbReturn) {
      JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );   
   }
   
   if(pjmMethod->pjmiMethodInfo->jfiReturnField.jcBaseType != 'D') {
      JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );   
   }
   
   if ( pjmMethod->jwMethodAccessFlags & ACC_SYNCHRONIZED )
   {
       if (!JvmReleaseLock( pjeeExecEnv->pjtsThreadState, 
              (JAVA_OBJREF)pjeeExecEnv->pjsfFrame->pjawLocals[0] ))
	       JVM_THROW_EXCEPTION( EXC_JAVAILLEGALMONITORSTATE );
   }
   
   jawValueLow = (JAVA_WORD)pjawOpStack[ pjeeExecEnv->ju32OpStackTop-- ] ;
   jawValueHigh = (JAVA_WORD)pjawOpStack[ pjeeExecEnv->ju32OpStackTop-- ] ;
   
   pjeeExecEnv->pjsfFrame->pjsfPrevFrame->pjawOpStackElements[ 	
	  ++pjeeExecEnv->pjsfFrame->pjsfPrevFrame->ju32TopElementIndex] = (JAVA_WORD)jawValueHigh;
	 pjeeExecEnv->pjsfFrame->pjsfPrevFrame->pjawOpStackElements[ 	
	  ++pjeeExecEnv->pjsfFrame->pjsfPrevFrame->ju32TopElementIndex] = (JAVA_WORD)jawValueLow;
	
   JvmRtDestroyStackFrame();
   JvmRtDestroyExecEnv();		
}

JVM_VOID JvmRtByteCode_areturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
   register JAVA_OBJREF joObjRef;
   JAVA_OBJREF joClassObject = JVM_NULL;
   register  JAVA_WORD  * pjawOpStack  = 
     pjeeExecEnv->pjsfFrame->pjawOpStackElements;
   register _JVM_Method * pjmMethod = 
     pjeeExecEnv->pjsfFrame->pjmCurrMethod;
   
   
   if(!pjmMethod->pjmiMethodInfo->jbReturn) {
      JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );   
   }
   
   if(!JvmLdrLoadNewClass(pjmMethod->pjmiMethodInfo->jfiReturnField.jsObjectName, JVM_NULL,
        pjeeExecEnv->pjsfFrame->pjccCurrClass->joClassLoaderObject,  &joClassObject)) {
       		eJVMExceptionType eException;
	  			JvmLdrGetDeferredException( pjmMethod->pjmiMethodInfo->jfiReturnField.jsObjectName, 
			     	pjeeExecEnv->pjsfFrame->pjccCurrClass->joClassLoaderObject,
			    	&eException );
				  JVM_THROW_EXCEPTION( eException );	 
		}
		
		joObjRef = (JAVA_OBJREF)pjawOpStack[pjeeExecEnv->ju32OpStackTop--];
		
		if(!IsAssignmentCompatible(joObjRef, joClassObject)) {
		   	  JVM_THROW_EXCEPTION( EXC_JAVACLASSCAST );
		}
		
		/** check if method is 'synchronized' **/
   if ( pjmMethod->jwMethodAccessFlags & ACC_SYNCHRONIZED )
   {
       joObjRef = (JAVA_OBJREF)pjeeExecEnv->pjsfFrame->pjawLocals[0];
       if (!JvmReleaseLock( pjeeExecEnv->pjtsThreadState, joObjRef ))
	       JVM_THROW_EXCEPTION( EXC_JAVAILLEGALMONITORSTATE );
   }
   
	pjeeExecEnv->pjsfFrame->pjsfPrevFrame->pjawOpStackElements[ 	
	++pjeeExecEnv->pjeePrev->ju32OpStackTop] = (JAVA_WORD)joObjRef;
	
  JvmRtDestroyStackFrame();
  JvmRtDestroyExecEnv();		
}

JVM_VOID JvmRtByteCode_return__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{ 
   register JAVA_OBJREF joObjRef;
   register _JVM_Method * pjmMethod = 
	  pjeeExecEnv->pjsfFrame->pjmCurrMethod;

   /** check if return type is void **/
   if ( pjmMethod->pjmiMethodInfo->jbReturn )
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

   /** check if method is 'synchronized' **/
   if ( pjmMethod->jwMethodAccessFlags & ACC_SYNCHRONIZED )
   {
       joObjRef = (JAVA_OBJREF)pjeeExecEnv->pjsfFrame->pjawLocals[0];
       if (!JvmReleaseLock( pjeeExecEnv->pjtsThreadState, joObjRef ))
	       JVM_THROW_EXCEPTION( EXC_JAVAILLEGALMONITORSTATE );
   }
  

  JvmRtDestroyStackFrame();
  JvmRtDestroyExecEnv();
}

JVM_VOID JvmRtByteCode_getstatic__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyCPByte,pjbyCPBlock;         
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  JAVA_WORD    jawIndex;
  JAVA_OBJREF joClassObject = JVM_NULL;
 _JVM_Class_Class * pjccRefClass;
 _JVM_Field       * pjfRefField;

  /** Const_Fieldref index into CP **/
  jawIndex =((pjbyThisCode[1] << 8 ) | (pjbyThisCode[2]));
  pjbyCPBlock = 
	  pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];
	if ( JVM_CP_TAG_FIELDREF != CP_TAG( pjbyCPBlock ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	  
	pjccRefClass = CP_CLASSREF( pjbyCPBlock );
  pjfRefField = CP_FIELDREF( pjbyCPBlock );  
  
  if (!( JVM_VALID( pjccRefClass ))) {
     pjccRefClass = JvmResolveClass(pjbyCPByte, pjeeExecEnv);
     
     if(pjccRefClass == JVM_NULL) {
        JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	      JVM_ASSERT( JVM_FALSE );
     }
			/** Save class reference **/
			CP_CLASSREF( pjbyCPBlock ) = pjccRefClass;
  }
  
  /** Load method ( assuming class is loaded before ) **/
  if (!( JVM_VALID( pjfRefField ))) {
		/** Const_fieldref **/
		pjbyCPByte = pjbyCPBlock;
		pjfRefField = JvmResolveField(pjbyCPByte, pjccRefClass, pjsfCurrFrame);
		
		if(!pjfRefField) {
		   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	     JVM_ASSERT( JVM_FALSE );
		}
        /** Save ref to method **/
		CP_FIELDREF( pjbyCPBlock )  = pjfRefField;
	}
	
	if(pjfRefField->jfiFieldType.jwFieldType & JVM_OBJECT_TYPE ) {
	  pjawOpStack[++ju32StackTop]= (JVM_INT)pjfRefField->u.jaoref;
	} else {
  	switch ( pjfRefField->jfiFieldType.jcBaseType ) {
  	case 'B' : pjawOpStack[++ju32StackTop] = pjfRefField->u.jaby; break;
		case 'C' : pjawOpStack[++ju32StackTop] = pjfRefField->u.jac; break;
		case 'F' : pjawOpStack[++ju32StackTop] = (JAVA_WORD)pjfRefField->u.jaf; break;
		case 'I' : pjawOpStack[++ju32StackTop] = pjfRefField->u.jai; break;
		case 'S' : pjawOpStack[++ju32StackTop] = pjfRefField->u.jas; break;
		case 'Z' : pjawOpStack[++ju32StackTop] = pjfRefField->u.jab; break;
		
		case 'D' : pjawOpStack[++ju32StackTop] = (JAVA_WORD)pjfRefField->u.jal;
		           pjawOpStack[++ju32StackTop] = (JAVA_WORD)(pjfRefField->u.jal >> sizeof(JAVA_WORD));
		           break;
    case 'J' : pjawOpStack[++ju32StackTop] = (JAVA_WORD)(JAVA_LONG)pjfRefField->u.jal;
		           pjawOpStack[++ju32StackTop] = (JAVA_WORD)(JAVA_LONG)(pjfRefField->u.jal >> sizeof(JAVA_WORD));
		           break;		           
		default  : 
			       JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	 }
	 
	}
	
	 pjeeExecEnv->pjsfFrame->jawPC += 3;     
}

JVM_VOID JvmRtByteCode_putstatic__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyCPByte,pjbyCPBlock;         
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  JAVA_WORD    jawIndex;
  JAVA_OBJREF joClassObject = JVM_NULL;
  _JVM_Class_Class * pjccRefClass;
  _JVM_Field       * pjfRefField;
  JAVA_INT          jawIntValue;
  JAVA_INT          jawIntValue1;

  /** Const_Fieldref index into CP **/
  jawIndex =((pjbyThisCode[1] << 8 ) | (pjbyThisCode[2]));
  pjbyCPBlock = 
	  pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

  if ( JVM_CP_TAG_FIELDREF != CP_TAG( pjbyCPBlock ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
 
  pjccRefClass = CP_CLASSREF( pjbyCPBlock );
  pjfRefField = CP_FIELDREF( pjbyCPBlock );

  if (!( JVM_VALID( pjccRefClass ) ))
  {
      pjccRefClass = JvmResolveClass(pjbyCPByte, pjeeExecEnv);
      if(pjccRefClass == JVM_NULL) {
        JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	      JVM_ASSERT( JVM_FALSE );
      }
			/** Save class reference **/
			CP_CLASSREF( pjbyCPBlock ) = pjccRefClass;
  }

  /** Load method ( assuming class is loaded before ) **/
  if (!( JVM_VALID( pjfRefField )))	{
		/** Const_fieldref **/
		pjbyCPByte = pjbyCPBlock;
		
		pjfRefField = JvmResolveField(pjbyCPByte, pjccRefClass, pjsfCurrFrame);
		
		if(!pjfRefField) {
		   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	     JVM_ASSERT( JVM_FALSE );
		}
       /** Save ref to method **/
		CP_FIELDREF( pjbyCPBlock )  = pjfRefField;
	}
	

	 jawIntValue = pjawOpStack[pjeeExecEnv->ju32OpStackTop--];
	 
	 if (pjfRefField->jwFieldAccessFlags & ACC_FINAL) {
	    if(!IsStrictValidFieldOf(pjccRefClass, pjfRefField->jsFieldName,
	                         pjfRefField->jsFieldDesc, &pjfRefField)) {
          JVM_THROW_EXCEPTION( EXC_ILLEGALACCESSERROR );	 	                         
          JVM_ASSERT(JVM_FALSE);
      }
      
      CP_FIELDREF( pjbyCPBlock )  = pjfRefField;
	 }
	 
	 if (!(pjfRefField->jwFieldAccessFlags & ACC_STATIC)) {
	       JVM_THROW_EXCEPTION( EXC_JAVAINCOMPATIBLECLASSCHANGEERROR );	 	                         
         JVM_ASSERT(JVM_FALSE);
	 }
	 
	 if(GetFieldOfClass(pjccRefClass, pjfRefField->jsFieldName,
	                               pjfRefField->jsFieldDesc, &pjfRefField)) {
	                               
      if (pjfRefField->jfiFieldType.jwFieldType & JVM_OBJECT_TYPE ) {
	       pjfRefField->u.jaoref = (JAVA_OBJREF)jawIntValue;
	   } else {
       if((pjfRefField->jfiFieldType.jcBaseType == 'J') ||
         	(pjfRefField->jfiFieldType.jcBaseType == 'D')) {
         	  jawIntValue1 = pjawOpStack[pjeeExecEnv->ju32OpStackTop--];
            JvmFieldStoreLongDoubleValue(pjfRefField, 
                                         jawIntValue1, 
                                         jawIntValue);
       } else {
         JvmFieldStoreWordValue(pjfRefField, jawIntValue);
       }
     }	    
   }
   
   pjeeExecEnv->pjsfFrame->jawPC += 3;     
}

JVM_VOID JvmRtByteCode_getfield__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ JvmDisplay("Dummy handler called");}

_JVM_Field * JvmResolveField(JVM_PBYTE pjbyCPByte,
                            _JVM_Class_Class * pjccRefClass,
                            _JVM_Stack_Frame * pjsfCurrFrame) {
    register JVM_U16    ju16Index,juTypeIndex;
    _JVM_Field * pjfRefField = JVM_NULL;
    JVM_STRING jsFieldName, jsFieldType;
                           
 		/** Skip class_index **/
		ju16Index    =  LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ) +
		                      sizeof( JVM_U16 ) );
		/** Name and Type info of fieldref **/
		pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];
		ju16Index    = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
		juTypeIndex  = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ) +
			              sizeof( JVM_U16));
		/** UTF8 for field Name **/
 		pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];
		if (!JvmLdrGetUtf8Bytes( pjbyCPByte, &jsFieldName, JVM_NULL ) )
		{
  		 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
			 JVM_ASSERT( JVM_FALSE );
		}

   		/** UTF8 index for method type **/
		pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ juTypeIndex ];
		if (!JvmLdrGetUtf8Bytes( pjbyCPByte, &jsFieldType, JVM_NULL ))
		{
  		JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
			JVM_ASSERT( JVM_FALSE );
		}

    if ( !IsValidFieldOf( pjccRefClass,jsFieldName, jsFieldType, &pjfRefField ))
			 	JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
			 	
		JvmSpringFree( &jsFieldName );
		JvmSpringFree( &jsFieldType );	 	
			 	
		return 	pjfRefField; 	
} 
 


_JVM_Method * JvmResolveMethod(JVM_PBYTE pjbyCPByte, 
                              _JVM_Class_Class * pjccRefClass,
                              _JVM_Stack_Frame * pjsfCurrFrame) {
    register JVM_U16    ju16Index,juTypeIndex;
    
    _JVM_Method * pjmRefMethod = JVM_NULL;
    JVM_STRING jsMethodName = JVM_NULL;
    JVM_STRING jsMethodType = JVM_NULL;
                                       
                                   
		/** Skip class_index **/
		ju16Index    =  LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ) +
		                      sizeof( JVM_U16 ) );
		/** Name and Type info of methodref **/
		pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];
		ju16Index    = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
		juTypeIndex  = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ) +
			              sizeof( JVM_U16));
		/** UTF8 for method Name **/
    pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];
		if (!JvmLdrGetUtf8Bytes( pjbyCPByte, &jsMethodName, JVM_NULL ) )
		{
  			JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
			JVM_ASSERT( JVM_FALSE );
		}

   		/** UTF8 index for method type **/
		pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ juTypeIndex ];
		if ( !JvmLdrGetUtf8Bytes( pjbyCPByte, &jsMethodType, JVM_NULL ) )
		{
  		 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
			 JVM_ASSERT( JVM_FALSE );
		}

    if ( !IsValidMethodOf( pjccRefClass,jsMethodName, jsMethodType, &pjmRefMethod ))
			 	JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	                                    
			 	
		JvmSpringFree( &jsMethodName );
		JvmSpringFree( &jsMethodType );
		
		
		
		return pjmRefMethod;
}

_JVM_Class_Class * JvmResolveClass(JVM_PBYTE pjbyCPByte, 
                                   _JVM_Exec_Env * pjeeExecEnv) {
   register JVM_U16 ju16Index = 0;
   JVM_STRING jsClassName = JVM_NULL;
   JAVA_OBJREF joClassObject;
   _JVM_Class_Class * pjccRefClass = JVM_NULL;
   _JVM_Stack_Frame * pjsfCurrFrame = pjeeExecEnv->pjsfFrame;
   
   /** class_index **/
   ju16Index = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
	 /** Const_class_info **/
   pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];
   if ( JVM_CP_TAG_CLASS != CP_TAG( pjbyCPByte ))
	    JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	 /** Name Index( Const_utf8_info ) **/
   ju16Index  = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
   pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];
   if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyCPByte ))
	     JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	 /** Load Class Name  **/
	 if ( JvmLdrGetUtf8Bytes( pjbyCPByte, &jsClassName, JVM_NULL ) )
	 {
           /** Load the class **/
			if ( !JvmLdrLoadNewClass( jsClassName, JVM_NULL, 
			pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,&joClassObject))
			{
				eJVMExceptionType eException;
	  			JvmLdrGetDeferredException( jsClassName, 
				pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,
				&eException );
				JVM_THROW_EXCEPTION( eException );	 
			}

			/* Must have loaded the class before */
			JVM_ASSERT( joClassObject != JVM_NULL );
      pjccRefClass = ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass;
	
   		JvmSpringFree( &jsClassName );
   		
   		if(((_JVM_Object *)joClassObject)->jiState == JAVA_OBJECT_STATE_LOADED) {
   		    ((_JVM_Object *)joClassObject)->jiState = JAVA_OBJECT_STATE_INITIALIZING;
       		JvmRtInvokeMethod(S_METHOD_CLINIT,
			                S_DESC_COMMON_CLINIT,
			                joClassObject,
			                JVM_NULL,
			                0,
			                pjeeExecEnv);
			   ((_JVM_Object *)joClassObject)->jiState = JAVA_OBJECT_STATE_INITIALIZED;
		  }
   		
   		
   		return pjccRefClass;
	 }
	 else
	 {
  	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	    JVM_ASSERT( JVM_FALSE );
	 }                                   
	 
	 return JVM_NULL;
}

 

JVM_VOID JvmRtByteCode_putfield__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyCPByte,pjbyCPBlock;         
  register  JAVA_OBJREF  pjaoObjRef;   
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  JAVA_WORD    jawIndex;
  JAVA_OBJREF joClassObject = JVM_NULL;
 _JVM_Class_Class * pjccRefClass;
 _JVM_Field       * pjfRefField;
 _JVM_Field       * pjfObjectField;
  JAVA_INT          jawIntValue;
  JAVA_INT          jawIntValue1;

  /** Const_Fieldref index into CP **/
  jawIndex =((pjbyThisCode[1] << 8 ) | (pjbyThisCode[2]));
  pjbyCPBlock = 
	  pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

  if ( JVM_CP_TAG_FIELDREF != CP_TAG( pjbyCPBlock ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
 
  pjccRefClass = CP_CLASSREF( pjbyCPBlock );
  pjfRefField = CP_FIELDREF( pjbyCPBlock );

  if (!( JVM_VALID( pjccRefClass ) ))
  {
      pjccRefClass = JvmResolveClass(pjbyCPByte, pjeeExecEnv);
      if(pjccRefClass == JVM_NULL) {
        JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	      JVM_ASSERT( JVM_FALSE );
      }
			/** Save class reference **/
			CP_CLASSREF( pjbyCPBlock ) = pjccRefClass;
  }

  /** Load method ( assuming class is loaded before ) **/
  if (!( JVM_VALID( pjfRefField )))	{
		/** Const_fieldref **/
		pjbyCPByte = pjbyCPBlock;
		
		pjfRefField = JvmResolveField(pjbyCPByte, pjccRefClass, pjsfCurrFrame);
		
		if(!pjfRefField) {
		   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	     JVM_ASSERT( JVM_FALSE );
		}
       /** Save ref to method **/
		CP_FIELDREF( pjbyCPBlock )  = pjfRefField;
	}
	

	 jawIntValue = pjawOpStack[pjeeExecEnv->ju32OpStackTop--];
	 
	 if (pjfRefField->jwFieldAccessFlags & ACC_FINAL) {
	    if(!IsStrictValidFieldOf(pjccRefClass, pjfRefField->jsFieldName,
	                         pjfRefField->jsFieldDesc, &pjfRefField)) {
          JVM_THROW_EXCEPTION( EXC_ILLEGALACCESSERROR );	 	                         
          JVM_ASSERT(JVM_FALSE);
      }
      
      CP_FIELDREF( pjbyCPBlock )  = pjfRefField;
	 }
	 
	 /* TODO more checks */
	 
	 if ((pjfRefField->jfiFieldType.jcBaseType == 'J') ||
	     (pjfRefField->jfiFieldType.jcBaseType == 'D')) {
      jawIntValue1 = pjawOpStack[pjeeExecEnv->ju32OpStackTop--];
   }
	 
	 pjaoObjRef = (JAVA_OBJREF)pjawOpStack[pjeeExecEnv->ju32OpStackTop--];
	 
	 if(pjaoObjRef == JVM_NULL) {
	   JVM_THROW_EXCEPTION(EXC_JAVANULLPOINTER);
	 }
	 
	 if(GetFieldOfObject(pjaoObjRef, pjfRefField->jsFieldName, 
	                  pjfRefField->jsFieldDesc, &pjfObjectField)) {
	   if (pjfObjectField->jfiFieldType.jwFieldType & JVM_OBJECT_TYPE ) {
	       pjfObjectField->u.jaoref = (JAVA_OBJREF)jawIntValue;
	   } else {
       if((pjfObjectField->jfiFieldType.jcBaseType == 'J') ||
         	(pjfObjectField->jfiFieldType.jcBaseType == 'D')) {
            JvmFieldStoreLongDoubleValue(pjfObjectField, 
                                         jawIntValue1, 
                                         jawIntValue);
       } else {
         JvmFieldStoreWordValue(pjfObjectField, jawIntValue);
       }
     }
   }
   
   	pjeeExecEnv->pjsfFrame->jawPC += 3;     
}

/** * * * * * I N V O K E S T A T I C * * * * * **/

JVM_VOID JvmRtByteCode_invokestatic__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
){ 
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyCPByte,pjbyCPBlock;         
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  JAVA_WORD    jawIndex;
  JVM_INT    jiArgs,jiCount;
 _JVM_Class_Class * pjccRefClass;
 _JVM_Method      * pjmRefMethod;
 JAVA_WORD * pjawArgs;
 

  /** Const_Methodref index into CP **/
  jawIndex =((pjbyThisCode[1] << 8 ) | (pjbyThisCode[2]));
  pjbyCPBlock = 
	  pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

  if ( JVM_CP_TAG_METHODREF != CP_TAG( pjbyCPBlock ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

  pjccRefClass  = CP_CLASSREF( pjbyCPBlock );
  pjmRefMethod =  CP_METHODREF( pjbyCPBlock );
  
  if (!( JVM_VALID( pjccRefClass ) ))  {
      pjccRefClass = JvmResolveClass(pjbyCPByte, pjeeExecEnv);
      
      if(pjccRefClass == JVM_NULL) {
         JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	       JVM_ASSERT( JVM_FALSE );
      }
			/** Save ref to class **/
			CP_CLASSREF( pjbyCPBlock ) = pjccRefClass;
  }
  
  /** Load method ( assuming class is loaded before ) **/
  if (!( JVM_VALID( pjmRefMethod )))
	{
		/** Const_methodref **/
		pjmRefMethod = JvmResolveMethod(pjbyCPBlock, pjccRefClass, pjsfCurrFrame);
		if(!pjmRefMethod) {
       JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
       JVM_ASSERT( JVM_FALSE );
		}
        /** Save ref to method **/
		CP_METHODREF( pjbyCPBlock )  = pjmRefMethod;
	}
	
  jiArgs = pjmRefMethod->pjmiMethodInfo->jiParamsWidthInWords ;

  if ( jiArgs ) {
	     pjawArgs = (JAVA_WORD *)JvmMemAlloc( sizeof( JAVA_WORD ) * jiArgs );
	     JVM_ALLOC_CHECK( pjawArgs )
         /** CHECK,CHECK for consistency and correctness **/
	     jiCount = jiArgs;
	     for ( ; (jiCount - 1) ; )
		    pjawArgs[ --jiCount ] = pjawOpStack[ ju32StackTop-- ];
  }
  
    if ( pjmRefMethod->jwMethodAccessFlags & ACC_SYNCHRONIZED )
 	    JvmAcquireLock( pjtsThreadState, pjccRefClass->joClassClassObject);
	
	 {
	  pjeeExecEnv->ju32OpStackTop = ju32StackTop;
	  pjeeExecEnv->pjsfFrame->jawPC += 3;     
  
	  JvmRtCallMethod( pjeeExecEnv, pjmRefMethod, pjccRefClass, pjccRefClass->joClassClassObject,     
		     pjawArgs, pjmRefMethod->pjmiMethodInfo->jiParameterCount );
   } 
}


/** * * * * * I N V O K E S P E C I A L * * * * * **/

JVM_VOID JvmRtByteCode_invokespecial__Handler(
  IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyCPByte, pjbyCPBlock;         
  register  JAVA_OBJREF  pjaoObjRef;   
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  JAVA_WORD  * pjawArgsArray = JVM_NULL;
  JAVA_WORD    jawIndex;
  JVM_INT    jiArgs,jiCount;
  JAVA_OBJREF joClassObject = JVM_NULL;

  _JVM_Class_Class * pjccRefClass;
  _JVM_Method      * pjmRefMethod; 

  /** Const_Methodref index into CP **/
  jawIndex =((pjbyThisCode[1] << 8 ) | (pjbyThisCode[2]));
  pjbyCPBlock = 
	  pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

  if ( JVM_CP_TAG_METHODREF != CP_TAG( pjbyCPBlock ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

  pjccRefClass  = CP_CLASSREF( pjbyCPBlock );
  pjmRefMethod =  CP_METHODREF( pjbyCPBlock );


  /** Load class **/
  if (!( JVM_VALID( pjccRefClass ) ))
  {
     pjccRefClass = JvmResolveClass(pjbyCPByte, pjeeExecEnv);
     if(!pjccRefClass) {
       JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	     JVM_ASSERT( JVM_FALSE );
     }
		 /** Save ref to class **/
		 CP_CLASSREF( pjbyCPBlock ) = pjccRefClass;
  }

	/** ACC_SUPER must be set **/
	if (!(pjccRefClass->jdwClassFlags  &  ACC_SUPER ))
	    JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	if ( !IsSuperclassOf( pjccRefClass, pjsfCurrFrame->pjccCurrClass ))
           JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );


	/** Load method ( assuming class is loaded before ) **/
  if (!( JVM_VALID( pjmRefMethod )))
	{
		/** Const_methodref **/
		pjbyCPByte = pjbyCPBlock;
		
		pjmRefMethod = JvmResolveMethod(pjbyCPByte, pjccRefClass, pjsfCurrFrame);
		if(!pjmRefMethod) {
		   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	     JVM_ASSERT( JVM_FALSE );
		}

        /** Save ref to method **/
		CP_METHODREF( pjbyCPBlock )  = pjmRefMethod;
		
 	}

		/** Handle invokespecial's strangeness **/
		/** This might modify pjmRefMethod and pjccRefClass **/
		if ( JvmSpringStrCmp( pjsfCurrFrame->pjccCurrClass->jsClassName,
		     S_JAVA_LANG_OBJECT ) != 0 ) 
		{
			   JvmGetNearestBaseMethod( pjsfCurrFrame->pjccCurrClass,
				  	pjmRefMethod->jsMethodName, pjmRefMethod->jsMethodDesc, 
				  	&pjmRefMethod, &pjccRefClass );
		}
	

    jiArgs = pjmRefMethod->pjmiMethodInfo->jiParamsWidthInWords ;

    if ( jiArgs ) {
	     pjawArgsArray = JvmMemAlloc( sizeof( JAVA_WORD ) * jiArgs );
	     JVM_ALLOC_CHECK( pjawArgsArray )
         /** CHECK,CHECK for consistency and correctness **/
	     jiCount = jiArgs;
	     for ( ; (jiCount - 1) ; )
		    pjawArgsArray[ --jiCount ] = pjawOpStack[ ju32StackTop-- ];
	   }

	  pjaoObjRef = (JVM_PVOID)pjawOpStack[ ju32StackTop-- ];
	/** Must acquire the lock before proceeding for methods with 'synchronized' attribute 
	    will be released in one of the 'ret's **/
    if ( pjmRefMethod->jwMethodAccessFlags & ACC_SYNCHRONIZED )
 	       JvmAcquireLock( pjtsThreadState, pjaoObjRef );

    if ( pjmRefMethod->jwMethodAccessFlags & ACC_NATIVE )  {
    	 pjeeExecEnv->ju32OpStackTop = ju32StackTop;
		  JvmRtSaveContext( pjeeExecEnv );
		  /** Block here until native method is complete **/
		  JvmRtCallMethod( pjeeExecEnv, pjmRefMethod, pjccRefClass,
			    pjaoObjRef, pjawArgsArray, jiArgs); 
		  JvmRtRestoreContext( pjeeExecEnv );
	  }	else  {
		 /** Context is changed **/
     /** Will be reset in all 'ret' instructions **/
       pjsfCurrFrame->jawPC = ( pjeeExecEnv->pjsfFrame->jawPC + 3 );        
	     JvmRtSetUpStackFrame( pjmRefMethod, pjccRefClass );
	     JvmRtSetUpParameters( pjaoObjRef, pjawArgsArray, jiArgs );
	     JvmRtSetUpExecEnv( pjeeExecEnv );
		 /** Reset the current thread's PC **/
 		 pjeeExecEnv->pjsfFrame->jawPC = 0;
	  }

	if ( pjawArgsArray )
		JvmMemFree( pjawArgsArray );
}



/** * * * * * I N V O K E I N T E R F A C E * * * * * **/

JVM_VOID JvmRtByteCode_invokeinterface__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyCPByte;         
  register  JAVA_OBJREF  pjaoObjRef;   
  register  JVM_PBYTE    pjbyThisCode    = pjeeExecEnv->pjbyCode;
  
  JAVA_WORD  * pjawArgsArray;
  JAVA_WORD    jawIndex;
  JVM_INT    jiArgs,jiCount;
  JVM_U16    ju16Index;
  JVM_STRING jsClassName, jsMethodName, jsMethodType;
  JAVA_OBJREF joClassObject = JVM_NULL;

  _JVM_Class_Class * pjccRefClass;
  _JVM_Method      * pjmRefMethod; 

  jawIndex =((pjbyThisCode[1] << 8 ) | (pjbyThisCode[2]));
  pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

  if ( JVM_CP_TAG_INTERFACEMETHODREF != CP_TAG( pjbyCPByte ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

  pjccRefClass  = CP_CLASSREF( pjbyCPByte );
  pjmRefMethod =  CP_METHODREF( pjbyCPByte );

  if (!( JVM_VALID( pjccRefClass ) ) && !( JVM_VALID( pjmRefMethod )))
  {
     ju16Index = *(JVM_WORD *)( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
     pjbyCPByte     = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];
   	 if ( JVM_CP_TAG_CLASS != CP_TAG( pjbyCPByte ))
	    JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

   	 ju16Index  = *(JVM_WORD *)( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
   	 pjbyCPByte     = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

     if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyCPByte ))
	     JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	 if ( JvmLdrGetUtf8Bytes( pjbyCPByte, &jsClassName, JVM_NULL ) )
	 {
	       /** Load the class **/
			if ( !JvmLdrLoadNewClass( jsClassName, JVM_NULL, 
			pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,&joClassObject))
			{
				eJVMExceptionType eException;
	  			JvmLdrGetDeferredException( jsClassName, 
				pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,
				&eException );
				JVM_THROW_EXCEPTION( eException );	 
			}

			/* Must have loaded the class before */
			JVM_ASSERT( joClassObject != JVM_NULL );

			if ( !(((_JVM_Object *)joClassObject)->joiObjectID.pjccClass->jdwClassFlags &
				  ACC_INTERFACE ))
			{
				JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
			}

			/** Const_interfacemethodref **/
			pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];
			ju16Index    =  LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ) +
				                      sizeof( JVM_U16 ) );
			/** Name and Type info **/
			pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];
			ju16Index    = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
			/** UTF8 for method Name **/
    		pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

			JvmGetUtf8Info( pjbyCPByte, &jsMethodName, JVM_NULL );

    		if ( jsMethodName[0] == '<' )
			   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 


			/** UTF8 index for method type **/
			ju16Index    = LD_WORD( pjbyCPByte + 
				sizeof( _JVM_Constant_Pool ) + sizeof( JVM_U16) );

			pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

			JvmGetUtf8Info( pjbyCPByte, &jsMethodType, JVM_NULL );

			/** Ref Method obtained here **/
			if ( !IsValidMethodOf( ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass, 
				 jsMethodName, jsMethodType, &pjmRefMethod )
			   )
			   JVM_THROW_EXCEPTION( EXC_JAVAINCOMPATIBLECLASSCHANGEERROR );	 

			if ( pjmRefMethod->jwMethodAccessFlags & ACC_STATIC )
			   JVM_THROW_EXCEPTION( EXC_JAVAINCOMPATIBLECLASSCHANGEERROR );	 

			if (!( pjmRefMethod->jwMethodAccessFlags & ACC_PUBLIC))
			   JVM_THROW_EXCEPTION( EXC_JAVAILLEGALACCESSERROR );	 	

			if (!( pjmRefMethod->jwMethodAccessFlags & ACC_ABSTRACT))
			   JVM_THROW_EXCEPTION( EXC_JAVAABSTRACTMETHODERROR );

			pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];
			CP_CLASSREF( pjbyCPByte ) = ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass;
			CP_METHODREF( pjbyCPByte )  = pjmRefMethod;
			pjccRefClass = ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass;

			JvmSpringFree( &jsClassName  );
			JvmSpringFree( &jsMethodName );
			JvmSpringFree( &jsMethodType );
	 }
	 else
	 {
  	   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	   JVM_ASSERT( JVM_FALSE );
	 }

  }


    jiArgs = pjmRefMethod->pjmiMethodInfo->jiParamsWidthInWords + 1 ;

	  if ( jiArgs != pjbyThisCode[3] )
	    JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 

	  pjawArgsArray = JvmMemAlloc( sizeof( JAVA_WORD ) * jiArgs );
	  JVM_ALLOC_CHECK( pjawArgsArray )

	  /** CHECK,CHECK for consistency and correctness **/
	  jiCount = jiArgs;
	  for ( ; (jiCount - 1) ; )
	  {
          pjawArgsArray[ --jiCount ] = pjawOpStack[ ju32StackTop-- ];
	  }

	  pjaoObjRef = (JVM_PVOID)pjawOpStack[ ju32StackTop-- ];

	  /** Must acquire the lock before proceeding for
	      methods with 'synchronized' attribute 
		  will be released in one of the 'ret's
	   **/
	  if ( pjmRefMethod->jwMethodAccessFlags & ACC_SYNCHRONIZED )
	  {
		  JvmAcquireLock( pjtsThreadState, pjaoObjRef );
	  }

	  if ( pjmRefMethod->jwMethodAccessFlags & ACC_NATIVE )
	  {
		  _JVM_Field jfiField;
		  
		  jfiField.jfiFieldType = pjmRefMethod->pjmiMethodInfo->jfiReturnField;

		  JvmRtSaveContext( pjeeExecEnv );

		  /** Block here until native method is complete **/
		  JvmRtCallMethod( pjeeExecEnv, pjmRefMethod, pjccRefClass,
			    pjaoObjRef, pjawArgsArray, jiArgs); 

		  JvmRtRestoreContext( pjeeExecEnv );
	  }
	  else
	  {
		 /** Context is changed **/
     	 /** Will be reset in all 'ret' instructions **/
        pjsfCurrFrame->jawPC = ( pjeeExecEnv->pjsfFrame->jawPC + 3 );
 	     JvmRtSetUpStackFrame( pjmRefMethod, pjccRefClass );
	     JvmRtSetUpParameters( pjaoObjRef, pjawArgsArray, jiArgs );
	     JvmRtSetUpExecEnv( pjeeExecEnv );
	  }

	  JvmMemFree( pjawArgsArray );

}

/** * * * * * N E W * * * * * **/


JVM_VOID JvmRtByteCode_new__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
	register  JAVA_WORD  * pjawOpStack  = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
	register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
	register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
	JAVA_WORD jawIndex;
	JVM_PBYTE pjbyCPByte;
	JVM_U16   ju16Index;
	JVM_STRING jsClassName;
	JAVA_OBJREF joClassObject;
	_JVM_Class_Class * pjccClass;

	jawIndex = ( pjbyThisCode[1] << 8 ) |  pjbyThisCode[2];

	pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

    if ( JVM_CP_TAG_CLASS != CP_TAG( pjbyCPByte ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	ju16Index  = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
	pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

	if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyCPByte ))
		 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	if ( JvmLdrGetUtf8Bytes( pjbyCPByte, &jsClassName, JVM_NULL ) )
	{
	    if ( jsClassName[0] != '[' )
		{
		   if ( !JvmLdrLoadNewClass( jsClassName, JVM_NULL, 
		    	pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,&joClassObject))
				JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
		}
		else
	 		 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	}

	pjccClass = ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass;

	if ( pjccClass->jdwClassFlags & ACC_INTERFACE  ||
	     pjccClass->jdwClassFlags & ACC_ABSTRACT )
			 JVM_THROW_EXCEPTION( EXC_JAVAINSTANTIATIONERROR );
	
    if (!(pjccClass->jdwClassFlags & ACC_PUBLIC)  ||
	     !IsSamePackage( pjccClass, pjsfCurrFrame->pjccCurrClass ))
	{
	    JVM_THROW_EXCEPTION( EXC_JAVAILLEGALACCESSERROR );
	}

	joClassObject = JvmRtCreateNewInstance( pjccClass );

	pjawOpStack[ pjeeExecEnv->ju32OpStackTop ] = (JAVA_WORD)joClassObject;

	pjeeExecEnv->pjsfFrame->jawPC += 3;

}


JVM_BOOL IsSamePackage(
IN  _JVM_Class_Class * pjccClassA,
IN  _JVM_Class_Class * pjccClassB
)
{
	JvmDisplay("IsSamePackage Not yet implemented\n");
	return JVM_FALSE;
}





/** * * * * * N E W A R R A Y * * * * * **/


JVM_VOID JvmRtByteCode_newarray__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
	register  JAVA_WORD  * pjawOpStack  = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
	register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
	register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
	JAVA_INT jaiCount;
	JAVA_WORD jawArrayType;
	JAVA_OBJREF joArrayObject;
	PFN_ARRAYFACTORY pfnArrayFactory[8] = {
		JvmRtCreateByteArray,
		JvmRtCreateCharArray,
		JvmRtCreateFloatArray,
		JvmRtCreateDoubleArray,
		JvmRtCreateByteArray,
		JvmRtCreateShortArray,
		JvmRtCreateIntArray,
		JvmRtCreateLongArray
	};

	jaiCount = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];

	if ( jaiCount < 0 )
		JVM_THROW_EXCEPTION( EXC_JAVANEGATIVEARRAYSIZE );

	jawArrayType = pjbyThisCode[1];

	if ( jawArrayType < 0 || jawArrayType >= 8 )
		JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	joArrayObject = (* pfnArrayFactory[ jawArrayType - T_BOOLEAN ] )( jaiCount );

	pjawOpStack[ pjeeExecEnv->ju32OpStackTop ] = (JAVA_WORD)joArrayObject;

	pjeeExecEnv->pjsfFrame->jawPC += 2;

}






/** * * * * * A N E W A R R A Y * * * * * **/


JVM_VOID JvmRtByteCode_anewarray__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
  register  JAVA_WORD  * pjawOpStack  = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
	register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
	register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
	JAVA_INT jaiCount;
	JAVA_WORD jawIndex;
	JVM_PBYTE pjbyCPByte;
	JVM_U16   ju16Index;
	JVM_STRING jsClassName;
	JAVA_OBJREF joClassObject;
	JAVA_WORD   jawLengths[1];
	JAVA_OBJREF joArrayObject;
	_JVM_Class_Class * pjccRefClass = JVM_NULL;

	jaiCount = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];

	jawIndex = ( pjbyThisCode[1] << 8 ) |  pjbyThisCode[2];

    pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

    if ( JVM_CP_TAG_CLASS != CP_TAG( pjbyCPByte ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	ju16Index  = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
	pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

	if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyCPByte ))
		 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	if ( JvmLdrGetUtf8Bytes( pjbyCPByte, &jsClassName, JVM_NULL ) )
	{
		 if ( jsClassName[0] == '[' )
		{
			/** Array Classes always have primitive loader as their class loader**/
			if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, jsClassName, &joClassObject ))     
				JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
		}
		else
	 		 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	}

	pjccRefClass = ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass;

	jawLengths[0] = (JAVA_WORD)jaiCount;

	joArrayObject = JvmRtCreateNewArray( pjccRefClass->pjfiArrayInfo->jcBaseType,
		 1, jawLengths,  pjccRefClass, 
		 pjccRefClass->joArrayBaseTypeObject->joiObjectID.pjccClass
	 );

	pjawOpStack[ pjeeExecEnv->ju32OpStackTop ] =(JAVA_WORD)joArrayObject;

	pjeeExecEnv->pjsfFrame->jawPC += 3;

}






/** * * * * * A R R A Y L E N G T H * * * * * **/


JVM_VOID JvmRtByteCode_arraylength__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
	register  JAVA_WORD  * pjawOpStack  = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
	register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;

	JAVA_ARRAYREF  joArrayObject;

	joArrayObject = (JAVA_ARRAYREF)pjawOpStack[ pjeeExecEnv->ju32OpStackTop ] ;

	if ( joArrayObject == JAVA_NULL )
	   JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

	if ( joArrayObject->joiObjectID.jbyID != REF_TYPE_ARRAY )
	   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	pjawOpStack[ pjeeExecEnv->ju32OpStackTop ] =
		(JAVA_INT )joArrayObject->jawLength;

	pjeeExecEnv->pjsfFrame->jawPC ++;
}




/** * * * * * A T H R O W * * * * * **/


JVM_VOID JvmRtByteCode_athrow__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
   register  JAVA_WORD  * pjawOpStack  = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
   register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
   register _JVM_Method * pjmMethod = 
     pjeeExecEnv->pjsfFrame->pjmCurrMethod;
   JAVA_OBJREF joObjRef = NULL;
   
   _JVM_Object *joExcObject, *joClassObject;

   joExcObject = (_JVM_Object *)pjawOpStack[ pjeeExecEnv->ju32OpStackTop ];

   if ( joExcObject == JAVA_NULL )
	   JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

   if ( !JvmLdrLoadNewClass( JVM_SPRING_CREATE("java/lang/Throwable"), JVM_NULL, 
		pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,&joClassObject))
	   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

   if ((joClassObject->joiObjectID.pjccClass == joExcObject->joiObjectID.pjccClass ) ||
	  ( IsSubclassOf( joClassObject->joiObjectID.pjccClass,
	                  joExcObject->joiObjectID.pjccClass   )))
   {
	   /** CHECK,CHECK : This portion not completed yet **/
/*	   _JVM_EXC_Table_Info * pjetiExcInfo = 
	    pjsfCurrFrame->pjmCurrMethod->pjciCodeInfo->ppjetiExceptionTable; 
	   if(!pjetiExcInfo) {
	   	   if ( pjmMethod->jwMethodAccessFlags & ACC_SYNCHRONIZED ) {
            joObjRef = (JAVA_OBJREF)pjeeExecEnv->pjsfFrame->pjawLocals[0];
            if (!JvmReleaseLock( pjeeExecEnv->pjtsThreadState, joObjRef ))
	            JVM_THROW_EXCEPTION( EXC_JAVAILLEGALMONITORSTATE );
         }
         JvmRtDestroyStackFrame();
         JvmRtDestroyExecEnv(JVM_NULL);
         
	   }
	   */
	  }
	   
	   
	    /*jawIndex = pjetiExcInfo->jwCatchType;
	    pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

      if ( JVM_CP_TAG_CLASS != CP_TAG( pjbyCPByte ))
	        JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
      }

     JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR ); */
}



/** * * * * * C H E C K C A S T * * * * * **/


JVM_VOID JvmRtByteCode_checkcast__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
   register  JAVA_WORD  * pjawOpStack  = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
   register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
   register  JAVA_WORD    jawIndex;
   register  JVM_U16      ju16Index;
   register  JVM_PBYTE    pjbyCPByte;
   	register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
   JVM_STRING jsClassName;
   JVM_BYTE   jbyS, jbyT;
   JVM_BOOL   jbFlag;
  _JVM_Object * joObjRef,*joClassObject;

   

   joObjRef = (JAVA_OBJREF)pjawOpStack[ (pjeeExecEnv->ju32OpStackTop)];
   jawIndex = ( pjbyThisCode[1] << 8 ) |  pjbyThisCode[2];

   pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

   if ( JVM_CP_TAG_CLASS != CP_TAG( pjbyCPByte ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

   ju16Index  = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
   pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

  if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyCPByte ))
     JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

 
  if ( JvmLdrGetUtf8Bytes( pjbyCPByte, &jsClassName, JVM_NULL ) )
  {
	  if ( jsClassName[0] == '[' )
	  {
        if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, jsClassName, &joClassObject ))     
	    	 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	  }
	  else
	  {
	     if ( !JvmLdrLoadNewClass( jsClassName, JVM_NULL, 
			pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,&joClassObject))
			  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	  }
  }


  if ( joObjRef != JAVA_NULL )
  {

	jbyS = joObjRef->joiObjectID.jbyID;
	jbyT = joClassObject->joiObjectID.jbyID;

	if ( jbyS == REF_TYPE_CLASS )
	{
		if ( jbyT == REF_TYPE_CLASS )
		{
			if (joObjRef->joiObjectID.pjccClass == joClassObject->joiObjectID.pjccClass)
				jbFlag = 1;
			else
			{
				jbFlag = IsSubclassOf( joObjRef->joiObjectID.pjccClass, 
				joClassObject->joiObjectID.pjccClass);
			}
		}
		else if ( jbyT == REF_TYPE_INTERFACE )
		{
			jbFlag = IsInterfaceImplementedBy(joObjRef->joiObjectID.pjccClass,
				joClassObject->joiObjectID.pjccClass );
		}
	}
	else if ( jbyS == REF_TYPE_ARRAY )
	{
		  if ( jbyT == REF_TYPE_CLASS )
		{
			  /** Is T java/lang/Object?? **/
			jbFlag = (joClassObject->joiObjectID.pjccClass->joObjectClassObject->joiObjectID.pjccClass
				       == joClassObject->joiObjectID.pjccClass );
		}
		else if ( jbyT == REF_TYPE_ARRAY )
		{
			  jbFlag = IsAssignmentCompatible( joObjRef, joClassObject );
		}
	}
	else if ( jbyS == REF_TYPE_INTERFACE )
	{
		JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	}

  }

  if ( !jbFlag )
  {
	  JVM_THROW_EXCEPTION( EXC_JAVACLASSCAST );
  }

  pjeeExecEnv->pjsfFrame->jawPC += 3;

}







/** * * * * * I N S T A N C E O F * * * * * **/

JVM_VOID JvmRtByteCode_instanceof__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
   
   register  JAVA_WORD  * pjawOpStack  = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
   register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
   register  JAVA_WORD    jawIndex;
   register  JVM_U16      ju16Index;
   register  JVM_PBYTE    pjbyCPByte;
   register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
   JVM_STRING jsClassName;
   JVM_BYTE   jbyS, jbyT;
   JVM_BOOL   jbFlag;
  _JVM_Object *joClassObject,*joObjRef;

   joObjRef = (JAVA_OBJREF)pjawOpStack[ (pjeeExecEnv->ju32OpStackTop)--];
   jawIndex = ( pjbyThisCode[1] << 8 ) |  pjbyThisCode[2];

   pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

   if ( JVM_CP_TAG_CLASS != CP_TAG( pjbyCPByte ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

   ju16Index  = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
   pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

  if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyCPByte ))
     JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

 
  if ( JvmLdrGetUtf8Bytes( pjbyCPByte, &jsClassName, JVM_NULL ) )
  {
	  if ( jsClassName[0] == '[' )
	  {
        if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, jsClassName, &joClassObject ))     
	    	 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	  }
	  else
	  {
	     if ( !JvmLdrLoadNewClass( jsClassName, JVM_NULL, 
			pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,&joClassObject))
			  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	  }
  }

  jbyS = joObjRef->joiObjectID.jbyID;
  jbyT = joClassObject->joiObjectID.jbyID;

  if ( jbyS == REF_TYPE_CLASS )
  {
	if ( jbyT == REF_TYPE_CLASS )
	{
      if (joObjRef->joiObjectID.pjccClass == joClassObject->joiObjectID.pjccClass)
	     jbFlag = 1;
      else
	  {
	    jbFlag = IsSubclassOf( joObjRef->joiObjectID.pjccClass, 
	       joClassObject->joiObjectID.pjccClass);
	  }
	}
	else if ( jbyT == REF_TYPE_INTERFACE )
	{
	   jbFlag = IsInterfaceImplementedBy(joObjRef->joiObjectID.pjccClass,
			   joClassObject->joiObjectID.pjccClass );
	}
  }
  else if ( jbyS == REF_TYPE_ARRAY )
  {
	  if ( jbyT == REF_TYPE_CLASS )
	  {
		  /** Is T java/lang/Object?? **/
		  jbFlag = (joClassObject->joiObjectID.pjccClass->joObjectClassObject->joiObjectID.pjccClass
			       == joClassObject->joiObjectID.pjccClass );
	  }
	  else if ( jbyT == REF_TYPE_ARRAY )
	  {
		  jbFlag = IsAssignmentCompatible( joObjRef, joClassObject );
	  }
  }

  pjawOpStack[ ++ (pjeeExecEnv->ju32OpStackTop) ] = jbFlag;

  pjeeExecEnv->pjsfFrame->jawPC += 3;

}






/** * * * * * M O N I T E R E N T E R * * * * * **/

JVM_VOID JvmRtByteCode_moniterenter__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
   register JAVA_OBJREF joObjRef;
   register  JAVA_WORD  * pjawOpStack  = pjeeExecEnv->pjsfFrame->pjawOpStackElements;

   joObjRef = (JAVA_OBJREF)pjawOpStack[ (pjeeExecEnv->ju32OpStackTop)--];

   if ( joObjRef == JAVA_NULL )
	   JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

   JvmAcquireLock( pjeeExecEnv->pjtsThreadState, joObjRef);

   pjeeExecEnv->pjsfFrame->jawPC++;

}


/** * * * * * M O N I T E R E X I T * * * * * **/

JVM_VOID JvmRtByteCode_moniterexit__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
   register JAVA_OBJREF joObjRef;
   register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;

   joObjRef = (JAVA_OBJREF)pjawOpStack[ (pjeeExecEnv->ju32OpStackTop)--];

   if ( joObjRef == JAVA_NULL )
	   JVM_THROW_EXCEPTION( EXC_JAVANULLPOINTER );

   if (!JvmReleaseLock( pjeeExecEnv->pjtsThreadState, joObjRef ))
      JVM_THROW_EXCEPTION( EXC_JAVAILLEGALMONITORSTATE );

   pjeeExecEnv->pjsfFrame->jawPC++;

}


/** * * * * * R E T * * * * * **/

JVM_VOID JvmRtByteCode_ret__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
  register  JVM_PBYTE    pjbyThisCode  = pjeeExecEnv->pjbyCode;
  JAVA_WORD index = pjbyThisCode[1];
  
  pjeeExecEnv->pjsfFrame->jawPC = pjeeExecEnv->pjsfFrame->pjawLocals[index];
  
}


/** * * * * * I I N C * * * * * **/

JVM_VOID JvmRtByteCode_iinc__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
  register  JVM_PBYTE    pjbyThisCode  = pjeeExecEnv->pjbyCode;
  JAVA_WORD index = pjbyThisCode[1];
  JAVA_WORD constant = pjbyThisCode[2];
  pjeeExecEnv->pjsfFrame->pjawLocals[index] += constant;
  
  pjeeExecEnv->pjsfFrame->jawPC += 3;
  
}



/** * * * * * W I D E * * * * * **/

JVM_VOID JvmRtByteCode_wide__Handler(
 
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
  register  JVM_PBYTE    pjbyThisCode  = pjeeExecEnv->pjbyCode;

  switch ( pjbyThisCode[1] )
  {
  case ILOAD:
  case FLOAD:
	  JvmRtByteCode_ifload__Handler( pjeeExecEnv );
	  break;
  case LLOAD:
  case DLOAD:
	  JvmRtByteCode_ldload__Handler( pjeeExecEnv );
	  break;
  case ALOAD:
	  JvmRtByteCode_aload__Handler( pjeeExecEnv );
	  break;
  case ISTORE:
  case FSTORE:
	  JvmRtByteCode_ifstore__Handler( pjeeExecEnv );
	  break;
  case LSTORE:
  case DSTORE:
	  JvmRtByteCode_ldstore__Handler( pjeeExecEnv );
	  break;
  case ASTORE:
	  JvmRtByteCode_astore__Handler( pjeeExecEnv );
	  break;
  case RET:
	  JvmRtByteCode_ret__Handler( pjeeExecEnv );
	  break;
  case IINC:
	  JvmRtByteCode_iinc__Handler( pjeeExecEnv );
	  break;
  default:
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	  JVM_ASSERT( FALSE );
  
  }
}



/** * * * * *   M U L T I A N E W A R R A Y   * * * * * **/

JVM_VOID JvmRtByteCode_multianewarray__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
  register  JVM_PBYTE    pjbyCPByte;
  register  JAVA_WORD    jawPC                = pjeeExecEnv->pjsfFrame->jawPC; 
  register  JVM_BYTE     jbyDimensions,jbyCount;
  register  JAVA_WORD    jawIndex; 

  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;

  _JVM_Class_Class * pjccRefClass;
  _JVM_Object *      joClassObject;
  JAVA_OBJREF        joArrayObject;
  JAVA_WORD        * pjawLengths;
   JVM_U16           ju16Index;
   JVM_STRING        jsClassName;

  jbyDimensions = pjbyThisCode[3];
  if ( jbyDimensions <= 0 )
	  JVM_THROW_EXCEPTION( EXC_JAVANEGATIVEARRAYSIZE );

  pjawLengths  = JvmMemAlloc( jbyDimensions * sizeof( JAVA_WORD ));
  JVM_ALLOC_CHECK( pjawLengths );

  jawIndex = ( pjbyThisCode[1] << 8 ) |  pjbyThisCode[2];

  pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

  if ( JVM_CP_TAG_CLASS != CP_TAG( pjbyCPByte ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

  ju16Index  = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
  pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

  if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyCPByte ))
     JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
 
  if ( JvmLdrGetUtf8Bytes( pjbyCPByte, &jsClassName, JVM_NULL ) )
  {
	 if ( jsClassName[0] == '[' )
	 {
		/** Array Classes always have primitive loader as their class loader**/
		if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, jsClassName, &joClassObject ))     
		 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
	 }
	 else
	 	 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  }

  pjccRefClass = joClassObject->joiObjectID.pjccClass;

  jbyCount = jbyDimensions;

  for ( ; jbyCount; )
  {
	  jbyCount--;
	  pjawLengths[ jbyCount ] = pjawOpStack[ pjeeExecEnv->ju32OpStackTop ]; 

	  --pjeeExecEnv->ju32OpStackTop;
  }

  joArrayObject = JvmRtCreateNewArray( pjccRefClass->pjfiArrayInfo->jcBaseType,
		 jbyDimensions, pjawLengths,  pjccRefClass, 
		 pjccRefClass->joArrayBaseTypeObject->joiObjectID.pjccClass
		 );

  JvmMemFree( pjawLengths );

  pjawOpStack[ ++(pjeeExecEnv->ju32OpStackTop) ] = (JAVA_WORD) joArrayObject;
  
}



/** * * * * *   I F N U L L   * * * * * **/

JVM_VOID JvmRtByteCode_ifnull__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawPC                = pjeeExecEnv->pjsfFrame->jawPC; 
  register  JAVA_WORD    jawRefValue;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;

  jawRefValue = pjawOpStack[ pjeeExecEnv->ju32OpStackTop -- ];

  if ( 0 == jawRefValue )
  {
	 jawPC = ( pjbyThisCode[1] << 8  ) |
     	     ( pjbyThisCode[2]       );

     if ( jawPC >  pjsfCurrFrame->pjmCurrMethod->pjciCodeInfo->jdwCodeLength )
          JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  }
  else
  {
     jawPC += 3;
  }

  pjeeExecEnv->pjsfFrame->jawPC = jawPC;
}



/** * * * * *   I F N O N N U L L   * * * * * **/

JVM_VOID JvmRtByteCode_ifnonnull__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawPC                = pjeeExecEnv->pjsfFrame->jawPC; 
  register  JAVA_WORD    jawRefValue;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;

  jawRefValue = pjawOpStack[ pjeeExecEnv->ju32OpStackTop -- ];

  if ( 0 != jawRefValue )
  {
	 jawPC = ( pjbyThisCode[1] << 8  ) |
     	     ( pjbyThisCode[2]       );

     if ( jawPC >  pjsfCurrFrame->pjmCurrMethod->pjciCodeInfo->jdwCodeLength )
          JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  }
  else
  {
     jawPC += 3;
  }

  pjeeExecEnv->pjsfFrame->jawPC = jawPC;
}



/** * * * * *   G O T O _ W   * * * * * **/

JVM_VOID JvmRtByteCode_goto_w__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawPC                = pjeeExecEnv->pjsfFrame->jawPC; 
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;

  jawPC = ( pjbyThisCode[1] << 24 ) | 
          ( pjbyThisCode[2] << 16 ) |
          ( pjbyThisCode[3] << 8  ) |
	      ( pjbyThisCode[4]       );

  if ( jawPC >  pjsfCurrFrame->pjmCurrMethod->pjciCodeInfo->jdwCodeLength )
      JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

  pjeeExecEnv->pjsfFrame->jawPC = jawPC;

}


/** * * * * *   J S R _ W   * * * * * **/

JVM_VOID JvmRtByteCode_jsr_w__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 )
{
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
  register  JAVA_WORD    jawPC                = pjeeExecEnv->pjsfFrame->jawPC; 
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;


  pjawOpStack[ pjeeExecEnv->ju32OpStackTop ++ ]  = jawPC + 5;

  jawPC = ( pjbyThisCode[1] << 24 ) | 
          ( pjbyThisCode[2] << 16 ) |
          ( pjbyThisCode[3] << 8  ) |
	      ( pjbyThisCode[4]       );
                                

  if ( jawPC >  pjsfCurrFrame->pjmCurrMethod->pjciCodeInfo->jdwCodeLength )
      JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
  

  pjeeExecEnv->pjsfFrame->jawPC = jawPC;

}



/** * * * * * D U M M Y * * * * * **/


JVM_VOID JvmRtByteCode_DUMMY__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
)
{
	 pjeeExecEnv->pjsfFrame->jawPC ++;
}

#endif /* __JDEPENDS__ */



















/**
  Case I : if S is a class type,
				- if T is a class type ,then S must be the same class as T or
				   S must be a subclass of T
                - if T is an interface type, S must implement interface T.

  Case II: if S is an array type, namely the type SC[],i.e , an array of
             components of type SC, then:
              If T is a class type, T must be Object, Or:
		       - if T is an array type, namely the type TC[], an array of
			     components of type TC,then either TC and SC must be the
			     same primitive type, or
			   - TC and SC must both be reference types with type SC assignable 
			     to TC the this same rules.
 **/

/** CHECK,CHECK A lot of things may be wrong here */
JVM_BOOL IsAssignmentCompatible(
IN  JAVA_OBJREF jaoSource,
IN  JAVA_OBJREF jaoTarget
)
{
  _JVM_Object * jaoT = (_JVM_Object *)jaoTarget;
  _JVM_Object * jaoS = (_JVM_Object *)jaoSource;
  
  if(!jaoS || !jaoT) {
    return JVM_TRUE;
  }

  if ( jaoS->joiObjectID.jbyID == REF_TYPE_CLASS )
  {
	/* Class  */
	if ( jaoT->joiObjectID.jbyID == REF_TYPE_CLASS )
	{
	   if ( IsKindOfEx( /* T */ jaoT->joiObjectID.pjccClass,
                	    /* S */ jaoS->joiObjectID.pjccClass
				       )
	   )
	   return JVM_TRUE;
	}
	/* Interface */
	else if ( jaoT->joiObjectID.jbyID == REF_TYPE_INTERFACE )
	{
		if ( HasInterface( /* S */ jaoS->joiObjectID.pjccClass,
			               /* T */jaoT->joiObjectID.pjccClass ))
			return JVM_TRUE;
	}

  }
  else if (jaoS->joiObjectID.jbyID == REF_TYPE_ARRAY )
  {
	if (jaoT->joiObjectID.jbyID == REF_TYPE_CLASS )
	{
		/** Is T java/lang/Object ?? **/
		return jaoT->joiObjectID.pjccClass == jaoT->joiObjectID.pjccClass->joObjectClassObject->
			                                joiObjectID.pjccClass;

	}
	else if (jaoT->joiObjectID.jbyID == REF_TYPE_ARRAY ) 
	{
        if (( ((_JVM_Array_Object * )jaoS)->juType == JAVA_TYPE_REF) &&
			( ((_JVM_Array_Object * )jaoT)->juType == JAVA_TYPE_REF))
		{
			return IsAssignmentCompatible( ((_JVM_Array_Object * )jaoS)->u.pjarRef,
				   ((_JVM_Array_Object * )jaoT)->u.pjarRef );
		}
		else if (  ((_JVM_Array_Object * )jaoS)->juType == 
			       ((_JVM_Array_Object * )jaoT)->juType 
				)
		  return JVM_TRUE;
	}
  }

  return JVM_FALSE;
		
}






/** T O D O More opcode handlers  **/







/** #define JVM_STATE_CHECK( x, y)\
  if ( !JVM_VALID((x)) ) {\
	JVM_THROW_EXCEPTION((y));\
	JVM_ASSERT( JVM_FALSE );\
  }
#define JVM_ALLOC_CHECK( x )\
	JVM_STATE_CHECK(x,EXC_JVMNOMEMORY) **/


    

JVM_THD_CLB_RETURN JVM_THD_CLB_PREFIX
 JvmThreadStartRoutine( JVM_THD_CLB_PARAM jtpParam );

/** CHECK,CHECK for proper implementation **/
/** Thread related functions **/

JVM_THREAD JvmGetCurrentThread()
{
  JVM_THREAD jtHandle = NULL;
  jtHandle = JvmSysGetCurrentThread();
  return jtHandle;
}

_JVM_Thread_State * JvmGetThreadState()
{
   JVM_THREAD jtHandle = JvmGetCurrentThread();
   _JVM_Thread_State *pjtsState = NULL;

   JVM_ASSERT( jtHandle != NULL);

   JvmSysAcquireMutex( _jmoGGlobalDataMutex );

   /* Access the Thread list */
   pjtsState = _pjtsGStartThread;

   /* Loop thro' until we found the right thread state */
   while ( pjtsState != NULL )
   {
     if ( pjtsState->jtHandle == jtHandle ) break;
     else
  	    pjtsState = pjtsState->pjtsNext;
   }

   JvmSysReleaseMutex( _jmoGGlobalDataMutex );

   /* New Thread */
   if ( pjtsState == NULL )
   	  pjtsState = JvmThreadInit(jtHandle);


   return pjtsState;
 
}

_JVM_Thread_State * JvmThreadInit(JVM_THREAD jtHandle)
{
  _JVM_Thread_State *pjtsThreadState = JVM_NULL;
  _JVM_Thread_State *pjtsState       = JVM_NULL;

  JVM_ASSERT_VALID( jtHandle );

  /* Can't use JVM-TRY-CATCH just yet */
   pjtsThreadState = (_JVM_Thread_State * )
	                 JvmMemCalloc( sizeof(_JVM_Thread_State) );
   pjtsThreadState->jtHandle = jtHandle;
   pjtsThreadState->jnThreadPriority =  JvmSysSetThreadPriority(jtHandle,
									      JVM_THREAD_NORMAL_PRIORITY);
 
   // should be NULL now ; will be initialized
   // by appropriate functions
   pjtsThreadState->pjefTopFrame          = JVM_NULL;
   pjtsThreadState->pjeiErrorStack        = JVM_NULL;
   pjtsThreadState->pjdehDefExHandler     = JvmDefaultExceptHandler;
   pjtsThreadState->jbTempBuf[ 0 ]        = 0;
   pjtsThreadState->jwThreadState         = JVM_THREAD_RUNNING;

   /** Stack frame manipulators **/
   pjtsThreadState->pjsfFrames            = JVM_NULL;
   pjtsThreadState->juNextFrame           = 0;
   pjtsThreadState->juAllocFrames         = 0;

 
   JvmSysAcquireMutex( _jmoGGlobalDataMutex );

   pjtsThreadState->pjtsNext = _pjtsGStartThread;

   _pjtsGStartThread    = 	pjtsThreadState;

   JvmSysReleaseMutex( _jmoGGlobalDataMutex );

   return pjtsThreadState;
}

JVM_VOID JvmThreadExit( _JVM_Thread_State *pjtsThread )
{
  JVM_ASSERT_VALID( pjtsThread );

  /* To be continued after runtime implementation
     becomes clear 
  */

}

JVM_THREAD JvmCreateThread()
{
	JVM_THREAD jtHandle = JvmSysCreateThread(JvmThreadStartRoutine , 0 );
	JVM_ASSERT_VALID( jtHandle );
	JvmThreadInit( jtHandle );
	return jtHandle;
}

JVM_VOID JvmDestroyThread(_JVM_Thread_State *pjtsThread)
{
	JVM_ASSERT_VALID( pjtsThread );
	JvmSysDestroyThread( pjtsThread->jtHandle );
	pjtsThread->jtHandle = NULL;
	JvmThreadExit( pjtsThread );
}

JVM_VOID JvmSetThreadPriority( _JVM_Thread_State *pjtsThread,
							   JVM_INT jnPriority )
{
  JVM_ASSERT_VALID( pjtsThread );
  pjtsThread->jnThreadPriority = 
   JvmSysSetThreadPriority( 
         pjtsThread->jtHandle, jnPriority );
}

JVM_VOID JvmWait(_JVM_Thread_State *pjtsThisThread ,
				 _JVM_Object *pjoObject,
				  JVM_U32 juTimeOut)
{
	_JVM_Thread_State_List *pjtslThStateList = JVM_NULL;
	JVM_U32 juPriority = 0;
	_JVM_Wait_Set *pjwsWaitSet = JVM_NULL;

	JVM_ASSERT_VALID( pjtsThisThread );
	JVM_ASSERT_VALID( pjoObject );
	JVM_ASSERT_VALID( pjoObject->pjlLock );

	JvmSysAcquireMutex( _jmoGExclCodeMutex );

	juPriority =  pjtsThisThread->jnThreadPriority;
	pjtsThisThread->juWaitTimeout = juTimeOut;

	if ( !JvmIsOwnerThread(pjtsThisThread, pjoObject ) ){
		JvmSysReleaseMutex( _jmoGExclCodeMutex );
		/* Throw IllegalMonitorStateException */
		JVM_THROW_EXCEPTION( EXC_JAVAILLEGALMONITORSTATE );
		/* Should never be reached */
		JVM_ASSERT( FALSE );
	}

	/* Save the lock count for the object,
	   The thread will use this to lock the 
	   object (ju16LockCount times ) 
	   when it gets a chance
	*/
	pjtsThisThread->jwLockCount = 
		pjoObject->pjlLock->jwLockCount;

	/* Unlock the object and remove ownership */
	pjoObject->pjlLock->jwLockCount = 0;
	pjoObject->pjlLock->pjtsOwnerThread = NULL;

	pjwsWaitSet = pjoObject->pjwsWaitSet; 
	JVM_ASSERT( pjwsWaitSet != NULL);

	pjtslThStateList = JvmAllocNewThreadStateList();
	pjtslThStateList->pjtsThisThread
			  = pjtsThisThread;
	pjtslThStateList->pjtslNext = 
		    pjwsWaitSet->jplList[ juPriority ].pjtslFirstThread;

	if ( pjtslThStateList->pjtslNext == NULL )
  	 pjwsWaitSet->jplList[ juPriority ].pjtslLastThread 
	  	 = pjtslThStateList;
	else 
      pjtslThStateList->pjtslNext->pjtslPrev = pjtslThStateList;

	pjwsWaitSet->jplList[ juPriority ].pjtslFirstThread 
		  = pjtslThStateList;
	/* it is critical to release the lock before
	   sleeping 
	*/
	JvmSysPrepareToSuspendThread(pjtsThisThread);

	JvmSysReleaseMutex( _jmoGExclCodeMutex );

	/* Go into a CPU non-intensive wait state */
	JvmSysSuspendThread(pjtsThisThread,0);

	JvmSysAcquireMutex( _jmoGExclCodeMutex );

	JvmSysSuspendThreadCleanUp(pjtsThisThread);

	JvmSysReleaseMutex( _jmoGExclCodeMutex );
}

JVM_VOID JvmNotify(_JVM_Thread_State *pjtsThreadState ,
				   _JVM_Object *pjoObject)
{
    _JVM_Thread_State_List *pjtslSelectThread = NULL;
	JVM_U32 juPriority = pjtsThreadState->jnThreadPriority;
	_JVM_Wait_Set *pjwsWaitSet = NULL;
	JVM_U16 ju16ActivePriority[ JVM_THREAD_MAX_PRIORITY - 1 ]; 
	JVM_U16 ju16Index = 0;

	JvmSysAcquireMutex( _jmoGExclCodeMutex );

	JVM_ASSERT_VALID( pjoObject );

	if ( !JvmIsOwnerThread(pjtsThreadState, pjoObject ) ){
	    JvmSysReleaseMutex( _jmoGExclCodeMutex );
		/* Throw IllegalMonitorStateException */
		JVM_THROW_EXCEPTION( EXC_JAVAILLEGALMONITORSTATE );
		/* Should never be reached */
		JVM_ASSERT( FALSE );
	}

	pjwsWaitSet = pjoObject->pjwsWaitSet; 
	JVM_ASSERT_VALID( pjwsWaitSet );

	JVM_ASSERT_VALID( pjoObject->pjlLock );

	/* if there are no threads waiting on the object, 
	 * return 
	 */
    if ( JvmIsWaitSetEmpty( pjwsWaitSet,&juPriority ) ) {
	  JvmSysReleaseMutex( _jmoGExclCodeMutex );
	  return;
	}

	for ( ;juPriority < JVM_THREAD_MAX_PRIORITY; juPriority ++)
	  if ( pjwsWaitSet->jplList[ juPriority ].pjtslFirstThread ) 
	    ju16ActivePriority[ ju16Index++ ] = juPriority;

    JVM_ASSERT( ju16Index <= 0 );
	/* Select a random priority */
	juPriority = ju16ActivePriority[ JvmSysGetRandom() % ju16Index ];
	/* Select the last thread */
	pjtslSelectThread = 
		pjwsWaitSet->jplList[ juPriority ].pjtslLastThread; 
    pjtslSelectThread->pjtslPrev->pjtslNext = NULL;

	pjwsWaitSet->jplList[ juPriority ].pjtslLastThread
		= pjtslSelectThread->pjtslPrev;

	pjtsThreadState = pjtslSelectThread->pjtsThisThread;

	/* Free the thread state list block */
	JvmFreeThreadStateList( pjtslSelectThread );
	/* Actually notify the thread */
	JvmSysResumeThread(pjtsThreadState);
	/* release exclusive mutex */
	JvmSysReleaseMutex( _jmoGExclCodeMutex );
}

/* Notify All */

JVM_VOID JvmNotifyAll(_JVM_Thread_State *pjtsThreadState ,
				      _JVM_Object *pjoObject)
{
    _JVM_Thread_State_List *pjtslSelectThread = NULL;
	JVM_U32 juPriority = 0;
	_JVM_Wait_Set *pjwsWaitSet = NULL;
	JVM_U16 ju16Index = 0;

	JVM_ASSERT_VALID( pjoObject );
	JVM_ASSERT_VALID( pjtsThreadState );

	JvmSysAcquireMutex( _jmoGExclCodeMutex );

	if ( !JvmIsOwnerThread(pjtsThreadState, pjoObject ) ){
	    JvmSysReleaseMutex( _jmoGExclCodeMutex );
		/* Throw IllegalMonitorStateException */
		JVM_THROW_EXCEPTION( EXC_JAVAILLEGALMONITORSTATE );
		/* Should never be reached */
		JVM_ASSERT( FALSE );
	}

    pjwsWaitSet = pjoObject->pjwsWaitSet; 

	JVM_ASSERT_VALID( pjwsWaitSet );
	JVM_ASSERT_VALID( pjoObject->pjlLock );

	/* if there are no threads waiting on the object, 
	 * return 
	 */
    if ( JvmIsWaitSetEmpty( pjwsWaitSet , &juPriority) ) {
	  JvmSysReleaseMutex( _jmoGExclCodeMutex );
	  return;
	}

	/* otherwise loop and wake up all the threads */

	for ( ;juPriority < JVM_THREAD_MAX_PRIORITY; juPriority ++)
	{
	  if ( pjwsWaitSet->jplList[ juPriority ].pjtslFirstThread ) 
	  {
		  do
		  {
			/* Select the last thread */
            pjtslSelectThread = 
				pjwsWaitSet->jplList[ juPriority ].pjtslFirstThread;    

			pjwsWaitSet->jplList[ juPriority ].pjtslFirstThread =
				pjtslSelectThread->pjtslNext;

			pjtsThreadState = pjtslSelectThread->pjtsThisThread;

         	/* Free the thread state list block */
	        JvmFreeThreadStateList( pjtslSelectThread );

	        /* Actually notify the thread */
	        JvmSysResumeThread(pjtsThreadState);

		  } while ( pjwsWaitSet->jplList[ juPriority ].pjtslFirstThread );

		 pjwsWaitSet->jplList[ juPriority ].pjtslLastThread = JVM_NULL;

	  }
	}

	/* release mutex */
	JvmSysReleaseMutex( _jmoGExclCodeMutex );
}


/* A thread may call this function to acquire the lock
   for the object,it will be added to the entry set
*/
JVM_VOID JvmAcquireLock(_JVM_Thread_State *pjtsThisThread ,
						_JVM_Object *pjoObject )
{
   _JVM_Thread_State_List *pjtslThStateList = NULL;
   _JVM_Entry_Set *pjesEntrySet = NULL;
   _JVM_Thread_State *pjtsOwnerThread = NULL;
    JVM_U32 juPriority = 0;
   

    JVM_ASSERT( pjtsThisThread != NULL );
	JVM_ASSERT( pjoObject != NULL );
	JVM_ASSERT( pjoObject->pjlLock != NULL );

	/* Synchronize with our threads first */
    JvmSysAcquireMutex( _jmoGExclCodeMutex );

	juPriority = pjtsThisThread->jnThreadPriority;


	pjtsOwnerThread =  pjoObject->pjlLock->pjtsOwnerThread;

	/* If the lock is available assign it to the 
	   current thread and mark it as owner */
	if (pjtsOwnerThread == NULL || pjtsOwnerThread == pjtsThisThread )
	{
	  /* First time acquiring a lock on this object */
	   if ( pjtsOwnerThread == NULL )
	      pjoObject->pjlLock->jwThreadCount++;

	  pjoObject->pjlLock->pjtsOwnerThread = pjtsThisThread;
	  pjoObject->pjlLock->jwLockCount++;
	  
      JvmSysReleaseMutex( _jmoGExclCodeMutex );
	  return;
	}

	/* Else, Add this thread to the entry set 
	   priority queue and wait for the lock to be released 
	 */

	pjesEntrySet = pjoObject->pjesEntrySet;
	JVM_ASSERT( pjesEntrySet != NULL );

	/* Add this thread to the entry */
	pjtslThStateList = JvmAllocNewThreadStateList();
	pjtslThStateList->pjtsThisThread
			  = pjtsThisThread;

	pjtslThStateList  =  pjesEntrySet->jplList[ juPriority ].pjtslLastThread;

	if ( pjtslThStateList  == NULL )
 	   pjesEntrySet->jplList[ juPriority ].pjtslFirstThread =
	     pjtslThStateList;

	if ( pjtslThStateList )
	  pjtslThStateList->pjtslNext = pjtslThStateList;

	pjesEntrySet->jplList[ juPriority ].pjtslLastThread = pjtslThStateList;

	JvmSysPrepareToSuspendThread(pjtsThisThread);

  	JvmSysReleaseMutex( _jmoGExclCodeMutex );

	/* Wait/sleep here until some other thread wakes us */
	JvmSysSuspendThread(pjtsThisThread,0);

	JvmSysAcquireMutex( _jmoGExclCodeMutex );

	JvmSysSuspendThreadCleanUp(pjtsThisThread);

	JvmSysReleaseMutex( _jmoGExclCodeMutex );
	/* we are free to execute now */
	/* Recursively call ourselves until we acquire the lock */
	JvmAcquireLock(pjtsThisThread,pjoObject);
}

JVM_BOOL JvmReleaseLock(_JVM_Thread_State *pjtsThisThread ,
						_JVM_Object *pjoObject   )
{
   _JVM_Thread_State_List *pjtslThreadState = NULL;
   _JVM_Thread_State_List *pjtslSelectThread = NULL;
   _JVM_Entry_Set *pjesEntrySet = NULL;
    JVM_U32 juPriority = 0;
	JVM_U32 juMaxPriority = 0;

    JVM_ASSERT( pjtsThisThread != NULL );
	JVM_ASSERT( pjoObject != NULL );
	JVM_ASSERT( pjoObject->pjlLock != NULL );

	/* Synchronize with our threads first */
    JvmSysAcquireMutex( _jmoGExclCodeMutex );

	juPriority = pjtsThisThread->jnThreadPriority;

	if ( !JvmIsOwnerThread(pjtsThisThread, pjoObject) )
	{
    	JvmSysReleaseMutex( _jmoGExclCodeMutex );
	    return JVM_FALSE;
    }

	 if ( pjoObject->pjlLock->jwLockCount )
	    pjoObject->pjlLock->jwLockCount--;

	 if ( pjoObject->pjlLock->jwLockCount == 0)
	 {
       pjoObject->pjlLock->pjtsOwnerThread = NULL;
	   /* Update the thread count */
	   pjoObject->pjlLock->jwThreadCount--;
	 }

	 /* Return, if there are no more threads
	    waiting for the object's lock */
	 if ( pjoObject->pjlLock->jwThreadCount == 0)
	 {
		 JvmSysReleaseMutex( _jmoGExclCodeMutex );
		 return JVM_TRUE;
	 }

    /* Search thro the priority queues to select the 
	   next thread to hand over control of the object */
	 
   	pjesEntrySet = pjoObject->pjesEntrySet;
    JVM_ASSERT( pjesEntrySet != NULL );

	for ( ;juPriority < JVM_THREAD_MAX_PRIORITY; juPriority ++)
	{
   	  pjtslSelectThread  = 
	     pjesEntrySet->jplList[ juPriority ].pjtslFirstThread;

	  if ( pjtslSelectThread )
	  	  if ( juMaxPriority < juPriority )
		      juMaxPriority = juPriority;
	}  

	  
	 pjtslSelectThread  = 
	     pjesEntrySet->jplList[ juMaxPriority ].pjtslFirstThread;

	 /* pjtsSelectThread now holds the next thread to 
 	    unleash */

	 /* Remove it from the entry set */
	 if ( pjtslSelectThread )
	   pjesEntrySet->jplList[ juMaxPriority ].pjtslFirstThread = 
	      pjtslSelectThread->pjtslNext;

	 /* Unleash the selected thread */
     JvmSysResumeThread(pjtslSelectThread->pjtsThisThread);

	 /* Free the thread memory */
	 JvmFreeThreadStateList( pjtslSelectThread );

	 /* remove our lock */
	JvmSysReleaseMutex( _jmoGExclCodeMutex );

	return JVM_TRUE;

}

JVM_VOID JvmSleep(JVM_U32 juMilliseconds)
{
   JvmSysSleep( JvmGetThreadState(),juMilliseconds );
}



JVM_BOOL JvmIsWaitSetEmpty( _JVM_Wait_Set *pjwsWaitSet,
						     JVM_U32 *pjwStartPriority 
					   	  )
{
	JVM_BOOL jbEmpty = TRUE;
	JVM_INT  juPriority = 0;

	JVM_ASSERT_VALID( pjwsWaitSet );

	for ( ;juPriority < JVM_THREAD_MAX_PRIORITY;juPriority++ )
    {
   	 if ( pjwsWaitSet->jplList[ juPriority ].pjtslFirstThread )
	 {
	  jbEmpty = FALSE;
	  /* priority of the first non-null priority list */
	  *pjwStartPriority = juPriority;
	  break;
	 }
    }

	return jbEmpty;
}



JVM_BOOL JvmIsOwnerThread(_JVM_Thread_State *pjtsThisThread,
						  _JVM_Object *pjoObject)
{
	JVM_ASSERT_VALID( pjtsThisThread );
	JVM_ASSERT_VALID( pjoObject );
	return pjtsThisThread == 
		 pjoObject->pjlLock->pjtsOwnerThread;
}


_JVM_Thread_State_List * JvmAllocNewThreadStateList()
{
	return (_JVM_Thread_State_List *)JvmMemAlloc( sizeof(_JVM_Thread_State_List) ); 
}

JVM_VOID JvmFreeThreadStateList( _JVM_Thread_State_List *pjtslList )
{
	JvmMemFree( (JVM_ULONG *)pjtslList );
}

#ifndef __JDEPENDS__


JVM_BOOL IsValidMethodOf( 
 IN      _JVM_Class_Class   * pjccClass,
 IN      JVM_STRING         jsMethodName,
 IN      JVM_STRING         jsMethodDesc, 
 OUT     _JVM_Method     ** ppjmMethod
 )
{
	_JVM_Method * pjmMethod ;
	 JVM_U16 ju16MethodIndex = 0;

    JVM_ASSERT_VALID( pjccClass );
	JVM_ASSERT_VALID( jsMethodName );
	JVM_ASSERT_VALID( ppjmMethod );
    *ppjmMethod = JVM_NULL;

	for ( ;ju16MethodIndex < pjccClass->jwMethodsCount;  ju16MethodIndex ++ )
	{
	   pjmMethod = pjccClass->ppjmMethods[ ju16MethodIndex ];
	   if ( JvmSpringStrCmp( pjmMethod->jsMethodName, jsMethodName ) == 0 )  
	   {
		   if ( JvmSpringStrCmp( pjmMethod->jsMethodDesc,jsMethodDesc ) == 0 )  
       {
		     *ppjmMethod = pjmMethod;
		     return JVM_TRUE;
       }
	   }
	}

    return JVM_FALSE;
}



JVM_BOOL JvmGetNearestBaseMethod( 
 IN  _JVM_Class_Class * pjccTheClass,
 IN   JVM_STRING jsMethodName,
 IN   JVM_STRING jsMethodDesc,
 OUT _JVM_Method ** ppjmMethod, 
 OUT _JVM_Class_Class ** ppjccClass
)
{
   _JVM_Class_Class *pjccAClass = JVM_NULL;

	JVM_ASSERT_VALID( pjccTheClass );
	JVM_ASSERT_VALID( ppjmMethod );
	JVM_ASSERT_VALID( ppjccClass );

	pjccAClass = pjccTheClass;

	/** Loop until we reach 'Object'( with NULL super) **/
	do
	{
    if ( !pjccAClass->joSuperClassObject )
        break;
    pjccAClass = pjccAClass->joSuperClassObject->joiObjectID.pjccClass;
		/** Check if the method is a valid method of pjccAClass **/
		if ( pjccAClass &&  IsValidMethodOf( pjccAClass, 
			     jsMethodName, jsMethodDesc, ppjmMethod ))
		{
			*ppjccClass = pjccAClass;
			return JVM_TRUE;
		}

	} while ( pjccAClass != JVM_NULL );

	return JVM_FALSE;
}


JVM_VOID JvmRtDestroyStackFrame()
{
   register _JVM_Thread_State * pjtsThdState = JvmGetThreadState();
   _JVM_Stack_Frame * pjsfFrame = pjtsThdState->pjsfCurrFrame;

   JVM_ASSERT_VALID( pjsfFrame );

   if ( JVM_VALID( pjsfFrame->pjawOpStackElements ) )
     JvmMemFree( pjsfFrame->pjawOpStackElements );

   if ( JVM_VALID( pjsfFrame->pju32StackMap ) )
     JvmMemFree( pjsfFrame->pju32StackMap );

   if ( JVM_VALID( pjsfFrame->pjawLocals ) )
     JvmMemFree( pjsfFrame->pjawLocals );

   pjtsThdState->pjsfCurrFrame = 
	       pjtsThdState->pjsfCurrFrame->pjsfPrevFrame;
   
   pjtsThdState->juNextFrame--;
}




/** CHECK,CHECK for errors **/
JVM_VOID JvmRtSetUpStackFrame( 
  IN    _JVM_Method *pjmMethod,
  IN    _JVM_Class_Class *pjccTheClass 
)
{
   register _JVM_Thread_State *pThreadState= JvmGetThreadState();
   register _JVM_Stack_Frame *pjsfFrame = JVM_NULL;
   register _JVM_ByteCode_Info *pjbiCodeInfo = JVM_NULL;
   register _JVM_LV_Table_Info *pjltiLocal  = JVM_NULL;
   register  JVM_INT   jiIndex = 0;

   JVM_ASSERT_VALID( pjmMethod );
   JVM_ASSERT_VALID( pjccTheClass );

   pjsfFrame = JvmRtGetNextStackFrame( pThreadState );
   JVM_ALLOC_CHECK( pjsfFrame );

   pjsfFrame->pjmCurrMethod  = pjmMethod;
   pjsfFrame->pjccCurrClass   = pjccTheClass;
   /** This will become dynamic later **/
   pjsfFrame->ju32OpStackSize  = JvmConfigGetSize( CONFIG_ID_OP_STACK );

   pjsfFrame->pjawOpStackElements = JvmMemAlloc( sizeof( JAVA_WORD ) * pjsfFrame->ju32OpStackSize );
   JVM_ALLOC_CHECK( pjsfFrame->pjawOpStackElements )

   /** CHECK,CHECK Probably add a conditional construct to check if 
       local var exceeded max limit **/
   if(!(pjmMethod->jwMethodAccessFlags & ACC_NATIVE)) {
      pjsfFrame->ju32LocalVarsSize = 1 + pjmMethod->pjmiMethodInfo->jiParamsWidthInWords 
                                      + (pjmMethod->pjciCodeInfo ? 
                                        pjmMethod->pjciCodeInfo->jiLocalVarsWidthInWords : 0);

      /** Time for allocation of local vars **/
       
      pjsfFrame->pjawLocals = JvmMemCalloc( sizeof( JAVA_WORD ) *  pjsfFrame->ju32LocalVarsSize);
      JVM_ALLOC_CHECK(  pjsfFrame->pjawLocals )

      pjsfFrame->ju32TopElementIndex = 0;
      pjsfFrame->jawPC  = 0;
      
      /** Create a stack map -a part of the 'root set'- for Garbage collection **/
      JvmCreateStackMap( pjmMethod, &pjsfFrame->pju32StackMap,
		     pjsfFrame->ju32LocalVarsSize );
   }

   /** Finally make the frame current **/
   pjsfFrame->pjsfPrevFrame = pThreadState->pjsfCurrFrame;
   pThreadState->pjsfCurrFrame  = pjsfFrame; 
}


_JVM_Stack_Frame * JvmRtGetNextStackFrame( 
  IN _JVM_Thread_State * pjtsThreadState 
)
{
    /** Re size Frame array if necessary **/
	if ( pjtsThreadState->juNextFrame >= pjtsThreadState->juAllocFrames )
	{
      _JVM_Stack_Frame * psfTemp =
      JvmMemRealloc( pjtsThreadState->pjsfFrames, 
		  ( pjtsThreadState->juAllocFrames + FRAME_BLOCK_SIZE ) *
		  sizeof( _JVM_Stack_Frame ));
	  if ( psfTemp ) {
        pjtsThreadState->pjsfFrames = psfTemp;   
		pjtsThreadState->juAllocFrames += FRAME_BLOCK_SIZE;
	  }
	}

	return &pjtsThreadState->pjsfFrames[ pjtsThreadState->juNextFrame++ ];
}



JVM_BOOL  JvmCreateStackMap( 
  IN OUT _JVM_Method *pjmMethod, 
  IN      JVM_U32 ** ppju32StackMap,
  IN      JVM_U32    ju32Count  
)
{
   _JVM_LV_Table_Info *  pjltiLVInfo = JVM_NULL;
   _JVM_Field_Info * pjfiFieldInfo;
    JVM_WORD jwCount, jwIndex, jwTemp;

   JVM_ASSERT_VALID( pjmMethod );
   JVM_ASSERT_VALID( ppju32StackMap );

   /** ju32Count should atleast account for 'this' ptr **/
   JVM_ASSERT( ju32Count > 0 );

   *ppju32StackMap = JvmMemCalloc( sizeof( JVM_U32 ) * ju32Count );
   JVM_ALLOC_CHECK( *ppju32StackMap )

   jwIndex = 0;
   (*ppju32StackMap)[jwIndex++] = IS_A_REFERENCE;
   
  for ( jwCount = 0; jwCount < pjmMethod->pjmiMethodInfo->jiParameterCount ;
          jwCount ++ )
  {
    pjfiFieldInfo = pjmMethod->pjmiMethodInfo->pjfiParameterFields[jwCount];
    if ( pjfiFieldInfo->jwFieldType & JVM_OBJECT_TYPE )
	{
		for ( jwTemp = 0; jwTemp < pjfiFieldInfo->jiWidthInWords ; jwTemp++ )
             (*ppju32StackMap)[jwIndex + jwTemp] =  IS_A_REFERENCE;
	}

    jwIndex += pjfiFieldInfo->jiWidthInWords; 

  }

  
  for ( jwCount = 0; jwCount < pjmMethod->pjciCodeInfo->jwLocalVarTableCount ; jwCount ++ )
  {
     pjltiLVInfo = pjmMethod->pjciCodeInfo->ppjltiLocalVarTable[ jwCount ];
	 if ( pjltiLVInfo->pjfField->jfiFieldType.jwFieldType & JVM_OBJECT_TYPE )
	 {
		 (*ppju32StackMap)[ jwIndex ] = IS_A_REFERENCE;
	 }
	 jwIndex++;
  }

   return JVM_TRUE; 
}





JVM_VOID  JvmRtByteCode_invokevirtual__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
)
{
  register _JVM_Thread_State *pjtsThreadState = pjeeExecEnv->pjtsThreadState;
  register  JVM_U32  ju32StackTop             = pjeeExecEnv->ju32OpStackTop;
  register _JVM_Stack_Frame  *pjsfCurrFrame   = pjeeExecEnv->pjsfFrame;
  register  JAVA_WORD  * pjawOpStack          = pjeeExecEnv->pjsfFrame->pjawOpStackElements;
  register  JVM_PBYTE    pjbyCPByte;         
  register  JAVA_OBJREF  pjaoObjRef;   
  register  JVM_PBYTE    pjbyThisCode         = pjeeExecEnv->pjbyCode;
  JAVA_WORD  * pjawArgsArray, jawIndex;
  JVM_INT    jiArgs,jiCount,jiIndex = 0;
  JVM_U16    ju16Index;
  JVM_STRING jsClassName, jsMethodName, jsMethodType;
  JAVA_OBJREF joClassObject = JVM_NULL;

  _JVM_Class_Class * pjccRefClass;
  _JVM_Method      * pjmRefMethod; 

  jawIndex =((pjbyThisCode[1] << 8 ) | (pjbyThisCode[2]));

  pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];

  if ( JVM_CP_TAG_METHODREF != CP_TAG( pjbyCPByte ))
	  JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

  pjccRefClass  = CP_CLASSREF( pjbyCPByte );
  pjmRefMethod =  CP_METHODREF( pjbyCPByte );

  if (!( JVM_VALID( pjccRefClass ) ) && !( JVM_VALID( pjmRefMethod )))
  {
     ju16Index = LD_WORD ( pjbyCPByte + sizeof( _JVM_Constant_Pool ));

     pjbyCPByte     = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

   	 if ( JVM_CP_TAG_CLASS != CP_TAG( pjbyCPByte ))
	    JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

   	 ju16Index  = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
   	 pjbyCPByte     = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ ju16Index ];

     if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyCPByte ))
	     JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

	 if ( JvmLdrGetUtf8Bytes( pjbyCPByte, &jsClassName, JVM_NULL ) )
	 {
           /** Load the class **/
			if ( !JvmLdrLoadNewClass( jsClassName, JVM_NULL, 
			pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,&joClassObject))
			{
				eJVMExceptionType eException;
	  			JvmLdrGetDeferredException( jsClassName, 
				pjsfCurrFrame->pjccCurrClass->joClassLoaderObject,
				&eException );
				JVM_THROW_EXCEPTION( eException );	 
			}

			/* Must have loaded the class before */
			JVM_ASSERT( joClassObject != JVM_NULL );

			/** Const_methodref **/
			pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];
			jiIndex    =  LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ) +
				                      sizeof( JVM_U16 ) );
			/** Name and Type info **/
			pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jiIndex ];
			jiIndex    = LD_WORD( pjbyCPByte + sizeof( _JVM_Constant_Pool ));
			/** UTF8 for method Name **/
    		pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jiIndex ];

			JvmGetUtf8Info( pjbyCPByte, &jsMethodName, JVM_NULL );

    		/** UTF8 index for method type **/
			jiIndex    = LD_WORD( pjbyCPByte + 
				sizeof( _JVM_Constant_Pool ) + sizeof( JVM_U16) );

			pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jiIndex ];

			JvmGetUtf8Info( pjbyCPByte, &jsMethodType, JVM_NULL );

			/** Ref Method obtained here **/
			if ( !IsValidMethodOf( ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass, 
				 jsMethodName, jsMethodType, &pjmRefMethod )
			   )
			   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 

			

			pjbyCPByte = (JVM_PBYTE)pjsfCurrFrame->pjccCurrClass->ppjcpConstantPool[ jawIndex ];
			CP_CLASSREF( pjbyCPByte ) = ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass;
			CP_METHODREF( pjbyCPByte )  = pjmRefMethod;
			pjccRefClass = ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass;

			JvmSpringFree( &jsClassName  );
			JvmSpringFree( &jsMethodName );
			JvmSpringFree( &jsMethodType );
	 }
	 else
	 {
  	   JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );	 
	   JVM_ASSERT( JVM_FALSE );
	 }

  }


      jiArgs = pjmRefMethod->pjmiMethodInfo->jiParamsWidthInWords + 1 ;

	  pjawArgsArray = JvmMemAlloc( sizeof( JAVA_WORD ) * jiArgs );
	  JVM_ALLOC_CHECK( pjawArgsArray )

	  /** CHECK,CHECK for consistency and correctness **/
	  jiCount = jiArgs;
	  for ( ; (jiCount - 1) ; )
	  {
          pjawArgsArray[ --jiCount ] = pjawOpStack[ ju32StackTop-- ];
	  }

	  pjaoObjRef = (JAVA_OBJREF)pjawOpStack[ ju32StackTop-- ];

	  /** Must acquire the lock before proceeding for
	      methods with 'synchronized' attribute 
		  will be released in one of the 'ret's
	   **/
	  if ( pjmRefMethod->jwMethodAccessFlags & ACC_SYNCHRONIZED )
	  {
		  JvmAcquireLock( pjtsThreadState, pjaoObjRef );
	  }

	  if ( pjmRefMethod->jwMethodAccessFlags & ACC_NATIVE )
	  {
		  JvmRtSaveContext( pjeeExecEnv );

		  /** Block here until native method is complete **/
		  JvmRtCallMethod( pjeeExecEnv, pjmRefMethod, pjccRefClass,
			    pjaoObjRef, pjawArgsArray, jiArgs );

		  JvmRtRestoreContext( pjeeExecEnv );
	  }
	  else
	  {
		 /** Context is changed **/
     	 /** Will be reset in all 'ret' instructions **/
 	     JvmRtSetUpStackFrame( pjmRefMethod, pjccRefClass );
	     JvmRtSetUpParameters( pjaoObjRef, pjawArgsArray, jiArgs );
	     JvmRtSetUpExecEnv( pjeeExecEnv );
		 pjeeExecEnv->pjsfFrame->jawPC = 0;
	  }


      if ( JVM_VALID( pjawArgsArray ))
	     JvmMemFree( pjawArgsArray );

	  /** Execution continues from new context **/
}

/** Ideally should save all mutable context
    states , e.g stack frame , relevent members of
	thread state etc. Be careful to include ALL
	changeable parameters to be copied.
	typically used before a native function call.
**/
JVM_VOID JvmRtSaveContext( 
  IN    _JVM_Exec_Env * pjeeExecEnv 
)
{
	JVM_ASSERT_VALID( pjeeExecEnv );
	
/** TODO **/
}


/** Restores the original copy **/
JVM_VOID JvmRtRestoreContext( 
  IN    _JVM_Exec_Env * pjeeExecEnv 
)
{
	JVM_ASSERT_VALID( pjeeExecEnv );

	/** TODO **/
}

#endif /* __JDEPENDS__ */

JVM_BOOL IsStrictValidFieldOf(
 IN      _JVM_Class_Class   * pjccClass,
 IN      JVM_STRING         jsFieldName,
 IN      JVM_STRING         jsFieldDesc, 
 OUT     _JVM_Field        ** ppjfField) 
 {
 _JVM_Field * pjfField;
  JVM_U16 ju16Index = 0;

  if(pjccClass == JVM_NULL)
    return JVM_FALSE;
    
	JVM_ASSERT_VALID( jsFieldName );
	JVM_ASSERT_VALID( ppjfField );

  *ppjfField = JVM_NULL;

	/** CHECK,CHECK - use binary search later , is the search list sorted?? **/
	for ( ;ju16Index < pjccClass->jwFieldsCount;  ju16Index ++ )
	{
	   pjfField = pjccClass->ppjfData[ ju16Index ];
	   if ( JvmSpringStrCmp( pjfField->jsFieldName, jsFieldName ) == 0 )
	   {
  		 if ( JvmSpringStrCmp( pjfField->jsFieldDesc,jsFieldDesc ) == 0 ) 
	  	 {
		      *ppjfField = pjfField;
		      return JVM_TRUE;
		   }
	   }
	}
	
  return JVM_FALSE;
 }
 
JVM_VOID JvmFieldStoreLongDoubleValue(
 IN	    _JVM_Field * pjfiField, 
 IN      JAVA_WORD   jawWordHigh,
 IN      JAVA_WORD   jawWordLow
) {
    if( pjfiField->jfiFieldType.jcBaseType == 'D') {
      /* Check this later to do the correct conversion */
      JAVA_DOUBLE  jadDouble = (JAVA_DOUBLE) ((jawWordHigh << 32) | jawWordLow);
      pjfiField->u.jad = jadDouble;
    } else if (pjfiField->jfiFieldType.jcBaseType == 'J') {
      JAVA_LONG  jalLong = WORDS_TO_LONG(jawWordHigh, jawWordLow);
      pjfiField->u.jal = jalLong;
    }
}
 
 
 
JVM_VOID JvmFieldStoreWordValue( 
 IN	    _JVM_Field * pjfiField, 
 IN      JAVA_WORD   jawWord
 )
{
	JVM_ASSERT_VALID( pjfiField );

    switch ( pjfiField->jfiFieldType.jcBaseType )
	{
  	case 'B' : pjfiField->u.jaby = ( JAVA_BYTE  ) jawWord; break;
		case 'C' : pjfiField->u.jac  = ( JAVA_CHAR  ) jawWord; break;
		case 'F' : pjfiField->u.jaf  = ( JAVA_FLOAT ) jawWord; break;
		case 'I' : pjfiField->u.jai  = ( JAVA_INT   ) jawWord; break;
		case 'S' : pjfiField->u.jas  = ( JAVA_SHORT ) jawWord; break;
		case 'Z' : pjfiField->u.jab  = ( JAVA_BOOL  ) jawWord; break;
		default  : 
			       JVM_ASSERT( JVM_FALSE );   
	}
} 

JVM_BOOL GetFieldOfClass( 
 IN      _JVM_Class_Class * pjccClass,
 IN      JVM_STRING         jsFieldName,
 IN      JVM_STRING         jsFieldDesc, 
 OUT     _JVM_Field        ** ppjfField
) {
  JVM_INT jiIndex = 0;
  if(pjccClass == JVM_NULL)
    return JVM_FALSE;
    
  for(; jiIndex < pjccClass->jwFieldsCount;  jiIndex++) {
     _JVM_Field * pjfField = pjccClass->ppjfData[jiIndex];
     
     if ( JvmSpringStrCmp( pjfField->jsFieldName, jsFieldName ) == 0 ) {
  		    if ( JvmSpringStrCmp( pjfField->jsFieldDesc,jsFieldDesc ) == 0 ) {
		        *ppjfField = pjfField;
		        return JVM_TRUE;
		      }
		  }
   }/*for*/
   
   return JVM_FALSE;
}
 
JVM_BOOL GetFieldOfObject( 
 IN      _JVM_Object * pjaoObject,
 IN      JVM_STRING         jsFieldName,
 IN      JVM_STRING         jsFieldDesc, 
 OUT     _JVM_Field        ** ppjfField
) {
  JVM_INT jiIndex = 0;
  if(pjaoObject == JVM_NULL)
    return JVM_FALSE;
    
  for(; jiIndex < pjaoObject->ClassesCount; jiIndex++) {
     JVM_INT jiIterator = 0;
     
     _JVM_Field ** ppjfField = pjaoObject->pppjfFields[jiIndex];
     for( ; jiIterator < pjaoObject->FieldsCount[jiIndex]; jiIterator++) {
        _JVM_Field * pjfField = ppjfField[jiIterator];
       
        if ( JvmSpringStrCmp( pjfField->jsFieldName, jsFieldName ) == 0 )
	      {
  		    if ( JvmSpringStrCmp( pjfField->jsFieldDesc,jsFieldDesc ) == 0 ) 
	  	    {
		        *ppjfField = pjfField;
		        return JVM_TRUE;
		      }
		    }
	   }/*for*/
   }/*for*/
   
   return JVM_FALSE;
}


JVM_BOOL IsValidFieldOf( 
 IN      _JVM_Class_Class   * pjccClass,
 IN      JVM_STRING         jsFieldName,
 IN      JVM_STRING         jsFieldDesc, 
 OUT     _JVM_Field        ** ppjfField
)
{
  if(IsStrictValidFieldOf(pjccClass, jsFieldName, jsFieldDesc, ppjfField));
    return JVM_TRUE;
   
  return IsValidFieldOf(
     pjccClass->joSuperClassObject->joiObjectID.pjccClass,
     jsFieldName,
     jsFieldDesc,
     ppjfField);
}



/** Thread Start procedure **/

JVM_THD_CLB_RETURN JVM_THD_CLB_PREFIX 
 JvmThreadStartRoutine( JVM_THD_CLB_PARAM jtpParam )
{
   /** Need to add exceptions **/
#ifndef __JDEPENDS__
   JvmRtExecute();
#endif /*  __JDEPENDS__ */
	
}


/** Execute Current Method 

  typedef struct _JVM_ByteCode_Info {
  /** CHECK,CHECK for repetition of this field 
    JVM_U16  ju16MaxLocals;
	JVM_U16  ju16MaxStack;
	JVM_U32 ju32CodeLength;
	JVM_PBYTE pjbyCode;
	JVM_U16 ju16LineNumTableCount;
	JVM_U16 ju16LocalVarTableCount;
	JVM_U16 ju16ExceptionTableCount;
   _JVM_EXC_Table_Info  **ppjetiExceptionTable;
   _JVM_LN_Table_Info   **ppjltiLineNumTable;
   _JVM_LV_Table_Info   **ppjltiLocalVarTable;
}


typedef struct _JVM_Method
{
  _JVM_Method_Info * pjmiMethodInfo;
  _JVM_ByteCode_Info   * pjciCodeInfo;
   JVM_U16 * pju16MthdExcTable;
   JVM_U16 ju16MthdExcTableCount;
   JVM_U16 ju16MethodAccessFlags;
   JVM_U16 ju16MethodCPIndex;
}_JVM_Method;

typedef struct _JVM_Exec_Env
{
  _JVM_Stack_Frame  * pjsfFrame;
  _JVM_Thread_State * pjtsThreadState;
   JVM_U32          ju32OpStackTop;
   struct _JVM_Exec_Env *pjeePrev;
}

 
  typedef struct _JVM_Stack_Frame
{
  struct _JVM_Stack_Frame *   pjsfPrevFrame;
  JVM_U32                   ju32OpStackSize;
  JAVA_WORD                *   pjawOpSElements;
  JAVA_WORD				   *   pjawLocalVars; 
  JVM_U16                  *   pju16StackMap;
 _JVM_Method			   *   pjmCurrMethod;
 _JVM_Class_Class		   *   pjccTheClass;
  JVM_U32                    ju32TopElementIndex;
  JVM_U16 					   ju16LocalVarCount;
  
  /* More things here 
}_JVM_Stack_Frame;


**/

#ifndef __JDEPENDS__

JVM_U32 JvmRtExecute()
{
   register _JVM_Thread_State *pjtsThreadState = JvmGetThreadState();

   pjtsThreadState->pjsfCurrFrame->jawPC = 0;

   JvmByteCodeExecute( pjtsThreadState );

   return 0L;

}


_JVM_Exec_Env * JvmRtSetUpExecEnv(
  IN  _JVM_Exec_Env * pjeeEnvCaller 
)
{
   _JVM_Exec_Env  *pjeeEnv; 
   _JVM_Thread_State * pjtsThreadState = JvmGetThreadState();

   /** Create a env block **/
   pjeeEnv = JvmMemAlloc( sizeof( _JVM_Exec_Env ));
   JVM_ALLOC_CHECK( pjeeEnv )

   JvmMemSet( pjeeEnv, 0, sizeof( _JVM_Exec_Env )); 

   /** Set up the current frame and info,if applicable **/
   pjeeEnv->pjsfFrame       =  pjtsThreadState->pjsfCurrFrame; 
	 pjeeEnv->ju32OpStackTop  =  pjeeEnv->pjsfFrame->ju32TopElementIndex;


   pjeeEnv->pjtsThreadState = pjtsThreadState;
   pjeeEnv->pjtsThreadState->pjeeEnvCaller   = pjeeEnvCaller;
   pjeeEnv->pjeePrev = pjeeEnvCaller;
   pjeeEnv->pjbyCode =  
     pjeeEnv->pjsfFrame->pjmCurrMethod->pjciCodeInfo ?
     pjeeEnv->pjsfFrame->pjmCurrMethod->pjciCodeInfo->pjbyCode : NULL;


   pjtsThreadState->pjeeCurrExecEnv = pjeeEnv;

   return pjeeEnv;
  
}


JVM_VOID JvmRtDestroyExecEnv()
{
	_JVM_Thread_State * pjtsThreadState = JvmGetThreadState();
	JvmMemFree( pjtsThreadState->pjeeCurrExecEnv );
  pjtsThreadState->pjeeCurrExecEnv = pjtsThreadState->pjeeEnvCaller;
  if(pjtsThreadState->pjeeEnvCaller){
     pjtsThreadState->pjeeEnvCaller = 
       pjtsThreadState->pjeeEnvCaller->pjeePrev;
  }
}


JVM_U32 JvmByteCodeExecute( 
  IN     _JVM_Thread_State *pjtsThreadState
)
{
   register JVM_PBYTE pjbyCode = JVM_NULL;
   register JVM_PBYTE pjbyByteCode = JVM_NULL;
   register JVM_U32 ju32EndPC = 0;
   register _JVM_Stack_Frame *pjsfFrame = JVM_NULL;
   

   /* Heavy assumptions about the current thread.*/
   JVM_ASSERT( pjtsThreadState == JvmGetThreadState());

   pjsfFrame = pjtsThreadState->pjsfCurrFrame;
   pjbyCode  = pjsfFrame->pjmCurrMethod->pjciCodeInfo->pjbyCode;
   ju32EndPC = pjsfFrame->pjmCurrMethod->pjciCodeInfo->jdwCodeLength;
   
   
   
   do
   {
    pjtsThreadState->pjeeCurrExecEnv->pjbyCode = 
		  pjbyByteCode = &pjbyCode[ pjsfFrame->jawPC ];
  
	  JVM_TRY
	  {
        /** Execute **/
	    (*_apfnGByteCodeHandlers[*pjbyByteCode])
			     (pjtsThreadState->pjeeCurrExecEnv);
	  }
	  JVM_CATCH_ALL_EXCEPTIONS()
	  {
		 /** Re throw the exception in Java's context **/
		 JAVA_THROW_EXCEPTION( JvmGetExceptionType() );
	  }
	  JVM_END_TRY


   	  /** These values are very very dynamic and volatile
	      => they keep changing for method invocation bytecodes
		     , native invocation that's why they are inside the loop 
			 The individual bytecode handlers change' em.
	   **/
	   
      pjsfFrame = pjtsThreadState->pjsfCurrFrame;
      pjbyCode  = pjsfFrame->pjmCurrMethod->pjciCodeInfo->pjbyCode;
      ju32EndPC = pjsfFrame->pjmCurrMethod->pjciCodeInfo->jdwCodeLength;

   } while ( pjsfFrame->jawPC < ju32EndPC );

   /** CHECK,CHECK Return values **/
   return 0L;

}
  
#endif /* __JDEPENDS__ */





/**
typedef struct _JVM_Class_Class
{
  JVM_U32     ju32ClassFlags;
  JVM_STRING    jsClassName;
  JVM_STRING    jsSourceFileName;
  JVM_U16      ju16SourceFileNameLen;
  JVM_U16      ju16FieldCount;
  JVM_U16      ju16MethodsCount;
  _JVM_Field  ** ppjfData;
  _JVM_Method ** ppjmMethods;
  Dictionary  * pdCNCPDict;
  JVM_U16     ju16CPEntryCount;

  _JVM_Constant_Pool       ** ppjcpConstantPool;
  struct _JVM_Class_Class   * pjccClassLoader;
  struct _JVM_Class_Class   * pjccSuperClass;
  struct _JVM_Class_Class   * pjccClassClass;
  struct _JVM_Class_Class   * pjccObjectClass;
  struct _JVM_Class_Class  ** ppjccInterfaces;
}_JVM_Class_Class;

  // An object of a class
typedef struct _JVM_Object
{
 _JVM_Wait_Set	     * pju16sWaitSet;
 _JVM_Entry_Set      * pjesEntrySet;
 _JVM_Lock           * pjlLock;
 _JVM_Thread_State   * pjtsOwner;
 _JVM_Class_Class    * pjccTheClass;
 _JVM_Field          *** pppjfFields;
}_JVM_Object;


typedef struct _JVM_Field_Info {
   JVM_WORD   jwFieldType;
   JVM_CHAR   jcBaseType;
   JVM_INT    jiWidthInWords;
   JVM_WORD   jwArrayDimension;
   JVM_STRING jsObjectName;
   JVM_INT    jiObjectNameLen;
   JVM_DWORD  jdwStatus;
}_JVM_Field_Info;
 
**/

/** CHECK,CHECK Should this function be threadsafe **/
_JVM_Array_Object * JvmRtCreateNewArray(
  IN   JVM_U16  juType,
  IN   JVM_U16  juDimension,
  IN   JVM_U32  * pjuLength,
  IN   _JVM_Class_Class * pjccThisArrayClass,
  IN   _JVM_Class_Class * pjccClass
)
{
	_JVM_Array_Object * pjaoArrayObject = JVM_NULL;
	 JVM_INT jiCount  = 0;
	 JVM_PVOID pjvArray;


	 JVM_ASSERT( pjuLength != 0 );

	 if ( !juDimension )
		return JVM_NULL;

	pjaoArrayObject = JvmMemCalloc( sizeof( _JVM_Array_Object ) );
	JVM_ALLOC_CHECK( pjaoArrayObject );

	pjaoArrayObject->joiObjectID.jbyID = REF_TYPE_ARRAY;
	pjaoArrayObject->joiObjectID.pjccClass  =  pjccThisArrayClass;

	if ( JVM_VALID(pjccClass) )
	{
	  /* Valid only if juType is JAVA_TYPE_REF */
	  JVM_ASSERT( juType == JAVA_TYPE_REF );

	  pjaoArrayObject->pjccArrayTypeClass = pjccClass;
	}

	pjaoArrayObject->juType = juType;
	pjaoArrayObject->jawDimension = juDimension;
    pjaoArrayObject->jawLowerBound = 0;
	

    /** WARRRRRRRRRRRRRRRRRRRRRNNNNNNNNNNNNNNNNNNIIIIIIIIIIIIINNNNNNNNNNGGGGGGGGG Array not implemented properly **/
	if (( 1 == juDimension  ) &&   pjaoArrayObject->juType )
	{
		 JAVA_WORD jawLength =  1 + pjuLength[0];
		 pjvArray = JvmMemCalloc( jawLength * 
			                     _ajuGSizes[ pjaoArrayObject->juType - JAVA_TYPE_BYTE ] + sizeof(_JVM_Object_ID) );
		 if (  juType == JAVA_TYPE_REF )
		 {
		   ((_JVM_Object *)pjvArray)->joiObjectID.jbyID = REF_TYPE_CLASS;
		   ((_JVM_Object *)pjvArray)->joiObjectID.pjccClass = pjccClass;
		 }

		 pjaoArrayObject->jawUpperBound = pjuLength[ juDimension -1 ] - 1;
		 pjaoArrayObject->jawLength = jawLength;

    	 /** Until I find a way to replace this... let switch reign supreme **/
		 switch( pjaoArrayObject->juType )
		 {
		  case JAVA_TYPE_CHAR  :
			   pjaoArrayObject->u.pjacChar  = (JAVA_CHAR *)pjvArray;
		  break;
		  case JAVA_TYPE_BYTE  :
			   pjaoArrayObject->u.pjabyByte = (JAVA_BYTE *)pjvArray;
		  break;
		  case JAVA_TYPE_SHORT  :
			   pjaoArrayObject->u.pjasShort = (JAVA_SHORT *)pjvArray;
		  break;
    	  case JAVA_TYPE_INT  :
			   pjaoArrayObject->u.pjaiInt   = (JAVA_INT *)pjvArray;
		  break;
		  case JAVA_TYPE_LONG  :
			   pjaoArrayObject->u.pjalLong  = (JAVA_LONG *)pjvArray;
		  break;
		  case JAVA_TYPE_FLOAT :
			   pjaoArrayObject->u.pjafFloat = (JAVA_FLOAT *)pjvArray;
		  break;
		  case JAVA_TYPE_DOUBLE  :
			   pjaoArrayObject->u.pjadDouble = (JAVA_DOUBLE *)pjvArray;
		  break;
		  case JAVA_TYPE_REF   :
			   pjaoArrayObject->u.pjarRef    = (JAVA_REF *)pjvArray; 
			   JvmMemSet(pjaoArrayObject->u.pjarRef, 0, jawLength * sizeof(JAVA_REF)); 
		  break;
		  default  :
			   JVM_ASSERT( JVM_FALSE );
		 }
	}
	else   /** CHECK,CHECK the order of processing multi arrays should 
		      we start in juDimension or end in it?? **/
	{
    	pjaoArrayObject->u.pjarRef = JvmMemCalloc(
			(1 + pjuLength[ juDimension - 1]) * sizeof( _JVM_Array_Object *)
 		);
		JVM_ALLOC_CHECK( pjaoArrayObject->u.pjarRef  );
    	pjaoArrayObject->u.pjarRef[0] = JvmMemCalloc( sizeof( _JVM_Object_ID ));
		JVM_ALLOC_CHECK( pjaoArrayObject->u.pjarRef[0]  );

		((struct _JVM_Object_ID *)(pjaoArrayObject->u.pjarRef[0]))->jbyID =
		    REF_TYPE_ARRAY;
		((struct _JVM_Object_ID *)(pjaoArrayObject->u.pjarRef[0]))->pjccClass = 
			pjccClass;

		pjaoArrayObject->jawLength   = pjuLength[ juDimension - 1];
		pjaoArrayObject->jawUpperBound = pjaoArrayObject->jawLength - 1;

		for ( jiCount = pjaoArrayObject->jawUpperBound; jiCount >= 0 ; 
		      jiCount-- )
		{
			pjaoArrayObject->u.pjarRef[ 1 + jiCount ] = (JAVA_ARRAYREF)
				JvmRtCreateNewArray( juType, juDimension - 1, pjuLength, pjccThisArrayClass,
				                         pjccClass ); 
		}

	}

   return pjaoArrayObject;

}

/** CHECK,CHECK for stupidities **/
JVM_VOID JvmRtDestroyAnArray( 
 IN     _JVM_Array_Object * pjaoArrayObject 
)
{
   JVM_INT jiCount;
   JVM_ASSERT_VALID( pjaoArrayObject );

   JVM_ASSERT( pjaoArrayObject->joiObjectID.jbyID == REF_TYPE_ARRAY );

   if ( pjaoArrayObject->jawDimension == 1 )
   {
	   switch( pjaoArrayObject->juType )
	   {
	    case JAVA_TYPE_CHAR  : JvmMemFree( pjaoArrayObject->u.pjacChar );  break;
		  case JAVA_TYPE_BYTE  : JvmMemFree( pjaoArrayObject->u.pjabyByte ); break;
		  case JAVA_TYPE_SHORT : JvmMemFree( pjaoArrayObject->u.pjasShort ); break;
		  case JAVA_TYPE_INT   : JvmMemFree( pjaoArrayObject->u.pjaiInt );   break;
		  case JAVA_TYPE_LONG  : JvmMemFree( pjaoArrayObject->u.pjalLong );  break;
		  case JAVA_TYPE_FLOAT : JvmMemFree( pjaoArrayObject->u.pjafFloat ); break;
		  case JAVA_TYPE_DOUBLE :JvmMemFree( pjaoArrayObject->u.pjadDouble );break;
		  case JAVA_TYPE_BOOL  : JvmMemFree( pjaoArrayObject->u.pjaiInt );   break;
		  case JAVA_TYPE_REF   : JvmMemFree( pjaoArrayObject->u.pjarRef[0] );
		                         JvmMemFree( pjaoArrayObject->u.pjarRef );   break;
		  default              :
								 JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );
								 JVM_ASSERT( JVM_FALSE );
	   }
   }
   else
   {
	  for (	jiCount = pjaoArrayObject->jawLength - 1; jiCount >= 0
		  ; jiCount-- )
	    JvmRtDestroyAnArray( pjaoArrayObject->u.pjarRef[ 1 + jiCount ] );

	  JvmMemFree( pjaoArrayObject->u.pjarRef[ 0 ] );
	  JvmMemFree( pjaoArrayObject->u.pjarRef );

   }

   JvmMemFree( pjaoArrayObject );
}




/** In-memory Instance layout

   [object ID  
    waitset
    entryset
    lock
    owner thread
    fields
     fields of this class
	 fields of super class 1
	 fields of super class 2
  	 ....
	 ....
	 fields of Object 
   ]

 **/

/** Dummy For Abstract classes and Interfaces **/

JVM_OBJREF JvmRtCreateNewInstanceEx( _JVM_Class_Class *pjccTheClass )
{
    _JVM_Object * pjoObject = JVM_NULL;
  	 JVM_INT jiIndex = 0;
	_JVM_Class_Class * pjccAClass = JVM_NULL;
	 JVM_STRING jsName = JVM_NULL;

	 JVM_ASSERT_VALID( pjccTheClass );

    if ( pjccTheClass->jdwClassFlags & ACC_ABSTRACT  ||
       pjccTheClass->jdwClassFlags & ACC_INTERFACE )
    {
       pjoObject = JvmMemCalloc( sizeof( _JVM_Object ));
	   JVM_STATE_CHECK( pjoObject, EXC_JAVAINSTANTIATIONERROR )

       pjoObject->joiObjectID.jbyID = REF_TYPE_CLASS; 
       pjoObject->joiObjectID.pjccClass    = pjccTheClass;

       pjoObject->pjwsWaitSet  = JvmMemCalloc( sizeof( _JVM_Wait_Set ));
       JVM_STATE_CHECK( pjoObject->pjwsWaitSet, EXC_JAVAINSTANTIATIONERROR )

       pjoObject->pjesEntrySet = JvmMemCalloc( sizeof( _JVM_Entry_Set ));
       JVM_STATE_CHECK( pjoObject->pjesEntrySet, EXC_JAVAINSTANTIATIONERROR )

	   pjoObject->pjlLock      = JvmMemCalloc( sizeof( _JVM_Lock ));
       JVM_STATE_CHECK( pjoObject->pjesEntrySet, EXC_JAVAINSTANTIATIONERROR )
	   pjoObject->pjtsOwner    = JvmGetThreadState();
    }
    else
	   return JvmRtCreateNewInstance( pjccTheClass );

    return pjoObject;
}


JVM_OBJREF JvmRtCreateNewInstance( _JVM_Class_Class *pjccTheClass )
{
	_JVM_Object * pjoObject = JVM_NULL;
	 JVM_INT jiIndex = 0;
	 JVM_U16 ju16ClassesCount = 0;
	 JVM_U16 ju16FieldCount = 0,ju16FieldIndex = 0;
	_JVM_Field ** ppjfFields = JVM_NULL;
	_JVM_Field *  pjfField = JVM_NULL;
	_JVM_Class_Class * pjccAClass = JVM_NULL;
	JVM_STRING jsName = JVM_NULL;

	JVM_ASSERT_VALID( pjccTheClass );

	if (pjccTheClass->jdwClassFlags & ACC_ABSTRACT )
	{
	  /* By pass ACC_ABSTRACT for VirtualMachineError */
	  if(JvmSpringStrCmp(pjccTheClass->jsClassName, JVM_SPRING_CREATE("java/lang/VirtualMachineError"))) {
		   JAVA_THROW_EXCEPTION( EXC_JAVAINSTANTIATIONERROR );
		   JVM_ASSERT( JVM_FALSE );
		}
	}

	pjoObject = JvmMemCalloc( sizeof( _JVM_Object ));
	JVM_STATE_CHECK( pjoObject, EXC_JAVAINSTANTIATIONERROR )
    pjoObject->joiObjectID.pjccClass    = pjccTheClass;
    pjoObject->pjwsWaitSet  = JvmMemCalloc( sizeof( _JVM_Wait_Set ));
    JVM_STATE_CHECK( pjoObject->pjwsWaitSet, EXC_JAVAINSTANTIATIONERROR )
	pjoObject->pjesEntrySet = JvmMemCalloc( sizeof( _JVM_Entry_Set ));
    JVM_STATE_CHECK( pjoObject->pjesEntrySet, EXC_JAVAINSTANTIATIONERROR )
	pjoObject->pjlLock      = JvmMemCalloc( sizeof( _JVM_Lock ));
    JVM_STATE_CHECK( pjoObject->pjesEntrySet, EXC_JAVAINSTANTIATIONERROR )
	pjoObject->pjtsOwner    = JvmGetThreadState();

	if ( pjccTheClass->jdwClassFlags & ACC_INTERFACE )
		pjoObject->joiObjectID.jbyID = REF_TYPE_INTERFACE;
	else 
		pjoObject->joiObjectID.jbyID = REF_TYPE_CLASS;


	/** CHECK,CHECK - What if a class has no super class?????? **/

	/** CHECK,CHECK - can we optimize this by redesigning?? **/
	/** Copy all fields to the object's own **/
    pjccAClass = pjccTheClass;

    for ( ;  pjccAClass  ;)
    {   
	  ju16ClassesCount++;
	  ju16FieldCount += pjccAClass->jwFieldsCount;
      pjccAClass =  pjccAClass->joSuperClassObject ?
     		pjccAClass->joSuperClassObject->joiObjectID.pjccClass : JVM_NULL;
    }

	if ( ju16ClassesCount ) {
      pjoObject->pppjfFields = ( _JVM_Field ***) JvmMemCalloc(
	      ju16ClassesCount * sizeof( _JVM_Field ** ) );
      JVM_ALLOC_CHECK( pjoObject->pppjfFields )
  }

	pjoObject->ClassesCount = ju16ClassesCount;

	pjoObject->FieldsCount = (JVM_U16 *)JvmMemCalloc( sizeof( JVM_U16) * ju16ClassesCount ); 

    jiIndex = 0;
  for ( pjccAClass = pjccTheClass;  pjccAClass  ; )
	{
     if (  pjccAClass->jwFieldsCount )
	   {
         pjoObject->FieldsCount[ jiIndex ] = pjccAClass->jwFieldsCount;
	     ppjfFields = pjoObject->pppjfFields[ jiIndex ] = (_JVM_Field **)
		     JvmMemCalloc( sizeof( _JVM_Field *) * pjccAClass->jwFieldsCount );
	     JVM_ALLOC_CHECK( ppjfFields )
	   }

	   for( ju16FieldIndex = 0 ; ju16FieldIndex < pjccAClass->jwFieldsCount;
   	        ju16FieldIndex ++ )
	   {
	       pjfField = ( _JVM_Field * )JvmMemCalloc( sizeof( _JVM_Field ));
           JVM_ALLOC_CHECK( pjfField ) 
           JvmMemCpy(pjfField, pjccAClass->ppjfData[ ju16FieldIndex],
		                       sizeof( _JVM_Field));
           if ( pjfField->jfiFieldType.jwFieldType & JVM_OBJECT_TYPE )
		   {
              // realloc space
             pjfField->jfiFieldType.jsObjectName = 
				 JvmSpringClone( pjfField->jfiFieldType.jsObjectName );
		   }

		   ppjfFields[ ju16FieldIndex ] = pjfField;
	   }

      pjccAClass = pjccAClass->joSuperClassObject  ? 
		  pjccAClass->joSuperClassObject->joiObjectID.pjccClass : JVM_NULL;
	    ++ jiIndex;
	}

   /** Can't escape -- time to think about GC !! **/

   JvmGCAddNewObject( pjoObject );

   /** All set **/
   return pjoObject;

  /** TODO **/
}


_JVM_Field * JvmRtGetFieldForName(
  IN JVM_OBJREF joRef, 
  IN JVM_STRING jsName 
)
{
	JVM_INT  jiClassIterator, jiFieldIterator;
	_JVM_Object * pjoObject;
	
	JVM_ASSERT_VALID( joRef );
	pjoObject = joRef;

    for ( jiClassIterator = 0; jiClassIterator < pjoObject->ClassesCount; jiClassIterator ++ ) 
	{
      JVM_INT FieldsCount = pjoObject->FieldsCount[ jiClassIterator ]; 
 	  for ( jiFieldIterator = 0; jiFieldIterator < FieldsCount;
		    jiFieldIterator ++ )
	  {
         _JVM_Field ** ppjfField = pjoObject->pppjfFields[jiClassIterator];
         if ( JvmSpringStrCmp( ppjfField[jiFieldIterator]->jsFieldName, jsName ) == 0 )
		 {
			 return ppjfField[jiFieldIterator];
		 }
	  }
	}

	return JVM_NULL;
}



JVM_BOOL JvmGCAddNewObject( JVM_OBJREF pjoObjRef )
{
	// return  BinTreeAddNode( JVM_GC_OBJECT_TREE, (JVM_U32)pjoObject ) );
//	JvmDisplay( "Invoking Dummy JvmGCAddNewObject In Runtime" );

	return JVM_TRUE;
}

JVM_BOOL JvmGCInit()
{
//	JvmDisplay( "Invoking Dummy JvmGCInit in Runtime" );
//	JVM_GC_OBJECT_TREE = BinTreeGetNewTree();
//	JVM_ALLOC_CHECK( JVM_GC_OBJECT_TREE );
	return JVM_TRUE;
}


JVM_BOOL JvmRtDestroyInstance( IN JVM_OBJREF joRef )
{
  // JvmDisplay( "Invoking Dummy JvmRtDestroyInstance");
   return JVM_TRUE;
}



/** The main Garbage collection routine **/
JVM_BOOL JvmGCTrace( /** ?? **/ )
{
  /** TODO **/
  return JVM_TRUE;
} 

JVM_PVOID JvmRtLinkNativeFunction( 
  IN      JVM_STRING jsFunctionName,
  IN      JVM_STRING jsLibrary 
)
{
   register JVM_FILE jfFile = JVM_NULL;
   register JVM_PVOID  jptrFunction = JVM_NULL;
   
   jfFile  = JvmSysLoadLibrary( jsLibrary );
   JVM_STATE_CHECK( jfFile, EXC_JAVAUNSATISFIEDLINKERROR )

   jptrFunction = JvmSysLinkFunction( jfFile, jsFunctionName );
   JVM_STATE_CHECK( jptrFunction, EXC_JAVAUNSATISFIEDLINKERROR );

   return jptrFunction;
}


#ifndef __JDEPENDS__


JVM_BOOL JvmRtInvokeMethod(  
  IN     JVM_STRING jsMethodName, 
  IN     JVM_STRING jsMethodDesc, 
  IN     JVM_OBJREF joObjRef,
  IN     JAVA_WORD  * pjawArgs,
  IN     JVM_INT      jiArgCount,
  IN    _JVM_Exec_Env * pjeeEnvCaller
)
{
  _JVM_Object * pjoObject = (_JVM_Object *) joObjRef;
  _JVM_Class_Class * pjccClass;
  register _JVM_Method * pjmMethod;
  register  JVM_WORD  jwCount = 0;

  JVM_ASSERT_VALID( pjoObject );

  pjccClass = pjoObject->joiObjectID.pjccClass;
  for ( ; jwCount <  pjccClass->jwMethodsCount; jwCount++ )
  {
	  pjmMethod = pjccClass->ppjmMethods[ jwCount ];
	  if ( JvmSpringStrCmp( pjmMethod->jsMethodName, jsMethodName ) == 0)
	  {
		  if ( JvmSpringStrCmp( pjmMethod->jsMethodDesc, jsMethodDesc ) == 0 )
		  	  break;
	  }
  }
  
  if ( jwCount < pjccClass->jwMethodsCount )
  {
       JvmRtCallMethod( pjeeEnvCaller, pjmMethod, pjccClass, joObjRef,     
		   pjawArgs, jiArgCount );
  }
   /** TODO ??**/
   return JVM_FALSE;
}


JVM_BOOL JvmRtCallMethod(  
 IN     _JVM_Exec_Env * pjeeEnvCaller,
 IN     _JVM_Method * pjmMethod,
 IN     _JVM_Class_Class *pjccClass,
 IN      JVM_OBJREF joObjRef,
 IN      JAVA_WORD  * pjawArgs,
 IN      JVM_INT      jiArgCount
)
{
  register  JVM_U32  ju32StackTop;    
  register  JAVA_WORD  * pjawOpStack; 
  register  _JVM_Field_Info  * pjfiReturn;
  JVM_BOOL jbIsLong = 0; 
  JVM_BOOL jbIsFP = 0; 
  JAVA_LONG jalLong = 0;
  JVM_PVOID pjvLong = &jalLong;
  	
  _JVM_Exec_Env * pjeeCurrentExecEnv = JVM_NULL;
  
	JVM_ASSERT_VALID( pjmMethod );
	JVM_ASSERT_VALID( pjccClass );
	JVM_ASSERT_VALID( pjfiReturn);
/**	JVM_ASSERT_VALID( pjeeEnvCaller ); **/

   JvmRtSetUpStackFrame( pjmMethod, pjccClass );
   JvmRtSetUpParameters( joObjRef, pjawArgs, jiArgCount );
	 pjeeCurrentExecEnv = JvmRtSetUpExecEnv( pjeeEnvCaller );
	 
	 ju32StackTop  = pjeeCurrentExecEnv->ju32OpStackTop;
	 pjawOpStack = pjeeCurrentExecEnv->pjsfFrame->pjawOpStackElements; 
	 
	 jbIsLong = jalLong = jbIsFP = 0;
	 
	 if(pjmMethod->pjmiMethodInfo->jbReturn) {
	   pjfiReturn = &pjmMethod->pjmiMethodInfo->jfiReturnField;
	   
	   if (pjfiReturn->jwFieldType & JVM_BASE_TYPE ) {
		  	if (( pjfiReturn->jcBaseType == 'F' ) ||
			      ( pjfiReturn->jcBaseType == 'D' ))	{
		 		      jbIsFP = 1;
		 	  }
    		if ((pjfiReturn->jcBaseType == 'D') || 
 	          (pjfiReturn->jcBaseType == 'J' )) {
			     jbIsLong = 1;
			     pjvLong = &jalLong;
			  }
	   }
	 }


	/** Should the native implementation also store
	    the return in the operand stack of the caller????? 
	    that should make life simpler and as one remembers,
	    simplicity is the hallmark of perfection !!**/

	if (pjmMethod->jwMethodAccessFlags & ACC_NATIVE ) 	{
		JVM_PVOID pfnNative;
		JAVA_WORD jawWord;
		JVM_BOOL  jbIsFP;
		JAVA_WORD saveEdx;
		JAVA_WORD * pjawArgArray;
		JVM_PVOID pjvWord = &saveEdx;
		
		JVM_FILE file = JvmSysLoadLibrary(JVM_SPRING_CREATE("c:\\jdp\\jvm\\src\\natives\\Debug\\natives.dll"));
		pfnNative = pjmMethod->pfnNativeMethod = JvmSysLinkFunction(file, pjmMethod->jsMethodName);
	
		pjawArgArray = JvmMemAlloc( sizeof( JAVA_WORD )*( jiArgCount + 1 ));
		JVM_ALLOC_CHECK( pjawArgArray );

		if ( jiArgCount ) {
    		JvmMemCpy( pjawArgArray, pjawArgs, sizeof( JAVA_WORD ) * jiArgCount);
    }

		pjawArgArray[ jiArgCount ] = JvmRtConstructNativeToken( pjeeEnvCaller );
		++jiArgCount;
	

 		JVM_ASSERT_VALID( pfnNative );
 		
 		/* native function's  return value will go into caller's
 		   operand stack */
 		pjawOpStack = (pjeeEnvCaller ? pjeeEnvCaller->pjsfFrame->pjawOpStackElements
 		               : pjawOpStack);
 		               
 		               
 	  /** JAVA_LONG or JAVA_DOUBLE **/
 	  
 	  if(jbIsLong) {
       JVM_NATIVE_CALL( jiArgCount, pjawArgArray, pfnNative, pjvLong, jbIsFP, jbIsLong, pjvWord )   
       if(pjmMethod->pjmiMethodInfo->jbReturn && pjeeEnvCaller) {
          pjawOpStack[++pjeeEnvCaller->ju32OpStackTop] = (JAVA_WORD)(jalLong >> 32);
		      pjawOpStack[++pjeeEnvCaller->ju32OpStackTop] = (JAVA_WORD)jalLong;
		   }
    } else { /* Otherwise */
      JVM_NATIVE_CALL( jiArgCount, pjawArgArray, pfnNative, jawWord, jbIsFP, jbIsLong, pjvWord );
      if(pjmMethod->pjmiMethodInfo->jbReturn && pjeeEnvCaller) {
        pjawOpStack[++pjeeEnvCaller->ju32OpStackTop] = jawWord;
      }
    }
    
    JvmMemFree( pjawArgArray );
   
    JvmRtDestroyStackFrame(); 
    JvmRtDestroyExecEnv();

    return JVM_TRUE;
 	}
 	
	JvmRtSetUpParameters( joObjRef, pjawArgs, jiArgCount );
	JvmRtExecute();
	
	 
/* These should be done by corresponding areturn,return,ireturn etc. 
	 JvmRtGetReturnValue( pjeeEnvCaller, pjmMethod, pjfiReturn );
	 	JvmRtDestroyStackFrame(); 
    JvmRtDestroyExecEnv();

	 */

	 return JVM_TRUE; 
}


JAVA_WORD  JvmRtConstructNativeToken( _JVM_Exec_Env * pjeeEnvCaller )
{
  /* TODO */return 0L;
}

JVM_VOID JvmRtGetReturnValue(
 IN     _JVM_Exec_Env * pjeeEnvCaller,
 IN     _JVM_Method * pjmMethod,
 IN OUT    _JVM_Field  * pjfiReturn
 )
{
  register  JAVA_WORD  * pjawOpStack = pjeeEnvCaller->pjsfFrame->pjawOpStackElements;  
	JAVA_WORD jawWord1;
	JAVA_WORD jawWord2;
	

	JVM_ASSERT_VALID( pjmMethod );
	JVM_ASSERT_VALID( pjeeEnvCaller );

	jawWord1 = pjeeEnvCaller->pjsfFrame->
		         pjawOpStackElements[ pjeeEnvCaller->ju32OpStackTop-- ];
	
	if ( pjmMethod->pjmiMethodInfo->jbReturn )  {
	   if((pjfiReturn->jfiFieldType.jcBaseType == 'D') ||
	      (pjfiReturn->jfiFieldType.jcBaseType == 'J')) {
	      jawWord2 = pjeeEnvCaller->pjsfFrame->
		         pjawOpStackElements[ pjeeEnvCaller->ju32OpStackTop--];
		    pjawOpStack[++pjeeEnvCaller->ju32OpStackTop] = jawWord1;     
		    pjawOpStack[++pjeeEnvCaller->ju32OpStackTop] = jawWord2;     
        JvmFieldStoreLongDoubleValue(pjfiReturn, jawWord1, jawWord2);
	    } else {
   	    pjawOpStack[++pjeeEnvCaller->ju32OpStackTop] = jawWord1;     
        JvmFieldStoreWordValue(pjfiReturn, jawWord1 );
    	}
	 }
	
}





JVM_VOID JvmRtSetUpParameters( 
 IN      JVM_OBJREF joObjRef, 
 IN      JAVA_WORD  * pjawArgs,
 IN      JVM_INT      jiArgCount
 )
{
	_JVM_Thread_State * pjtsThreadState = JvmGetThreadState();
	_JVM_Stack_Frame  * pjsfFrame;
	JVM_PVOID  pjbyLocals;

	 JVM_ASSERT_VALID( pjtsThreadState );
	 
	  pjsfFrame = pjtsThreadState->pjsfCurrFrame; 

	 /** Native methods => special treatment **/
	 if ( pjsfFrame->pjmCurrMethod->jwMethodAccessFlags & ACC_NATIVE )
		 return;

	 /** Check for enough space  **/
	 JVM_ASSERT( pjsfFrame->ju32LocalVarsSize >= (JVM_U32)jiArgCount );

	 /** No 'this' ptr for a Static method **/
	 if ( pjsfFrame->pjmCurrMethod->jwMethodAccessFlags & ACC_STATIC ) 
	    pjbyLocals = &pjsfFrame->pjawLocals[0];
	 else 
	 {
		/** args **/
		pjbyLocals = &pjsfFrame->pjawLocals[1];
		/** 'this' **/
		pjsfFrame->pjawLocals[0] = (JAVA_WORD) joObjRef;
	 }

	 /** more args **/
	 if ( pjawArgs )
	   JvmMemCpy( pjbyLocals , pjawArgs, jiArgCount * sizeof(JAVA_WORD));
}




JVM_BOOL JvmRtCreateObject_JavaLangString_v0( 
 IN  JVM_STRING jsName,
 IN  JAVA_OBJREF * pjaoJavaLangStringRef 
)
{
   _JVM_Object * pjaoRef;
   _JVM_Array_Object * pjaoArray;
	JVM_STRING jsMutantChars;
	JVM_OBJREF joStringClassObject;
	JAVA_INT   jaiLen = 0;
	JAVA_WORD  jawArgs[1];

    if ( !JvmLdrLoadNewClass( S_JAVA_LANG_STRING,
	   JVM_NULL, PRIMITIVE_LOADER, &joStringClassObject ))
	   return JVM_FALSE;

   /** CHECK,CHECK should I invoke constructor and <init>?? **/
   pjaoRef = JvmRtCreateNewInstance( ((_JVM_Object *)joStringClassObject)->joiObjectID.pjccClass ); 

   jaiLen = (JAVA_INT)JvmSpringGetLen( jsName );
   pjaoArray =  JvmRtCreateCharArray( jaiLen );
   jsMutantChars = JvmSpringToMutantCString( jsName );
	/** Fill the array **/
   JvmMemCpy( &pjaoArray->u.pjacChar[0], jsMutantChars,
   JvmSpringGetMutantCStringLen( jsName ));

   JVM_TRY 
   {
	   jawArgs[0] = (JAVA_WORD)pjaoArray;
       JvmRtInvokeMethod( S_METHOD_INIT, S_DESC_JLSTRING_INIT, 
		  pjaoRef, jawArgs, 1, JvmGetThreadState()->pjeeEnvCaller );
   }
   JVM_CATCH_ALL()
   {
	   JvmRtDestroyInstance( pjaoRef );
	   /** CHECK,CHECK : free up rest of the stuff **/
       return JVM_FALSE;
   }
   JVM_END_TRY

/**   pjfField = JvmRtGetFieldForName( pjaoRef, S_VALUE );
   if ( JVM_VALID(pjfField) )
	  pjfField->u.jaoref = pjaoArray;
   pjfField = JvmRtGetFieldForName( pjaoRef, S_OFFSET );
   if ( JVM_VALID(pjfField) )
     pjfField->u.jai = 0;
   pjfField = JvmRtGetFieldForName( pjaoRef, S_COUNT );
   if ( JVM_VALID(pjfField) )
   pjfField->u.jai = jaiLen; **/
   /**   JvmDisplay("Invoking Dummy JvmRtCreateObject_JavaLangString_v0\n"); **/
   return JVM_TRUE; 
}

#endif /* __JDEPENDS__ */

JAVA_OBJREF JvmRtCreateByteArray( JAVA_INT jaiCount )
{
	JAVA_OBJREF joClassObject,joArrayObject;
	JAVA_WORD   jawLengths[ 1 ];

	jawLengths[0] = jaiCount;
	if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, JVM_SPRING_CREATE("[B"), &joClassObject ))     
			JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

    joArrayObject = JvmRtCreateNewArray(JAVA_TYPE_BYTE ,1, 
		jawLengths, ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass , JVM_NULL );


	return joArrayObject;
}

JAVA_OBJREF JvmRtCreateCharArray( JAVA_INT jaiCount )
{
	JAVA_OBJREF joClassObject,joArrayObject;
	JAVA_WORD   jawLengths[ 1 ];

	jawLengths[0] = jaiCount;

	if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, JVM_SPRING_CREATE("[C"), &joClassObject ))     
			JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

    joArrayObject = JvmRtCreateNewArray(JAVA_TYPE_CHAR ,1, 
		jawLengths, ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass , JVM_NULL );


	return joArrayObject;
}


JAVA_OBJREF JvmRtCreateFloatArray( JAVA_INT jaiCount )
{
	JAVA_OBJREF joClassObject,joArrayObject;
	JAVA_WORD   jawLengths[ 1 ];

	jawLengths[0] = jaiCount;

	if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, JVM_SPRING_CREATE("[F"), &joClassObject ))     
			JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

    joArrayObject = JvmRtCreateNewArray(JAVA_TYPE_FLOAT ,1, 
		jawLengths, ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass , JVM_NULL );


	return joArrayObject;
}


JAVA_OBJREF JvmRtCreateDoubleArray( JAVA_INT jaiCount )
{
	JAVA_OBJREF joClassObject,joArrayObject;
	JAVA_WORD   jawLengths[ 1 ];

	jawLengths[0] = jaiCount;

	if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, JVM_SPRING_CREATE("[D"), &joClassObject ))     
			JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

    joArrayObject = JvmRtCreateNewArray(JAVA_TYPE_DOUBLE ,1, 
		jawLengths, ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass , JVM_NULL );


	return joArrayObject;
}


JAVA_OBJREF JvmRtCreateIntArray( JAVA_INT jaiCount )
{
	JAVA_OBJREF joClassObject,joArrayObject;
	JAVA_WORD   jawLengths[ 1 ];

	jawLengths[0] = jaiCount;

	if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, JVM_SPRING_CREATE("[I"), &joClassObject ))     
			JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

    joArrayObject = JvmRtCreateNewArray(JAVA_TYPE_INT ,1, 
		jawLengths, ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass , JVM_NULL );


	return joArrayObject;
}


JAVA_OBJREF JvmRtCreateLongArray( JAVA_INT jaiCount )
{
	JAVA_OBJREF joClassObject,joArrayObject;
	JAVA_WORD   jawLengths[ 1 ];

	jawLengths[0] = jaiCount;

	if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, JVM_SPRING_CREATE("[J"), &joClassObject ))     
			JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

    joArrayObject = JvmRtCreateNewArray(JAVA_TYPE_LONG ,1, 
		jawLengths, ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass , JVM_NULL );


	return joArrayObject;
}


JAVA_OBJREF JvmRtCreateShortArray( JAVA_INT jaiCount )
{
	JAVA_OBJREF joClassObject,joArrayObject;
	JAVA_WORD   jawLengths[ 1 ];

	jawLengths[0] = jaiCount;

	if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, JVM_SPRING_CREATE("[S"), &joClassObject ))     
			JVM_THROW_EXCEPTION( EXC_JAVAVIRTUALMACHINEERROR );

    joArrayObject = JvmRtCreateNewArray(JAVA_TYPE_SHORT ,1, 
		jawLengths, ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass , JVM_NULL );


	return joArrayObject;
}


JVM_BOOL IsReference( JVM_PVOID pjvValue )
{
	JvmDisplay("IsReference Not implemented yet\n");
	return JVM_FALSE;
}




































