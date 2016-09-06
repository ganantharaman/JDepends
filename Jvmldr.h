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

#ifndef __JVM_LOADER_INCLUDE__
#define __JVM_LOADER_INCLUDE__

#include <cmnmcros.h>


typedef JVM_VOID ( * JVM_PFN_CP_TYPES )(JVM_PBYTE *,
										_JVM_Constant_Pool ***,
										JVM_WORD *,
										JVM_DWORD *
									   );

typedef JVM_BOOL ( * JVM_PFN_ATTRIB_HANDLER )( JVM_PBYTE  *,
                                               JVM_PVOID  *,
                                              _JVM_Class_Class *,
											   JVM_WORD 
											  );
JVM_BOOL JvmLdrInit();
JVM_BOOL JvmLdrExit();

JVM_STRING JvmLdrGetStringName( 
 IN    _JVM_Class_Class *pjccTheClass,
 IN     JVM_WORD  jwCPIndex,
 OUT  JVM_WORD * pjwNameLen
 );

JVM_BOOL JvmLdrAddNewClassLoader( 
  IN     JVM_OBJREF joClassLoader 
);

JVM_BOOL JvmLdrGetClassObject(
  IN  JVM_STRING jsClassName, 
  IN  JVM_OBJREF joClassLoader,
  OUT JVM_OBJREF * pjoObjRef
);


JVM_BOOL JvmLdrLoadNewClass(
  IN     JVM_STRING  jsClassFileName,
  IN     JVM_PBYTE   pjbyClassBytes,
  IN     JVM_OBJREF  joClassLoader, 
  OUT    JVM_OBJREF  * pjoClassObject
);


JVM_OBJREF JvmLoadClass(
  IN  JVM_STRING jsClassName,
  IN  JVM_OBJREF joClassLoader 
);

JVM_BOOL JvmLdrLoadSystemClasses(
 IN     JVM_OBJREF  joClassLoader
);


JVM_BOOL JvmLdrAddNewClass( 
  IN     JVM_OBJREF joClassLoaderObject ,
  IN     JVM_OBJREF joClassObject, 
  IN     JVM_STRING jsClassName 
);

JVM_FILE JvmOpenClassFile( 
  IN     JVM_STRING jsClassFileName 
);

JVM_PBYTE JvmReadClassFile( 
  IN     JVM_FILE jfiClassFile
);

JVM_VOID JvmDeferException( 
  IN     JVM_STRING jsClassName,
  IN     JVM_OBJREF joClassLoader,
  IN    eJVMExceptionType ejeException 
);

JVM_BOOL JvmLdrCheckMagic( 
  IN     JVM_PBYTE * ppjbClassStream
);

JVM_BOOL JvmLdrCheckVersion(
  IN  	 JVM_PBYTE * ppjbClassStream 
);

_JVM_Class_Class * JvmGetNewClass(
);

JVM_VOID JvmCloseClassFile( 
  IN     JVM_FILE  jfiClassFile,
  IN     JVM_PBYTE * ppjbyClassStream
);


JVM_BOOL JvmGenericAttribHandler(
  IN OUT  JVM_PBYTE  * ppjbClassStream,
  IN OUT  JVM_PVOID    pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN  	  JVM_WORD jwAttribCount,
  IN      JVM_WORD jwAttribHost
);


JVM_BOOL  JvmLdrReadFieldAttributes( 
  IN      JVM_WORD    jwAttribCount, 
  IN     _JVM_Field * pjfField, 
  IN      JVM_PBYTE * ppjbClassStream,
  IN      JVM_WORD    jwFieldsCount,
  IN OUT _JVM_Class_Class *pjccTheClass
);

JVM_VOID JvmAddNewCPType(
  IN OUT JVM_PBYTE *  ppjbClassStream,
  IN OUT _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD  *  pjwCurPoolSize, 
  IN OUT JVM_DWORD *  pjdwStatus,
  IN     JVM_WORD     jwTag,
  IN     JVM_WORD     jwBytesToCopy
);

