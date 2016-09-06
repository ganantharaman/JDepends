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

/* Dependencies  1) JVM Data type header
                 2) JVM exception header
				 3) JVM runtime header
*/

#ifndef __JVM_RUNTIME_INCLUDE__
#define __JVM_RUNTIME_INCLUDE__

#include <jvmmcros.h>
#include <jvmexcmn.h>
#include <jvmerr.h>
#include <hashtabl.h>
#include <linklist.h>

#define S_VALUE   JVM_SPRING_CREATE("value")
#define S_OFFSET  JVM_SPRING_CREATE("offset")
#define S_COUNT   JVM_SPRING_CREATE("count")



typedef struct _JVM_Exec_Env  _JVM_Exec_Env; 


typedef struct _JVM_Sys_Objects
{
   link_list_t * pllFileHandlesList;
   JVM_STRING  jsPathMark;
   JVM_STRING  jsClassPath;
   JVM_STRING  jsCurrentWorkingDir;
} _JVM_Sys_Objects;



typedef struct _JVM_Java_Data
{
  JVM_LONG jlCount;
  JVM_WORD *pjwData;
}_JVM_Java_Data;

/* ACTIVE */
typedef struct _JVM_Constant_Pool{ 
   JVM_PVOID pjvClassRef;
   JVM_PVOID pjvAttrRef;
   JVM_BYTE  jbyTag;
   JVM_WORD  jwLen;
   JVM_PBYTE pjbInfo;
}_JVM_Constant_Pool;



enum{
	JVM_THREAD_RUNNING,
	JVM_THREAD_WAITING,
	JVM_THREAD_SLEEPING,
};


typedef struct _JVM_Field_Info {
   JVM_WORD   jwFieldType;
   JVM_CHAR   jcBaseType;
   JVM_INT    jiWidthInWords;
   JVM_WORD   jwArrayDimension;
   JVM_STRING jsObjectName;
   JVM_INT    jiObjectNameLen;
   JVM_DWORD  jdwStatus;
}_JVM_Field_Info;

typedef struct _JVM_Method_Info {
  _JVM_Field_Info  **pjfiParameterFields;
  _JVM_Field_Info    jfiReturnField;
   JVM_BOOL          jbReturn;
   JVM_INT           jiParameterCount;
   JVM_INT           jiParamsWidthInWords;
   JVM_DWORD         jdwStatus;
}_JVM_Method_Info;


typedef struct _JVM_Field
{
	union {
		JAVA_CHAR   jac;
    	JAVA_BYTE   jaby;
		JAVA_SHORT  jas;
		JAVA_INT    jai;
		JAVA_BOOL   jab; 
		JAVA_LONG   jal; 
		JAVA_FLOAT  jaf;
		JAVA_DOUBLE jad;
		JAVA_OBJREF jaoref;
		JVM_STRING  js;
	}u;
   JVM_WORD jwFieldCPIndex;
   JVM_STRING jsFieldName;
   JVM_STRING jsFieldDesc;
   JVM_WORD jwFieldAccessFlags;
   _JVM_Field_Info  jfiFieldType;
}_JVM_Field;


/** CHECK,CHECK to be expanded **/

typedef struct  _JVM_LN_Table_Info {
   JVM_WORD jwStartPC;
   JVM_WORD jwLineNumber;
}_JVM_LN_Table_Info;

typedef struct  _JVM_LV_Table_Info {
   JVM_WORD jwStartPC;
   JVM_WORD jwLength;
   JVM_WORD jwNameIndex;
   JVM_WORD jwDescIndex;
   JVM_WORD jwIndex;
   _JVM_Field *pjfField;
}_JVM_LV_Table_Info;


typedef struct _JVM_EXC_Table_Info {
   JVM_WORD jwStartPC;
   JVM_WORD jwEndPC;
   JVM_WORD jwHandlerPC;
   JVM_WORD jwCatchType;
}_JVM_EXC_Table_Info;

