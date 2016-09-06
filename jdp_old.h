#ifndef __JDP_HDR_INCLUDE__
#define __JDP_HDR_INCLUDE__

#include <mem.h>
#include <lock.h>
typedef unsigned long unsigned32;
#include <stream.h>

typedef void  *JVM_PTR_t; 

typedef struct _JVM_DClass_Info {
   STREAM strm;
   JVM_PTR_t pjccClass;
}_JVM_DClass_Info;

#ifdef __cplusplus
extern "C" {
#endif

int JvmDependsInit();
void JvmDependsExit();

_JVM_DClass_Info * JvmLoadDependsClass(	char * classname );
int JvmFreeDependsClass( _JVM_DClass_Info * pClass );

int JvmDHasSuperClass( _JVM_DClass_Info * pClass );
int JvmDHasInterface( _JVM_DClass_Info * pClass );

char * JvmDGetSuperClassName( _JVM_DClass_Info * pClass );
char * JvmDGetInterfaceName( _JVM_DClass_Info * pClass, int index );

#ifdef __cplusplus
};
#endif


#endif