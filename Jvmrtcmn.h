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

#ifndef __JVM_RTCMN_INCLUDE__
#define __JVM_RTCMN_INCLUDE__

typedef JVM_VOID ( * PFN_BYTECODEHANDLER ) ( _JVM_Exec_Env * );


#define JVM_MAX_TYPE_SIZE sizeof( JAVA_LONG )
#define IS_A_REFERENCE 1

#define HI32(x) ((JVM_U32)(((JVM_U64)(x) >> JVM_DWORD_IN_BITS) & ~0))
#define LO32(x)  ((JVM_U32)(x)) 
#define MAKE_64( hi, lo )  (((JVM_U64)(hi) << JVM_DWORD_IN_BITS) | (lo))
#define HI_JWORD(x)  (JAVA_WORD)((x) >> JVM_JWORD_IN_BITS)
#define LO_JWORD(x)  (JAVA_WORD)(((x) << JVM_JWORD_IN_BITS) >> JVM_JWORD_IN_BITS)


#define ILOAD    0x15
#define LLOAD    0x16
#define FLOAD    0x17
#define DLOAD    0x18
#define ALOAD    0x19
#define ISTORE   0x36
#define LSTORE   0x37
#define FSTORE   0x38
#define DSTORE   0x39
#define ASTORE   0x3a
#define RET      0xa9
#define IINC     0x84
#define ILOAD_0  0x1a
#define LLOAD_0  0x1e
#define FLOAD_0  0x22
#define DLOAD_0  0x22
#define ALOAD_0  0x2a
#define ISTORE_0 0x3b
#define LSTORE_0 0x3f
#define FSTORE_0 0x43
#define DSTORE_0 0x47
#define ASTORE_0 0x4b
#define WIDE	 0xc4


JVM_BOOL JvmRtInit( 
  IN OUT G_STATE_P state
);

JVM_BOOL JvmReleaseLock(_JVM_Thread_State *pjtsThisThread ,
						_JVM_Object *pjoObject   );

JVM_VOID  JvmRtByteCode_nop__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_aconst_null__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_iconst_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_lconst_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_fconst_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_dconst_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_bipush__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_sipush__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_ldc_common__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_ldc2_w__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_ifload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_ldload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_aload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_iload_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_lload_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_fload_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_dload_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_aload_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_iaload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_laload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_faload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_daload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_aaload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_baload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_caload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_saload__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_ifstore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_ldstore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_astore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_istore_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_lstore_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_fstore_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_dstore_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_astore_n__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_iastore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_lastore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_fastore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_dastore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_aastore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_bastore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_castore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID  JvmRtByteCode_sastore__Handler( 
  IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_pop__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_pop2__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_dup__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_dup_x1__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_dup_x2__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_dup2__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_dup2_x1__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_dup2_x2__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_swap__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_iadd__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_ladd__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_fadd__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_dadd__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_isub__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_lsub__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_fsub__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_dsub__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
 );

JVM_VOID JvmRtByteCode_imul__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lmul__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);


JVM_VOID JvmRtByteCode_fmul__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_dmul__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_idiv__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ldiv__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_fdiv__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ddiv__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_irem__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lrem__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_frem__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_drem__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ineg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lneg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_fneg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_dneg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ishl__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lshl__Handler(
  IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ishr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lshr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_iushr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lushr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_iand__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_land__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ior__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lor__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ixor__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lxor__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_iinc__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_i2l__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_i2f__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_i2d__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_l2i__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_l2f__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_l2d__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_f2i__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_f2l__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_f2d__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_d2i__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_d2l__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_d2f__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_i2b__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);


JVM_VOID JvmRtByteCode_i2c__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_i2s__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lcmp__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_fcmpl__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_fcmpg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_dcmpl__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_dcmpg__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ifeq__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ifne__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_iflt__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ifge__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ifgt__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ifle__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_if_icmpeq__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_if_icmpne__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_if_icmplt__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_if_icmpge__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_if_icmpgt__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_if_icmple__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_if_acmpeq__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_if_acmpne__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_goto__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_jsr__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ret__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_tableswitch__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_lookupswitch__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ireturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);


JVM_VOID JvmRtByteCode_lreturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_freturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_dreturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_areturn__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_return__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_getstatic__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_putstatic__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_getfield__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_getfield__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_putfield__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_invokevirtual__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_invokespecial__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_invokestatic__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_invokeinterface__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_DUMMY__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_new__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_newarray__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_anewarray__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_arraylength__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_athrow__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_checkcast__Handler(
 IN  OUT   _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_instanceof__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_moniterenter__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_moniterexit__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_wide__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_multianewarray__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ifnull__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_ifnonnull__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);


JVM_VOID JvmRtByteCode_goto_w__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JVM_VOID JvmRtByteCode_jsr_w__Handler(
 IN  OUT    _JVM_Exec_Env * pjeeExecEnv
);

JAVA_OBJREF JvmRtCreateByteArray( 
 IN  JAVA_INT jaiCount 
);

JAVA_OBJREF JvmRtCreateCharArray(
 IN  JAVA_INT jaiCount 
);

JAVA_OBJREF JvmRtCreateFloatArray(
 IN  JAVA_INT jaiCount 
);

JAVA_OBJREF JvmRtCreateDoubleArray(
 IN  JAVA_INT jaiCount 
);

JAVA_OBJREF JvmRtCreateShortArray(
 IN  JAVA_INT jaiCount 
);

JAVA_OBJREF JvmRtCreateIntArray(
 IN  JAVA_INT jaiCount 
);

JAVA_OBJREF JvmRtCreateLongArray(
 IN  JAVA_INT jaiCount 
);

JVM_BOOL  JvmCreateStackMap( 
  IN OUT _JVM_Method *pjmMethod, 
  IN      JVM_U32 ** ppju32StackMap,
  IN      JVM_U32    ju32StartIndex
);

JVM_BOOL JvmGCAddNewObject(
  IN  JVM_OBJREF pjoObjRef 
);

JAVA_WORD  JvmRtConstructNativeToken(
  IN _JVM_Exec_Env * pjeeEnvCaller 
);

JVM_VOID JvmRtFieldStoreValue(
 IN  _JVM_Field *pjfiField,
 IN   JAVA_WORD  jawValue
);

_JVM_Field * JvmRtGetFieldForName(
  IN JVM_OBJREF joRef, 
  IN JVM_STRING jsName 
);




#endif /* __JVM_RTCMN_INCLUDE__ */
