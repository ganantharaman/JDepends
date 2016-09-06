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

#include <jvmerr.h>
#include <jvmmem.h>
#include <jvmrt.h>
#include <jvmutil.h>
#include <jvmsprng.h>
#include <jvmsys.h>

#define STDARG_H
#define STDIO_H
#define ASSERT_H
#include <jvmhdrs.h>


_JVM_Error_Info * JvmErrAddFormatMessage(
  IN  const JVM_PCHAR pjcFormat,
  IN  ...
)
{
 _JVM_Error_Info   *pjeiNode          = JVM_NULL;  
  va_list vaArg;
  va_start( vaArg,  pjcFormat );

  pjeiNode = JvmErrGetNewErrorNode();

  vsprintf( 
	 pjeiNode->jsJVMErrorMessage,
	 pjcFormat,
	 vaArg
	 );

  return pjeiNode;
}


_JVM_Error_Info * JvmErrGetNewErrorNode()
{
  _JVM_Thread_State *pjtsThread = JvmGetThreadState();
  _JVM_Error_Info   *pjeiNode          = JVM_NULL;  


   JVM_ASSERT_VALID( pjtsThread );
   pjeiNode           = JvmErrCreateErrorNode();

   if ( !JVM_VALID( pjtsThread->pjeiErrorStack ))
      pjtsThread->pjeiErrorStack = pjeiNode;

   pjeiNode->pjeiNext = pjtsThread->pjeiErrorStack;

   return pjeiNode;
}


_JVM_Error_Info * JvmErrCreateErrorNode()
{
  _JVM_Error_Info * pjeiNode = JVM_NULL;

  pjeiNode = (_JVM_Error_Info *) JvmMemAlloc( 
	          sizeof( _JVM_Error_Info )
			  );

  if ( JVM_VALID( pjeiNode ) )
  {
    pjeiNode->jdwJVMErrorCode  = JVM_STATUS_OK;
	pjeiNode->pjeiNext         = JVM_NULL; 
	pjeiNode->jsJVMErrorMessage   = JvmErrGetErrorMessage( JVM_STATUS_OK );
  }

  return pjeiNode;
}

JVM_VOID JvmFreeErrorNode( 
  IN  _JVM_Error_Info ** ppjeiNode 
)
{
	JvmMemFree( *ppjeiNode );
	*ppjeiNode = JVM_NULL;
}


JVM_VOID JvmErrSetErrorCode( 
  IN   JVM_DWORD jdwErrCode,
  IN   JVM_STRING jsContext
)
{
   _JVM_Error_Info * pjeiNode  =  JvmErrGetNewErrorNode();

    pjeiNode->jsJVMErrorMessage   = JvmErrGetErrorMessage( jdwErrCode );
	pjeiNode->jdwJVMErrorCode     = jdwErrCode;
	if ( JVM_VALID( jsContext ))
	  pjeiNode->jsContextInfo       = JvmSpringClone( jsContext );
}



JVM_VOID JvmErrSetErrorMessage( 
  IN   JVM_DWORD jdwErrCode,
  IN   JVM_STRING jsContext,
  IN   JVM_INT   jiType,
  IN   JVM_DWORD jdwValue
)
{
   JVM_STRING jsErrMsg = JvmErrGetErrorMessage( jdwErrCode );
   _JVM_Error_Info * pjeiNode = JVM_NULL;

   switch ( jiType )
   {
     case TYPE_INTEGER   : pjeiNode = JvmErrAddFormatMessage("%s %d",jsErrMsg,(JVM_INT)jdwValue);
					  break; 
	 case TYPE_CHARACTER : pjeiNode = JvmErrAddFormatMessage("%s %c",jsErrMsg,(JVM_CHAR)jdwValue);
		              break; 
   case TYPE_FLOAT     : pjeiNode = JvmErrAddFormatMessage("%s %f",jsErrMsg,(JVM_FLOAT)jdwValue);
		              break; 
	 case TYPE_LONG      : pjeiNode = JvmErrAddFormatMessage("%s %f",jsErrMsg,(JVM_LONG)jdwValue);
		              break; 
     default        : pjeiNode = JvmErrAddFormatMessage("%s",jsErrMsg);
					  break;
   }
   if ( JVM_VALID( jsContext ))
    pjeiNode->jsContextInfo = JvmSpringClone( jsContext );

}

JVM_VOID JvmErrSetErrorMessageEx( 
  IN   JVM_DWORD jdwErrCode,
  IN   JVM_INT   jiType,
  IN   JVM_STRING jsValue
)
{
   if ( JVM_VALID( jsValue ))
   {
	  JvmErrAddFormatMessage("%s %s", JvmErrGetErrorMessage( jdwErrCode ),
		                              jsValue );
   }
 
}



JVM_STRING JvmErrGetErrorMessage( 
  IN   JVM_DWORD jdwErrorCode 
)
{
   JVM_DWORD jdwMask = ~(JVM_DWORD)0 - ( ~(JVM_DWORD)0 >> JVM_ERROR_POOLS );
   JVM_DWORD jdwMsgMax = atoi( _aajbErrorMessage[0] );
   JVM_STRING jsErrorMessage = JVM_NULL;

   JVM_UINT juIndex = ( jdwErrorCode & jdwMask ) * (JVM_DWORD_IN_BITS - JVM_ERROR_POOLS) + 
	     JvmBitIndex (( jdwErrorCode << JVM_ERROR_POOLS ) >> JVM_ERROR_POOLS );

   if ( juIndex <= jdwMsgMax )
      jsErrorMessage = JvmSpringFromBytes(_aajbErrorMessage[ juIndex ],0,1,1);

   return jsErrorMessage;
}


JVM_VOID JvmErrSetOSErrorCode(
 IN  JVM_ERROR jeError,
 IN  JVM_STRING jsContext
) 
{
	/** Create a new error node and links it to pjeiErrStack in the thread state structure **/
    _JVM_Error_Info * pjeiNode    =  JvmErrGetNewErrorNode();

    pjeiNode->jsJVMErrorMessage   = JvmErrGetErrorMessage( ERR_JVM_OS_ERROR );
	if ( JVM_VALID( jsContext ))
	  pjeiNode->jsContextInfo     = JvmSpringClone(jsContext);

	pjeiNode->jdwJVMErrorCode     = ERR_JVM_OS_ERROR;
	pjeiNode->jeOSErrorCode       = jeError;
	pjeiNode->jsOSErrorMessage    = JvmSysGetOSErrorMessage( jeError );

}