typedef struct _JVM_ByteCode_Info {
  /** CHECK,CHECK for repetition of this field **/
	JVM_WORD  jwMaxLocals;
	JVM_WORD  jwMaxStack;
	JVM_DWORD jdwCodeLength;
	JVM_PBYTE pjbyCode;
	JVM_WORD  jwLineNumTableCount;
	JVM_WORD  jwLocalVarTableCount;
	JVM_WORD  jwExceptionTableCount;
	JVM_INT   jiLocalVarsWidthInWords;
  _JVM_EXC_Table_Info  **ppjetiExceptionTable;
  _JVM_LN_Table_Info   **ppjltiLineNumTable;
  _JVM_LV_Table_Info   **ppjltiLocalVarTable;
} _JVM_ByteCode_Info;


typedef struct _JVM_Method {
  /** CHECK,CHECK **/
  _JVM_Method_Info * pjmiMethodInfo;
   /** The Bytecode **/
  _JVM_ByteCode_Info   * pjciCodeInfo;
   /** Exception table **/
   JVM_WORD * pjwMthdExcTable;
   /** Exception table count **/
   JVM_WORD jwMthdExcTableCount;
   /** Access Flags + other properties**/
   JVM_WORD jwMethodAccessFlags;
   /** Method's CP Index **/
   JVM_WORD jwMethodCPIndex;
   /** Method Name **/
   JVM_STRING jsMethodName;
   /** Method Descriptor **/
   JVM_STRING jsMethodDesc;
   /** Native method ptr  **/
   JVM_PVOID  pfnNativeMethod;
}_JVM_Method;

typedef struct _JVM_Class_State
{
  eJVMExceptionType ejeException ;
  JVM_INT           jiState;
}_JVM_Class_State;

/** for now just store the CP indices **/
typedef struct _JVM_NestedClass_Info{
    JVM_WORD jwInnerClassInfoIndex;
	JVM_WORD jwOuterClassInfoIndex;
	JVM_WORD jwInnerNameIndex;
	JVM_WORD jwInnerClassAccessFlags;
}_JVM_NestedClass_Info;

typedef struct _JVM_InnerClass_Info
{
   JVM_WORD jwNumberOfClasses;
  _JVM_NestedClass_Info ** ppjniNestedClassInfo;
} _JVM_InnerClass_Info;


typedef struct _JVM_Class_Class
{
  JVM_DWORD     jdwClassFlags;
  JVM_STRING    jsClassName;
  JVM_WORD      jwClassNameLen;
  JVM_STRING    jsSuperClassName;
  JVM_STRING    jsSourceFileName;
  JVM_WORD      jwSourceFileNameLen;
  JVM_WORD      jwFieldsCount;
  JVM_WORD      jwMethodsCount;
  JVM_WORD      jwIntfCount;
  JVM_U16 *     pju16InterfacesCPIndex;
 _JVM_Field  ** ppjfData;
 _JVM_Method ** ppjmMethods;
  Dictionary  * pdNCPDict;
  JVM_WORD     jwCPEntryCount;
  JVM_INT      jiInitState;
  JVM_INT      jiPrepState;
  JVM_FILE     jfNativeLibrary;

  _JVM_Constant_Pool       ** ppjcpConstantPool;
  struct _JVM_Object        * joClassLoaderObject;
  struct _JVM_Object        * joSuperClassObject;
  struct _JVM_Object        * joClassClassObject;
  struct _JVM_Object        * joObjectClassObject;
  struct _JVM_Object        * joArrayBaseTypeObject;
  struct _JVM_Object       ** ppjoInterfaces;
  struct _JVM_Field_Info    * pjfiArrayInfo;
  struct _JVM_InnerClass_Info * pjiiInnerClassInfo;
}_JVM_Class_Class;

/* Stack frame */
typedef struct _JVM_Stack_Frame
{
  struct _JVM_Stack_Frame  *   pjsfPrevFrame;
  JVM_U32                      ju32OpStackSize;
  JAVA_WORD                *   pjawOpStackElements;
  JAVA_WORD			       	   *   pjawLocals; 
  JVM_U32                      ju32LocalVarsSize;
  JVM_U32                      ju32TopElementIndex;
  JVM_U32                  *   pju32StackMap; 
 _JVM_Method			         *   pjmCurrMethod;
 _JVM_Class_Class		       *   pjccCurrClass;
  JAVA_WORD                     jawPC;
  /* More things here */
}_JVM_Stack_Frame;


