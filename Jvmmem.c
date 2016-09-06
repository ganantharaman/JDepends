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


#include <mem.h>
#include <jvmmem.h>
#include <jvmexcpt.h>
#include <jvmexcmn.h>

/* Init functions ideally can't throw exceptions since 
   exception context and  frame are assumed 
   to be uninitialized .
   But exception functions get the thread context 
   since JvmGetCurrentThread which is used in
   almost all exception fns, give an existing
   thread control block ptr or create one and give
   it anyway.
*/

JVM_BOOL JvmMemInit(G_STATE_P state)
{
  if ( !IS_OSL_READY( state ) )
 		return JVM_FALSE;

	if (!mem_init() ) {
	    JVM_THROW_EXCEPTION( EXC_JVMNOMEMORY );
		  JVM_ASSERT( FALSE );
	}
  
	MEML_READY( state );
	return JVM_TRUE;
}

/* NON - Re entrant */
JVM_PVOID JvmMemAlloc(
  IN  JVM_ULONG julReqSize
)
{ 
  JVM_PVOID p = mem_alloc( julReqSize ); 
  if (!p) {
 	    JVM_THROW_EXCEPTION( EXC_JVMNOMEMORY );
		  JVM_ASSERT( FALSE );    
  }
  return p;
}


/* NON Re-entrant */
JVM_VOID JvmMemFree( 
  IN JVM_PVOID  pjpvBlockToFree 
)
{   mem_free( pjpvBlockToFree );}	

/* Cleanup */
JVM_VOID JvmMemExit()
{  mem_exit();  }


JVM_PVOID JvmMemCalloc(
IN  JVM_ULONG julSize
)
{
  JVM_PVOID p = mem_calloc(julSize); 
  if (!p) {
 	    JVM_THROW_EXCEPTION( EXC_JVMNOMEMORY );
		  JVM_ASSERT( FALSE );    
  }
  
  return p;
}


JVM_PVOID JvmMemRealloc( 
  IN  JVM_PVOID pjvBlock,
  IN  JVM_ULONG julNewSize
)
{ 
  JVM_PVOID p = mem_realloc(pjvBlock, julNewSize);
  if (!p) {
 	    JVM_THROW_EXCEPTION( EXC_JVMNOMEMORY );
		  JVM_ASSERT( FALSE );    
  }
  
  return p;
}


JVM_BOOL JvmMemIsValidHeapPtr( JVM_PVOID pjvAddress )
{  return mem_is_valid_heap_ptr(pjvAddress);  }

JVM_VOID JvmMemDump()
{  mem_dump(); }






				