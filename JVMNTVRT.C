
#include <jvmrt.h>
#include <jvmexcmn.h>
#include <jvmexcpt.h>
#include <jvmsys.h>
#include <jvmutil.h>
#include <hashtabl.h>

extern _JVM_Mutex_Object  _jmoGGlobalDataMutex;

/** Array object 
typedef _JVM_Array_Object {

  JVM_INT   jiLength;
  JVM_CHAR  jiType;
  union {
     _JVM_Object ** ppjoArray;
	 JAVA_INT    *  pjaiArray;
	 JAVA_FLOAT  *  pjafArray;
	 JAVA_DOUBLE *  pjadArray;
	 JAVA_LONG   *  pjalArray;
	 JAVA_CHAR   *  pjacArray;
	 JAVA_BYTE   *  pjabyArray;
	 JAVA_BOOL   *  pjabArray;
	 JAVA_SHORT  *  pjabArray;
  }
}

**/


/** Native Functions of Runtime class from Sun JDK 1.0.2
    All functions marked 'public' MUST be implemented
	and all marked 'private' may be implemented contingent
	on Sniper MicroVM implementation. 
	of which loadFileInternal is necessary

    Note that the types indicated are Java types 
    1)   private native void exitInternal(int status);
    2)   private static native void runFinalizersOnExit0(boolean value); 
	3)   private native Process execInternal(String cmdarray[], String envp[]) 
    	 throws IOException;
    4)   public native long freeMemory();
	5)   public native long totalMemory();
	6)   public native void gc();
	7)   public native void runFinalization();
	8)   public native void traceInstructions(boolean on);
	9)   public native void traceMethodCalls(boolean on);
	10)  private synchronized native String initializeLinkerInternal();
	11)  private native String buildLibName(String pathname, String filename);
	12)  private native int loadFileInternal(String filename);
**/
	
/** 1)   private native void exitInternal(int status); **/

JVM_VOID JvmRuntimeExitInternal( 
  IN  _JVM_Object *pjoObject,
  IN   JAVA_INT    jaiStatus
  )
{
   JVM_ASSERT_VALID( pjoObject );

   if ( !JvmRtObjectIsKindOf( pjoObject, "Runtime") )
	   return ;


   /** TODO **/
}

/** 2)   private static native void runFinalizersOnExit0(boolean value); **/
JVM_VOID JvmRuntimeRunFinalizersOnExit0( 
  IN  _JVM_Class_Class *pjccTheClass,
  IN   JAVA_INT    jaiStatus
  )
{
   JVM_ASSERT_VALID( pjoObject );	 

   if ( !JvmRtObjectIsKindOf( pjoObject, "Runtime") )
	  return;

   /** TODO **/
}

/** 3) private native Process execInternal(String cmdarray[], String envp[]) 
    	 throws IOException; **/
_JVM_Object * JvmRuntimeExecInternal( _JVM_Object * ppjoObject,