_JVM_Constant_Pool ** JvmLdrAddNewCPInfoToCPVector( 
  IN     _JVM_Constant_Pool ** ppjcpVector,
  IN OUT  JVM_WORD          *  pjwCurCPSize, 
  IN      JVM_PBYTE            pjbCPInfo
);


JVM_STRING JvmGetClassName(
  IN  _JVM_Class_Class * pjccTheClass, 
  IN   JVM_WORD jwCPConstClassIndex,
  OUT  JVM_WORD * pjwClassNameLen
);


JVM_BOOL  JvmHdlAttrCode( 
  IN      JVM_PBYTE  * ppjbClassStream,
  IN      JVM_PVOID    pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);

JVM_BOOL JvmHdlAttrConstantValue( 
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);

JVM_BOOL JvmHdlAttrExceptions( 
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);

JVM_BOOL JvmHdlAttrLineNumberTable( 
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);

JVM_BOOL JvmHdlAttrLocalVarTable( 
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);

JVM_BOOL JvmHdlAttrSourceFile( 
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);

JVM_BOOL JvmHdlAttrInnerClasses(
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);

JVM_BOOL JvmHdlAttrSynthetic(
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);

JVM_BOOL JvmHdlAttrDeprecated(
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);

JVM_BOOL JvmHdlAttrDefault( 
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
);


JVM_VOID JvmCPTypeConstUtf8(
  IN OUT  JVM_PBYTE * ppjbClassStream,
  IN OUT _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT  JVM_WORD *  pjwCurPoolSize, 
  IN OUT  JVM_DWORD * pjdwStatus
);

JVM_VOID JvmCPTypeConstInteger(
  IN OUT  JVM_PBYTE * ppjbClassStream,
  IN OUT _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT  JVM_WORD *  pjwCurPoolSize, 
  IN OUT  JVM_DWORD * pjdwStatus
);

JVM_VOID JvmCPTypeConstFloat(
  IN OUT JVM_PBYTE            * ppjbClassStream,
  IN OUT _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD             * pjwCurPoolSize, 
  IN OUT JVM_DWORD            * pjdwStatus
);

JVM_VOID JvmCPTypeConstDouble(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpThePool,
  IN OUT JVM_WORD            * pjwCurPoolSize, 
  IN     JVM_DWORD * pjdwStatus
);

JVM_VOID JvmCPTypeConstLong(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpThePool,
  IN OUT JVM_WORD             * pjwCurPoolSize, 
  IN     JVM_DWORD * pjdwStatus
);

JVM_VOID JvmCPTypeConstClass(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpThePool,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
);


JVM_VOID JvmCPTypeConstString(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpThePool,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
);


JVM_VOID JvmCPTypeConstFieldRef(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpThePool,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
);

JVM_VOID JvmCPTypeConstMethodRef(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpThePool,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
);

JVM_VOID JvmCPTypeConstInterfaceMethodRef(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpThePool,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
);

JVM_VOID JvmCPTypeConstNameAndType(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpThePool,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
);

JVM_BOOL  IsKindOf( 
  IN       JVM_STRING  jsSubjectClassName,
  IN       JVM_STRING  jsTargetClassName,
  IN       JVM_OBJREF  joClassLoader
);

JVM_BOOL  JvmLdrReadFields( 
  IN OUT  JVM_PBYTE * ppjbClassStream,
  IN      _JVM_Class_Class * pjccTheClass,
  IN      JVM_WORD jwFieldsCount
);

JVM_BOOL  JvmLdrReadMethods( 
  IN OUT  JVM_PBYTE * ppjbClassStream,
  IN OUT _JVM_Class_Class * pjccTheClass,
  IN      JVM_WORD jwMethodsCount
);

JVM_BOOL JvmLdrCVProviderInteger( 
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_INT * pjaiIntValue     
);

JVM_BOOL JvmLdrCVProviderLong( 
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_LONG * pjalLongValue     
);

JVM_BOOL JvmLdrCVProviderFloat( 
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_FLOAT * pjalFloatValue     
);

