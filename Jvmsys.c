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

#include <linklist.h>
#include <jvmrt.h>
#include <jvmsys.h>
#include <jvmexcpt.h>
#include <jvmmem.h>
#include <jvmsprng.h>
#include <jvmutil.h>

#define ASSERT_H
#define STDIO_H
#include <jvmhdrs.h>

#include <util.h>
#include <jvmconfg.h>


extern JVM_VOID JvmErrSetErrorCode( 
  IN   JVM_DWORD  jdwErrCode,
  IN   JVM_STRING jsString
);

extern link_list_t * _pllGFileHandleList;
extern _JVM_Mutex_Object   _jmoGFileHandleAccessMutex;

JVM_VOID JvmSys_All_Threads_Start_Routine(os_thread_msg_t *msg);


#define JVMOSAPI_H "Includes jvmos header"
#include <jvmhdrs.h>

extern _JVM_Sys_Objects * _psoGSysObjects;


/** NOTE ://////////////////////////////////////////////////////////
/   This macro throws an exception only if EXC_NOEXCEPTION is NOT set
/   Why would anyone not want to thro an exception when something fails
/	at this level? Well, the reason has to do with Java's specific
/	exception that needs to be thrown at a higher level.
/	In these cases we just return an invalid value and higher level
/	layer is responsible for throwing the exception if necessary.
//////////////////////////////////////////////////////////////////**/

#define JVM_SYS_CHECK_RETCODE( ret, exc ) \
   if (!ret) \
   {\
	  if ( EXC_NOEXCEPTION != (exc) )\
       JVM_THROW_EXCEPTION((exc));\
   }  

#define JVM_SYS_CHECK_ERROR( err, con, exc ) \
   if ((err) != OS_SUCCESS_STATUS) \
   {\
      JvmErrSetOSErrorCode((err),(con));\
	  if ( EXC_NOEXCEPTION != (exc) )\
       JVM_THROW_EXCEPTION((exc));\
   }  


JVM_BOOL JvmSysOSInit(G_STATE_P  state)
{
   if (_os_init())
   {
	   OSL_READY( state ); 
	   return JVM_TRUE;
   }

   return JVM_FALSE;
}


/** NON- Reentrant **/
JVM_BOOL JvmSysInit(G_STATE_P state)
{ 
	 JVM_PBYTE  pjbyBytes;
	 JVM_ERROR  jeError;
	 os_cache_t *   ocPtrCache;

	if (IS_SPGL_READY( state ) && 
		  IS_OSL_READY( state ))
	{

	_psoGSysObjects     = JvmMemCalloc( sizeof( _JVM_Sys_Objects ));
	_psoGSysObjects->pllFileHandlesList 
		= link_list_create(
#ifdef __DEBUG__
		"FileHandlesList",
#endif
		LINK_LIST_REGULAR,
		LINK_DATA_TYPE_ULONG,sizeof(JVM_ULONG),
		JvmSysFileHandleCompare,
		JVM_NULL
		);
	  JVM_ALLOC_CHECK( _psoGSysObjects );  

	 _os_cache_init(&ocPtrCache);
	 _os_get_path_mark( &pjbyBytes, ocPtrCache);
	 if ( pjbyBytes )
       _psoGSysObjects->jsPathMark  = JVM_SPRING_CREATE( pjbyBytes );
     _os_get_env( ENV_PATH,&pjbyBytes, &jeError );
	 if ( pjbyBytes )
      _psoGSysObjects->jsClassPath = JVM_SPRING_CREATE( pjbyBytes );
	 _os_get_current_dir(&pjbyBytes, ocPtrCache, &jeError);
	 if ( pjbyBytes )
      _psoGSysObjects->jsCurrentWorkingDir = JVM_SPRING_CREATE( pjbyBytes );
	 _os_cache_flush( ocPtrCache ); 

	 SYSL_READY( state );
	  
    return JVM_TRUE;
	}

	return JVM_FALSE;
}