typedef struct _JVM_Thread_State 
{
  struct _JVM_Thread_State *pjtsNext;          /** Next Thread State           **/
  JVM_THREAD jtHandle;                         /** Current thread handle       **/
  JVM_UINT   jnThreadPriority;                 /** Thread Priority 0(Highest)-9(lowest) **/
 _JVM_Except_Frame * pjefTopFrame;             /** Top of exception stack      **/ 
 _JVM_Stack_Frame  * pjsfFrames;               /** Stack frames array          **/
 _JVM_Error_Info   *pjeiErrorStack;            /** Top of Error Stack          **/
 _JVM_PFN_Default_Except_Handler pjdehDefExHandler;   /** Default exception handler **/
 _JVM_Stack_Frame *pjsfCurrFrame;
 _JVM_Exec_Env * pjeeEnvCaller;                /** Execution Environment of caller  **/
 _JVM_Exec_Env * pjeeCurrExecEnv;              /** Our Execution env           **/
  JVM_UINT juWaitTimeout;                      /** Used for thread timeout     **/
  JVM_UINT juAllocFrames;                       /** Allocated stack frame count **/
  JVM_UINT juNextFrame;                        /** Next Available Frame index  **/
  JVM_WORD jwLockCount;                        /** Current Lock count          **/ 
  JVM_WORD jwThreadState;                      /** Thread's current state(idle etc.) **/
  G_STATE     SystemState;                     /** Subsystem 'Go' State indicators   **/
  JVM_BUFFER jbTempBuf;                        /** Temporary buffer            **/
}_JVM_Thread_State;


/* Private Mutex for accessing JVM Globals 
   only one mutex object is used */
typedef struct _JVM_Mutex_Object
{
   JVM_MUTEX  jmMutex;
   /* Owner thread */
  _JVM_Thread_State *pjtsOwnerThread;
}_JVM_Mutex_Object;

/* JVM Lock */
typedef struct _JVM_Lock{
  _JVM_Thread_State *pjtsOwnerThread;
  JVM_WORD jwLockCount;
  JVM_WORD jwThreadCount;
}_JVM_Lock;



typedef struct _JVM_Thread_State_List{
	_JVM_Thread_State      *pjtsThisThread;
    struct _JVM_Thread_State_List *pjtslNext;
    struct _JVM_Thread_State_List *pjtslPrev;
}_JVM_Thread_State_List;


typedef struct _JVM_Priority_List {
  struct _JVM_Thread_State_List  *pjtslFirstThread;
  struct _JVM_Thread_State_List  *pjtslLastThread;
}_JVM_Priority_List;

/* We arrange the threads based on priority */
typedef struct _JVM_Wait_Set
{
   _JVM_Priority_List  jplList[JVM_THREAD_MAX_PRIORITY];
} _JVM_Wait_Set;

typedef struct _JVM_Entry_Set
{
  _JVM_Priority_List  jplList[JVM_THREAD_MAX_PRIORITY];
} _JVM_Entry_Set;




// Execution env
typedef struct _JVM_Exec_Env
{
  JVM_U32 ju32OpStackTop;
  JVM_PBYTE pjbyCode;
  _JVM_Stack_Frame     * pjsfFrame;
  _JVM_Thread_State    * pjtsThreadState;
  struct _JVM_Exec_Env * pjeePrev;
}_JVM_Exec_Env;

typedef struct _JVM_Object_ID
{
   JVM_BYTE jbyID;
  _JVM_Class_Class     * pjccClass;
} _JVM_Object_ID;

typedef enum _JVM_Object_State {
  JAVA_OBJECT_STATE_LOADING = 0,
  JAVA_OBJECT_STATE_LOADED,
  JAVA_OBJECT_STATE_INITIALIZING,
  JAVA_OBJECT_STATE_INITIALIZED,
  JAVA_OBJECT_STATE_FINALIZING,
  JAVA_OBJECT_STATE_FINALIZED,
} _JVM_Object_State;


