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

#include <jvmconfg.h>
#include <jvmsys.h>
#include <jvmsprng.h>
#include <jvmmem.h>
#include <jvmutil.h>
#include <jvmexcpt.h>
#include <cmdproc.h>
#include <jvmldr.h>

#define JVMOSAPI_H
#define ASSERT_H
#include <jvmhdrs.h>


__inline void JvmDefaultArgHandler( cmd_config_info_t *pcci );
__inline void JvmArgHandler_FinalHeapSize( cmd_config_info_t *pcci );
__inline void JvmArgHandler_InitHeapSize( cmd_config_info_t *pcci );
__inline void JvmArgHandler_JavaStackSize( cmd_config_info_t *pcci );
__inline void JvmArgHandler_NativeStackSize( cmd_config_info_t *pcci );
__inline void JvmArgHandler_ClassPath( cmd_config_info_t *pcci );
__inline void JvmArgHandler_JIT( cmd_config_info_t *pcci );
__inline void JvmArgHandler_Verify( cmd_config_info_t *pcci );
__inline void JvmArgHandler_Debug( cmd_config_info_t *pcci );
__inline void JvmArgHandler_Version( cmd_config_info_t *pcci );
__inline void JvmArgHandler_Help( cmd_config_info_t *pcci );


JVM_CONFIG_INFO * _pGCIConfigInfo;
extern _JVM_Mutex_Object   _jmoGConfigMutex;


extern _JVM_Object        _jaoGPrimitiveLoader;
#define PRIMITIVE_LOADER  &_jaoGPrimitiveLoader

#define ARG_HELP    0x0001
#define ARG_VERSION 0x0002
#define ARG_DEBUG   0x0004
#define ARG_VERIFY  0x0008
#define ARG_JIT     0x0010


arg_table_t aGJvmArg_Table[] = {
	{ "12",      JvmDefaultArgHandler  },
	{ "help",    JvmArgHandler_Help    },
	{ "?",       JvmArgHandler_Help    },
	{ "ver",     JvmArgHandler_Version },
	{ "debug",   JvmArgHandler_Debug   },
	{ "cpath",   JvmArgHandler_ClassPath },
	{ "nss",     JvmArgHandler_NativeStackSize },
	{ "jss",     JvmArgHandler_JavaStackSize   },
	{ "ihs",     JvmArgHandler_InitHeapSize    },
	{ "fhs",     JvmArgHandler_FinalHeapSize   },
	{ "verify",  JvmArgHandler_Verify  },
	{ "jit",     JvmArgHandler_JIT     }
};


JVM_BOOL JvmConfigParse( int nArgc, char ** ppArgv )
{
	int jiStatus = 0;
    if ( parse_command_line_arguments( nArgc, ppArgv, 
    	   aGJvmArg_Table, _pGCIConfigInfo, &jiStatus ))
		   /** CHECK,CHECK is this the right time to call JvmConfigCompleteParse ?? **/
		return JvmConfigCompleteParse( _pGCIConfigInfo );

	return JVM_FALSE;
}


JVM_BOOL JvmConfigInit(G_STATE_P state)
{
    if (!IS_SYSL_READY( state )  &&	!IS_OSL_READY( state ))	
		return JVM_FALSE;

   _pGCIConfigInfo = (JVM_CONFIG_INFO *)JvmMemCalloc( sizeof(JVM_CONFIG_INFO) );
   if ( !_pGCIConfigInfo )
	   return JVM_FALSE;

   _pGCIConfigInfo->ju32Flags = 0L;
   _pGCIConfigInfo->nJavaArgc = 0L;
   _pGCIConfigInfo->ppJavaArgv   = JVM_NULL;
   _pGCIConfigInfo->jarArgv = JVM_NULL;
   _pGCIConfigInfo->nNativeStackSize =  
   _pGCIConfigInfo->nJavaStackSize   =
   _pGCIConfigInfo->nFinalHeapSize   = 
   _pGCIConfigInfo->nInitHeapSize    = 0x00ffffff;
   _pGCIConfigInfo->jsClassPath      = JvmSysGetEnv( ENV_CLASSPATH );

    CFGL_READY( state );
 
   return JVM_TRUE;
}

JVM_VOID JvmConfigExit()
{
	if ( JVM_VALID( _pGCIConfigInfo ))
	   JvmMemFree( _pGCIConfigInfo );
}


/* return size of stack heap etc. */
JVM_LONG JvmConfigGetSize( JVM_INT jbyParam )
{
  JVM_LONG jlValue = 0;
  /** obtain lock **/
  JvmSysAcquireMutex( _jmoGConfigMutex );

  switch ( jbyParam ) {
    case CONFIG_ID_STACK :
	   jlValue = _pGCIConfigInfo->nJavaStackSize;
	   break;
    case  CONFIG_ID_OP_STACK:
		jlValue = 0x00000fff;
		break;
  }


  JvmSysReleaseMutex( _jmoGConfigMutex );
  /* To be implemented */
  return jlValue;
}	   

/*
 * mainly to convert plain c string array to java/lang/string object array 
 */