JVM_STRING  JvmSysGetEnv( JVM_UINT juEnvID )
{
	 JVM_ERROR  jeError;
	 JVM_PBYTE  pjbyEnvName;
	 JVM_PBYTE  pjbyBytes = JVM_NULL;
	 JVM_STRING jsEnvString = JVM_NULL;
	 JVM_STRING jsEnvName  = JVM_NULL;

	 pjbyEnvName = _os_get_env_name_from_id( juEnvID );
	 if ( pjbyEnvName )
	 {
       jsEnvName = JVM_SPRING_CREATE( pjbyEnvName );
	    _os_get_env( juEnvID, &pjbyBytes, &jeError );
	    JVM_SYS_CHECK_ERROR( jeError, jsEnvName, EXC_NOEXCEPTION );
	    if ( JVM_VALID( pjbyBytes ))
	      jsEnvString = JVM_SPRING_CREATE( pjbyBytes );
	 }
	return jsEnvString;
}

JVM_STRING JvmSysGetPathMarks(JVM_BOOL jbRefresh)
{
	os_cache_t * ocPtrCache;
	JVM_INT  jiStatus;
	JVM_PBYTE  pjbyBytes;

  if ( jbRefresh )
	{
     jiStatus = _os_cache_init(&ocPtrCache);
     JVM_SYS_CHECK_RETCODE( jiStatus, EXC_JAVAVIRTUALMACHINEERROR );
	   _os_get_path_mark( &pjbyBytes, ocPtrCache);
	   _psoGSysObjects->jsPathMark  = JVM_SPRING_CREATE( pjbyBytes );
	   _os_cache_flush( ocPtrCache ); 
	}

	return _psoGSysObjects->jsPathMark;
}

JVM_VOID JvmSysAcquireMutex( _JVM_Mutex_Object jmoMutex )
{ 
   lock_acquire(&jmoMutex.jmMutex);
}

JVM_VOID JvmSysReleaseMutex( _JVM_Mutex_Object jmoMutex )
{ 
   lock_release(&jmoMutex.jmMutex);
}

JVM_THREAD JvmSysGetCurrentThread()
{ 
   return _os_get_current_thread();  
}

JVM_INT JvmSysSetThreadPriority( JVM_THREAD jtHandle,JVM_INT jiPriority )
{  
	JVM_ERROR jeError;
   _os_set_thread_priority(jtHandle,jiPriority, &jeError );     
    JVM_SYS_CHECK_ERROR( jeError, JVM_NULL,EXC_JAVAVIRTUALMACHINEERROR );
	return jiPriority;
}

JVM_INT JvmSysGetThreadPriority(JVM_THREAD jtThread)
{  
	JVM_ERROR jeError;
	JVM_INT jiPriority;
	_os_get_thread_priority(jtThread, &jiPriority, &jeError); 
	JVM_SYS_CHECK_ERROR( jeError, JVM_NULL ,EXC_JAVAVIRTUALMACHINEERROR );
	return jiPriority;
}

JVM_THREAD JvmSysCreateThread(_JVM_PFN_Thread_Start jptsThreadStart,JVM_ULONG juParam)
{   
	JVM_ERROR  jeError;
	JVM_THREAD jhThread;
  _Jvm_Sys_Thread_Args * pjstaThdArgs = JvmMemAlloc( sizeof( _Jvm_Sys_Thread_Args ));
  pjstaThdArgs->jptsThreadStart = jptsThreadStart;
  pjstaThdArgs->juParam = juParam;
  _os_create_thread(JvmSys_All_Threads_Start_Routine,pjstaThdArgs, "",
	                &jhThread, JVM_NULL, &jeError);  
    /** exception must be thrown from a higher level **/
  JVM_SYS_CHECK_ERROR( jeError,JVM_NULL, EXC_NOEXCEPTION );
	return jhThread;
}

JVM_VOID JvmSysPrepareToSuspendThread( _JVM_Thread_State *pjtsThread )
{   
	JVM_ERROR jeError;
	_os_prepare_to_suspend_thread( pjtsThread->jtHandle, &jeError );  
	JVM_SYS_CHECK_ERROR( jeError, JVM_NULL,EXC_JAVAVIRTUALMACHINEERROR );
}

