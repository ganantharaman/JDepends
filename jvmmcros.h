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

#ifndef __JVM_MACROS_INCLUDE__
#define __JVM_MACROS_INCLUDE__

/* Replace with platform specific include */
#define JVMOS_H
#include <jvmhdrs.h>

/** Subsystem masks **/
#define state_exception  0x00000001
#define state_runtime    0x00000002
#define state_spring     0x00000004
#define state_config     0x00000008
#define state_loader     0x00000010
#define state_memory     0x00000020
#define state_oslayer    0x00000040
#define state_syslayer   0x00000080



/* JVM Data Type Pointer Macros */
typedef  JVM_BYTE*        JVM_PBYTE;
typedef  JVM_INT*         JVM_PINT;
typedef  JVM_SIGNED16*    JVM_PSIGNED16;
typedef  JVM_SHORT*       JVM_PSHORT;
typedef  JVM_SIGNED32*    JVM_PIGNED32;
typedef  JVM_UNSIGNED16*  JVM_PUNSIGNED16;
typedef  JVM_UNSIGNED32*  JVM_PUNSIGNED32;
typedef  JVM_LONG*        JVM_PLONG;
typedef  JVM_UNSIGNEDLONG JVM_PUNSIGNEDLONG;
typedef  JVM_VOID*        JVM_PVOID;
typedef  JVM_BYTE*        JVM_STRING; 
typedef  JVM_BYTE*        JVM_PCHAR;

#define JVM_OBJREF      JVM_PVOID
#define JAVA_OBJREF     JVM_PVOID

#define JVM_U16         JVM_UNSIGNED16
#define JVM_U32         JVM_UNSIGNED32


/* Debug implementation specific */
#ifndef JVM_DEBUG
#if defined(JVM_ASSERT)
 #undef  JVM_ASSERT
 #define JVM_ASSERT(x)	 ((void)0)
#endif
#if defined(JVM_ASSERT_VALID)
 #undef  JVM_ASSERT_VALID
 #define JVM_ASSERT_VALID(x) ((void)0)
#endif
#endif 


#define JVM_VALID( x ) ((JVM_NULL_TYPE)(x) != JVM_NULL)

/* sizes */
#define BYTELEN             sizeof( JVM_WORD ) 
#define WORDLEN             sizeof( JVM_WORD ) 
#define DWORDLEN            sizeof( JVM_DWORD ) 
#define JVM_POINTER_IN_BITS sizeof( JVM_PVOID ) * JVM_BITS_PER_BYTE
#define JVM_DWORD_IN_BITS   (DWORDLEN * JVM_BITS_PER_BYTE)
#define JVM_JWORD_IN_BITS   (sizeof( JAVA_WORD ) * JVM_BITS_PER_BYTE) 



typedef struct G_STATE {
	JVM_U32 mask;
}G_STATE, *G_STATE_P;

/* subsystem state masks */
#define RESET_STATE(state) state->mask = 0x00000000;
#define EXCL_READY(state) state->mask |= state_exception
#define RTL_READY(state)  state->mask |= state_runtime
#define SPGL_READY(state) state->mask |= state_spring
#define CFGL_READY(state) state->mask |= state_config
#define LDRL_READY(state) state->mask |= state_loader
#define MEML_READY(state) state->mask |= state_memory
#define OSL_READY(state)  state->mask |= state_oslayer
#define SYSL_READY(state) state->mask |= state_syslayer

#define IS_EXCL_READY(state) ( state->mask & state_exception)
#define IS_RTL_READY(state)  ( state->mask & state_runtime)
#define IS_SPGL_READY(state) ( state->mask & state_spring)
#define IS_CFGL_READY(state) ( state->mask & state_config)
#define IS_LDRL_READY(state) ( state->mask & state_loader)
//#define IS_MEML_READY(state) ( state->mask & state_memory)
#define IS_OSL_READY(state)  ( state->mask & state_oslayer)
#define IS_SYSL_READY(state) ( state->mask & state_syslayer)


#define JVM_STATE_CHECK( x, y)\
  if ( !JVM_VALID((x)) ) {\
	JVM_THROW_EXCEPTION((y));\
	JVM_ASSERT( JVM_FALSE );\
  }

#define JVM_ALLOC_CHECK( x )\
	JVM_STATE_CHECK(x,EXC_JVMNOMEMORY) 


/* Parameter Input output denoters */
#define IN        
#define OUT     

/** Exception specific **/
/* This is for accomodating all possible exceptions 
    both local and Java exceptions to be raised */
enum { 
  MIN_JVMEXCEPTIONVAL = 0,
  MAX_JVMEXCEPTIONVAL = 256
};

