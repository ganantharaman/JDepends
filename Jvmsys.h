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

#ifndef __JVM_SYSTEM_INCLUDE__
#define __JVM_SYSTEM_INCLUDE__

#include <jvmmcros.h>
#include <jvmrt.h>

#define  JVM_SYS_POSTTHREADMESSAGE( thread, type, value )\
        JvmSysPostThreadMessage_##type( thread, value );

typedef struct _Jvm_Sys_Thread_Args {
  _JVM_PFN_Thread_Start jptsThreadStart;
  JVM_ULONG juParam;
}_Jvm_Sys_Thread_Args;

typedef struct _JVM_Sys_Jar_Info {
  JVM_PVOID  jarinfo;
  JVM_PBYTE  pjbyBytes;
}_JVM_Sys_Jar_Info;


JVM_BOOL   JvmSysOSInit(G_STATE_P state);
JVM_BOOL   JvmSysInit(G_STATE_P state);
JVM_VOID   JvmSysInitComplete(G_STATE_P state);
JVM_VOID   JvmSysBeginExit();
JVM_VOID   JvmSysEndExit();
JVM_VOID   JvmSysPumpWindowsMessages();


JVM_VOID   JvmSysAcquireMutex( _JVM_Mutex_Object jmoMutex );
JVM_VOID   JvmSysReleaseMutex( _JVM_Mutex_Object jmoMutex );

JVM_INT    JvmSysGetThreadPriority(JVM_THREAD jtHandle);
JVM_INT    JvmSysSetThreadPriority( JVM_THREAD jtHandle,JVM_INT jiPriority );
JVM_THREAD JvmSysCreateThread(_JVM_PFN_Thread_Start jptsThreadStart,JVM_ULONG juParam);
JVM_VOID   JvmSysPrepareToSuspendThread(_JVM_Thread_State *pjtsThisThread);
JVM_VOID   JvmSysSuspendThread( _JVM_Thread_State *pjtsThread,JVM_UNSIGNED32 juMilliSec  );
JVM_VOID   JvmSysSuspendThreadCleanUp( _JVM_Thread_State *pjtsThread);
JVM_VOID   JvmSysResumeThread( _JVM_Thread_State *pjtsThread );
JVM_VOID   JvmSysDestroyThread( JVM_THREAD jtHandle );
JVM_VOID   JvmSysExitThread();

JVM_VOID   JvmSysInitMutex( _JVM_Mutex_Object *pjmoMutex );
JVM_VOID   JvmSysKillMutex( _JVM_Mutex_Object *pjmoMutex );
JVM_INT    JvmSysFileHandleCompare( JVM_PVOID pData1, JVM_PVOID pData2 );

JVM_THREAD JvmSysGetCurrentThread();

JVM_VOID   JvmSysSleep(_JVM_Thread_State *pjtsThread ,JVM_UNSIGNED32 juMs);

JVM_PVOID JvmSysMalloc( JVM_UINT juSize );
JVM_VOID  JvmSysFree( JVM_PVOID pjvPtr );
JVM_PVOID JvmSysReAlloc(  JVM_PVOID pjvPtr,JVM_UINT juSize );
JVM_PVOID JvmSysCalloc( JVM_UINT nSize );

JVM_UINT JvmSysGetRandom();

JVM_FILE JvmSysOpenFile( JVM_STRING jsName, JVM_WORD jwFlags );
JVM_VOID JvmSysWriteFile( JVM_FILE jfFile, JVM_PBYTE pjbyBytes, JVM_DWORD jdwSize);
JVM_PVOID JvmSysLinkFunction( JVM_FILE jfLib, JVM_STRING jsFuncName );
JVM_FILE JvmSysLoadLibrary( JVM_STRING jsLibrary );
JVM_VOID JvmSysReadFile( JVM_FILE jfFile, JVM_PBYTE * ppjbyBytes, JVM_DWORD jdwSize );
JVM_VOID JvmSysCloseFile( JVM_FILE jfFile );
JVM_U32  JvmSysGetFileSize( JVM_STRING jsFileName );
JVM_BOOL JvmSysIsValidFileHandle( JVM_FILE jfFile );

JVM_STRING JvmSysGetOSErrorMessage( JVM_ERROR jeError );
JVM_STRING JvmSysGetPathMarks();
JVM_STRING JvmSysGetEnv( JVM_UINT juEnvID ); 
JVM_INT JvmSysLocateClass( JVM_STRING jsEnvString, JVM_STRING jsClassName, JVM_STRING * pjsRetString );
JVM_VOID JvmSysFreeJarInfo( _JVM_Sys_Jar_Info * pInfo );
_JVM_Sys_Jar_Info * JvmSysExtractFromJarFile( JVM_STRING jsJarName, JVM_STRING jsFileName );


JVM_VOID  JvmSysPostThreadMessage_int(JVM_STRING  jsThreadName, JVM_INT   jiValue);
JVM_VOID  JvmSysPostThreadMessage_long(JVM_STRING jsThreadName, JVM_LONG  jlValue);
JVM_VOID  JvmSysPostThreadMessage_ptr(JVM_STRING  jsThreadName, JVM_PVOID pjvValue);





#endif /* __JVM_SYSTEM_INCLUDE__ */