JVM_VOID JvmSysSuspendThread( _JVM_Thread_State *pjtsThread,JVM_UNSIGNED32 juMilliSec  )
{  
	JVM_ERROR jeError;
	_os_suspend_thread( pjtsThread->jtHandle,juMilliSec, &jeError );
	JVM_SYS_CHECK_ERROR( jeError,JVM_NULL, EXC_JAVAVIRTUALMACHINEERROR );
}

JVM_VOID JvmSysSuspendThreadCleanUp( _JVM_Thread_State *pjtsThread )
{  
	JVM_ERROR jeError;
	_os_suspend_thread_cleanup( pjtsThread->jtHandle, &jeError ); 
	JVM_SYS_CHECK_ERROR( jeError,JVM_NULL, EXC_JAVAVIRTUALMACHINEERROR );
}

JVM_VOID JvmSysResumeThread( _JVM_Thread_State *pjtsThread )
{  
	JVM_ERROR jeError;
	_os_resume_thread( pjtsThread->jtHandle, &jeError );
	JVM_SYS_CHECK_ERROR( jeError, JVM_NULL, EXC_JAVAVIRTUALMACHINEERROR );
}

JVM_VOID JvmSysDestroyThread( JVM_THREAD jtHandle )
{
	JVM_ERROR jeError;
	_os_destroy_thread( jtHandle, &jeError );
	JVM_SYS_CHECK_ERROR( jeError, JVM_NULL, EXC_JAVAVIRTUALMACHINEERROR );
}

JVM_VOID JvmSysExitThread( )
{
	_os_exit_thread();
}

JVM_VOID JvmSysSleep( _JVM_Thread_State *pjtsThread,JVM_UNSIGNED32 juMilliSec )
{
	pjtsThread->jwThreadState = JVM_THREAD_SLEEPING;
  _os_sleep( juMilliSec );
}


JVM_VOID JvmSysInitMutex( _JVM_Mutex_Object *pjmoMutex )
{
	JVM_ASSERT_VALID( pjmoMutex );
	lock_init(&pjmoMutex->jmMutex);  
}

JVM_VOID JvmSysKillMutex( _JVM_Mutex_Object *pjmoMutex )
{	
  lock_destroy(&pjmoMutex->jmMutex);  
}

JVM_UINT JvmSysGetRandom()
{   return _os_get_random();  }

JVM_VOID JvmSysBeginExit()
{ 
	link_list_destroy(_psoGSysObjects->pllFileHandlesList);

	JvmSpringFree( &_psoGSysObjects->jsPathMark          );
	JvmSpringFree( &_psoGSysObjects->jsClassPath         );
	JvmSpringFree( &_psoGSysObjects->jsCurrentWorkingDir );

	JvmMemFree( _psoGSysObjects );
}

JVM_VOID JvmSysEndExit()
{   _os_exit();  }

JVM_PVOID JvmSysMalloc( JVM_U32 ju32Size )
{  
	JVM_ERROR jeError;
	JVM_PVOID pjv;
  _os_malloc( ju32Size, &pjv, &jeError); 
	JVM_SYS_CHECK_ERROR( jeError, JVM_NULL, EXC_NOEXCEPTION );
	return pjv;
}

JVM_PVOID JvmSysCalloc( JVM_U32 ju32Size )
{
 	JVM_ERROR jeError;
	JVM_PVOID pjv;
 _os_calloc( ju32Size, &pjv, &jeError ); 
	JVM_SYS_CHECK_ERROR( jeError, JVM_NULL, EXC_NOEXCEPTION );

	return pjv;
}

JVM_VOID JvmSysFree( JVM_PVOID pjvPtr )
{   _os_free( pjvPtr );  }

JVM_PVOID JvmSysReAlloc(  JVM_PVOID pjvPtr,JVM_U32 ju32Size )
{  	
	JVM_ERROR jeError;
	JVM_PVOID pjv;
	_os_realloc( pjvPtr, ju32Size, &pjv, &jeError );  
	JVM_SYS_CHECK_ERROR( jeError, JVM_NULL, EXC_NOEXCEPTION );
	return pjv;
}