/** Parse specific **/
enum {
 JVM_MAX_NAME    = 510,
 JVM_BASE_TYPE   = 0x0001,
 JVM_OBJECT_TYPE = 0x0002,
 JVM_ARRAY_TYPE  = 0x0004
};

/** Class specific */
enum {
	CLASS_INITIALIZED = 1,
  CLASS_PREPARED
};

/** Java specific **/
enum {
	  JAVA_TYPE_PRIMITIVE = 0x0001,
    JAVA_TYPE_INTERFACE = 0x0002,
    JAVA_TYPE_ARRAY     = 0x0004
};


#define JAVA_TYPE_CHAR       'C'
#define JAVA_TYPE_BYTE       'B'
#define JAVA_TYPE_SHORT      'S'
#define JAVA_TYPE_INT        'I'
#define JAVA_TYPE_LONG       'J'
#define JAVA_TYPE_FLOAT      'F'
#define JAVA_TYPE_DOUBLE     'D'
#define JAVA_TYPE_BOOL       'Z' 
#define JAVA_TYPE_REF        'L' 



/** Field/Method/Class Access Flags **/
enum {
 ACC_PUBLIC     =   0x0001,
 ACC_PRIVATE    =   0x0002,
 ACC_PROTECTED 	=   0x0004,
 ACC_STATIC  	=   0x0008,
 ACC_FINAL  	=   0x0010,
 ACC_SYNCHRONIZED =  0x0020,
 ACC_SUPER      =   0x0020,
 ACC_VOLATILE  	=   0x0040,
 ACC_TRANSIENT  =   0x0080,
 ACC_NATIVE     =   0x0100,
 ACC_INTERFACE  =   0x0200,
 ACC_ABSTRACT   =   0x0400,
 ACC_STRICT     =   0x0800,
 FLAG_SYNTHETIC =   0x1000,
 FLAG_DEPRECATED =  0x2000
};

/** File I/O specific **/
enum {
 JVM_FILE_READ   =  0x0001,
 JVM_FILE_WRITE  =  0x0002,
 JVM_FILE_APPEND =  0x0004
};

/** Threading specific **/

/* Java max thread priority */
enum { 
 JVM_THREAD_MAX_PRIORITY = 10,
 JVM_THREAD_NORMAL_PRIORITY= 5
};

/** Mne (Miscellaneous naked enums) **/
enum { JVM_NOT_OK, JVM_OK  };
enum { JVM_NO_GOOD, JVM_GOOD };
enum {
 TYPE_CHARACTER = 1, 
 TYPE_INTEGER,
 TYPE_STRING,
 TYPE_LONG,
 TYPE_FLOAT, 
 TYPE_DOUBLE 
};

enum { T_BOOLEAN = 4, T_CHAR, T_FLOAT, T_DOUBLE, T_BYTE, T_SHORT, T_INT, T_LONG };

enum {  REF_TYPE_CLASS = 1,
        REF_TYPE_ARRAY,
        REF_TYPE_INTERFACE
};

enum { 
 CHUNK  = 256,
 FRAME_BLOCK_SIZE = 16
};

/** IEEE 754 'Single precision ' floating pt implementation **/

#define IEEE754_F_POS_INFINITY  0x7f800000
#define IEEE754_F_NEG_INFINITY  0xff800000
#define IEEE754_F_POS_MAX       0x7fffffff
#define IEEE754_F_NEG_MAX       0xffffffff
#define IEEE754_F_NaN           0xffffffff

#define IEEE754_D_POS_INFINITY  0x7f80000000000000
#define IEEE754_D_NEG_INFINITY  0xff80000000000000
#define IEEE754_D_POS_MAX       0x7fffffffffffffff
#define IEEE754_D_NEG_MAX       0xffffffffffffffff
#define IEEE754_D_NaN           0xffffffffffffffff


/* some support for constantly used temp buffers */
typedef JVM_BYTE JVM_BUFFER[4096 + 1];

/* Array creators */
typedef JAVA_OBJREF ( * PFN_ARRAYFACTORY )( JAVA_INT );

/* Refs */
#define JAVA_REF _JVM_Object *
#define JAVA_ARRAYREF _JVM_Array_Object *

/** JVM Spec 1.0.2 version **/
#define JVM_MINOR_VERSION 3
#define JVM_MAJOR_VERSION 45

/* Only JVM Spec 1.0.2 Constant pool types */
#define  JVM_MAX_CP_TYPES 12