// An object of a class
typedef struct _JVM_Object
{
  /** SNIPER WARNING, WARNING ,WARNING ---Do not change the order or position**/
 _JVM_Object_ID  joiObjectID;
  /** SNIPER WARNING Ends **/

  struct _JVM_Object  * pPrev;
  struct _JVM_Object  * pNext;
  _JVM_Object_State    jiState;
  _JVM_Field          *** pppjfFields;
  _JVM_Method         *** pppjmMethods;   
  JVM_U16             * FieldsCount;
  JVM_U16               ClassesCount;
  JVM_ULONG             julDataCount;
  JVM_WORD              jwRefCount;
  _JVM_Wait_Set	      * pjwsWaitSet;
  _JVM_Entry_Set      * pjesEntrySet;
  _JVM_Lock           * pjlLock;
  _JVM_Thread_State   * pjtsOwner;
}_JVM_Object;

typedef struct _JVM_Array_Object 
{
   /** SNIPER WARNING, WARNING ,WARNING ---Do not change the order or position**/
	_JVM_Object_ID  joiObjectID;
   /** SNIPER WARNING Ends **/
	union {
		   JAVA_OBJREF  *  pjarRef;
		   JAVA_LONG    *  pjalLong;
		   JAVA_DOUBLE  *  pjadDouble;
		   JAVA_BYTE    *  pjabyByte;
		   JAVA_CHAR    *  pjacChar;
		   JAVA_INT     *  pjaiInt;
       JAVA_SHORT   *  pjasShort;
		   JAVA_FLOAT   *  pjafFloat;
	}u;

	JAVA_WORD  jawLowerBound;
	JAVA_WORD  jawUpperBound;
	JAVA_WORD  jawLength;
	JAVA_WORD  jawDimension;
	JVM_U16    juType;
   _JVM_Class_Class * pjccArrayTypeClass;

} _JVM_Array_Object; 


typedef JVM_THD_CLB_RETURN (JVM_THD_CLB_PREFIX * _JVM_PFN_Thread_Start)  
                            ( JVM_THD_CLB_PARAM );

_JVM_Thread_State * JvmGetThreadState();
_JVM_Thread_State * JvmThreadInit();
JVM_THREAD JvmGetCurrentThread();

_JVM_Thread_State_List * JvmAllocNewThreadStateList();
JVM_VOID JvmFreeThreadStateList( _JVM_Thread_State_List *pjtslList );

/* usually used in conjunction with Global data
   access functions */
_JVM_Mutex_Object * JvmAcquireMutex();
_JVM_Mutex_Object * JvmReleaseMutex();

/* Usually used in conjunction with Main memory
   Object access functions  */
JVM_VOID JvmAcquireLock(_JVM_Thread_State *pjtsThisThread ,
						_JVM_Object *pjciObject );

/* Usually used for synchronization */ 
JVM_VOID JvmWait(_JVM_Thread_State *pjtsThisThread ,
				 _JVM_Object *pjciObject,
				  JVM_UINT juTimeOut);

JVM_VOID JvmNotify(_JVM_Thread_State *pjtsThreadState ,
				   _JVM_Object *pjciObject);

JVM_VOID  JvmNotifyAll();

JVM_BOOL JvmIsOwnerThread(  _JVM_Thread_State *pjtsThisThread,
						    _JVM_Object *pjciObject);
JVM_BOOL JvmIsWaitSetEmpty( _JVM_Wait_Set *pjwsWaitSet,
						     JVM_UINT *pjwStartPriority 
					   	  );

JVM_VOID JvmRtSetUpStackFrame( 
  IN    _JVM_Method *pjmMethod,
  IN    _JVM_Class_Class *pjccTheClass 
);



/* Used only for Thread stacks */

JVM_OBJREF JvmRtCreateNewInstance(
  IN      _JVM_Class_Class *pjccTheClass 
);

JVM_OBJREF JvmRtCreateNewInstanceEx(
  IN      _JVM_Class_Class *pjccTheClass 
);

_JVM_Field * JvmResolveField(
  IN     JVM_PBYTE pjbyCPByte,
  IN    _JVM_Class_Class * pjccRefClass,
  IN    _JVM_Stack_Frame * pjsfCurrFrame);
  
_JVM_Class_Class * JvmResolveClass(
  IN    JVM_PBYTE pjbyCPByte, 
  IN   _JVM_Exec_Env * pjeeExecEnv);  