JVM_FILE JvmSysOpenFile( JVM_STRING jsFileName, JVM_WORD jwMode )
{
	 JVM_ERROR jeError;
	 JVM_FILE jfFile; 
    _os_open_file( jsFileName, jwMode, &jfFile, &jeError );
	 JVM_SYS_CHECK_ERROR( jeError, jsFileName, EXC_JVMFILEIOERROR );
	 /** Store the file handles for future validity checks **/
	 JvmSysAcquireMutex( _jmoGFileHandleAccessMutex );
	 link_list_add_data( _psoGSysObjects->pllFileHandlesList , &jfFile );
	 JvmSysReleaseMutex( _jmoGFileHandleAccessMutex );
	 return jfFile;
}

/** Validates a file handle **/
JVM_BOOL JvmSysIsValidFileHandle( JVM_FILE jfFile )
{
	JVM_BOOL jbReturn = JVM_FALSE;
    JvmSysAcquireMutex( _jmoGFileHandleAccessMutex );
	if ( link_list_find_data( _psoGSysObjects->pllFileHandlesList , &jfFile ) )
		jbReturn = JVM_TRUE;
	JvmSysReleaseMutex( _jmoGFileHandleAccessMutex );
	return jbReturn;
}

JVM_VOID JvmSysCloseFile( JVM_FILE jfFile )
{
   JVM_ERROR jeError;
	_os_close_file( jfFile, &jeError );
	 JVM_SYS_CHECK_ERROR( jeError,JVM_NULL, EXC_NOEXCEPTION );
   JvmSysAcquireMutex( _jmoGFileHandleAccessMutex );
	 link_list_delete_data( _psoGSysObjects->pllFileHandlesList , &jfFile );
	 JvmSysReleaseMutex( _jmoGFileHandleAccessMutex );
}

JVM_U32 JvmSysGetFileSize( JVM_STRING jsFileName )
{
  	 JVM_ERROR jeError;
	 JVM_U32   ju32Size;
    _os_get_file_size( jsFileName, &ju32Size, &jeError );
	 JVM_SYS_CHECK_ERROR( jeError, jsFileName, EXC_JVMFILEIOERROR );
	 return ju32Size;
}

JVM_VOID JvmSysWriteFile( JVM_FILE jfFile, JVM_PBYTE pjbyBytes, JVM_DWORD jdwSize)
{
	 JVM_ERROR jeError;
	 JVM_DWORD jdwByteCount;
 	 _os_write_file( jfFile, pjbyBytes, jdwSize, &jdwByteCount, &jeError );
	 JVM_SYS_CHECK_ERROR( jeError, JVM_NULL, EXC_JVMFILEIOERROR );
}

JVM_VOID JvmSysReadFile( JVM_FILE jfFile, JVM_PBYTE * ppjbyBytes, JVM_DWORD jdwSize )
{
	 JVM_ERROR jeError;
	 JVM_DWORD  jdwReadByteCount = 0;
	 _os_read_file( jfFile, ppjbyBytes,jdwSize, &jdwReadByteCount, &jeError );
	 JVM_SYS_CHECK_ERROR( jeError, JVM_NULL, EXC_JVMFILEIOERROR );
}


JVM_FILE JvmSysLoadLibrary( JVM_STRING jsLibrary )
{
    JVM_ERROR jeError;
	JVM_FILE jfLib = JVM_NULL;
	_os_load_library( jsLibrary, &jfLib, &jeError );
	JVM_SYS_CHECK_ERROR( jeError, jsLibrary, EXC_JAVAVIRTUALMACHINEERROR );
	return jfLib;
}


JVM_PVOID JvmSysLinkFunction( JVM_FILE jfLib, JVM_STRING jsFuncName )
{
  JVM_ERROR jeError;
	JVM_PVOID pjvPtr  = JVM_NULL;
	_os_link_function(jsFuncName, jfLib, &pjvPtr, &jeError );
	JVM_SYS_CHECK_ERROR( jeError, jsFuncName, EXC_JAVAVIRTUALMACHINEERROR );
	return pjvPtr;
}


