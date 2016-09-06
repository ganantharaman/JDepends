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

#ifndef __JVM_PARSE_INCLUDE__
#define __JVM_PARSE_INCLUDE__

#include <jvmrt.h>

#define S_JVM_PARSE_METHODDESCRIPTOR    JVM_SPRING_CREATE("JvmParseMethodDescriptor")
#define S_JVM_PARSE_PARAMETERDESCRIPTOR JVM_SPRING_CREATE("JvmParseParameterDescriptor")
#define S_JVM_PARSE_RETURNDESCRIPTOR    JVM_SPRING_CREATE("JvmParseReturnDescriptor")
#define S_JVM_PARSE_FIELDDESCRIPTOR     JVM_SPRING_CREATE("JvmParseFieldDescriptor")


JVM_BOOL JvmParseMethodDescriptor( 
   IN OUT  JVM_PBYTE * ppjbByteStream ,
   OUT    _JVM_Method_Info *pjmiMethod
);

JVM_VOID JvmStoreParametersSize( 
 IN		_JVM_Method_Info * pjmMethod 
);



JVM_DWORD JvmParseParameterDescriptor( 
   IN OUT  JVM_PBYTE  * ppjbByteStream ,
   OUT    _JVM_Field_Info *** pppjfiParamFields, 
   OUT     JVM_INT    * pjiParamCount
);

JVM_DWORD JvmParseReturnDescriptor(
   IN OUT  JVM_PBYTE      * ppjbByteStream ,
   OUT     JVM_BOOL       * pjbReturnExistFlag,
   OUT    _JVM_Field_Info * pjfiField
);

JVM_VOID JvmParseCleanupFields(
   IN     _JVM_Field_Info *** pppjfiFields,
   IN	   JVM_INT jiParameterCount
);

JVM_VOID JvmParseCleanAFieldInfo( 
 IN     _JVM_Field_Info * ppjfiFieldInfo
);

JVM_BOOL JvmParseFieldDescriptor( 
   IN OUT  JVM_PBYTE      * ppjbByteStream ,
   OUT    _JVM_Field_Info * pjfiField
);


JVM_BOOL IsJvmBaseType( 
   IN OUT  JVM_PBYTE * ppjpbStream,
   OUT     JVM_PBYTE pjpbType,
   OUT     JVM_INT  * pjiWidthInWords
);

JVM_BOOL IsJvmObjectType(
   IN OUT  JVM_PBYTE  * ppjbStream,
   OUT     JVM_STRING * pjsObjectTypeName,
   OUT     JVM_INT    * pjiObjectTypeNameLen
);

JVM_BOOL IsJvmArrayType(
   IN OUT  JVM_PBYTE  * ppjbStream 
);



#endif /* __JVM_PARSE_INCLUDE__ */



