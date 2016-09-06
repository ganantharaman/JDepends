
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <jvmw32os.h>

#define min(a, b)  (((a) < (b)) ? (a) : (b)) 

int MakeShortFromLongPath( char * longpath, char **sh_path );


void check_line(
 char * p, int cmp_len,
 const char * string, int line_count, 
 const char * file_name, FILE *fpout
);


int grep(
 const char * file_name,
 const char * string
);

unsigned short _I86_LoadWord( unsigned char * pBytes );

#define MAIN_THREAD 0xffffffff

DWORD ThreadHandler( LPVOID );
DWORD ThreadHandlerTwo( LPVOID lpParam );

extern void JvmStartTheMusic();

int main(int argc,char *argv[] )
{

  JvmStartTheMusic(argc,argv);

	



  /*  _OS_THREAD * pThread1 = NULL;
	
	DWORD dwErrCode;
	_OS_THREAD_MESSAGE * potmMessage = _OS_CreateThreadMessage(&dwErrCode);
	int nCount = 0;

	_OS_Init();
	_OS_CreateThread(( DWORD )ThreadHandler, NULL, "thd_One", &pThread1, &dwErrCode );
	 Sleep( 10000 );

    if ( potmMessage )
	{
		potmMessage->_data_type		= data_type_int;
		potmMessage->u._int_value	= TM_SUSPEND;
		potmMessage->_t_sender      = _OS_GetCurrentThread();
		potmMessage->_t_receiver    = _OS_GetThreadFromName("thd_One");
	}

    _OS_PostThreadMessage( potmMessage, &dwErrCode ); 

  	Sleep( 5000 );

	_OS_ResumeThread( pThread1, &dwErrCode );
	
    Sleep( 5000 );
	
	_OS_Exit(); 

	*/

	/*__asm volatile
   {			 
	   push eax  
       push ebx  
	   push ecx  
       lea  eax,dwArgs 
	   mov ecx,0 
__loop:          
       cmp ecx, nArgs  
	   je near __fncall
       mov ebx,dword ptr[eax+4*ecx]
	   push ebx  
	   inc ecx   
	   jmp near __loop 
__fncall:               
	   call lpfnFunc    
       mov ebx, eax     
	   mov eax, nArgs   
	   mov ecx, 4	   
	   mul ecx		   
       add esp, eax	   
	   mov ecx, bIsFP  
	   cmp ecx,1	   
	   jne  __normret  
	   fstp dword ptr[dwReturn]
	   jmp near __exit 
__normret:             
	   mov dwReturn, ebx
__exit:                
	   pop ecx		   
	   pop ebx		   
	   pop eax		   
   }
	
	  */

	fprintf( stdout, "Exiting Main Thread ......\n");

     return 1;
}	

/*
unsigned short _I86_LoadWord( unsigned char * pBytes )
{
	unsigned char byte      = *pBytes;
	*pBytes        = *(pBytes + 1 );
	*(pBytes + 1 ) = byte;
	return ( *(unsigned short *)pBytes );
}

unsigned long _I86_LoadDoubleWord( unsigned char * pBytes )
{
	unsigned char byte	    = *pBytes;
	*pBytes			= *(pBytes + 3 );
	*(pBytes + 3 )  = byte;
	byte			= *( pBytes + 1 );
	*( pBytes + 1 ) = *(pBytes + 2);
	*( pBytes + 2 ) = byte;
	return ( *(unsigned long *)pBytes );
}
*/