enum { 
	JVM_CP_TAG_UTF8     =   1,
	JVM_CP_TAG_DUMMY,	            /** DUMMY **/
    JVM_CP_TAG_INTEGER,            
    JVM_CP_TAG_FLOAT,
	JVM_CP_TAG_LONG,
	JVM_CP_TAG_DOUBLE,
	JVM_CP_TAG_CLASS,
	JVM_CP_TAG_STRING,
	JVM_CP_TAG_FIELDREF,
	JVM_CP_TAG_METHODREF,
	JVM_CP_TAG_INTERFACEMETHODREF,
	JVM_CP_TAG_NAMEANDTYPE
};

/* Invalid Tag for CP[0] */
#define JVM_CP_TAG_INVALID   0xff

/** Opcode boundaries **/
#define  JVM_MIN_OPCODE 0
#define  JVM_MAX_OPCODE 201

#define  JVM_MIN_QUICK_OPCODE 203
#define  JVM_MAX_QUICK_OPCODE 228

/** Native Loader types ( read only )**/
#define PRIMITIVE_LOADER  &_jaoGPrimitiveLoader

/** Trusted class names **/

#define S_JAVA_LANG_OBJECT          			   JVM_SPRING_CREATE("java/lang/Object")
#define S_JAVA_LANG_CLASS           			   JVM_SPRING_CREATE("java/lang/Class")
#define S_JAVA_LANG_STRING          			   JVM_SPRING_CREATE("java/lang/String")
#define S_JAVA_LANG_THROWABLE       			   JVM_SPRING_CREATE("java/lang/Throwable")
#define S_JAVA_LANG_SYSTEM          			   JVM_SPRING_CREATE("java/lang/System")
#define S_JAVA_LANG_EXCEPTION       			   JVM_SPRING_CREATE("java/lang/Exception")
#define S_JAVA_LANG_THREADGROUP     			   JVM_SPRING_CREATE("java/lang/ThreadGroup")
#define S_JAVA_LANG_CLASSLOADER	    			   JVM_SPRING_CREATE("java/lang/ClassLoader")    
#define S_JAVA_LANG_BOOLEAN	        			   JVM_SPRING_CREATE("java/lang/Boolean")    
#define S_JAVA_LANG_CHARACTER	    			   JVM_SPRING_CREATE("java/lang/Character")    
#define S_JAVA_LANG_FLOAT	        			   JVM_SPRING_CREATE("java/lang/Float")    
#define S_JAVA_LANG_INTEGER	        			   JVM_SPRING_CREATE("java/lang/Integer")    
#define S_JAVA_LANG_LONG  	        			   JVM_SPRING_CREATE("java/lang/Long")    
#define S_JAVA_LANG_MATH  	        			   JVM_SPRING_CREATE("java/lang/Math")    
#define S_JAVA_LANG_CLONEABLE 	    			   JVM_SPRING_CREATE("java/lang/Cloneable")    
#define S_JAVA_LANG_COMPILER  	    			   JVM_SPRING_CREATE("java/lang/Compiler")    
#define S_JAVA_LANG_INTERNALERROR   			   JVM_SPRING_CREATE("java/lang/InternalError")
#define S_JAVA_LANG_CLASSNOTFOUNDEXCEPTION   	   JVM_SPRING_CREATE("java/lang/ClassNotFoundException")
#define S_JAVA_LANG_LINKAGEERROR    			   JVM_SPRING_CREATE("java/lang/LinkageError")         
#define S_JAVA_LANG_CLASSCASTEXCEPTION			   JVM_SPRING_CREATE("java/lang/ClassCastException")
#define S_JAVA_LANG_CLASSCIRCULARITYERROR   	   JVM_SPRING_CREATE("java/lang/ClassCircularityError ")
#define S_JAVA_LANG_CLASSFORMATERROR        	   JVM_SPRING_CREATE("java/lang/ClassFormatError")
#define S_JAVA_LANG_ABSTRACTMETHODERROR     	   JVM_SPRING_CREATE("java/lang/AbstractMethodError")
#define S_JAVA_LANG_ARITHMETICEXCEPTION    		   JVM_SPRING_CREATE("java/lang/ArithmeticException")
#define S_JAVA_LANG_ARRAYSTOREEXCEPTION     	   JVM_SPRING_CREATE("java/lang/ArrayStoreException")
#define S_JAVA_LANG_ILLEGALACCESSERROR      	   JVM_SPRING_CREATE("java/lang/IllegalAccessError")
#define S_JAVA_LANG_INSTANTIATIONERROR      	   JVM_SPRING_CREATE("java/lang/InstantiationError")
#define S_JAVA_LANG_INTERRUPTEDEXCEPTION    	   JVM_SPRING_CREATE("java/lang/System")         
#define S_JAVA_LANG_ILLEGALACCESSEXCEPTION  	   JVM_SPRING_CREATE("java/lang/IllegalAccess")    
#define S_JAVA_LANG_CLONENOTSUPPORTED       	   JVM_SPRING_CREATE("java/lang/CloneNotSupported")    
#define S_JAVA_LANG_ILLEGALARGUMENTEXCEPTION       JVM_SPRING_CREATE("java/lang/IllegalArgumentException") 
#define S_JAVA_LANG_ILLEGALMONITORSTATEEXCEPTION   JVM_SPRING_CREATE("java/lang/Illegalmonitorstateexception")      
#define S_JAVA_LANG_ILLEGALTHREADSTATEEXCEPTION    JVM_SPRING_CREATE("java/lang/Illegalthreadstateexception")   
#define S_JAVA_LANG_INCOMPATIBLECLASSCHANGEERROR   JVM_SPRING_CREATE("java/lang/Incompatibleclasschangeerror")   
#define S_JAVA_LANG_INDEXOUTOFBOUNDSEXCEPTION	   JVM_SPRING_CREATE("java/lang/IndexOutOfBoundsException")   
#define S_JAVA_LANG_ARRAYINDEXOUTOFBOUNDSEXCEPTION JVM_SPRING_CREATE("java/lang/ArrayIndexOutOfBoundsException")

