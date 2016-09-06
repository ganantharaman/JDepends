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

#ifndef __JDP_HDR_INCLUDE__
#define __JDP_HDR_INCLUDE__

#include <mem.h>
#include <lock.h>
typedef unsigned long unsigned32;
#include <stream.h>



typedef void  *JVM_PTR_t;

#ifndef __NO_ACC_FLAGS__  /* Internal define */

enum {
 ACC_PUBLIC     =   0x0001,
 ACC_PRIVATE    =   0x0002,
 ACC_PROTECTED 	=   0x0004,
 ACC_STATIC  	=   0x0008,
 ACC_FINAL  	=   0x0010,
 ACC_SYNCHRONIZED =  0x0020,
 ACC_SUPER      =   0x0020,
 ACC_VOLATILE  	=   0x0040,
 ACC_TRANSIENT  =   0x0080,
 ACC_NATIVE     =   0x0100,
 ACC_INTERFACE  =   0x0200,
 ACC_ABSTRACT   =   0x0400,
 ACC_STRICT     =   0x0800,
 FLAG_SYNTHETIC =   0x1000,
 FLAG_DEPRECATED =  0x2000
}; 

#endif



typedef struct _JVM_DField_Info {
   char *    field_name;
   char      field_type_name[256];
   int       is_a_class;
   /* Reserved , do not access */
   JVM_PTR_t pjfField;
}_JVM_DField_Info;

struct _JVM_DCall_Graph;

typedef struct _JVM_DMethod_Info {
   char *    method_name;
  _JVM_DField_Info ret_info; 
  _JVM_DField_Info *  parameters;
   int param_count;
  _JVM_DField_Info *  local_vars;
   int local_vars_count;
   char *    byte_codes;
   unsigned long byte_code_count;
   char **   exception_classes;
   int exceptions_count;
   unsigned long * call_offsets;
   unsigned int call_offsets_len;
   struct _JVM_DCall_Graph * call_graph;
   /* Reserved , do not access */
   JVM_PTR_t pjmMethod;
   int required_len;
}_JVM_DMethod_Info;


typedef struct _JVM_DCP_Info {
   char    * string;
   unsigned char *  bytes;
   unsigned int     len_bytes;
   const char  * type;
}_JVM_DCP_Info;

typedef struct _JVM_DClass_Info {
   STREAM strm;
   char * class_name;
   char * super_class_name;
   _JVM_DCP_Info * cp_info;
   int cp_info_count;
   char ** interface_names;
   char ** all_referred_classes;
   int referred_classes_count;
   char ** contained_classes;
   int contained_classes_count; 
   _JVM_DField_Info * fields;
   int fields_count;
  _JVM_DMethod_Info * methods;
   int methods_count;
   /* Reserved , do not access */ 
   JVM_PTR_t pjccClass;
}_JVM_DClass_Info;

typedef struct _JVM_DCall_Graph {
  _JVM_DMethod_Info * jmethod;
  _JVM_DClass_Info  * jclass;
  struct _JVM_DCall_Graph ** nodes;
  int nodes_len;
}_JVM_DCall_Graph ;


#ifdef __cplusplus
extern "C" {
#endif

int JvmDependsInit();
void JvmDependsExit();

_JVM_DClass_Info * JvmLoadDependsClassFromFile(	char * filename );
_JVM_DClass_Info * JvmLoadDependsClassFromJAR(	char * jarname, char * classname );

int JvmFreeDependsClass( _JVM_DClass_Info * pClass );

int JvmDHasSuperClass( _JVM_DClass_Info * pClass );
int JvmDHasInterface( _JVM_DClass_Info * pClass );

int JvmDIsAnInterface( _JVM_DClass_Info * pClass );
int JvmDIsAbstract( _JVM_DClass_Info * pClass );
int JvmDIsFinal( _JVM_DClass_Info * pClass );

char * JvmDGetSuperClassName( _JVM_DClass_Info * pClass );
char * JvmDGetInterfaceName( _JVM_DClass_Info * pClass, int index );
char * JvmDGetSpringFromCString(char * cstr);
void   JvmDFreeSpring(char ** pSpring);

_JVM_DField_Info * JvmDGetFieldsInfo( _JVM_DClass_Info * pClassInfo );
_JVM_DMethod_Info * JvmDGetMethodsInfo( _JVM_DClass_Info * pClassInfo, 
                     char * MethodName, char * jsMethodDesc, int * pIndex );
_JVM_DCall_Graph * JvmDGetCallGraph( _JVM_DCall_Graph * pGraph, 
                                     _JVM_DClass_Info * pClassInfo,
                                     _JVM_DMethod_Info * pMethodInfo);
int JvmDGetByteCodes( _JVM_DMethod_Info * pMethodInfo );
int JvmDGetContainedClasses(  _JVM_DClass_Info * pClassInfo );

int JvmDGetReferredClasses( _JVM_DClass_Info * pClassInfo );
int JvmDGetCPInfo( _JVM_DClass_Info * pClassInfo );

unsigned long JvmDGetFieldAttributes( _JVM_DField_Info * pInfo);
unsigned long JvmDGetMethodAttributes( _JVM_DMethod_Info * pInfo);
unsigned long JvmDGetClassAttributes( _JVM_DClass_Info * pInfo );

char * JvmDGetSourceFileName(_JVM_DClass_Info * pClass);


#ifdef __cplusplus
};
#endif


#endif