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
#ifndef __JVM_ERR_INCLUDE__
#define __JVM_ERR_INCLUDE__

#include <jvmmcros.h>


typedef struct _JVM_Error_Info
{
   struct _JVM_Error_Info *pjeiNext;
   JVM_DWORD    jdwJVMErrorCode;
   JVM_ERROR    jeOSErrorCode;
   JVM_STRING   jsOSErrorMessage;
   JVM_STRING   jsJVMErrorMessage;
   JVM_STRING   jsContextInfo;
} _JVM_Error_Info;

#define JVM_ERROR_POOLS 3

#define JVM_STATUS_OK  							 0x00000001
#define ERR_JVM_PARSE_INCORRECTMETHODDESCRIPTOR  0x00000002
#define ERR_JVM_PARSE_ERRORINRETURNTYPE		     0x00000004
#define ERR_JVM_PARSE_ERRORINPARAMETER			 0x00000008
#define ERR_JVM_PARSE_OBJECTTYPENAMETOOLONG		 0x00000010
#define ERR_JVM_PARSE_INCORRECTFIELDDESCRIPTOR   0x00000020
#define ERR_JVM_OS_ERROR                         0x00000040 


extern JVM_PBYTE _aajbErrorMessage[];

_JVM_Error_Info * JvmErrAddFormatMessage(
  IN  const JVM_PCHAR pjcFormat,
  IN  ...
);

JVM_VOID JvmErrSetErrorCode( 
  IN   JVM_DWORD jdwErrCode,
  IN   JVM_STRING jsContext
);


JVM_VOID JvmErrSetErrorMessage( 
  IN   JVM_DWORD jdwErrCode,
  IN   JVM_STRING jsContext,
  IN   JVM_INT   jiType,
  IN   JVM_DWORD jdwValue
);

JVM_VOID JvmErrSetErrorMessageEx( 
  IN   JVM_DWORD jdwErrCode,
  IN   JVM_INT   jiType,
  IN   JVM_STRING jsValue
);

JVM_STRING JvmErrGetErrorMessage( 
  IN   JVM_DWORD jdwErrorCode 
);

JVM_VOID JvmFreeErrorNode( 
  IN  _JVM_Error_Info ** ppjeiNode 
);

_JVM_Error_Info * JvmErrGetNewErrorNode(
);

_JVM_Error_Info * JvmErrCreateErrorNode(
);

JVM_VOID JvmErrGetOSErrorCode( 
 IN   JVM_ERROR jeError
);

JVM_VOID JvmErrGetOSErrorMessage(
 IN   JVM_ERROR jeError
);

JVM_VOID JvmErrSetOSErrorCode( 
 IN   JVM_ERROR jeError,
 IN   JVM_STRING jsContext
);

#endif /* __JVM_ERR_INCLUDE__ */