#define S_JAVA_LANG_STRING_1D_ARR   JVM_SPRING_CREATE("[Ljava/lang/String;")                
#define S_JAVA_LANG_STRING_2D_ARR   JVM_SPRING_CREATE("[[Ljava/lang/String;")


#define LEN_JAVA_LANG_STRING        JvmSpringGetLen( S_JAVA_LANG_STRING )


/** Trusted method names **/
#define S_METHOD_LOADCLASS          JVM_SPRING_CREATE("loadClass")
#define S_METHOD_INIT               JVM_SPRING_CREATE("<init>")
#define S_METHOD_CLINIT             JVM_SPRING_CREATE("<clinit>")

/** Trusted method descriptors **/
#define S_DESC_JLSTRING_INIT        JVM_SPRING_CREATE("([C)V")
#define S_DESC_COMMON_INIT          JVM_SPRING_CREATE("()V")
#define S_DESC_COMMON_CLINIT        JVM_SPRING_CREATE("()V")
#define S_DESC_JLCLASS_LOADCLASS    JVM_SPRING_CREATE("(Ljava/lang/String;Z)Ljava/lang/Class")

/** Recognized Attributes **/
#define S_ATTRIB_CODE               JVM_SPRING_CREATE("Code")
#define S_ATTRIB_CONSTANTVALUE      JVM_SPRING_CREATE("ConstantValue")
#define S_ATTRIB_EXCEPTIONS         JVM_SPRING_CREATE("Exceptions")
#define S_ATTRIB_LINENUMBERTABLE    JVM_SPRING_CREATE("LineNumberTable")
#define S_ATTRIB_LOCALVARIABLETABLE JVM_SPRING_CREATE("LocalVariableTable")
#define S_ATTRIB_SOURCEFILE         JVM_SPRING_CREATE("SourceFile")  
#define S_ATTRIB_INNERCLASSES       JVM_SPRING_CREATE("InnerClasses")  
#define S_ATTRIB_DEPRECATED         JVM_SPRING_CREATE("Deprecated")  
#define S_ATTRIB_SYNTHETIC          JVM_SPRING_CREATE("Synthetic")  


/** Host Attribute IDs **/
enum { ATTRIB_HOST_CODE = 1, 
       ATTRIB_HOST_METHOD,
       ATTRIB_HOST_FIELD,
       ATTRIB_HOST_CLASS
};

/** Others **/
enum { STRING_LOADED = 1 };

/** Useful macros **/
#define  SIZE_POOL    sizeof( _JVM_Constant_Pool )
#define  MV_WORD( x )   ++((JVM_WORD  * ) (x));
#define  MV_DWORD( x )  ++((JVM_DWORD * ) (x));
#define  CP_LEN( x )       ((_JVM_Constant_Pool *)(x))->jwLen
#define  CP_TAG( x )       ((_JVM_Constant_Pool *)(x))->jbyTag
#define  CP_CLASSREF( x )  ((_JVM_Constant_Pool *)(x))->pjvClassRef
/** For now both use the same attribute **/
#define  CP_METHODREF( x )   ((_JVM_Constant_Pool *)(x))->pjvAttrRef 
#define  CP_FIELDREF( x )   ((_JVM_Constant_Pool *)(x))->pjvAttrRef


#endif /*  __JVM_MACROS_INCLUDE__ */

