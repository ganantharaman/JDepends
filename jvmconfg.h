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

#include <jvmrt.h>
#include <jvmmcros.h>


enum {
    CONFIG_ID_STACK = 0,
	  CONFIG_ID_OP_STACK,
	  CONFIG_ID_MAX,
	  CONFIG_REGULAR_CLASS,
	  CONFIG_JARRED_CLASS,
    CONFIG_ERROR
};


typedef struct JVM_CONFIG_INFO
{
  JAVA_INT jiJavaArgc;
  JAVA_ARRAYREF jarRefArgv;
  JVM_INT nNativeStackSize;
  JVM_INT nJavaStackSize;
  JVM_INT nMaxHeapSize;
  JVM_INT nInitHeapSize;
  JVM_INT nFinalHeapSize;
  JVM_U32 ju32Flags;
  JVM_INT   nJavaArgc;
  JVM_PBYTE* ppJavaArgv;
  JAVA_ARRAYREF jarArgv;
  JVM_STRING jsClassPath;
}JVM_CONFIG_INFO;



JVM_BOOL JvmConfigInit( G_STATE_P state);
JVM_VOID JvmConfigExit();

JVM_LONG JvmConfigGetSize(JVM_INT jiID);

JVM_INT JvmConfigLocateClass( JVM_STRING jsClassName, JVM_STRING * pjsFileName );

JVM_BOOL JvmConfigCompleteParse( JVM_CONFIG_INFO * pjciInfo );

JVM_BOOL JvmConfigGetJavaArgs( IN JAVA_ARRAYREF * pjarArgs );