DWORD ThreadHandler( LPVOID lpParam )
{
	DWORD dwErrCode;
	DWORD nCount = 0;
	_OS_THREAD * pThread = _OS_GetCurrentThread();
	_OS_THREAD_MESSAGE Message;

	_OS_GetNextThreadMessage( pThread, &Message, &dwErrCode );

	do
	{
		if ( Message._t_sender && (Message._data_type == data_type_int ))
		{
		  switch( Message.u._int_value )
		  {
		  case TM_EXIT:   
			               fprintf(stdout,"Thd 1 Stopping ...\n");
			              _OS_ExitThread(); 
						  
		  case TM_QUIT:	   return 1;
		  case TM_SUSPEND:
			  _OS_PrepareToSuspendThread(pThread , &dwErrCode );
				/* Waits here , until somebody wakes us up */
		      _OS_SuspendThread( pThread, 0, &dwErrCode );
			  _OS_SuspendThreadCleanUp( pThread, &dwErrCode );
			  break;
		  default:
			  break;
   		  }
		}

	    _OS_GetNextThreadMessage( pThread, &Message, &dwErrCode );

   	   fprintf( stdout, "Thd 1 - printing\n" );

	} while ( TRUE );
	

	return 1;
}


DWORD ThreadHandlerOne( LPVOID lpParam )
{
	DWORD dwErrCode;
	DWORD nCount = 0;
	_OS_THREAD * pThread = _OS_GetCurrentThread();

	grep("check.c","e");

	_OS_PrepareToSuspendThread(pThread , &dwErrCode );

	/* Waits here , until somebody wakes us up */
	_OS_SuspendThread( pThread, 0, &dwErrCode );
    _OS_SuspendThreadCleanUp( pThread, &dwErrCode );

	do
	{
		fprintf( stdout, "Thd 1 - resumed printing\n" );
	}
	while ( TRUE );

	return 1;
}


DWORD ThreadHandlerTwo( LPVOID lpParam )
{
	DWORD dwErrCode;
	DWORD nCount = 0;
	_OS_THREAD * pThread = _OS_GetCurrentThread();

	grep("check.c","c");

/*	do
	{
    //	 fprintf( stdout, "WRWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW - g\n" );
	}
	while ( nCount++ < 2000 );
	*/

	_OS_PrepareToSuspendThread(pThread , &dwErrCode );

	/* Waits here , until somebody wakes us up */
	_OS_SuspendThread( pThread, 0, &dwErrCode );
    _OS_SuspendThreadCleanUp( pThread, &dwErrCode );

	do
	{
		fprintf( stdout, "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR resumed printing\n" );
	}
	while ( TRUE );

	return 1;
}

unsigned int testfunc( unsigned int arg1 )
{
   return arg1;
}





int MakeShortFromLongPath( char * path, char ** sh_path )
{
	char  long_path[ _MAX_PATH];
	char  short_path[ _MAX_PATH];
	char *return_path, * ptr, *sem_ptr, *temp;
	unsigned int len;

	if ( !path || !sh_path )
		return 0;
	return_path = sem_ptr = NULL;
	ptr = path;
	do
	{
	   short_path[0] = 0;
	   temp = NULL;
	   len  = 0;
   	   sem_ptr = strchr( ptr, ';' );
	   len = sem_ptr ? sem_ptr - ptr : strlen( ptr );

	   if ( len ) {
	       // create a long_path and send it to 
	       // GetShortPathName
	       memcpy( long_path, ptr, len );
	       long_path[len] = 0;
     	   GetShortPathName( long_path, short_path, sizeof( short_path ));
           if ( *short_path ) {
		   len  = return_path ? strlen( return_path ) : 0;
	       temp = realloc( return_path, len + strlen( short_path ) + 2);
	       // allocation failed
	       if ( !temp ) {
	        if ( return_path )
		     free( return_path );
		    return 0;
		   }
    	   // first time - do a dummy copy
		   if ( return_path ) 
		   	  strcat( temp, short_path );
		   else
			  strcpy( temp, short_path);
    	    return_path = temp;
 	       // Append a ';' anyway
           strcat( return_path,";");
		 }
	   }
	   // advance ptr
	   ptr = sem_ptr ? sem_ptr + 1 : NULL;
	   if ( ptr > ( path + strlen( path )))
		   ptr = NULL;
    }
	while ( ptr != NULL );

	if ( return_path )
	  return_path[ strlen( return_path ) - 1] = 0;
	*sh_path = return_path;
	return 1;
}