JVM_BOOL JvmConfigCompleteParse( struct JVM_CONFIG_INFO * pjciInfo )
{
	/** TODO 
	_JVM_Array_Object * JvmRtCreateNewArray(
  IN   JVM_U16  juType,
  IN   JVM_U16  juDimension,
  IN   JVM_U32  * pjuLength,
  IN   _JVM_Class_Class * pjccThisArrayClass,
  IN   _JVM_Class_Class * pjccClass
)
   **/
	JVM_U32 ju32Length[ 1 ];
	JVM_OBJREF joArrayClassObject;
	JVM_OBJREF joStringClassObject;
	JVM_INT jiCount = 0;

	if ( !JvmLdrMakeNewArrayClass( PRIMITIVE_LOADER, S_JAVA_LANG_STRING_1D_ARR, &joArrayClassObject ))     
	       return JVM_FALSE;

	JVM_ASSERT( pjciInfo != NULL );
	ju32Length[0] = pjciInfo->nJavaArgc;

	if ( !JvmLdrLoadNewClass(S_JAVA_LANG_STRING, JVM_NULL,
		    PRIMITIVE_LOADER, &joStringClassObject))
			return JVM_FALSE;

	pjciInfo->jarArgv = JvmRtCreateNewArray( JAVA_TYPE_REF, 1, ju32Length, 
		  ((_JVM_Object *)joArrayClassObject)->joiObjectID.pjccClass, 
		  ((_JVM_Object *)joStringClassObject)->joiObjectID.pjccClass);

	if ( JVM_VALID(pjciInfo->jarArgv) )
	{
#ifndef __JDEPENDS__
		for (jiCount=0; jiCount < pjciInfo->nJavaArgc; jiCount ++ ) {
		   JvmRtCreateObject_JavaLangString_v0( 
			   JVM_SPRING_CREATE( pjciInfo->ppJavaArgv[ jiCount ] ), 
			   &pjciInfo->jarArgv->u.pjarRef[jiCount+1] 
			  );
		}
#endif /* __JDEPENDS__ */
	}

	return JVM_TRUE;
}


/* 
 *   Get Java Arguments    
 */
JVM_BOOL JvmConfigGetJavaArgs( IN JAVA_ARRAYREF * pjarArgs ) 
{
	*pjarArgs = JVM_NULL;

	/** obtain lock **/
	JvmSysAcquireMutex( _jmoGConfigMutex );

	/** Assign Argv **/
	if ( _pGCIConfigInfo && _pGCIConfigInfo->jarArgv ) 
        *pjarArgs = _pGCIConfigInfo->jarArgv;

	/** Release lock **/
	JvmSysReleaseMutex( _jmoGConfigMutex );

	return (*pjarArgs != JVM_NULL);
}


JVM_INT JvmConfigLocateClass( JVM_STRING jsClassName, JVM_STRING * pjsFileName )
{
  /* Other things? First look up argument given Class Path */

  return JvmSysLocateClass( JvmSysGetEnv( ENV_CLASSPATH ), jsClassName, pjsFileName );
  
}


/** A r g u m e n t   H a n d l e r s **/

/** H e l p **/
__inline void JvmArgHandler_Help( cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->ju32Flags |= ARG_HELP;     }

/** V e r s i o n **/
__inline void JvmArgHandler_Version( cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->ju32Flags |= ARG_VERSION;  }

/** D e b u g **/
__inline void JvmArgHandler_Debug( cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->ju32Flags |= ARG_DEBUG;    }

/** V e r i f y **/
__inline void JvmArgHandler_Verify( cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->ju32Flags |= ARG_VERIFY;   }

/** J I T **/
__inline void JvmArgHandler_JIT(cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->ju32Flags |= ARG_JIT;      }

/** C l a s s p a t h **/
__inline void JvmArgHandler_ClassPath( cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->jsClassPath = JVM_SPRING_CREATE(pcci->arg); }

/** N a t i v e  S t a c k  S i z e **/
__inline void JvmArgHandler_NativeStackSize( cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->nNativeStackSize = atoi(&pcci->arg[4]); }

/** J a v a  S t a c k  S i z e **/
__inline void JvmArgHandler_JavaStackSize( cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->nJavaStackSize = atoi(&pcci->arg[4]); }

/** I n i t  H e a p  S i z e **/
__inline void JvmArgHandler_InitHeapSize(cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->nInitHeapSize = atoi(&pcci->arg[4]); }

/** F i n a l  H e a p  S i z e **/
__inline void JvmArgHandler_FinalHeapSize( cmd_config_info_t *pcci )
{  ((struct JVM_CONFIG_INFO *)pcci->pv)->nFinalHeapSize = atoi(&pcci->arg[4]); }


/** d e f a u l t **/
__inline void JvmDefaultArgHandler(cmd_config_info_t *pcci )
{
   /** TODO **/  
   struct JVM_CONFIG_INFO * pjciConfigInfo;
   JVM_PBYTE * pjbyArgv;

   if ( !pcci  ) return;

   pjciConfigInfo = pcci->pv;
   pjbyArgv = pjciConfigInfo->ppJavaArgv;

   pjbyArgv = JvmMemRealloc(pjbyArgv, (pjciConfigInfo->nJavaArgc + 1)* sizeof( JVM_PVOID ));
   JVM_ALLOC_CHECK( pjbyArgv )

   pjbyArgv[ pjciConfigInfo->nJavaArgc ] = JvmMemCalloc( JvmStrLen( pcci->arg ) + 1 );
   JVM_ALLOC_CHECK( pjbyArgv[pjciConfigInfo->nJavaArgc] )

   JvmStrCpy( pjbyArgv[ pjciConfigInfo->nJavaArgc ], pcci->arg);
   pjciConfigInfo->ppJavaArgv = pjbyArgv;
   ++pjciConfigInfo->nJavaArgc;

}