JVM_BOOL JvmRtInvokeMethod(  
  IN     JVM_STRING jsMethodName, 
  IN     JVM_STRING jsMethodDesc,
  IN     JVM_OBJREF joObjRef,
  IN     JAVA_WORD  * pjawArgs,
  IN     JVM_INT      jiArgCount,
  IN    _JVM_Exec_Env * pjeeEnvCaller
);

JVM_BOOL JvmRtCreateObject_JavaLangString_v0( 
 IN  JVM_STRING jsName,
 IN  JAVA_OBJREF * pjaoJavaLangStringRef 
);

JVM_VOID JvmRtSaveContext( 
  IN    _JVM_Exec_Env * pjeeExecEnv 
);


JVM_VOID JvmRtRestoreContext( 
  IN    _JVM_Exec_Env * pjeeExecEnv 
);

_JVM_Exec_Env * JvmRtSetUpExecEnv(
  IN  _JVM_Exec_Env * pjeeEnvCaller 
);

_JVM_Array_Object * JvmRtCreateNewArray(
  IN   JVM_U16  juType,
  IN   JVM_U16  juDimension,
  IN   JVM_U32  * pjuLength,
  IN   _JVM_Class_Class * pjccThisArrayClass,
  IN   _JVM_Class_Class * pjccClass
);

JVM_VOID JvmRtDestroyExecEnv();

JVM_VOID JvmRtGetReturnValue(
 IN     _JVM_Exec_Env * pjeeEnvCaller,
 IN     _JVM_Method * pjmMethod,
 OUT    _JVM_Field  * pjfiReturn
 );

JVM_VOID JvmFieldStoreValue( 
 IN	    _JVM_Field * pjfiField, 
 IN      JAVA_WORD   jawWord
 );

JVM_VOID JvmRtSetUpParameters( 
 IN      JVM_OBJREF joObjRef, 
 IN      JAVA_WORD  * pjawArgs,
 IN      JVM_INT      jiArgCount
 );

JVM_BOOL IsReference(
 IN JVM_PVOID pjvValue 
);

JVM_BOOL IsAssignmentCompatible(
IN  JAVA_OBJREF jaoSource,
IN  JAVA_OBJREF jaoTarget
);

JVM_BOOL IsValidMethodOf( 
 IN      _JVM_Class_Class   * pjccClass,
 IN      JVM_STRING         jsMethodName,
 IN      JVM_STRING         jsMethodDesc, 
 OUT     _JVM_Method     ** ppjmMethod
 );

JVM_BOOL IsValidFieldOf( 
 IN      _JVM_Class_Class   * pjccClass,
 IN       JVM_STRING         jsFieldName,
 IN       JVM_STRING         jsFieldDesc, 
 OUT     _JVM_Field       ** ppjfField
);


JVM_BOOL JvmRtCallMethod(  
 IN     _JVM_Exec_Env * pjeeEnvCaller,
 IN     _JVM_Method * pjmMethod,
 IN     _JVM_Class_Class *pjccClass,
 IN      JVM_OBJREF joObjRef,
 IN      JAVA_WORD  * pjawArgs,
 IN      JVM_INT      jiArgCount
);

JVM_BOOL IsSamePackage(
IN  _JVM_Class_Class * pjccClassA,
IN  _JVM_Class_Class * pjccClassB
);

JVM_U32 JvmRtExecute();

JVM_U32 JvmByteCodeExecute( 
  IN     _JVM_Thread_State *pjtsThreadState
);


_JVM_Stack_Frame * JvmRtGetNextStackFrame( 
  IN _JVM_Thread_State * pjtsThreadState 
);

JVM_BOOL JvmGetNearestBaseMethod( 
 IN  _JVM_Class_Class * pjccTheClass,
 IN   JVM_STRING jsMethodName,
 IN   JVM_STRING jsMethodDesc,
 OUT _JVM_Method ** ppjmMethod, 
 OUT _JVM_Class_Class ** ppjccClass
);


JVM_VOID JvmRtDestroyStackFrame();

#endif /* __JVM_RUNTIME_INCLUDE__ */