JVM_STRING JvmSysGetOSErrorMessage( JVM_ERROR jeOSError )
{
	JVM_PBYTE  pjbyBytes;
	JVM_STRING jsMessage = JVM_NULL;
  JVM_INT jiStatus;
  os_cache_t * ocCache;
  jiStatus = _os_cache_init( &ocCache );
  JVM_SYS_CHECK_RETCODE( jiStatus, EXC_JAVAVIRTUALMACHINEERROR );
  _os_get_error_message( jeOSError, ocCache, &pjbyBytes );
  jsMessage = JVM_SPRING_CREATE( pjbyBytes );
	_os_cache_flush( ocCache );
	return jsMessage;
}

JVM_VOID JvmSysPostThreadMessage_int( JVM_STRING jsThreadName, JVM_INT jiIntVal )
{
	os_thread_msg_t * potmMessage = _os_create_thread_message();
	if ( potmMessage )
	{
		potmMessage->_data_type		= data_type_int;
		potmMessage->u._int_value	= jiIntVal;
		potmMessage->_t_sender      = _os_get_current_thread();
		potmMessage->_t_receiver    = _os_get_thread_from_name( jsThreadName );
	}
	_os_post_thread_message( potmMessage );
}


JVM_VOID JvmSysPostThreadMessage_long( JVM_STRING jsThreadName, JVM_LONG jlLongVal )
{
	os_thread_msg_t * potmMessage = _os_create_thread_message();
	if ( potmMessage )
	{
		potmMessage->_data_type		= data_type_long;
		potmMessage->u._long_value	= jlLongVal;
		potmMessage->_t_sender      = _os_get_current_thread();
		potmMessage->_t_receiver    = _os_get_thread_from_name( jsThreadName );
	}
	_os_post_thread_message( potmMessage );
}

JVM_VOID JvmSysPostThreadMessage_ptr( JVM_STRING jsThreadName, JVM_PVOID pjvPtrValue )
{
	os_thread_msg_t * potmMessage = _os_create_thread_message();
	if ( potmMessage )
	{
		potmMessage->_data_type		= data_type_ptr;
		potmMessage->u._ptr_value	= pjvPtrValue;
		potmMessage->_t_sender      = _os_get_current_thread();
		potmMessage->_t_receiver    = _os_get_thread_from_name( jsThreadName );
	}
	_os_post_thread_message( potmMessage );
}

JVM_VOID JvmSysInitComplete(G_STATE_P state)
{
   _os_init_complete();
}


JVM_INT  JvmSysFileHandleCompare( JVM_PVOID pData1, JVM_PVOID pData2 )
{
  return *(JVM_FILE *)pData1 ==  *(JVM_FILE *)pData2; 
}

/* until I find a better way */
JVM_VOID JvmSys_All_Threads_Start_Routine(os_thread_msg_t *msg)
{
  _Jvm_Sys_Thread_Args *  pjtaArgs;
  if ( msg ) {
    pjtaArgs = (_Jvm_Sys_Thread_Args *)msg->args;
    if ( pjtaArgs->jptsThreadStart )
      (* pjtaArgs->jptsThreadStart )( pjtaArgs->juParam );
  }
  JvmMemFree( pjtaArgs );  
}