JVM_BOOL JvmLdrCVProviderDouble( 
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_DOUBLE * pjalDoubleValue     
);

JVM_BOOL JvmLdrCVProviderString(
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_OBJREF * pjaoObjrefValue     
);

JVM_BOOL  JvmLdrReadMethodAttributes( 
  IN      JVM_WORD    jwAttribCount, 
  IN     _JVM_Method * pjmMethod,
  IN      JVM_PBYTE  * ppjbClassStream,
  IN      JVM_WORD    jwMethodsCount,
  IN OUT _JVM_Class_Class *pjccTheClass
);

JVM_BOOL JvmGetUtf8Info(
  IN   JVM_PBYTE pjbyCPUtf8, 
  OUT  JVM_PBYTE * pjbyUtf8bytes,
  OUT  JVM_WORD  * pjwLen 
);


JVM_VOID JvmFreeThePool(                         /** TODO **/
 IN  _JVM_Constant_Pool ** ppjcpThePool,
 IN   JVM_WORD             jwPoolSize 
);

JVM_VOID JvmFreeTheClass(						 /** TODO **/
 IN  _JVM_Class_Class * pjccTheClass 
);

JVM_VOID JvmCleanUpMethods(						 /** TODO **/
 IN     _JVM_Method ** ppjmMethods,
 IN      JVM_WORD      jwMethodsCount  
);

JVM_VOID JvmLdrFreeAMethod( 
 IN 	_JVM_Method * pjmMethod 
);


JVM_VOID JvmCleanUpFields( 
 IN     _JVM_Field ** ppjfData ,
 IN      JVM_WORD     jwFieldsCount
);

JVM_VOID JvmCleanUpCode(						
 IN		_JVM_ByteCode_Info * pjbiCodeInfo 
);

JVM_VOID JvmLdrFreeAnLVTable( 
 IN     _JVM_LV_Table_Info * pjltiLVTable 
);

JVM_VOID JvmLdrFreeAField(
 IN     _JVM_Field * pjfField
);

JVM_BOOL  IsCPClassType( 
  IN       JVM_STRING  jsClassName,
  IN       JVM_BOOL  jbCheckIfSubclass,
  IN      _JVM_Class_Class * pjccTheClass
);

JVM_BOOL IsJvmOpcode(
  IN JVM_BYTE jbByte 
);


JVM_BOOL  IsCPElementType( 
  IN       JVM_WORD  jwCPElementTag,
  IN      _JVM_Class_Class * pjccTheClass,
  IN       JVM_WORD jwCPIndex
);

JVM_BOOL InvokeLoadClass(
  IN	 JVM_OBJREF * pjoObjRef,
  IN     JVM_OBJREF joLoader, 
  IN     JVM_STRING jsClassName,
  IN     JAVA_BOOL   jabLinkAndInit
);

JVM_BOOL JvmLdrGetUtf8Bytes( 
  IN   JVM_PBYTE  pjbyUtf8, 
  OUT  JVM_STRING * ppjbyString,
  OUT  JVM_WORD   * pjwStringLen
);

JVM_PFN_ATTRIB_HANDLER JvmGetAttribHandler( 
  IN OUT _JVM_Class_Class * pjccTheClass,
  IN      JVM_WORD jwUtf8Index
);

JVM_BOOL JvmLdrMakeNewArrayClass( 
 IN      JVM_OBJREF joClassLoader,
 IN      JVM_STRING jsArrayName, 
 OUT     JVM_OBJREF * pjoClassObjectRef
); 

JVM_BOOL IsSubclassOf( 
 IN 	_JVM_Class_Class * pjccS,
 IN     _JVM_Class_Class * pjccT
);

JVM_VOID HashDictionaryRundown( 
 IN 	 Hash_Type  htype
);

JVM_VOID HashClassStateRundown( 
  IN     Hash_Type htype
);

JVM_VOID DependsDictRundown( 
  IN Hash_Type htype 
);


#endif /* __JVM_LOADER_INCLUDE__ */


 