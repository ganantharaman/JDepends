#include <jvmmem.h>
#include <jvmsys.h>
#include <hashtabl.h>
#include <jvmparse.h>
#include <jvmldr.h>
#include <jvmsprng.h>


/*************************************/
/* Common Globals  */

/* Warning : Must call JvmSysInitMutex for 
   each of the mutex object declared here
   before use 
 */
_JVM_Mutex_Object   _jmoGGlobalDataMutex;
_JVM_Mutex_Object   _jmoGExclCodeMutex;
_JVM_Mutex_Object   _jmoGClassStateMutex;
_JVM_Mutex_Object   _jmoGCLdrDictMutex;
_JVM_Mutex_Object   _jmoGCLdrStateMutex;
_JVM_Mutex_Object   _jmoGAttrVtblMutex;
_JVM_Mutex_Object   _jmoGSpringMutex;
_JVM_Mutex_Object   _jmoGFileHandleAccessMutex;
				    
Hash_Table        * _pdictGCLdrClass;
Hash_Table        * _pdictGCLdrClassState;
Hash_Table        * _pjhtGSpringState;



_JVM_Object         _jaoGPrimitiveLoader;



/*************************************/

/*************************************/
/* OS Specific Globals */

#define JVMOSGB_H
#include <jvmhdrs.h>

/*************************************/


void JvmDump() {}


int main(int argc,char ** argv)
{
   JVM_PBYTE pjbByte1 ,pjbByte2,pjbByte3,pjbByte4,pjbByte5 = JVM_NULL;
   JVM_PBYTE pjbStream = "(LJava\\Lang\\Classloader;II)LJava\\Lang\\Class;"; 

   _JVM_Method_Info jmiInfo;
   JVM_OBJREF joClassObject;                        
   JVM_OBJREF joClassClassException;                        
   
   jmiInfo.pjfiParameterFields = JVM_NULL;
   jmiInfo.jbReturn = JVM_FALSE;
   jmiInfo.jiParameterCount = 0;
   jmiInfo.jdwStatus = JVM_STATUS_OK;

   

   /* Must Never Fail */
   JvmInit();

/*   pjpbByte1 = ( JVM_PBYTE )realloc(NULL, 20 );

   strcpy(pjpbByte1,"HowdyHowdyHowdyHowd");

   pjpbByte1 = realloc( pjpbByte1,200);

   strcat( pjpbByte1,"HowdyHow");
  */

   pjbByte1 = ( JVM_PBYTE )JvmMemAlloc( 20 );
   strcpy( pjbByte1, "Hello");
   JvmDump();

   pjbByte2 = ( JVM_PBYTE )JvmMemAlloc( 20 );
   strcpy( pjbByte2, "Hello");
   JvmDump();
   pjbByte3 = ( JVM_PBYTE )JvmMemAlloc( 20 );
   strcpy( pjbByte3, "Hello");
   JvmDump();
   pjbByte4 = ( JVM_PBYTE )JvmMemAlloc( 4095 );
   strcpy( pjbByte4, "Hello");
   JvmDump();
   pjbByte5 = ( JVM_PBYTE )JvmMemAlloc( 4095 );
   strcpy( pjbByte5, "Hello");
   JvmDump();
  

   JvmMemFree( pjbByte1 );
   JvmDump();
   JvmMemFree( pjbByte2 );
   JvmDump();
   JvmMemFree( pjbByte5 );
   JvmDump();

   pjbByte1 = ( JVM_PBYTE )JvmMemAlloc( 20 );
   JvmDump();
   pjbByte2 = ( JVM_PBYTE )JvmMemAlloc( 20 );
   JvmDump();

   JvmMemFree( pjbByte3 );
   JvmMemFree( pjbByte4 );
   JvmDump();

   pjbByte5 = ( JVM_PBYTE )JvmMemAlloc( 8000 );
   JvmDump();

   JvmMemFree( pjbByte2 );
   JvmMemFree( pjbByte1 ); 
   JvmDump();

   /* JvmParseArguments(argc,argv); */

   JvmMemFree( pjbByte5 );
   JvmDump();
   
   JvmParseMethodDescriptor((JVM_PBYTE *)&pjbStream,
	                         &jmiInfo );
	 
	 _putenv(strdup("CLASSPATH=C:\\j2sdk1.4.1_01\\jre\\lib"));
	 
	 JvmLdrLoadNewClass(JVM_SPRING_CREATE("java\lang\Exception"), JVM_NULL, PRIMITIVE_LOADER, &joClassObject);
	 JvmLdrLoadNewClass(JVM_SPRING_CREATE("java\lang\Error"), JVM_NULL, PRIMITIVE_LOADER, &joClassObject);
	 JvmLdrLoadNewClass(JVM_SPRING_CREATE("java\lang\VirtualMachineError"), JVM_NULL, PRIMITIVE_LOADER, &joClassObject);
	 
   JvmLdrLoadNewClass(JVM_SPRING_CREATE("c:\\jdp\\jvm\\test\\Main.class"), JVM_NULL, PRIMITIVE_LOADER, &joClassObject);
   JvmRtInvokeMethod(JVM_SPRING_CREATE("main"), 
                     JVM_SPRING_CREATE("([Ljava/lang/String;)V"),
                     joClassObject, JVM_NULL, 0, JVM_NULL, JVM_NULL); 
  
   JvmExit();

   return JVM_OK;
}