_JVM_Sys_Jar_Info * JvmSysExtractFromJarFile( JVM_STRING jsJarName, JVM_STRING jsClassName )
{

  _JVM_Sys_Jar_Info * pjsjiInfo = JVM_NULL;
  JVM_PBYTE pjbyBytes, pjbyFullName;
  JVM_STRING jsNewClassName;
  jar_contents_t * contents;
  register JVM_INT  jiClassNameLen;
  jiClassNameLen = JvmSpringGetLen( jsClassName );
  pjbyFullName = JVM_NULL;
  pjbyBytes = JvmMemCalloc( jiClassNameLen + 1 );
  JvmMemCpy( pjbyBytes, jsClassName, jiClassNameLen );
   
   /** Check if there is a .class extension **/
  if (jiClassNameLen <= 7 || (pjbyBytes[ jiClassNameLen - 7 ] != '.') ||
     ( pjbyBytes[ jiClassNameLen - 6 ] != 'C' &&
       pjbyBytes[ jiClassNameLen - 6 ] != 'c' ) ||
       (JvmMemCmp( &pjbyBytes[ jiClassNameLen - 5 ], "lass", 4) != 0)) {
         pjbyBytes = JvmMemRealloc( pjbyBytes, jiClassNameLen + 8 );         
         JvmStrCat( pjbyBytes, ".class");    
     } 

  jsNewClassName =  JvmSpringFromBytes( pjbyBytes,0,1,0 );  
  contents =  extract_from_jarfile( jsJarName, jsNewClassName );

  if ( contents ) {
    pjsjiInfo = JvmMemAlloc( sizeof( _JVM_Sys_Jar_Info ));
    pjsjiInfo->jarinfo = contents;
    pjsjiInfo->pjbyBytes = contents->strptr;
  }

  return pjsjiInfo;
}

JVM_VOID JvmSysFreeJarInfo( _JVM_Sys_Jar_Info * pInfo )
{
  if ( pInfo )
    free_jar_contents((jar_contents_t *)pInfo->jarinfo );
}


JVM_VOID   JvmSysPumpWindowsMessages()
{
  _os_pump_windows_messages();
}



JVM_INT JvmSysLocateClass( JVM_STRING jsEnvString, JVM_STRING jsClassName, JVM_STRING * pjsRetString )
{
   register JVM_INT  jiClassNameLen;
   JVM_PBYTE pjbyBytes, pjbyFullName;
   JVM_INT jiRet;
   JVM_LONG jiLen;
   jiClassNameLen = JvmSpringGetLen( jsClassName );
   pjbyFullName = JVM_NULL;

   pjbyBytes = JvmMemCalloc( jiClassNameLen + 1 );
   JvmMemCpy( pjbyBytes, jsClassName, jiClassNameLen );
   
   /** Check if there is a .class extension **/
    if (jiClassNameLen <= 7 || (pjbyBytes[ jiClassNameLen - 7 ] != '.') ||
          ( pjbyBytes[ jiClassNameLen - 6 ] != 'C' &&
            pjbyBytes[ jiClassNameLen - 6 ] != 'c' ) ||
          (JvmMemCmp( &pjbyBytes[ jiClassNameLen - 5 ], "lass", 4) != 0)) {
         pjbyBytes = JvmMemRealloc( pjbyBytes, jiClassNameLen + 8 );         
         JvmStrCat( pjbyBytes, ".class");    
     } 

     jiLen = jiClassNameLen + 512;
     pjbyFullName = JvmMemCalloc( jiLen );
     JvmMemCpy( pjbyFullName, jsClassName, jiClassNameLen );  
     jiRet = locate_class_file(jsEnvString , pjbyBytes, pjbyFullName, &jiLen); 
     /* Successful? **/
     if ( jiRet  == AP_CLASS_NOT_FOUND) {
       JvmMemFree( pjbyFullName );
       JvmMemFree( pjbyBytes );
	     jiRet = CONFIG_ERROR;
       return jiRet;
     } else if ( jiRet == AP_INVALID_LEN ) {
       pjbyFullName =  JvmMemRealloc( pjbyFullName, jiLen );
       jiRet = locate_class_file(jsEnvString, pjbyBytes, pjbyFullName, &jiLen);  
       JVM_ASSERT( jiRet == AP_CLASS_FOUND);
	 }

     jiRet = ( jiRet == AP_CLASS_IN_JAR ) ? CONFIG_JARRED_CLASS : CONFIG_REGULAR_CLASS;
     
     *pjsRetString =  JvmSpringFromBytes( pjbyFullName,0,1,0 );
     JvmMemFree( pjbyFullName );
     JvmMemFree( pjbyBytes );
     return  jiRet;
}




