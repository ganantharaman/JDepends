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

#include <jvmexcmn.h>
#include <jvmmem.h>
#include <jvmrt.h>
#include <jvmexcpt.h>
#include <jvmsprng.h>
#include <jvmconfg.h>
#include <jvmldr.h>
#include <jvmsys.h>
#include <jvmutil.h>
#include <jvmparse.h>
#include <cmnmcros.h>
#include <bytecode.h>

#include <stdio.h>
#include <stream.h>


#include <jdp.h>


#define JVM_LOAD_CLASS( class )  \
  if ( !JvmLdrLoadNewClass( class, JVM_NULL,joClassLoader, JVM_NULL ))\
 	   return JVM_FALSE;\

#if defined( __JVM_CHECKTOOL_TEST_ENV__ )

JVM_OBJREF JvmRtCreateNewInstance( _JVM_Class_Class *pjccTheClass );
JVM_VOID JvmAddDummyCPEntry(
  IN  OUT _JVM_Constant_Pool *** pppjcpThePool,
  IN  OUT  JVM_WORD     *  pjwCurCPSize
);

_JVM_Mutex_Object   _jmoGGlobalDataMutex;
_JVM_Mutex_Object   _jmoGExclCodeMutex;
_JVM_Mutex_Object   _jmoGClassStateMutex;
_JVM_Mutex_Object   _jmoGCLdrDictMutex;
_JVM_Mutex_Object   _jmoGCLdrStateMutex;
_JVM_Mutex_Object   _jmoGAttrVtblMutex;
_JVM_Mutex_Object   _jmoGSpringMutex;
_JVM_Mutex_Object   _jmoGConfigMutex;
_JVM_Mutex_Object   _jmoGFileHandleAccessMutex;
				    
_JVM_Thread_State * _pjtsGStartThread = JVM_NULL;
Hash_Table        * _pdictGCLdrClass;
Hash_Table        * _pdictGCLdrClassState;
Hash_Table        * _pjhtGSpringTable;

_JVM_Sys_Objects  * _psoGSysObjects = JVM_NULL;
link_list_t *         _pllGFileHandleList = NULL;



_JVM_Object         _jaoGPrimitiveLoader;

JVM_BOOL JvmGCAddNewObject( JVM_OBJREF pjoObjRef );

#endif



typedef struct SPLCODE_INFO{
    _JVM_Method * method;
     JVM_BYTE     opcode;
	 JVM_DWORD    pos;
	 JVM_DWORD    skip;
} SPLCODE_INFO;

enum { __GAP__ = 30 };

static void LdrPrettyPrintCode( _JVM_Method * method );
static void LdrGetCallOffsets(  JVM_DWORD ** call_offsets, JVM_DWORD * call_offsets_len,  _JVM_Method * method );
_JVM_DCall_Graph * InitCallGraph();

/** For lookupswitch, tableswitch and wide **/
void * skip_calc( void * info )
{
   SPLCODE_INFO * sp_info = ( SPLCODE_INFO *)info;
   JVM_PBYTE code    = sp_info->method->pjciCodeInfo->pjbyCode;
   JVM_U32 low,high,offset,pad;
   assert( code[sp_info->pos] == sp_info->opcode);
   high = low = offset = pad = 0;

   if (sp_info->opcode == lookupswitch) 
   {
	  /* lookupswitch - Pg: 300 JVM Spec.1.0.2 */
    /* <lookupswitch><0-3byte pad><h1><h2><h3><h4><high * 8> */
    pad = 3 - (sp_info->pos % 4);
	  offset  = sp_info->pos + pad + 5 ;
    high = code[offset] | code[offset + 1] | 
           code[offset + 2] | code[offset + 3];
    high *= 8;
   /** Now we are ready to calculate skip **/
	  sp_info->skip = high + 9 + pad;
   }
   else if ( sp_info->opcode == tableswitch ) 
   { 
      /* tableswitch - Pg: 336 JVM Spec.1.0.2 */  
    pad =  3 -  (sp_info->pos % 4);
	  offset = sp_info->pos + pad + 5;
    low =  code[offset] | code[offset + 1] |
           code[offset + 2] | code[offset + 3];
	  offset += 4;
    high = code[offset] | code[offset + 1] |
           code[offset + 2] | code[offset + 3];
	  assert(low <= high);
	  /** Calculate skip **/
	  sp_info->skip = ((high - low + 1) * 4) + 13 + pad;
   }
   else if ( sp_info->opcode == wide ) 
   {
     sp_info->skip = ( code[sp_info->pos + 1] == iinc ) ? 5 : 3;
   }

   return (void *)sp_info;
}

static void LdrGetCallOffsets(  JVM_DWORD ** call_offsets, JVM_DWORD * call_offsets_len,  _JVM_Method * method )
{
  _JVM_ByteCode_Info * bc_info;
   SPLCODE_INFO sp_info;
   JVM_DWORD dw = 0,skip,i=0;
   JVM_DWORD alloc_size = CHUNK, m = 0;
   JVM_DWORD * offsets = NULL;

   if(method == NULL) return;
   bc_info = method->pjciCodeInfo;
   sp_info.method = method;

   // No code
   if (!bc_info) {
     if ( call_offsets )
       *call_offsets = NULL;
     if ( call_offsets_len )
       *call_offsets_len = 0L;
     return; 
   }

   offsets = JvmMemAlloc( sizeof( JVM_DWORD ) * alloc_size );

   for ( ; dw < bc_info->jdwCodeLength; )
   {
	    JVM_BYTE opcode = bc_info->pjbyCode[dw];
      assert( opcode < MAX_OPCODES );

 	    if ( opcode == lookupswitch || opcode == tableswitch || opcode == wide )
      {
	      sp_info.opcode = opcode;
	      sp_info.pos    = dw; 
	      sp_info.skip   = 1;
        (* op_tab[opcode].pfn)( &sp_info );
	      skip = sp_info.skip;
      } 
      else
       skip = op_tab[opcode].skip;

     if ( opcode == invokespecial || opcode == invokevirtual || opcode == invokestatic ||
          opcode == invokeinterface )
     {
       if ( m >= alloc_size ) {
         JVM_DWORD * tmp = JvmMemRealloc( offsets, sizeof( JVM_DWORD) * (alloc_size + CHUNK));
         offsets = tmp;
         alloc_size += CHUNK;
       }
       offsets[m++] = dw;          
     }

     dw += skip;
   }

   // Update 
   if ( call_offsets )
     *call_offsets = offsets;
   if ( call_offsets_len )
     *call_offsets_len = m;

}


static void LdrStreamPrintCode( STREAM strm, _JVM_Method * method )
{
  _JVM_ByteCode_Info * bc_info;
   SPLCODE_INFO sp_info;
   JVM_DWORD dw = 0,len,skip,i,wrap;
   JVM_BUFFER info;
 
   assert( method != NULL );
   bc_info = method->pjciCodeInfo;
   sp_info.method = method;

   if (!bc_info) return;
 
   for ( ; dw < bc_info->jdwCodeLength; )
   {
	    JVM_BYTE opcode = bc_info->pjbyCode[dw];
      assert( opcode < MAX_OPCODES );

      JvmSysPumpWindowsMessages();

    if ( opcode == lookupswitch || opcode == tableswitch || opcode == wide )
      {
	      sp_info.opcode = opcode;
	      sp_info.pos    = dw; 
	      sp_info.skip   = 1;
        (* op_tab[opcode].pfn)( &sp_info );
	      skip = sp_info.skip;
      } 
      else
       skip = op_tab[opcode].skip;

 
	   len = JvmStrLen(op_tab[opcode].name);
	   JvmMemSet( info, ' ', sizeof( info ));
	   JvmMemCpy( info + 3, op_tab[opcode].name, len );
	   info[__GAP__] = 0;

	   for ( wrap = 1,i= 0; i < skip; i++ )
     {
       wrap ++;
       if ( wrap > 8 )
       {
         JVM_DWORD tmp_len = 0;   
         wrap = 1;
         sprintf( info + JvmStrLen(info), "%s", "\r\n");
         tmp_len = JvmStrLen(info); 
         JvmMemSet( info + tmp_len, ' ', __GAP__);
         info[tmp_len + __GAP__] = 0;
       }
	     sprintf( info + JvmStrLen(info), " 0x%02x", bc_info->pjbyCode[dw + i] );
     }
 
     dw += skip;
     stream_write(strm, info, JvmStrLen(info));
     stream_write(strm, "\r\n", 2 );
   }

}

static void LdrPrettyPrintCode( _JVM_Method * method )
{
   DECLARE_LOG_DATA("c:\\jvm\\out\\class.log")
   STREAM strm = stream_open();
   char * buffer;

/* LOG_WRITE("%s", method->jsMethodName)
   LOG_WRITE("%s", "\r\n") */
   LOG_WRITE("%s","{")
   
   LdrStreamPrintCode( strm, method ); 

   buffer = stream_get_buffer( strm );
   LOG_WRITE("%s", buffer )    
   LOG_WRITE("%s","}")

   stream_close( strm );

}





/** CHECK,CHECK The following are incomplete 
    JVM_BOOL JvmLdrMakeNewArrayClass( 
	IN      JVM_OBJREF joClassLoader,
	IN      JVM_STRING jsArrayName, 
	OUT     JVM_OBJREF * pjoClassObjectRef
	) 

   


/** CHECK,CHECK The following are not implemented yet 

   
 1) ---- functions ----

    JVM_BOOL JvmRtIsStringLoaded( JVM_PBYTE pjbyString, JVM_WORD jwStringLen, 
         JAVA_OBJECTREF * pjaorObjectRef ) 
	JVM_BOOL JvmSetStringObjectContents(
           JAVA_OBJECTREF jaorObjectRef, 
		   JVM_PBYTE      pjbyString,
		   JVM_WORD       jwStringLen
		   )
	JVM_BOOL JvmFreeThePool(); 
	JVM_BOOL JvmFreeTheClass();
	JVM_BOOL JvmCleanUpMethods(_JVM_Method **);

 2)  ----- Structures -------

     struct _JVM_Code_Info   
     struct _JVM_Exception_Info 

 3) Replace sizeof( <string> ) Please!
 4) None of the created Hash tables /Dictionaries are being freed !!
 5) Get the hell out of JVM on no memory!
    All memory errors MUST throw EXC_NOMEMORY exception.


 **/

/** #define CA-BOOM Cancel All - Bailing Out On Memory-error **/

/** Check tool test environment is our test project all the modules
    undergo severe testing here.
	For setting up Test env 
	#define __DEBUG__, __I386__,__TRACE__( if necessary )
	    and __JVM_CHECKTOOL_TEST_ENV__
 **/

#if defined(__JDEPENDS__)
/** Do not expand code **/
#ifdef __EXPAND_CODE__
 #undef __EXPAND_CODE__
#endif


JVM_VOID JvmInit();
JVM_VOID JvmExit();
int JvmGetTypeName(_JVM_Field_Info * pFieldInfo, char * name);
_JVM_DClass_Info * JvmDLookupClass( JVM_OBJREF joObjectRef ) ;

typedef struct _JVM_Type_Strings{
	JVM_CHAR     id;
	char * name;
}_JVM_Type_Strings;

Hash_Table * _pdictGDClass = NULL;
_JVM_Mutex_Object  _jmoGDClassDictMutex;

_JVM_Type_Strings type_names[8] = 
{
	{'B', "byte"},
	{'C', "char"},
	{'D', "double"},
	{'F', "float"},
	{'I', "int"},
	{'J', "long"},
	{'S', "short"},
	{'Z', "boolean"}
};

char cp_type_strings[JVM_MAX_CP_TYPES][32]  =
{
	  "UTF8",
	  "",
	  "INTEGER",
	  "FLOAT",
	  "LONG",
	  "DOUBLE",
	  "CLASS",
	  "STRING",
	  "FIELDREF",
	  "METHODREF",
	  "INTERFACE METHODREF",
	  "NAME AND TYPE"
};

int JvmDependsInit() 
{
   JVM_OBJREF  joClassLoader = PRIMITIVE_LOADER;
   Hash_Type htinvalid;

   JvmInit();
   htinvalid.u.pv = NULL;
#ifdef __DEBUG__
  CreateDictionary(
		 "DClassDict",
		 &_pdictGDClass,
     htinvalid,
     HASH_TYPE_PTR,
     HASH_TYPE_PTR,
     DependsDictRundown
  );
#else
   CreatePtrDictionary(
		 &_pdictGDClass,
     htinvalid,
     HASH_TYPE_PTR,
     HASH_TYPE_PTR,
     DependsDictRundown
	 );
#endif

   JvmSysInitMutex( &_jmoGDClassDictMutex );

   JVM_LOAD_CLASS( S_JAVA_LANG_OBJECT        );
   return 1;
}

JVM_VOID DependsDictRundown( Hash_Type htype )
{
  if ( htype.u.pv )
     JvmMemFree( htype.u.pv );
}

char * JvmDGetSpringFromCString(char * pstr) {
  return JvmSpringFromBytes( pstr,0,1,0 );  
}

void JvmDFreeSpring(char ** pSpring) {
   JvmSpringFree(pSpring);
}

_JVM_DClass_Info * JvmDLookupClass( _JVM_Class_Class * pjccClass ) 
{
   _JVM_DClass_Info * pClass = NULL;
   Hash_Type htkey,htval;

   JvmSysAcquireMutex( _jmoGDClassDictMutex );
  
   htkey.u.pv = pjccClass;
   htval = 	 GetFromDictionary( _pdictGDClass, htkey );
   pClass = ( _JVM_DClass_Info * ) htval.u.pv;

   if (!pClass ) {
     htkey.u.pv = pjccClass;
     /* Initialize pClass */
     pClass = (_JVM_DClass_Info *)JvmMemCalloc( sizeof(_JVM_DClass_Info));
     if ( pClass ) {
       /* Get class information */
       pClass->strm = NULL;
       pClass->pjccClass = pjccClass;
       pClass->class_name = pjccClass->jsClassName;
       pClass->referred_classes_count = pClass->contained_classes_count = 0;
       pClass->all_referred_classes = pClass->contained_classes = NULL;
       htval.u.pv = pClass;
       AddToDictionary( _pdictGDClass, htkey, htval );
     }
   }

   JvmSysReleaseMutex( _jmoGDClassDictMutex );
   return pClass;
}

_JVM_DClass_Info * JvmLoadDependsClassFromJAR( char * jarfilename, char * classname)
{
  _JVM_DClass_Info * pClass = JVM_NULL;
  JVM_STRING jar =   jarfilename ? JvmSpringFromBytes( jarfilename,0,1,0 ) : NULL;
  _JVM_Sys_Jar_Info * info ;
  JVM_OBJREF joClassObject;
  info = JvmSysExtractFromJarFile( jar, classname );

  if ( info ) {
    if ( !JvmLdrLoadNewClass( JVM_NULL, info->pjbyBytes, PRIMITIVE_LOADER, &joClassObject) ) {
	    return NULL;
    }
    JvmSysFreeJarInfo( info ); 
    pClass= JvmDLookupClass( ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass);
  }

  return pClass;
}

char * JvmDGetSourceFileName(_JVM_DClass_Info * pClass)
{
  _JVM_Class_Class * pjccClass = (_JVM_Class_Class *)pClass->pjccClass;
  return pjccClass ? pjccClass->jsSourceFileName : NULL;
}


_JVM_DClass_Info * JvmLoadDependsClassFromFile( char * classname )
{
  /* Later, Later */
#if 0
   DECLARE_LOG_DATA("c:\\jvm\\out\\depends.log") 
   char * strm_buffer = NULL;
   _JVM_Method * pjmMethod ;
  _JVM_Field  * pjfField;
   JVM_U16 ju16Index = 0;
   JVM_U16 ju16Iter = 0;
   JVM_BUFFER buffer;
   JVM_BUFFER retname;
#endif

   JVM_STRING class =   classname ? JvmSpringFromBytes( classname,0,1,0 ) : NULL;
   JVM_OBJREF joClassObject;
  
  _JVM_DClass_Info * pClass    = NULL;
  _JVM_Class_Class * pjccClass = NULL;

    if ( !classname ) return NULL;

    if ( !JvmLdrLoadNewClass( class, JVM_NULL, PRIMITIVE_LOADER, &joClassObject) ) {
     JvmDeferException( class, PRIMITIVE_LOADER, EXC_JAVACLASSFORMATERROR );
	   return NULL;
    }

   pClass = JvmDLookupClass( ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass );

    /* Later, Later */
#if 0
   /* create stream */
   pClass->strm      = stream_open();

   f_stream_write(  pClass->strm, "%s\n", pjccClass->jsClassName );
   sprintf( buffer, "ClassFlags: 0x%08x\r\n", pjccClass->jdwClassFlags);
   stream_write( pClass->strm , buffer, strlen( buffer ));
   f_stream_write( pClass->strm, "ClassName: %s\r\n", pjccClass->jsClassName );
   f_stream_write( pClass->strm, "SuperClass: %s\r\n",pjccClass->jsSuperClassName );
   f_stream_write( pClass->strm, "InterfacesCount: %d\r\n",pjccClass->jwIntfCount );
   f_stream_write( pClass->strm, "FieldsCount: %d\r\n", pjccClass->jwFieldsCount );
   f_stream_write( pClass->strm, "%s...\r\n", "Fields" );

   for ( ;ju16Index < pjccClass->jwFieldsCount;  ju16Index ++ )
   {
     pjfField = pjccClass->ppjfData[ ju16Index ];
	   f_stream_write( pClass->strm, "   %s - %s\r\n", pjfField->jsFieldName,
		   pjfField->jsFieldDesc );
   }

   f_stream_write( pClass->strm, "MethodsCount: %d\r\n", pjccClass->jwMethodsCount );

   for ( ju16Index = 0;ju16Index < pjccClass->jwMethodsCount;  ju16Index ++ )
   {
     pjmMethod = pjccClass->ppjmMethods[ ju16Index ];
	   /** Print out <return type> <methodname>( arg1, arg2... )
	   // if <methodname> is <init> change it to that of the class name
	   // display full information including the sizeof the array and 
	   // Object type name if any!!
	   **/
	   if ( pjmMethod->pjmiMethodInfo->jbReturn )
	     JvmGetTypeName(&pjmMethod->pjmiMethodInfo->jfiReturnField, retname);
	   else
       strcpy( retname, "void" );

	   /* write return type and method name */
	   f_stream_write( pClass->strm, "%s %s(", retname, 
		   pjmMethod->jsMethodName );

	   /* write out the formal parameter list */
	   for (ju16Iter = 0 ;ju16Iter < pjmMethod->pjmiMethodInfo->jiParameterCount; ju16Iter++ )
	   {
       JvmGetTypeName(pjmMethod->pjmiMethodInfo->pjfiParameterFields[ju16Iter]
        , retname); 
	     f_stream_write( pClass->strm, "%s%s", 
		        ( ju16Iter == 0 ) ? "":",", retname ); 
	   }
	   stream_write( pClass->strm, ")\r\n{\r\n", 6);
	   /* write out mneumonic and bytecode */
	   LdrStreamPrintCode(pClass->strm, pjmMethod );
	   stream_write( pClass->strm, "}\r\n\r\n", 5);
   }
  /** WARNING: By writing out the buffer we reset the buffer and empty
      its contents . Use these lines only on debug mode **/
   strm_buffer =  stream_get_buffer(pClass->strm);
   LOG_WRITE( "%s", strm_buffer);
#endif

   return pClass;
}


/* 
 *  Returns a string name for a given Field type 
 *  Arrays are returned as name[(<arraysize>)]
 *  No error checking is performed
 */
int JvmGetTypeName(_JVM_Field_Info * pFieldInfo, char * name)
{
	char * t_name;
	int index = 0;
	if ( pFieldInfo->jwFieldType & JVM_BASE_TYPE ) {
       for ( index = 0; index < 8; index++ )
		   if ( type_names[index].id == pFieldInfo->jcBaseType )
			   t_name = type_names[index].name;
       index = 0;
	}else if ( pFieldInfo->jwFieldType & JVM_OBJECT_TYPE ) {
		t_name = pFieldInfo->jsObjectName;
    index = 1;
	}

  if ( pFieldInfo->jwFieldType & JVM_ARRAY_TYPE ) {
    sprintf(name, "%s[(%d)]", t_name, pFieldInfo->jwArrayDimension);
	} else {
		strcpy( name, t_name );
	}

  return index;
}

/*
 *  Return true if class has super class false otherwise 
 */    
int JvmDHasSuperClass( _JVM_DClass_Info * pClass )
{
  return ((char *)((_JVM_Class_Class * )pClass->pjccClass)->jsSuperClassName != JVM_NULL);
}

/*
 *  Return interface count , should be 0 if class has not interface
 */
int JvmDHasInterface( _JVM_DClass_Info * pClass )
{
  return ((_JVM_Class_Class * )pClass->pjccClass)->jwIntfCount;
}

int JvmDIsAnInterface( _JVM_DClass_Info * pClass )
{
  return (((_JVM_Class_Class * )pClass->pjccClass)->jdwClassFlags & ACC_INTERFACE);
}

int JvmDIsAbstract( _JVM_DClass_Info * pClass )
{
  return (((_JVM_Class_Class * )pClass->pjccClass)->jdwClassFlags & ACC_ABSTRACT);
}

int JvmDIsFinal( _JVM_DClass_Info * pClass )
{
  return (((_JVM_Class_Class * )pClass->pjccClass)->jdwClassFlags & ACC_FINAL);
}

/*
 *  Get the interface name
 */
char *  JvmDGetInterfaceName( _JVM_DClass_Info * pClass, int index )
{
  _JVM_Class_Class * pjccClass = (_JVM_Class_Class * )pClass->pjccClass;
  if ( index >= pjccClass->jwIntfCount )
	  return NULL;
  /* jsClassName is still a JVM_SPRING */
  return JvmGetClassName(pjccClass,
			pjccClass->pju16InterfacesCPIndex[ index ] , NULL);
}

/*
 *  Get super class name 
 */
char *  JvmDGetSuperClassName( _JVM_DClass_Info * pClass )
{
  return (char *)((_JVM_Class_Class * )pClass->pjccClass)->jsSuperClassName;
}

_JVM_DField_Info * JvmDGetFieldsInfo( _JVM_DClass_Info * pClassInfo )
{
  _JVM_Class_Class * pjccClass;
   JVM_U16 ju16Index = 0;
  _JVM_Field  * pjfField;
  _JVM_DField_Info * info; 
   JVM_ASSERT(pClassInfo  != NULL);
   pjccClass = (_JVM_Class_Class *)pClassInfo->pjccClass;

   /* WARNING : Note that pjfField member for field info structures are set to NULL currently */
   if ( pjccClass->jwFieldsCount ) {
     info = JvmMemCalloc( sizeof( _JVM_DField_Info ) * pjccClass->jwFieldsCount);
     for ( ;ju16Index < pjccClass->jwFieldsCount;  ju16Index ++ )
     {
        pjfField = pjccClass->ppjfData[ ju16Index ];
        info[ju16Index].pjfField   = pjfField;
        info[ju16Index].field_name =  pjfField->jsFieldName;
        info[ju16Index].is_a_class = 
          JvmGetTypeName( &pjfField->jfiFieldType, info[ju16Index].field_type_name);
        if ( info[ju16Index].is_a_class ) {
           pClassInfo->referred_classes_count++;
           pClassInfo->contained_classes_count++;
        }
     }
     pClassInfo->fields = info;
     pClassInfo->fields_count = pjccClass->jwFieldsCount;     
     return info;
   }
   return NULL;
}

/* typedef struct _JVM_DMethods_Info {
   char *    method_name;
  _JVM_DField_Info ret_info; 
  _JVM_DField_Info *  parameters;
  _JVM_DField_Info *  local_vars;
   char *    byte_codes;
}_JVM_DMethods_Info;
*/
_JVM_DMethod_Info * JvmDGetMethodsInfo( _JVM_DClass_Info * pClassInfo, 
                                       char * jsMethodName, char * jsMethodDesc, int * pIndex )
{
  _JVM_Class_Class * pjccClass;
   JVM_U16 ju16Index = 0, jwCount,ju16Iter;
  _JVM_Method  * pjmMethod;
  _JVM_DMethod_Info * info = NULL; 
  _JVM_LV_Table_Info   * pjltiLVInfo;
   JVM_ASSERT(pClassInfo  != NULL);
   pjccClass = (_JVM_Class_Class *)pClassInfo->pjccClass;

   /* if resolved already just return the index */
   if ( pClassInfo->methods )
   {
     if ( jsMethodName && jsMethodDesc && pIndex )
     {
       for ( ju16Index = 0;ju16Index < pClassInfo->methods_count;  ju16Index ++ )
       {
           pjmMethod = pClassInfo->methods[ju16Index].pjmMethod;
           if ( JvmSpringStrCmp( pjmMethod->jsMethodName, jsMethodName ) == 0 ) {
		            if ( JvmSpringStrCmp( pjmMethod->jsMethodDesc,jsMethodDesc ) == 0 )  
                      *pIndex = ju16Index;
            }
       }
     }

     return pClassInfo->methods;
   }
  

   /* WARNING : Note that pjfField member for field info structures are set to NULL currently */
   if ( pjccClass->jwMethodsCount )
   {
     info = JvmMemCalloc( sizeof(_JVM_DMethod_Info) * pjccClass->jwMethodsCount);
     for ( ju16Index = 0;ju16Index < pjccClass->jwMethodsCount;  ju16Index ++ )  
     {
         pjmMethod = pjccClass->ppjmMethods[ ju16Index ];
	       info[ju16Index].method_name = pjmMethod->jsMethodName;

         /* Update any special request for method lookup */
         if ( jsMethodName && jsMethodDesc && pIndex ) {
            if ( JvmSpringStrCmp( pjmMethod->jsMethodName, jsMethodName ) == 0 ) {
		            if ( JvmSpringStrCmp( pjmMethod->jsMethodDesc,jsMethodDesc ) == 0 )  
                      *pIndex = ju16Index;
            }
         }

    	   if ( pjmMethod->pjmiMethodInfo->jbReturn )
	         info[ju16Index].ret_info.is_a_class = 
             JvmGetTypeName(&pjmMethod->pjmiMethodInfo->jfiReturnField, info[ju16Index].ret_info.field_type_name);
	       else
           strcpy( info[ju16Index].ret_info.field_type_name, "void" );

         jwCount = pjmMethod->pjmiMethodInfo->jiParameterCount;
         if ( jwCount )
         {
           info[ju16Index].parameters = JvmMemCalloc( sizeof( _JVM_DField_Info ) * jwCount);
           info[ju16Index].param_count = jwCount; 
           for (ju16Iter = 0 ;ju16Iter < jwCount; ju16Iter++ )
           {
              /* keep it NULL */
              info[ju16Index].parameters[ju16Iter].field_name = NULL; 
              info[ju16Index].parameters[ju16Iter].is_a_class = 
                JvmGetTypeName(pjmMethod->pjmiMethodInfo->pjfiParameterFields[ju16Iter]
                          , info[ju16Index].parameters[ju16Iter].field_type_name ); 
	         }
         }

         if ( pjmMethod->pjciCodeInfo )
         {
           jwCount = pjmMethod->pjciCodeInfo->jwLocalVarTableCount;
           info[ju16Index].local_vars = NULL;
           if ( jwCount )
           {
            info[ju16Index].local_vars = JvmMemCalloc( sizeof( _JVM_DField_Info ) * jwCount); 
            info[ju16Index].local_vars_count = jwCount; 
            for (ju16Iter = 0 ;ju16Iter < jwCount; ju16Iter++ )
            {
                pjltiLVInfo = pjmMethod->pjciCodeInfo->ppjltiLocalVarTable[ju16Iter];
                /* keep it NULL */
                info[ju16Index].local_vars[ju16Iter].field_name = NULL; 
                info[ju16Index].local_vars[ju16Iter].is_a_class =
                   JvmGetTypeName(&pjltiLVInfo->pjfField->jfiFieldType
                             , info[ju16Index].local_vars[ju16Iter].field_type_name ); 
                if ( info[ju16Index].local_vars[ju16Iter].is_a_class )
                      pClassInfo->referred_classes_count++;
	          }      
           }
         }
         
         jwCount = pjmMethod->jwMthdExcTableCount;
         if ( jwCount )
         {
            info[ju16Index].exception_classes = JvmMemAlloc( sizeof( char *) * jwCount);
            info[ju16Index].exceptions_count = jwCount;
            for (ju16Iter = 0;ju16Iter < jwCount;ju16Iter++)
            {
               info[ju16Index].exception_classes[ju16Iter] = 
                   JvmLdrGetStringName( pjccClass, pjmMethod->pjwMthdExcTable[ju16Iter], NULL );    
            }
         }
         /* Use separate method to get byte code */
         info[ju16Index].byte_codes = NULL;
         info[ju16Index].pjmMethod = pjmMethod;
     }/* for */
      pClassInfo->methods = info;
      pClassInfo->methods_count =  pjccClass->jwMethodsCount;
   } /* if */

  return info;
}

int JvmDGetByteCodes( _JVM_DMethod_Info * pMethodInfo )
{
   STREAM strm;
   _JVM_Method * pjmMethod; 
   JVM_ASSERT( pMethodInfo != NULL);
   pjmMethod = (_JVM_Method *)pMethodInfo->pjmMethod;
   JVM_ASSERT( pjmMethod != NULL);

   strm = stream_open(); 
   stream_write( strm, "{\r\n", 3);

	 /* write out mneumonic and bytecode */
   /* And while in it Get the call offsets */
   /* Call offsets are useful for constructing function call Graphs */
   LdrStreamPrintCode(strm, pjmMethod);
   LdrGetCallOffsets(&pMethodInfo->call_offsets, &pMethodInfo->call_offsets_len, pjmMethod);

   stream_write( strm, "}\r\n\r\n", 5);
   pMethodInfo->byte_codes = stream_get_buffer(strm);
   pMethodInfo->byte_code_count =  pjmMethod->pjciCodeInfo ? pjmMethod->pjciCodeInfo->jdwCodeLength : 0; 
   /* we free buffer */
   stream_reset( strm );
   return 1;
}


int JvmDGetCPInfo( _JVM_DClass_Info * pClassInfo )
{
     _JVM_DCP_Info * info;
     _JVM_Class_Class   * pClass;
     JVM_PBYTE pjbyBytes;
     JVM_WORD jwIndex, jwTag;

     JVM_ASSERT( pClassInfo );

     pClass = (_JVM_Class_Class * )pClassInfo->pjccClass;
     pClassInfo->cp_info = JvmMemAlloc( sizeof( _JVM_DCP_Info ) * pClass->jwCPEntryCount );
     pClassInfo->cp_info_count = pClass->jwCPEntryCount;

     for ( jwIndex = 0; jwIndex < pClass->jwCPEntryCount; jwIndex++)
     {
       pjbyBytes = (JVM_PBYTE)pClass->ppjcpConstantPool[ jwIndex ];
       info = &pClassInfo->cp_info[ jwIndex ] ;
       info->string    = JvmLdrGetStringName( pClass, jwIndex, NULL );
       info->len_bytes = CP_LEN( pjbyBytes );
       info->type =  NULL;
       jwTag = CP_TAG( pjbyBytes );
       info->bytes     =  pjbyBytes + sizeof( _JVM_Constant_Pool ) + 
         (( jwTag == JVM_CP_TAG_UTF8 ) ? WORDLEN : 0); 
       if ( jwTag <= JVM_MAX_CP_TYPES )
         info->type      =  cp_type_strings[jwTag - 1];
     }

     return TRUE;
}

int JvmResolveMethodRef( _JVM_Class_Class * pClass, int index, JVM_STRING * pjsClassName ,
                        JVM_STRING * pjsMethodName, JVM_STRING * pjsMethodDesc)
{
  JVM_PBYTE pjbyByte; 
  JVM_WORD ju16Index;
  JVM_STRING jsClassName, jsMethodName, jsMethodDesc;
  JVM_WORD tag;

  jsClassName = jsMethodName = jsMethodDesc = JVM_NULL;
  pjbyByte = (JVM_PBYTE)pClass->ppjcpConstantPool[ index ];

  tag = CP_TAG( pjbyByte);

  if ( JVM_CP_TAG_METHODREF != tag  && 
       JVM_CP_TAG_INTERFACEMETHODREF != tag )
     assert ( FALSE );

  ju16Index = LD_WORD( pjbyByte + SIZE_POOL);
  LD_WORD( pjbyByte + SIZE_POOL );

  assert( ju16Index < pClass->jwCPEntryCount );

  jsClassName = JvmLdrGetStringName( pClass, ju16Index, NULL);

  ju16Index = LD_WORD( pjbyByte + SIZE_POOL + WORDLEN);
  LD_WORD( pjbyByte + SIZE_POOL + WORDLEN);

  assert( ju16Index < pClass->jwCPEntryCount );

  /** Name and Type info of methodref **/
  pjbyByte = (JVM_PBYTE)pClass->ppjcpConstantPool[ ju16Index ];

  ju16Index  = LD_WORD( pjbyByte + SIZE_POOL);
  LD_WORD( pjbyByte + SIZE_POOL);

  assert( ju16Index < pClass->jwCPEntryCount );

  /* Get Method Name */ 
  jsMethodName = JvmLdrGetStringName( pClass, ju16Index, NULL);

	ju16Index = LD_WORD( pjbyByte + SIZE_POOL + WORDLEN);
  LD_WORD( pjbyByte + SIZE_POOL +	WORDLEN);

  assert( ju16Index < pClass->jwCPEntryCount );
  
  /* Get Method Desc */
  jsMethodDesc = JvmLdrGetStringName( pClass, ju16Index, NULL);

  *pjsClassName  = jsClassName;
  *pjsMethodName = jsMethodName;
  *pjsMethodDesc = jsMethodDesc;

  return 1;

}


JVM_BOOL IsValidMethodOf( 
 IN      _JVM_Class_Class   * pjccClass,
 IN      JVM_STRING         jsMethodName,
 IN      JVM_STRING         jsMethodDesc, 
 OUT     _JVM_Method     ** ppjmMethod
 )
{
	_JVM_Method * pjmMethod ;
	 JVM_U16 ju16MethodIndex = 0;

    JVM_ASSERT_VALID( pjccClass );
	JVM_ASSERT_VALID( jsMethodName );
	JVM_ASSERT_VALID( ppjmMethod );
    *ppjmMethod = JVM_NULL;

	for ( ;ju16MethodIndex < pjccClass->jwMethodsCount;  ju16MethodIndex ++ )
	{
	   pjmMethod = pjccClass->ppjmMethods[ ju16MethodIndex ];
	   if ( JvmSpringStrCmp( pjmMethod->jsMethodName, jsMethodName ) == 0 )  
	   {
		   if ( JvmSpringStrCmp( pjmMethod->jsMethodDesc,jsMethodDesc ) == 0 )  
       {
		     *ppjmMethod = pjmMethod;
		     return JVM_TRUE;
       }
	   }
	}

    return JVM_FALSE;
}


/** Find out if there is a method with the given name and description
    existing in any of the given Class's bases if so note the method and 
	class and return .
	note that this fn. 
	does NOT change any passed in parameters if no such method is found
**/
JVM_BOOL JvmGetNearestBaseMethod( 
 IN  _JVM_Class_Class * pjccTheClass,
 IN   JVM_STRING jsMethodName,
 IN   JVM_STRING jsMethodDesc,
 OUT _JVM_Method ** ppjmMethod, 
 OUT _JVM_Class_Class ** ppjccClass
)
{
   _JVM_Class_Class *pjccAClass = JVM_NULL;

	JVM_ASSERT_VALID( pjccTheClass );
	JVM_ASSERT_VALID( ppjmMethod );
	JVM_ASSERT_VALID( ppjccClass );

	pjccAClass = pjccTheClass;

	/** Loop until we reach 'Object'( with NULL super) **/
	do
	{
    if ( !pjccAClass->joSuperClassObject )
        break;
    pjccAClass = pjccAClass->joSuperClassObject->joiObjectID.pjccClass;
		/** Check if the method is a valid method of pjccAClass **/
		if ( pjccAClass &&  IsValidMethodOf( pjccAClass, 
			     jsMethodName, jsMethodDesc, ppjmMethod ))
		{
			*ppjccClass = pjccAClass;
			return JVM_TRUE;
		}

	} while ( pjccAClass != JVM_NULL );

	return JVM_FALSE;
}

_JVM_DCall_Graph * InitCallGraph()
{
  _JVM_DCall_Graph * Graph;
  Graph = JvmMemAlloc( sizeof( _JVM_DCall_Graph ));

  Graph->nodes   = NULL;
  Graph->nodes_len = 0L;
  Graph->jmethod = NULL;
  Graph->jclass  = NULL;

  return Graph;
}


_JVM_DCall_Graph * JvmDGetCallGraph( _JVM_DCall_Graph * pGraph, _JVM_DClass_Info * pClassInfo, _JVM_DMethod_Info * pMethodInfo)
{
  _JVM_Class_Class   * pjccClass, * pjccRefClass;
  _JVM_DClass_Info   * pClass;
  _JVM_DCall_Graph    * pCallGraph = NULL;

  _JVM_Method * pjmMethod, * pjmRefMethod;
  _JVM_ByteCode_Info * bc_info;
   JVM_WORD  index = 0;
   JVM_DWORD info_index = 0;
   JVM_STRING jsClassName, jsMethodName, jsMethodDesc;
   JVM_OBJREF joClassObject;
   JVM_DWORD  i;

   JVM_ASSERT( pClassInfo != NULL );
   JVM_ASSERT( pMethodInfo  != NULL );

   pjccClass = (_JVM_Class_Class *)pClassInfo->pjccClass;
   pjmMethod = (_JVM_Method *) pMethodInfo->pjmMethod;

   if ( !pGraph ) {
     pCallGraph = pMethodInfo->call_graph = InitCallGraph();
     pCallGraph->jclass  = pClassInfo;
     pCallGraph->jmethod = pMethodInfo;
   } else {
     pCallGraph = pGraph;
   }

    // Call offets must be constructed before 
   if (!pMethodInfo->call_offsets )
     return pCallGraph;

   bc_info = pjmMethod->pjciCodeInfo;

  if (!bc_info) return pCallGraph; 

  if ( pMethodInfo->call_offsets_len )
  {
   pCallGraph->nodes = JvmMemAlloc( sizeof( _JVM_DCall_Graph *) * pMethodInfo->call_offsets_len );
   pCallGraph->nodes_len =  0;
   for (i = 0 ; i <  pMethodInfo->call_offsets_len; i++) //dw < bc_info->jdwCodeLength; )
   {
     JVM_DWORD dwOff = pMethodInfo->call_offsets[i];
	   JVM_BYTE opcode = bc_info->pjbyCode[dwOff];
     if ( opcode == invokespecial ||
         opcode == invokestatic  ||
         opcode == invokevirtual ||
         opcode == invokeinterface 
         )
     {
      if (op_tab[opcode].skip != 3 || op_tab[opcode].skip != 5  )
         JVM_ASSERT( FALSE );

      index = 0L;
      index = bc_info->pjbyCode[dwOff+1];
      index =  ( index << 8 )| bc_info->pjbyCode[dwOff+2];
      if ( index >=0 && index < pjccClass->jwCPEntryCount)
      {
        JvmResolveMethodRef( pjccClass, index, &jsClassName, &jsMethodName, &jsMethodDesc );

        // Load the class
        if ( JVM_VALID( jsClassName )) 
        {
           _JVM_DCall_Graph * pNodeCallGraph = NULL;
           if ( !JvmLdrLoadNewClass( jsClassName, JVM_NULL, PRIMITIVE_LOADER, &joClassObject) ) {
 	                 return pCallGraph;
           }

           ++pCallGraph->nodes_len;
           
           pClass = JvmDLookupClass( ((_JVM_Object *)joClassObject)->joiObjectID.pjccClass );
           pjccRefClass = pClass->pjccClass;

           /* Get Methods info */
           JvmDGetMethodsInfo( pClass, jsMethodName, jsMethodDesc, &info_index );
           pjmRefMethod = pClass->methods[info_index].pjmMethod;

           /* Must get the bytes code of the obtained method */
           LdrGetCallOffsets(&pClass->methods[info_index].call_offsets,
                             &pClass->methods[info_index].call_offsets_len,
                             pjmRefMethod);

           /* All set */
           pNodeCallGraph = InitCallGraph();
           pNodeCallGraph->jclass  = pClass;
           pNodeCallGraph->jmethod = &pClass->methods[info_index];
           pCallGraph->nodes[i] = pNodeCallGraph;  //JvmDGetCallGraph( pClass, &pClass->methods[info_index]);

        } // valid class
      } // index
     } // opcode
   
   } // for
  } // if

  return pCallGraph;
}

int JvmDGetReferredClasses( _JVM_DClass_Info * pClassInfo )
{
     _JVM_Constant_Pool ** jcpPool;
     _JVM_Class_Class   * pClass;
      JVM_PBYTE pjbyBytes;
      JVM_WORD jwIndex, iter = 0;
      JVM_WORD * pjwClassCount;

     JVM_ASSERT( pClassInfo != NULL);

     pClass = (_JVM_Class_Class * )pClassInfo->pjccClass;

     jcpPool = pClass->ppjcpConstantPool;
     pjwClassCount = JvmMemAlloc( sizeof( JVM_WORD ) * pClass->jwCPEntryCount );

     for ( jwIndex = 0; jwIndex < pClass->jwCPEntryCount; jwIndex++)
     {
        pjbyBytes = (JVM_PBYTE)jcpPool[jwIndex];
        if ( JVM_CP_TAG_CLASS == CP_TAG( pjbyBytes ))
           pjwClassCount[iter++] = jwIndex;
     }

     pClassInfo->all_referred_classes = JvmMemCalloc( sizeof(char *) * iter );
     pClassInfo->referred_classes_count = iter;
     
     for ( jwIndex = 0; jwIndex < iter ; jwIndex ++ )
     {
          pClassInfo->all_referred_classes[jwIndex] = 
            JvmLdrGetStringName(pClass, pjwClassCount[jwIndex], NULL);
     }

     JvmMemFree(pjwClassCount);

     return 1;
}

unsigned long JvmDGetFieldAttributes( _JVM_DField_Info * pInfo)
{
  JVM_ASSERT( pInfo != NULL);
  JVM_ASSERT( pInfo->pjfField != NULL);

  return ((_JVM_Field *)pInfo->pjfField)->jwFieldAccessFlags;
}

unsigned long JvmDGetMethodAttributes( _JVM_DMethod_Info * pInfo)
{
  JVM_ASSERT( pInfo != NULL);
  JVM_ASSERT( pInfo->pjmMethod != NULL);

  return ((_JVM_Method *)pInfo->pjmMethod)->jwMethodAccessFlags;
}

unsigned long JvmDGetClassAttributes( _JVM_DClass_Info * pInfo )
{
  JVM_ASSERT( pInfo != NULL);
  return ((_JVM_Class_Class *)pInfo->pjccClass)->jdwClassFlags;
}


int JvmDGetContainedClasses(  _JVM_DClass_Info * pClassInfo )
{
    _JVM_DField_Info * fields = pClassInfo->fields;
     int index;
     int iter = 0;
     if (pClassInfo->contained_classes_count)  
     {
       pClassInfo->contained_classes = JvmMemCalloc( sizeof(char *) * pClassInfo->contained_classes_count );         
       for ( index = 0; index < pClassInfo->fields_count; index++)
       {
         if ( fields[index].is_a_class )
             pClassInfo->contained_classes[iter++] = fields[index].field_type_name;
       } 
     } 
    return 1;
}




void JvmDFreeData( void * data )
{
  if ( data ) JvmMemFree( data );
}

/*
 *  Free class management structure
 */
int JvmFreeDependsClass( _JVM_DClass_Info * pClass )
{
  /* Will be freed else where  */
	return 1;
}

/*
 *  Exit
 */
void JvmDependsExit() 
{
   JvmSysKillMutex( &_jmoGDClassDictMutex );
   /*TODO  destroy hash table too */
   DestroyDictionary( _pdictGDClass );
   JvmExit();
}

#endif






static JVM_PBYTE _pjbyGJVMOpcodes = JVM_NULL; /** 0x00 to 0xc9 **/
static JVM_PBYTE _pjbyGJVMQuickOpcodes = JVM_NULL; /** 0xcb to 0xe4 ( barring 0xdc )**/ 

#ifndef  __JVM_CHECKTOOL_TEST_ENV__

extern _JVM_Mutex_Object  _jmoGCLdrDictMutex;
extern _JVM_Mutex_Object  _jmoGAttrVtblMutex;
extern _JVM_Mutex_Object  _jmoGCLdrStateMutex;
extern _JVM_Mutex_Object  _jmoGClassStateMutex;

extern  Dictionary   *    _pdictGCLdrClassState;
extern  Dictionary   *    _pdictGCLdrClass;

extern _JVM_Object        _jaoGPrimitiveLoader;

#endif /** !__JVM_CHECKTOOL_TEST_ENV__  **/


/** Reserved opcodes are 
0xca - breakpoint
0xfe - impdep1
0xff - impdep2
**/

typedef struct _JVM_Attrib_Info{
   char * ccAttribString;
   JVM_PFN_ATTRIB_HANDLER pfnAttribHandler;
}_JVM_Attrib_Info;


_JVM_Attrib_Info _ajaiAttrInfoTab[]  = {
		  {  NULL,                JvmHdlAttrDefault         },
      { "Code",               JvmHdlAttrCode            }, 
		  { "ConstantValue",      JvmHdlAttrConstantValue   }, 
		  { "Exceptions",         JvmHdlAttrExceptions      },
		  { "LineNumberTable",    JvmHdlAttrLineNumberTable },
		  { "LocalVariableTable", JvmHdlAttrLocalVarTable   },
      { "LocalVariables", JvmHdlAttrLocalVarTable   },
		  { "SourceFile",         JvmHdlAttrSourceFile      },
      { "InnerClasses",       JvmHdlAttrInnerClasses    },
      { "Synthetic",          JvmHdlAttrSynthetic       },
      { "Deprecated",         JvmHdlAttrDeprecated      }
 };



/** NON-Reentrant **/
JVM_BOOL JvmLdrInit(G_STATE_P state )
{
    if ( IS_EXCL_READY( state )   && IS_SPGL_READY( state )) 
	{
#if defined(__EXPAND_CODE__) && defined(__LOG__) && defined(__DEBUG__)
		stream_init(1);
#endif
		LDRL_READY( state );
        return JVM_TRUE;
	}

	return JVM_FALSE;
}

/** Nothing to do **/
JVM_BOOL JvmLdrExit()
{
	return JVM_TRUE;
}


JVM_BOOL JvmLdrLoadSystemClasses(
 IN     JVM_OBJREF  joClassLoader
)
{
   /*
    *  java/lang 
	*/
   /** Root **/
   JVM_LOAD_CLASS( S_JAVA_LANG_OBJECT        );
   /** Exceptions **/
   JVM_LOAD_CLASS( S_JAVA_LANG_THROWABLE     );
   JVM_LOAD_CLASS( S_JAVA_LANG_STRING        );
   JVM_LOAD_CLASS( S_JAVA_LANG_EXCEPTION     );
   JVM_LOAD_CLASS( S_JAVA_LANG_SYSTEM        );
   JVM_LOAD_CLASS( S_JAVA_LANG_INTERNALERROR );
   JVM_LOAD_CLASS( S_JAVA_LANG_CLASSNOTFOUNDEXCEPTION );
   JVM_LOAD_CLASS( S_JAVA_LANG_CLASSCASTEXCEPTION    );
   JVM_LOAD_CLASS( S_JAVA_LANG_CLASSCIRCULARITYERROR );
   JVM_LOAD_CLASS( S_JAVA_LANG_CLASSFORMATERROR      );
   JVM_LOAD_CLASS( S_JAVA_LANG_ABSTRACTMETHODERROR   );
   JVM_LOAD_CLASS( S_JAVA_LANG_ARITHMETICEXCEPTION   );
   JVM_LOAD_CLASS( S_JAVA_LANG_ARRAYSTOREEXCEPTION   );
   JVM_LOAD_CLASS( S_JAVA_LANG_ILLEGALACCESSERROR    );
   JVM_LOAD_CLASS( S_JAVA_LANG_INSTANTIATIONERROR    );
   JVM_LOAD_CLASS( S_JAVA_LANG_INTERRUPTEDEXCEPTION  );
   JVM_LOAD_CLASS( S_JAVA_LANG_LINKAGEERROR          );
   JVM_LOAD_CLASS( S_JAVA_LANG_ILLEGALACCESSEXCEPTION);
   JVM_LOAD_CLASS( S_JAVA_LANG_ILLEGALARGUMENTEXCEPTION     );
   JVM_LOAD_CLASS( S_JAVA_LANG_ILLEGALMONITORSTATEEXCEPTION );
   JVM_LOAD_CLASS( S_JAVA_LANG_ILLEGALTHREADSTATEEXCEPTION  );
   JVM_LOAD_CLASS( S_JAVA_LANG_INCOMPATIBLECLASSCHANGEERROR );
   JVM_LOAD_CLASS( S_JAVA_LANG_INDEXOUTOFBOUNDSEXCEPTION );
   JVM_LOAD_CLASS( S_JAVA_LANG_ARRAYINDEXOUTOFBOUNDSEXCEPTION );


   /** Primary **/

   JVM_LOAD_CLASS( S_JAVA_LANG_THREADGROUP );
   JVM_LOAD_CLASS( S_JAVA_LANG_CLASSLOADER );
   JVM_LOAD_CLASS( S_JAVA_LANG_BOOLEAN     );
   JVM_LOAD_CLASS( S_JAVA_LANG_CHARACTER   );
   JVM_LOAD_CLASS( S_JAVA_LANG_FLOAT       );
   JVM_LOAD_CLASS( S_JAVA_LANG_INTEGER     );
   JVM_LOAD_CLASS( S_JAVA_LANG_LONG        );
   JVM_LOAD_CLASS( S_JAVA_LANG_MATH        );
   JVM_LOAD_CLASS( S_JAVA_LANG_CLONEABLE   );
   JVM_LOAD_CLASS( S_JAVA_LANG_COMPILER    );
   JVM_LOAD_CLASS( S_JAVA_LANG_CLONENOTSUPPORTED );


   return JVM_TRUE;
  
}




 /** CHECK,CHECK check all the clean up routines  and exit points **/
 /** PENDING Have to free jsSuperClassName and other temp strings **/
 /********************************************************************
 / Function:	JvmLdrLoadNewClass
 / Purpose:		Loads a new class from a bytestream or class file and
 /              creates a new class object representing the class
 /              OR returns an existing class object for the class
 /
 / Input:		ClassFileName or Stream bytes and Class Loader
 /
 / Output:		Reference to 'Class' Object represtenting the input class.
 /
 / Return:      TRUE , if successful
 /              FALSE, if not
 /
 / Exceptions:	EXC_JVMFILEIOERROR,EXC_CLASSNOTPRESENT,
 /				EXC_JAVACLASSFORMATERROR,EXC_JVMVERSIONNOTSUPPORTED,
 /				EXC_JVMNOMEMORY  ( Deferred )
 / Modifications
 / History:	    10/09/98 - A.G :
 /              Added pjoClassObject input parameter and relevent code.
 /									
 /                                  
 /   
 / Comments:
 /
 **********************************************************************/

JVM_BOOL JvmLdrLoadNewClass(
  IN     JVM_STRING  jsClassFileName,
  IN     JVM_PBYTE   pjbyClassBytes,
  IN     JVM_OBJREF  joClassLoader, 
  OUT    JVM_OBJREF  * pjoClassObject
)
{
  JVM_FILE  jfiFile                  = JVM_NULL;
  JVM_PBYTE pjbyClassStream          = JVM_NULL;
  JVM_PBYTE pjbySavedClassStream          = JVM_NULL;
  JVM_PBYTE pjbyFiller               = JVM_NULL;
  JVM_BYTE  jbyTag                   = 0;      
  _JVM_Constant_Pool ** ppjcpThePool = JVM_NULL;
  _JVM_Class_Class   *  pjccTheClass = JVM_NULL; 
  JVM_WORD  jwSaveCPCount            = 0;
  JVM_WORD  jwIntfCount              = 0;
  JVM_WORD  jwIterator               = 0;
  JVM_WORD  jwSuperClassNameLen      = 0;
  JVM_WORD  jwInterfaceNameLen       = 0;
  JVM_WORD  jwCount                  = 0;
  JVM_DWORD jdwStatus                = JVM_GOOD;
  JVM_WORD  jwCPSize 				 = 0;
  JVM_STRING jsSuperClassName        = JVM_NULL;
  JVM_STRING jsInterfaceName         = JVM_NULL;
  JVM_OBJREF joClassObject           = JVM_NULL;
  JVM_U16   * pju16InterfacesCPIndex = JVM_NULL;
  JVM_BOOL    jbIsObject             = FALSE;
  JVM_INT     jiRet                  = CONFIG_REGULAR_CLASS;
  _JVM_Sys_Jar_Info * info  = JVM_NULL;
  JVM_STRING  jsFullFileName;
  

  JVM_PFN_CP_TYPES ajpctCPFnTbl[JVM_MAX_CP_TYPES]  =
  {
	  JvmCPTypeConstUtf8,
	  JVM_NULL,
	  JvmCPTypeConstInteger,
	  JvmCPTypeConstFloat,
	  JvmCPTypeConstLong,
	  JvmCPTypeConstDouble,
	  JvmCPTypeConstClass,
	  JvmCPTypeConstString,
	  JvmCPTypeConstFieldRef,
	  JvmCPTypeConstMethodRef,
	  JvmCPTypeConstInterfaceMethodRef,
	  JvmCPTypeConstNameAndType
  };

  DECLARE_LOG_DATA("c:\\jvm\\out\\class.log")

  if ( JVM_VALID(jsClassFileName) )
  {
	  /** Check if the class is already loaded , if so return **/
    if ( JvmLdrGetClassObject( jsClassFileName, joClassLoader, &joClassObject ) )
	{
		if ( JVM_VALID(pjoClassObject) )
		   *pjoClassObject = joClassObject;

		return JVM_TRUE;
	}
    	  

   JVM_TRY
   {
	  jiRet = JvmConfigLocateClass( jsClassFileName, &jsFullFileName );
	  LOG_WRITE( "\n\nReading %s...\n", jsClassFileName )
	  if ( jiRet ==  CONFIG_REGULAR_CLASS ) {
        jfiFile         = JvmOpenClassFile( jsFullFileName );
        pjbySavedClassStream = pjbyClassStream = JvmReadClassFile( jfiFile );
	  } else if ( jiRet == CONFIG_JARRED_CLASS) {
      info  = JvmSysExtractFromJarFile( jsFullFileName, jsClassFileName ); 
      if ( !info ) {
        JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
        return FALSE;
      }
       pjbyClassStream = info->pjbyBytes;
    } else {
        JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
        return FALSE;
    }
   }
   JVM_CATCH_TWO_EXCEPTIONS( EXC_JVMFILEIOERROR, EXC_CLASSNOTPRESENT )
   {
 	  JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
	  return FALSE;
   }
   JVM_END_TRY

  }
  else
  {
	  pjbyClassStream = pjbyClassBytes;
  }

  /** Add a new Class Loader  **/
  JvmLdrAddNewClassLoader( joClassLoader );

  if ( !JvmLdrCheckMagic( &pjbyClassStream ))
  {
    JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
    if ( JVM_VALID(jfiFile ) )
      	  JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
    if ( info ) JvmSysFreeJarInfo( info );
	  return JVM_FALSE;
  }

  if ( !JvmLdrCheckVersion( &pjbyClassStream ) )
  {
 	  JvmDeferException( jsClassFileName, joClassLoader, EXC_JVMVERSIONNOTSUPPORTED );
    if ( JVM_VALID(jfiFile ) )
     	  JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
    if ( info ) JvmSysFreeJarInfo( info );
	  return JVM_FALSE;
  }

  jwSaveCPCount =  LD_WORD( pjbyClassStream );

  MV_WORD( pjbyClassStream );

  if ( !jwSaveCPCount  )
  {
	  JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
    if ( JVM_VALID(jfiFile ) )
    	  JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
    if ( info ) JvmSysFreeJarInfo( info );
	  return JVM_FALSE;
  }

  /** Add CP[0] as dummy with no data.This is for easier CP reference for other indexes **/
  JvmAddDummyCPEntry( &ppjcpThePool, &jwCPSize );
  

  LOG_WRITE("CPCount:0x%x", jwSaveCPCount); 

  for ( ; jwCPSize < jwSaveCPCount; )
  {
     jbyTag = *pjbyClassStream;

     if ( jbyTag > JVM_MAX_CP_TYPES + 1 || jbyTag == 2 || jbyTag <= 0)
	 {
      JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
      JvmFreeThePool( ppjcpThePool, jwCPSize );
      if ( JVM_VALID(jfiFile ) )
   	    JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
      if ( info ) JvmSysFreeJarInfo( info );
	    return JVM_FALSE;
	 }


	 (*(ajpctCPFnTbl[ jbyTag - 1]))(&pjbyClassStream,
        	  	                    &ppjcpThePool,
	          				        &jwCPSize,
						            &jdwStatus 
					               );

	 if ( jdwStatus != JVM_OK )
	 {
	   JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
	   JvmFreeThePool( ppjcpThePool, jwCPSize );
     if ( JVM_VALID(jfiFile ) )
      	  JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
     if ( info ) JvmSysFreeJarInfo( info );
	   return JVM_FALSE;
	 }
  }

  JVM_ASSERT_VALID( ppjcpThePool );

  /** Gets a new _JVM_Class_Class **/
  pjccTheClass  = JvmGetNewClass();
  /** Set the class loader object value **/
  pjccTheClass->joClassLoaderObject = joClassLoader;

  /** constant pool **/
  pjccTheClass->ppjcpConstantPool = ppjcpThePool;
  pjccTheClass->jwCPEntryCount    = jwSaveCPCount;
  /* Access Flags */
  pjccTheClass->jdwClassFlags    |= ( LD_WORD( pjbyClassStream ) ); 

  pjccTheClass->jdwClassFlags |= ACC_SUPER;

  LOG_WRITE( "ClassFlags: 0x%08x", pjccTheClass->jdwClassFlags )

  MV_WORD( pjbyClassStream );

  pjccTheClass->jsClassName = JvmGetClassName( pjccTheClass,
	  LD_WORD( pjbyClassStream ), &pjccTheClass->jwClassNameLen  );

  LOG_WRITE( "ClassName: %s\r\n", pjccTheClass->jsClassName )

   MV_WORD( pjbyClassStream );

  /** Just save the super class name now. We do not load the super classes recursively
      here because for class loader based classes the class loader itself must have 
      been loaded first ,otherwise loading might fail.
	  e.g consider calling loadClass method when loadClass has not been read in yet!!
	  So we first load the methods and fields of the class .
  **/
  jwCount = LD_WORD( pjbyClassStream );
  MV_WORD( pjbyClassStream );

  if ( jwCount != 0x0000 )
  {
      jsSuperClassName =   JvmGetClassName( pjccTheClass,
		  jwCount, &jwSuperClassNameLen );
	  if ( !JVM_VALID( jsSuperClassName ) )
	  {
    	  JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
	      JvmFreeTheClass( pjccTheClass );
        if ( JVM_VALID(jfiFile ) )
      	  JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
        if ( info ) JvmSysFreeJarInfo( info );
        return JVM_FALSE;
	  }
  }

  LOG_WRITE( "SuperClass: %s", jsSuperClassName )

  pjccTheClass->jwIntfCount = jwIntfCount = LD_WORD( pjbyClassStream );

  MV_WORD( pjbyClassStream );

  /** Load the interfaces later , but just note down their CP index **/
  if ( jwIntfCount != 0x0000 )
  {
	  pju16InterfacesCPIndex = JvmMemCalloc( jwIntfCount * sizeof( JVM_U16 ));
	  JVM_ALLOC_CHECK( pju16InterfacesCPIndex )

	  for ( jwIterator = 0; jwIterator < jwIntfCount;jwIterator ++ )
	  {
		 pju16InterfacesCPIndex[ jwIterator ] =  LD_WORD( pjbyClassStream );

		 MV_WORD( pjbyClassStream );
	  }

    pjccTheClass->pju16InterfacesCPIndex = pju16InterfacesCPIndex;
  }

  LOG_WRITE( "InterfacesCount: %d", jwIntfCount )

  /** TODO More work on the class file - FieldInfo,MethodInfo,AttribInfo **/

  jwCount = LD_WORD( pjbyClassStream );
  MV_WORD( pjbyClassStream );

  if ( jwCount != 0x0000 )
  {
     if ( !JvmLdrReadFields( &pjbyClassStream, pjccTheClass, jwCount ))
	 {
		 JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
		 JvmFreeTheClass( pjccTheClass );
     if ( JVM_VALID(jfiFile ) )
    	  JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
     if ( info ) JvmSysFreeJarInfo( info );
 		 return JVM_FALSE;
	 } 
  }

  LOG_WRITE( "FieldsCount: %d", jwCount )


  jwCount = LD_WORD( pjbyClassStream );
  MV_WORD( pjbyClassStream );

  if ( jwCount != 0x0000 )
  {
	 if ( !JvmLdrReadMethods( &pjbyClassStream, pjccTheClass, jwCount ))
	 {
		 JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
		 JvmFreeTheClass( pjccTheClass );
     if ( JVM_VALID(jfiFile ) )
       JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
     if ( info ) JvmSysFreeJarInfo( info );
  	 return JVM_FALSE;
	 } 
  }

  LOG_WRITE( "MethodsCount: %d", jwCount )

  jwCount = LD_WORD( pjbyClassStream );
  MV_WORD( pjbyClassStream );

  if ( jwCount != 0x0000 )
  {
     if ( !JvmGenericAttribHandler(
           &pjbyClassStream,
		   JVM_NULL,
		   pjccTheClass,
		   jwCount,
		   ATTRIB_HOST_CLASS
		 )
	 )
	 {
		 JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
		 JvmFreeTheClass( pjccTheClass );
     if ( JVM_VALID(jfiFile ) )
       JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
     if ( info ) JvmSysFreeJarInfo( info );
  	 return JVM_FALSE;
	 } 
  }

  /** Storage for any native library references **/
  pjccTheClass->jfNativeLibrary = JVM_NULL;


  /** We no longer need the streams **/
  if ( JVM_VALID(jfiFile ) )
  {
	  JvmCloseClassFile( jfiFile, &pjbySavedClassStream );
  }

  if ( info ) JvmSysFreeJarInfo( info );

  /** Try to Load the super classes recursively **/
  if ( JVM_VALID(jsSuperClassName) )
  {
    JvmLoadClass( jsSuperClassName, joClassLoader );
	  JvmLdrGetClassObject( jsSuperClassName, joClassLoader,
		  &pjccTheClass->joSuperClassObject );
	 /* {
		  JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
		  JvmFreeTheClass( pjccTheClass );
		  return JVM_FALSE;
	  }*/

	  pjccTheClass->jsSuperClassName = jsSuperClassName;
  }

  joClassObject = JvmRtCreateNewInstanceEx( pjccTheClass );
  /** once loaded need to add the class name and joClassObject
      to the _pdictGCLdrClass hash table map **/
  JvmLdrAddNewClass( joClassLoader, joClassObject, pjccTheClass->jsClassName );

  if ((jwIntfCount > 0)  &&  pju16InterfacesCPIndex)
  {
	  pjccTheClass->ppjoInterfaces = (_JVM_Object ** )
		          JvmMemCalloc( sizeof( _JVM_Object * ) * jwIntfCount );
	  JVM_ALLOC_CHECK( pjccTheClass->ppjoInterfaces )
	  for ( jwIterator = 0; jwIterator < jwIntfCount;jwIterator ++ )
	  {
	    jsInterfaceName = JvmGetClassName(pjccTheClass,
			pju16InterfacesCPIndex[ jwIterator ] , &jwInterfaceNameLen );

	    JvmLoadClass( jsInterfaceName, joClassLoader );
	 		JvmLdrGetClassObject( jsInterfaceName, joClassLoader,
	   		&pjccTheClass->ppjoInterfaces[ jwIterator ]);
	 /*	{
		  JvmDeferException( jsClassFileName, joClassLoader, EXC_JAVACLASSFORMATERROR );
		  JvmFreeTheClass( pjccTheClass );
		  return JVM_FALSE;
		}
    */
	  }
  }

  /** Update java.lang.Object and java.lang.Class objects  **/
  jbIsObject = (JvmSpringStrCmp( pjccTheClass->jsClassName, S_JAVA_LANG_OBJECT ) == 0);

  if (!jbIsObject)
  {
	  pjccTheClass->joObjectClassObject = JvmLoadClass( S_JAVA_LANG_OBJECT, joClassLoader );
  	if ( JvmSpringStrCmp( pjccTheClass->jsClassName, S_JAVA_LANG_CLASS ) != 0 )
	  {
      pjccTheClass->joClassClassObject  = JvmLoadClass( S_JAVA_LANG_CLASS,  joClassLoader );
	  }
  }
  

  if ( jbIsObject )
  {
	 pjccTheClass->joClassClassObject  = JvmLoadClass( S_JAVA_LANG_CLASS,  joClassLoader );
  }


  if ( JVM_VALID(pjoClassObject) )
  {
	  *pjoClassObject = joClassObject;
	  ((_JVM_Object *)joClassObject)->jiState = JAVA_OBJECT_STATE_LOADED;
  }

  return JVM_TRUE;
  
}


JVM_VOID JvmAddDummyCPEntry(
  IN  OUT _JVM_Constant_Pool *** pppjcpThePool,
  IN  OUT  JVM_WORD     *  pjwCurCPSize
)
{
  JVM_PBYTE pjbyFiller =   (JVM_PBYTE) JvmMemCalloc( sizeof( _JVM_Constant_Pool ) + WORDLEN + 1 );
  JVM_ALLOC_CHECK( pjbyFiller );

  CP_TAG( pjbyFiller ) = JVM_CP_TAG_INVALID;
  CP_LEN( pjbyFiller ) = 0;
  *((JVM_WORD *)(pjbyFiller + sizeof( _JVM_Constant_Pool ) )) = 0;

  *pppjcpThePool =  JvmLdrAddNewCPInfoToCPVector( *pppjcpThePool, pjwCurCPSize, pjbyFiller );
}


JVM_BOOL JvmLdrAddNewClassLoader( 
  IN     JVM_OBJREF joClassLoader 
)
{
   Hash_Table * phtClass;
   Hash_Type htkey,htval,htinvalid;

   JvmSysAcquireMutex( _jmoGCLdrDictMutex );
   htkey.u.pv = joClassLoader;
   htval = 	 GetFromDictionary( _pdictGCLdrClass, htkey );
   phtClass = ( Hash_Table * ) htval.u.pv;

   if ( phtClass == JVM_NULL )
   {
	   htinvalid.u.pv = NULL;
#ifdef __DEBUG__
	   if ( !CreateHashTable(
		   "PerClassHashTbl",
		   &phtClass,
		   htinvalid,
		   HASH_TYPE_PTR,
		   HASH_TYPE_PTR,
		   JVM_NULL ) 
 	   )
#else
      if ( !CreateHashTable(
		   &phtClass,
		   htinvalid,
		   HASH_TYPE_PTR,
		   HASH_TYPE_PTR,
		   JVM_NULL ) 
 	   )
#endif
		  return JVM_FALSE;

	  htkey.u.pv = joClassLoader;
	  htval.u.pv = phtClass;
      AddToDictionary( _pdictGCLdrClass, htkey, htval );
   }
  
   JvmSysReleaseMutex( _jmoGCLdrDictMutex );

   return  JVM_TRUE;
	 
}

JVM_BOOL JvmLdrAddNewClass( 
  IN     JVM_OBJREF joClassLoaderObject ,
  IN     JVM_OBJREF joClassObject, 
  IN     JVM_STRING jsClassName 
)
{
  Hash_Table * phtClass;
  Hash_Type htkey;
  Hash_Type htval;

  JvmSysAcquireMutex( _jmoGCLdrDictMutex );

  htkey.u.pv = 	joClassLoaderObject;

  htval = GetFromDictionary( _pdictGCLdrClass, htkey );
  phtClass = ( Hash_Table * )htval.u.pv;

  if ( phtClass == JVM_NULL )
	  return JVM_FALSE ;

  htkey.u.pv = JVM_SPRING_TO_CSTR( jsClassName );
  htval.u.pv = joClassObject;
  
  AddToHashTable( phtClass, htkey, htval );

  JvmSysReleaseMutex( _jmoGCLdrDictMutex );

  return JVM_TRUE;
}

JVM_BOOL JvmLdrGetClassObject(
  IN  JVM_STRING jsClassName, 
  IN  JVM_OBJREF joClassLoader,
  OUT JVM_OBJREF * pjoObjRef
  )
{
   Hash_Table * phtClass;
   Hash_Type htkey;
   Hash_Type htval;
   
   JvmSysAcquireMutex( _jmoGCLdrDictMutex );

   htkey.u.pv = joClassLoader;
   htval = GetFromDictionary( _pdictGCLdrClass, htkey );
   phtClass = ( Hash_Table * )htval.u.pv;

   if ( phtClass  == JVM_NULL )
	   return JVM_FALSE;

   htkey.u.pv = JVM_SPRING_TO_CSTR( jsClassName ); 
   htval =    GetFromHashTable( phtClass, htkey);

   *pjoObjRef = (JVM_OBJREF)htval.u.pv;
   JvmSysReleaseMutex( _jmoGCLdrDictMutex );

   return *pjoObjRef != JVM_NULL ; 
	   
}

/**
typedef struct _JVM_Field_Info {
   JVM_WORD   jwFieldType;
   JVM_CHAR   jcBaseType;
   JVM_INT    jiWidthInWords;
   JVM_WORD   jwArrayDimension;
   JVM_STRING jsObjectName;
   JVM_INT    jiObjectNameLen;
   JVM_DWORD  jdwStatus;
}_JVM_Field_Info;
 **/


JVM_BOOL JvmLdrMakeNewArrayClass( 
 IN      JVM_OBJREF joClassLoader,
 IN      JVM_STRING jsArrayName, 
 OUT     JVM_OBJREF * pjoClassObjectRef
) 
{
  _JVM_Class_Class * pjccClass = JVM_NULL;
  _JVM_Field_Info jfiFieldInfo;
   JVM_OBJREF     joClassObject = JVM_NULL;
   JVM_PBYTE pjbyArrayName;
   JVM_PBYTE pjbyStart;

   /** Array Names are always ASCII **/
   pjbyStart = pjbyArrayName = JvmMemCalloc( JvmStrLen( jsArrayName ) + 1 );
   JVM_ALLOC_CHECK( pjbyArrayName );
   JvmStrCpy( pjbyArrayName, jsArrayName );

   if ( JvmLdrGetClassObject( jsArrayName, joClassLoader, &joClassObject ) )
   {
      if ( JVM_VALID( pjoClassObjectRef ) )
	  {
		  *pjoClassObjectRef = joClassObject;
		  return JVM_TRUE;
	  }
   }

   JvmMemSet( &jfiFieldInfo, 0 ,sizeof( _JVM_Field_Info ));

   if ( !JvmParseFieldDescriptor(&pjbyArrayName, &jfiFieldInfo ) )
         goto __error_exit;

   JVM_ASSERT( jfiFieldInfo.jwFieldType & JVM_ARRAY_TYPE );

   pjccClass = JvmGetNewClass();

   pjccClass->jdwClassFlags       = ACC_FINAL|ACC_PUBLIC;
   pjccClass->jsClassName         = JVM_SPRING_CREATE( jsArrayName );
   JVM_ALLOC_CHECK( pjccClass->jsClassName )

   JvmStrCpy( pjccClass->jsClassName, jsArrayName );

   pjccClass->joClassLoaderObject = joClassLoader;
   if ( !JvmLdrLoadNewClass( S_JAVA_LANG_OBJECT,JVM_NULL,joClassLoader, 
	      &pjccClass->joSuperClassObject ))
   	   goto __error_exit;
   if ( !JvmLdrLoadNewClass( S_JAVA_LANG_CLASS, JVM_NULL,joClassLoader,
	      &pjccClass->joClassClassObject ))
	   goto __error_exit;

   pjccClass->joObjectClassObject = pjccClass->joSuperClassObject;
   pjccClass->ppjoInterfaces      = JVM_NULL;  
   pjccClass->pjfiArrayInfo       = JvmMemCalloc(sizeof(_JVM_Field_Info));
   JVM_ALLOC_CHECK( pjccClass->pjfiArrayInfo )

   JvmMemCpy( pjccClass->pjfiArrayInfo, &jfiFieldInfo, sizeof( _JVM_Field_Info ));
   
   /** Resolve the class if object type **/
   if ( jfiFieldInfo.jwFieldType & JVM_OBJECT_TYPE )
   {
      if ( !JvmLdrLoadNewClass( jfiFieldInfo.jsObjectName, JVM_NULL,
                  joClassLoader, &pjccClass->joArrayBaseTypeObject ))
               goto __error_exit;
   }

   joClassObject = JvmRtCreateNewInstanceEx( pjccClass );

   JvmLdrAddNewClass( joClassLoader, joClassObject, pjccClass->jsClassName );

   *pjoClassObjectRef = joClassObject;

   return JVM_TRUE;

__error_exit:

   if ( pjbyStart )
     JvmMemFree( pjbyStart );
   if ( pjccClass )
     JvmFreeTheClass( pjccClass );

   return JVM_FALSE;
}



JVM_OBJREF JvmLoadClass(
  IN  JVM_STRING jsClassName,
  IN  JVM_OBJREF joClassLoader 
)
{
   JVM_PBYTE pjbyClassBytes = JVM_NULL;
   JVM_OBJREF joClassObject = JVM_NULL, 
	          joObjRef;
   register Hash_Table * phtClass    = JVM_NULL;
   Hash_Type  htkey,htval;
  

   if ( joClassLoader != PRIMITIVE_LOADER )
   {
	  JVM_TRY
	  {
		InvokeLoadClass(&joClassObject, joClassLoader, jsClassName, JAVA_FALSE );
	  }
	  JVM_CATCH_ALL_EXCEPTIONS()
      {
		 JVM_THROW_EXCEPTION( JVM_GET_EXCEPTION_TYPE());
		 JVM_ASSERT( JVM_FALSE );
	  }
	  JVM_END_TRY

 	 /** Try getting the class object from hash table 
	    if unsuccessful , add the joClassObject. This is because
	    JvmLdrLoadNewClass could have been called during InvokeLoadClass
	    that would have already added the class for the class loader
	    in question.
	  **/

	  JvmSysAcquireMutex( _jmoGCLdrDictMutex );
	  htkey.u.pv = joClassLoader;
	  htval = GetFromDictionary( _pdictGCLdrClass, htkey );
	  phtClass = ( Hash_Table * )htval.u.pv;
	
	  JVM_ASSERT_VALID( phtClass );
	  htkey.u.pv =  jsClassName;
	  htval = GetFromHashTable( phtClass, htkey);
      joObjRef = (JVM_OBJREF)htval.u.pv;

	  if (joObjRef == JVM_NULL) 
	  {
		 htkey.u.pv = JVM_SPRING_TO_CSTR( jsClassName );
		 htval.u.pv = joClassObject;
	     AddToHashTable( phtClass, htkey, htval );
	  }

	  JvmSysReleaseMutex( _jmoGCLdrDictMutex );
  }
  else
  {
	 if (!JvmLdrLoadNewClass( jsClassName, JVM_NULL, joClassLoader, JVM_NULL )) {
       JvmDeferException( jsClassName, joClassLoader, EXC_JAVACLASSFORMATERROR );
       return JVM_NULL;
   }
  }

  JvmLdrGetClassObject( jsClassName, joClassLoader,&joClassObject );

  return joClassObject;
}


/** CHECK,CHECK - How are the stored strings going to be destroyed ??? **/

JVM_STRING JvmLdrGetStringName( 
 IN    _JVM_Class_Class *pjccTheClass,
 IN     JVM_WORD  jwCPIndex,
 OUT  JVM_WORD * pjwNameLen
 )
{
   JVM_PBYTE pjbyString,pjbyUtf8;
   JVM_WORD  jwNameIndex = 0xffff;
   JVM_STRING jsName = JVM_NULL;
   Hash_Type  htkey,htval;

   JVM_ASSERT_VALID( pjccTheClass );
   JVM_ASSERT_VALID( pjccTheClass->ppjcpConstantPool );
   JVM_ASSERT_VALID( pjccTheClass->pdNCPDict );

   pjbyString = pjbyUtf8 = JVM_NULL;

   if (  jwCPIndex < pjccTheClass->jwCPEntryCount )
   {
	   htkey.u.ul = jwCPIndex;
	   htval = GetFromDictionary( pjccTheClass->pdNCPDict, htkey );
	   jsName = (JVM_STRING)htval.u.pv;

	   if ( jsName != JVM_NULL )
     {
		   if ( JVM_VALID(pjwNameLen ) )
		     *pjwNameLen = (JVM_WORD)JvmSpringGetLen( jsName );
     }
	   else
     {
	     pjbyString = (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ jwCPIndex ];
       switch( CP_TAG( pjbyString ) ) {
       case JVM_CP_TAG_STRING :
       case JVM_CP_TAG_CLASS :   
                  jwNameIndex =  LD_WORD ( pjbyString + sizeof( _JVM_Constant_Pool ) );
                  // Dummy for restoring changed bytes
                  LD_WORD ( pjbyString + sizeof( _JVM_Constant_Pool ) ); 
                  if ( jwNameIndex >= pjccTheClass->jwCPEntryCount )
                       return jsName;
                  break;
       case JVM_CP_TAG_UTF8 :
                 jwNameIndex = jwCPIndex; break;
       default : return jsName;
       }
       pjbyUtf8 =  (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ jwNameIndex ];
       if ( JvmLdrGetUtf8Bytes( pjbyUtf8, &jsName, pjwNameLen ) )
       {
    		  htkey.u.ul = jwCPIndex;
			    htval.u.pv = jsName;
			    AddToDictionary( pjccTheClass->pdNCPDict, htkey, htval );
       }
     }
   }
   return jsName;
}




/** Assumes that CP is filled up **/
/** Uses the passed in CP index to extract a class name from the Constant Pool **/
/** Assumes that the CP index is good i.e pointing to an entry which has class name
    in it . If the class name has already been resolved i.e 'extracted' it is
	stored in a Dictionary that returns the name rather than going through expensive
	'extract' operation again. 
 **/
/** CHECK,CHECK consider size optimizing by writing a 
    single function for giving the utf8 info for a field/class/constant string etc.
	CHECK,CHECK cleanup a little bit /check for stupidities and logic
	**/

JVM_STRING JvmGetClassName(
  IN  _JVM_Class_Class * pjccTheClass, 
  IN   JVM_WORD jwCPIndex,
  OUT  JVM_WORD * pjwClassNameLen
)
{
   return JvmLdrGetStringName( pjccTheClass, jwCPIndex, pjwClassNameLen );
}
						     



/** CHECK,CHECK the contents of this structure **/

/** PENDING		**/
/** CHECK,CHECK check again **/
JVM_BOOL  JvmLdrReadMethods( 
  IN OUT  JVM_PBYTE * ppjbClassStream,
  IN OUT _JVM_Class_Class * pjccTheClass,
  IN      JVM_WORD jwMethodsCount
)
{
	JVM_WORD   jwCount      = 0;
	JVM_WORD   jwDescLen    = 0;
	JVM_WORD   jwAttributesCount = 0;
	JVM_PBYTE  pjbyMethodDesc = JVM_NULL;
	JVM_STRING jsMethodDesc = JVM_NULL;
   _JVM_Method * pjmMethod = JVM_NULL;
    JVM_PBYTE pjbyUtf8 = JVM_NULL;

	DECLARE_LOG_DATA("c:\\jvm\\out\\class.log")

	JVM_ASSERT_VALID( ppjbClassStream );
    JVM_ASSERT_VALID( pjccTheClass );
	JVM_ASSERT( jwMethodsCount != 0 );


	pjccTheClass->ppjmMethods = ( _JVM_Method ** ) JvmMemCalloc(
	              sizeof( _JVM_Method *) *  jwMethodsCount );
	JVM_ALLOC_CHECK( pjccTheClass->ppjmMethods )

    LOG_WRITE("%s...","Methods")

	do
	{
	  pjmMethod = pjccTheClass->ppjmMethods[ jwCount ]
           = JvmMemCalloc( sizeof( _JVM_Method ) );
	  JVM_ALLOC_CHECK( pjmMethod )
	 /** Access Flags **/
	 pjmMethod->jwMethodAccessFlags = LD_WORD( *ppjbClassStream );
	 MV_WORD( *ppjbClassStream );

	 /** Method Name CP Index **/
	 pjmMethod->jwMethodCPIndex  = 	LD_WORD( *ppjbClassStream );
	 MV_WORD( *ppjbClassStream );

     JvmLdrGetUtf8Bytes( (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ pjmMethod->jwMethodCPIndex ],
		  &pjmMethod->jsMethodName, JVM_NULL );
 
	 JVM_ASSERT_VALID( pjccTheClass->ppjcpConstantPool );

	 /** Method Descriptor index **/
	 pjbyUtf8 = (JVM_PBYTE)
		 pjccTheClass->ppjcpConstantPool[ LD_WORD( *ppjbClassStream ) ];
 	 MV_WORD( *ppjbClassStream );

	 if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyUtf8 ))
	 {
		   JvmCleanUpMethods( pjccTheClass->ppjmMethods, jwMethodsCount );
		   pjccTheClass->ppjmMethods = JVM_NULL;
	       return JVM_FALSE;
	 }

	 JvmLdrGetUtf8Bytes( pjbyUtf8, &pjmMethod->jsMethodDesc, JVM_NULL );

	 pjbyMethodDesc = pjmMethod->jsMethodDesc;

	 LOG_WRITE1("   %s --> %s", pjmMethod->jsMethodName, pjbyMethodDesc )

   pjmMethod->pjmiMethodInfo = JvmMemCalloc( sizeof( _JVM_Method_Info ) );

	 /** Parse the method descriptor right now **/
	 if ( !JvmParseMethodDescriptor( &pjbyMethodDesc, pjmMethod->pjmiMethodInfo ) )
	 {
		  JvmCleanUpMethods( pjccTheClass->ppjmMethods, jwMethodsCount );
    	  pjccTheClass->ppjmMethods = JVM_NULL;
    	  return JVM_FALSE;
	 }

	 jwAttributesCount = LD_WORD( *ppjbClassStream );
	 MV_WORD(  *ppjbClassStream );

	 if ( jwAttributesCount != 0)
	 {
		
		/** Methods can have a Code attribute ( containing the actual bytecodes )
		    and Exceptions attribute 
          Code attribute : can by itself contain two other attributes according to
		    JVM spec 1.0.2 . These are LineNumberTable  and LocalVariables 
			attributes . The local variable attribute will have   all the 
			local variables used in this method
		 **/
		if ( !JvmLdrReadMethodAttributes(
			  jwAttributesCount, pjmMethod,
              ppjbClassStream, jwMethodsCount,
			  pjccTheClass
		  ))
		  return JVM_FALSE;
	 }

	 pjmMethod->pfnNativeMethod = JVM_NULL;

	 ++jwCount;

	} while ( jwCount < jwMethodsCount );

	pjccTheClass->jwMethodsCount = jwMethodsCount;

	return JVM_TRUE;
}



JVM_BOOL  JvmLdrReadMethodAttributes( 
  IN      JVM_WORD    jwAttribCount, 
  IN     _JVM_Method * pjmMethod,
  IN      JVM_PBYTE  * ppjbClassStream,
  IN      JVM_WORD    jwMethodsCount,
  IN OUT _JVM_Class_Class *pjccTheClass
)
{
  JVM_ASSERT_VALID( ppjbClassStream );

  /** JVM Spec 1.0.2 recognizes only 'Code' and 'Exceptions' Attributes  **/
  if ( !JvmGenericAttribHandler(
	     ppjbClassStream,
		 pjmMethod,
		 pjccTheClass,
		 jwAttribCount,
		 ATTRIB_HOST_METHOD
		 )
	 )
	 return JVM_FALSE;

  return JVM_TRUE;
}

JVM_BOOL  JvmHdlAttrSourceFile( 
  IN      JVM_PBYTE  * ppjbClassStream,
  IN      JVM_PVOID    pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD jwAttrHost
)
{
	JVM_WORD jwSourceIndex = 0xffff;
	JVM_PBYTE pjbyUtf8 = JVM_NULL;
	JVM_ASSERT_VALID( pjccTheClass );

	if (( ATTRIB_HOST_CLASS == jwAttrHost ) &&
	    ( !JVM_VALID( pjccTheClass->jsSourceFileName ) )
	   )
    {
		/** skip attrib length **/
		MV_DWORD( *ppjbClassStream );

		jwSourceIndex = LD_WORD( *ppjbClassStream );
		MV_WORD( *ppjbClassStream );

		if ( jwSourceIndex >= 0 && jwSourceIndex < pjccTheClass->jwCPEntryCount )
		{
    		pjbyUtf8 = (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ jwSourceIndex ];
	
			if ( JvmLdrGetUtf8Bytes( pjbyUtf8,
				    &pjccTheClass->jsSourceFileName,
					&pjccTheClass->jwSourceFileNameLen 
					)
				)
			return JVM_TRUE;
   		}
	}

	return JVM_FALSE;
}


JVM_BOOL  JvmHdlAttrExceptions( 
  IN      JVM_PBYTE  * ppjbClassStream,
  IN      JVM_PVOID    pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD jwAttrHost
)
{
   register JVM_WORD  jwCount       = 0;
   register JVM_WORD jwCPIndex      = 0xffff;
  _JVM_Method * pjmMethod = (_JVM_Method *)pjvGeneric;

   JVM_ASSERT_VALID( ppjbClassStream );
   JVM_ASSERT_VALID( pjmMethod );
   JVM_ASSERT_VALID( pjccTheClass );

   /** Must be a method attribute and there should
       be only one present **/
   if (( jwAttrHost != ATTRIB_HOST_METHOD ) ||
	   ( pjmMethod->jwMthdExcTableCount   ))
	   return JVM_FALSE;

   /** Skip Attribute Length **/
   MV_DWORD( *ppjbClassStream );

   pjmMethod->jwMthdExcTableCount = LD_WORD( *ppjbClassStream );
   MV_WORD( *ppjbClassStream )

   /** No exc thrown from method **/
   if ( !pjmMethod->jwMthdExcTableCount )
   {
	   pjmMethod->jwMthdExcTableCount = 0xffff;
	   return JVM_TRUE;
   }

   pjmMethod->pjwMthdExcTable =  ( JVM_WORD * ) JvmMemCalloc( 
	     pjmMethod->jwMthdExcTableCount * sizeof( JVM_WORD ) );
   JVM_ALLOC_CHECK( pjmMethod->pjwMthdExcTable )

   for ( ; jwCount < pjmMethod->jwMthdExcTableCount; jwCount ++ )
   {
	 jwCPIndex = LD_WORD( *ppjbClassStream );
	 MV_WORD(  *ppjbClassStream );

	 if  (!IsCPElementType( JVM_CP_TAG_CLASS, pjccTheClass, jwCPIndex ))
	 {
		 JvmMemFree( pjmMethod->pjwMthdExcTable );
		 return JVM_FALSE;
	 }
	 pjmMethod->pjwMthdExcTable[ jwCount ] = jwCPIndex;
   }

   return JVM_TRUE;
}



JVM_BOOL  JvmHdlAttrCode( 
  IN      JVM_PBYTE  * ppjbClassStream,
  IN      JVM_PVOID    pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
)
{
   JVM_DWORD jdwAttribLen  = 0;
   JVM_WORD  jwTableCount  = 0;
   JVM_WORD  jwAttribCount = 0;
   _JVM_Method * pjmMethod = (_JVM_Method *)pjvGeneric;
   
   DECLARE_LOG_DATA("c:\\jvm\\out\\class.log")

   _JVM_ByteCode_Info  * pjbiBCInfo = JVM_NULL;
   _JVM_EXC_Table_Info * pjetiETInfo  = JVM_NULL;
  

   JVM_ASSERT_VALID( ppjbClassStream );
   JVM_ASSERT_VALID( pjmMethod );
   JVM_ASSERT_VALID( pjccTheClass );

   if ( jwAttrHost != ATTRIB_HOST_METHOD ) 
	   return JVM_FALSE;

   jdwAttribLen = LD_DWORD( *ppjbClassStream );
   MV_DWORD( *ppjbClassStream );

   pjbiBCInfo =   pjmMethod->pjciCodeInfo
	   = ( _JVM_ByteCode_Info *) JvmMemCalloc( sizeof( _JVM_ByteCode_Info ));
   JVM_ALLOC_CHECK( pjbiBCInfo )

   pjbiBCInfo->jwMaxStack    = LD_WORD( *ppjbClassStream );
   MV_WORD( *ppjbClassStream );
   pjbiBCInfo->jwMaxLocals   = LD_WORD( *ppjbClassStream );
   MV_WORD( *ppjbClassStream );
   pjbiBCInfo->jdwCodeLength = LD_DWORD( *ppjbClassStream );
   MV_DWORD( *ppjbClassStream );

   pjbiBCInfo->pjbyCode = (JVM_PBYTE) JvmMemCalloc(pjbiBCInfo->jdwCodeLength + 1);
   JVM_ALLOC_CHECK( pjbiBCInfo->pjbyCode );

   /** Copy the method code bytes **/
   JvmMemCpy(pjbiBCInfo->pjbyCode, *ppjbClassStream, pjbiBCInfo->jdwCodeLength );

#if defined(__DEBUG__) && defined(__EXPAND_CODE__) && defined(__LOG__)
    LdrPrettyPrintCode( pjmMethod );
#endif

   *ppjbClassStream += pjbiBCInfo->jdwCodeLength;

   pjbiBCInfo->jwExceptionTableCount =  LD_WORD( *ppjbClassStream );
   MV_WORD(  *ppjbClassStream  );

   if ( pjbiBCInfo->jwExceptionTableCount )
   {
     pjbiBCInfo->ppjetiExceptionTable = ( _JVM_EXC_Table_Info **) JvmMemCalloc(
		   (pjbiBCInfo->jwExceptionTableCount) * sizeof(  _JVM_EXC_Table_Info * ));
	 JVM_ALLOC_CHECK( pjbiBCInfo->ppjetiExceptionTable )

     do
	 {
	  pjetiETInfo = pjbiBCInfo->ppjetiExceptionTable[ jwTableCount ] = 
	 	  ( _JVM_EXC_Table_Info * )	JvmMemCalloc(sizeof(  _JVM_EXC_Table_Info ) );
	  JVM_ALLOC_CHECK( pjetiETInfo ) 

	  pjetiETInfo->jwStartPC = LD_WORD( *ppjbClassStream );
	  MV_WORD( *ppjbClassStream );

	  if ( ( pjetiETInfo->jwStartPC >= pjbiBCInfo->jdwCodeLength ) ||
		   !IsJvmOpcode( pjbiBCInfo->pjbyCode[ pjetiETInfo->jwStartPC ] )
		 )
           goto _abnormalexit;   

	  pjetiETInfo->jwEndPC = LD_WORD( *ppjbClassStream );
	  MV_WORD( *ppjbClassStream );

	  if (( pjetiETInfo->jwEndPC >= pjbiBCInfo->jdwCodeLength ) ||
		  !IsJvmOpcode( pjbiBCInfo->pjbyCode[ pjetiETInfo->jwEndPC ] )
		 ) 
           goto _abnormalexit;

	  pjetiETInfo->jwHandlerPC = LD_WORD( *ppjbClassStream );
	  MV_WORD( *ppjbClassStream );

	  if (( pjetiETInfo->jwHandlerPC >= pjbiBCInfo->jdwCodeLength ) ||
		  !IsJvmOpcode( pjbiBCInfo->pjbyCode[ pjetiETInfo->jwHandlerPC ] )
		 ) 
           goto _abnormalexit;
	  pjetiETInfo->jwCatchType = LD_WORD( *ppjbClassStream );
	  MV_WORD( *ppjbClassStream );

	  /** CHECK,CHECK - Do we need to load the exception class right now?? **/
	  if ( pjetiETInfo->jwCatchType )
	  {
		 JVM_STRING jsExceptionClass;
		 jsExceptionClass = JvmGetClassName(pjccTheClass, pjetiETInfo->jwCatchType,JVM_NULL);
     if ( !JvmLoadClass( jsExceptionClass, pjccTheClass->joClassLoaderObject ) ) {
       LOG_WRITE( "Warning: Unable to load %s", jsExceptionClass);
			 // goto _abnormalexit;
     }

		 if (( pjetiETInfo->jwCatchType >= pjccTheClass->jwCPEntryCount  ) ||
		    (!IsCPElementType( JVM_CP_TAG_CLASS, pjccTheClass, pjetiETInfo->jwCatchType)) ||
			(!IsKindOf(jsExceptionClass, S_JAVA_LANG_THROWABLE, pjccTheClass->joClassLoaderObject) 
      )) {
       LOG_WRITE( "Warning: Exception class %s is not of type %s",
            jsExceptionClass, S_JAVA_LANG_THROWABLE);
		  // goto _abnormalexit;
     }
	  }
	  ++jwTableCount;

	 } while ( jwTableCount < pjbiBCInfo->jwExceptionTableCount );
   }

   jwAttribCount =  LD_WORD( *ppjbClassStream );
   MV_WORD( *ppjbClassStream );

   if ( jwAttribCount )
   {
		  if ( !JvmGenericAttribHandler( ppjbClassStream, 
			           pjmMethod, pjccTheClass, 
					   jwAttribCount,
					   ATTRIB_HOST_CODE
					   )
    		 )
    		  goto _abnormalexit;
	
   }
          return JVM_TRUE;

 _abnormalexit :
   		  JvmCleanUpCode( pjbiBCInfo );
		  pjmMethod->pjciCodeInfo = JVM_NULL;
		  return JVM_FALSE;

}


JVM_BOOL JvmGenericAttribHandler(
  IN OUT  JVM_PBYTE  * ppjbClassStream,
  IN OUT  JVM_PVOID    pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN  	  JVM_WORD jwAttribCount,
  IN      JVM_WORD jwAttribHost
)
{
  JVM_PFN_ATTRIB_HANDLER apfnAttrHandler = JVM_NULL;
  JVM_BOOL jbStatus = JVM_FALSE;

  do
  {
	  if ((apfnAttrHandler=JvmGetAttribHandler(pjccTheClass, LD_WORD( *ppjbClassStream)))== JVM_NULL )
    			  return JVM_FALSE;

 	  MV_WORD( *ppjbClassStream );

	  jbStatus = (* apfnAttrHandler)(
			ppjbClassStream,
			pjvGeneric,
			pjccTheClass,
			jwAttribHost
		 	); 
	  if ( !jbStatus )
		  return JVM_FALSE;

  } while ( --jwAttribCount );

  return JVM_TRUE;

}

/** CHECK,CHECK Optimization deserved **/
JVM_PFN_ATTRIB_HANDLER JvmGetAttribHandler( 
  IN OUT _JVM_Class_Class * pjccTheClass,
  IN      JVM_WORD jwUtf8Index
)
{
 	register JVM_PBYTE pjbyUtf8  = JVM_NULL;
	JVM_WORD jwLen      = 0;
    JVM_STRING jsString = JVM_NULL; 
	JVM_STRING jsAttribString;

	register JVM_INT jiArrayLen = sizeof( _ajaiAttrInfoTab ) / sizeof( _JVM_Attrib_Info );

	JVM_ASSERT_VALID( pjccTheClass );
	JVM_ASSERT_VALID( pjccTheClass->ppjcpConstantPool );

	pjbyUtf8 = (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ jwUtf8Index ];
	
	if ( !JvmLdrGetUtf8Bytes( pjbyUtf8, &jsString, &jwLen ))
		return JVM_FALSE;

	for ( ; jiArrayLen >= 0 ; )
	{
      --jiArrayLen;
    if (!jiArrayLen) break;
	  jsAttribString = JVM_SPRING_CREATE( _ajaiAttrInfoTab[jiArrayLen].ccAttribString );
      if (jwLen == JvmSpringGetLen(jsAttribString))
	  {
	    if ( JvmMemCmp( jsString, jsAttribString, jwLen ) == 0)
	           return _ajaiAttrInfoTab[jiArrayLen].pfnAttribHandler;
	  }
	}

	return _ajaiAttrInfoTab[0].pfnAttribHandler;
}



JVM_BOOL JvmHdlAttrLineNumberTable(
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
)
{
   _JVM_LN_Table_Info * pjltiLNInfo = JVM_NULL;
    JVM_WORD jwCount = 0;
   _JVM_Method * pjmMethod = (_JVM_Method *)pjvGeneric;
    JVM_ASSERT_VALID(  pjmMethod );
	  JVM_ASSERT_VALID(  pjmMethod->pjciCodeInfo );

	if ( jwAttrHost != ATTRIB_HOST_CODE )
		return JVM_FALSE;
	
	/** skip attrib_length **/
	MV_DWORD( *ppjbByteStream );

	pjmMethod->pjciCodeInfo->jwLineNumTableCount = LD_WORD ( *ppjbByteStream );
	MV_WORD( *ppjbByteStream); 
	if ( !pjmMethod->pjciCodeInfo->jwLineNumTableCount )
		return JVM_TRUE;
	pjmMethod->pjciCodeInfo->ppjltiLineNumTable
		        = (_JVM_LN_Table_Info **)JvmMemCalloc( pjmMethod->pjciCodeInfo->jwLineNumTableCount
				             * sizeof( _JVM_LN_Table_Info * ) 
						   ); 
	for ( ; jwCount < pjmMethod->pjciCodeInfo->jwLineNumTableCount; jwCount++ )
	{
	   pjltiLNInfo               =  JvmMemCalloc( sizeof( _JVM_LN_Table_Info ));
	   pjltiLNInfo->jwStartPC    = 	LD_WORD( *ppjbByteStream); 
	   MV_WORD( *ppjbByteStream); 
   	   pjltiLNInfo->jwLineNumber =  LD_WORD( *ppjbByteStream );
	   MV_WORD( *ppjbByteStream); 
	   pjmMethod->pjciCodeInfo->ppjltiLineNumTable[ jwCount ] 
		    =  pjltiLNInfo;
	} 

    return JVM_TRUE;
}

JVM_BOOL JvmHdlAttrDefault(
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
)
{
   JVM_DWORD len = LD_DWORD( *ppjbByteStream );
   *ppjbByteStream = len + sizeof( JVM_DWORD ) +  *ppjbByteStream; 

	 return JVM_TRUE;
}

/** For JDK1.2 support **/
JVM_BOOL JvmHdlAttrDeprecated(
  IN      JVM_PBYTE * ppjbByteStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
)
{
   JVM_DWORD jdwAttribLen  = 0;
   JVM_ASSERT_VALID( pjccTheClass );

   if(jwAttrHost == ATTRIB_HOST_CLASS)
   	   pjccTheClass->jdwClassFlags |= FLAG_DEPRECATED;
   else if(jwAttrHost == ATTRIB_HOST_FIELD)
      ((_JVM_Field *)pjvGeneric)->jwFieldAccessFlags |= FLAG_DEPRECATED;
   else if(jwAttrHost == ATTRIB_HOST_METHOD)
   	  ((_JVM_Method *)pjvGeneric)->jwMethodAccessFlags |= FLAG_DEPRECATED;  
   else
      return JVM_FALSE;

   jdwAttribLen = LD_DWORD( *ppjbByteStream );
   if( jdwAttribLen != 0 )
      return JVM_FALSE;
   MV_DWORD( *ppjbByteStream );

   return JVM_TRUE;  
}

/** For JDK1.2 support **/
JVM_BOOL JvmHdlAttrInnerClasses(
  IN      JVM_PBYTE * ppjbClassStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
)
{
   JVM_DWORD jdwAttribLen  = 0;
   JVM_WORD  jwNumberOfClasses;
  _JVM_NestedClass_Info * pjniNestedClassInfo;
  _JVM_InnerClass_Info  * pjiiInnerClassInfo;
   JVM_ASSERT_VALID( pjccTheClass );

   if ( jwAttrHost != ATTRIB_HOST_CLASS ) 
	   return JVM_FALSE;

   jdwAttribLen = LD_DWORD( *ppjbClassStream );
   MV_DWORD( *ppjbClassStream );

   jwNumberOfClasses = LD_WORD( *ppjbClassStream );
   MV_WORD( *ppjbClassStream );

   if ( jwNumberOfClasses ) {
      
	  pjiiInnerClassInfo = pjccTheClass->pjiiInnerClassInfo = 
		  JvmMemAlloc( sizeof( _JVM_InnerClass_Info ));
	  JVM_ALLOC_CHECK( pjiiInnerClassInfo );
	  pjiiInnerClassInfo->ppjniNestedClassInfo = 
		  JvmMemAlloc( sizeof( _JVM_NestedClass_Info *) * jwNumberOfClasses);
	  JVM_ALLOC_CHECK( pjiiInnerClassInfo->ppjniNestedClassInfo );
	  pjiiInnerClassInfo->jwNumberOfClasses = jwNumberOfClasses;
	  for ( ;jwNumberOfClasses; )
	  {
		  jwNumberOfClasses--;
		  pjniNestedClassInfo = JvmMemAlloc(sizeof(_JVM_NestedClass_Info));
          JVM_ALLOC_CHECK( pjniNestedClassInfo );
		  pjniNestedClassInfo->jwInnerClassInfoIndex =
			  LD_WORD( *ppjbClassStream );
		  MV_WORD( *ppjbClassStream );
          pjniNestedClassInfo->jwOuterClassInfoIndex = 
             LD_WORD( *ppjbClassStream );
		  MV_WORD( *ppjbClassStream );
		  pjniNestedClassInfo->jwInnerNameIndex = 
             LD_WORD( *ppjbClassStream );
		  MV_WORD( *ppjbClassStream );
		  pjniNestedClassInfo->jwInnerClassAccessFlags = 
             LD_WORD( *ppjbClassStream );
		  MV_WORD( *ppjbClassStream );
		  pjiiInnerClassInfo->ppjniNestedClassInfo[ jwNumberOfClasses ]
			  = pjniNestedClassInfo;

	  }
   }

   return JVM_TRUE;
}

/** For JDK1.2 support **/
JVM_BOOL JvmHdlAttrSynthetic(
  IN      JVM_PBYTE * ppjbClassStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
)
{
   JVM_DWORD jdwAttribLen  = 0;
   JVM_ASSERT_VALID( pjccTheClass );

   if(jwAttrHost == ATTRIB_HOST_CLASS)
      pjccTheClass->jdwClassFlags |= FLAG_SYNTHETIC;
   else if(jwAttrHost == ATTRIB_HOST_FIELD)
      ((_JVM_Field *)pjvGeneric)->jwFieldAccessFlags |= FLAG_SYNTHETIC;
   else if(jwAttrHost == ATTRIB_HOST_METHOD)
   	  ((_JVM_Method *)pjvGeneric)->jwMethodAccessFlags |= FLAG_SYNTHETIC;  
   else
      return JVM_FALSE;

   jdwAttribLen = LD_DWORD( *ppjbClassStream );
   if( jdwAttribLen != 0 )
      return JVM_FALSE;
   MV_DWORD( *ppjbClassStream );

   return JVM_TRUE;
}



/** CHECK,CHECK have to cleanup locals **/
JVM_BOOL JvmHdlAttrLocalVarTable(
  IN      JVM_PBYTE * ppjbByteStream,
  IN     _JVM_Method *pjmMethod,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
)
{
   _JVM_LV_Table_Info *  pjltiLVInfo = JVM_NULL;
    JVM_INT  jiWordWidth = 0; 
    JVM_WORD jwCount   = 0;
	JVM_WORD jwDescLen = 0;
	JVM_STRING jsFieldDesc = JVM_NULL;
	JVM_PBYTE  pjbyFieldDesc = JVM_NULL;
	JVM_PBYTE  pjbyUtf8    = JVM_NULL;

    JVM_ASSERT_VALID(  pjmMethod );
	JVM_ASSERT_VALID(  pjmMethod->pjciCodeInfo );

	if ( jwAttrHost != ATTRIB_HOST_CODE )
		return JVM_FALSE;

	/** skip attrib_length **/
	MV_DWORD( *ppjbByteStream );

	pjmMethod->pjciCodeInfo->jwLocalVarTableCount =  LD_WORD( *ppjbByteStream );
	MV_WORD( *ppjbByteStream );

	if ( !pjmMethod->pjciCodeInfo->jwLocalVarTableCount )
		return JVM_TRUE;

	pjmMethod->pjciCodeInfo->ppjltiLocalVarTable
		        = (_JVM_LV_Table_Info **)JvmMemCalloc( pjmMethod->pjciCodeInfo->jwLocalVarTableCount
				          *  sizeof( _JVM_LV_Table_Info * ) 
						   ); 
	for ( ;jwCount < pjmMethod->pjciCodeInfo->jwLocalVarTableCount;
	       jwCount++ 
        )
	{
	   pjltiLVInfo               =  JvmMemCalloc( sizeof( _JVM_LV_Table_Info ));
	   JVM_ALLOC_CHECK( pjltiLVInfo );

	   pjltiLVInfo->jwStartPC    = 	LD_WORD( *ppjbByteStream );
	   MV_WORD( *ppjbByteStream); 
 	   pjltiLVInfo->jwLength   = 	LD_WORD( *ppjbByteStream );
	   MV_WORD( *ppjbByteStream); 
	   pjltiLVInfo->jwNameIndex  = 	LD_WORD( *ppjbByteStream); 
	   MV_WORD( *ppjbByteStream); 
	   pjltiLVInfo->jwDescIndex  = 	LD_WORD( *ppjbByteStream); 
	   MV_WORD( *ppjbByteStream); 

	   pjbyUtf8 = (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ pjltiLVInfo->jwDescIndex ];  

	  if ( JVM_CP_TAG_UTF8  != CP_TAG( pjbyUtf8 ) )
	  {
	  	  JvmCleanUpCode( pjmMethod->pjciCodeInfo );
		  pjmMethod->pjciCodeInfo = JVM_NULL;
          return JVM_FALSE;
	  }

	 // COMMENTED jwDescLen = *(JVM_WORD *)(pjbyUtf8 + sizeof( _JVM_Constant_Pool ));
   jwDescLen = LD_WORD(pjbyUtf8 + sizeof( _JVM_Constant_Pool ));
	 pjbyFieldDesc = jsFieldDesc = (JVM_STRING)JvmMemCalloc( jwDescLen + 1);
	 JvmMemCpy( jsFieldDesc, pjbyUtf8 + sizeof( _JVM_Constant_Pool ) + 
		                                sizeof( JVM_WORD ),
							   jwDescLen );
	 jsFieldDesc[ jwDescLen ] = 0;
	 pjltiLVInfo->pjfField =   JvmMemCalloc( sizeof( _JVM_Field ) );
 	 /** CA-BOOM **/ 
	 JVM_ALLOC_CHECK( pjltiLVInfo->pjfField )
	 
	 JvmMemSet( &pjltiLVInfo->pjfField->jfiFieldType, 0, sizeof( _JVM_Field_Info ) );
	 	 
	 /** Parse the field descriptor right now **/
	 if ( !JvmParseFieldDescriptor( &pjbyFieldDesc, &pjltiLVInfo->pjfField->jfiFieldType ) )
	 {
		  JvmCleanUpCode( pjmMethod->pjciCodeInfo );
		  pjmMethod->pjciCodeInfo = JVM_NULL;
	  	  JvmMemFree( jsFieldDesc );
    	  return JVM_FALSE;
	 }

	 /** CHECK,CHECK index of this variable! Do we need it ?? **/   
	 /** DONE,DONE  YEAH,YEAH We need it **/
	 pjltiLVInfo->jwIndex = LD_WORD( *ppjbByteStream); 
	 MV_WORD( *ppjbByteStream); 

	 /** Everything fine so far, **/
	 pjmMethod->pjciCodeInfo->ppjltiLocalVarTable[ jwCount ]
		  =  pjltiLVInfo;

	 JvmMemFree( jsFieldDesc );
	} 

	/** Add up the local var word widths **/

	for ( jwCount = 0; jwCount < pjmMethod->pjciCodeInfo->jwLocalVarTableCount; jwCount ++ )
	{
		pjltiLVInfo = pjmMethod->pjciCodeInfo->ppjltiLocalVarTable[ jwCount ];
		jiWordWidth +=  pjltiLVInfo->pjfField->jfiFieldType.jiWidthInWords;
	}

	pjmMethod->pjciCodeInfo->jiLocalVarsWidthInWords = jiWordWidth;

    return JVM_TRUE;

}




JVM_BOOL IsJvmOpcode(
  IN JVM_BYTE jbByte 
)
{
	return ((jbByte >= JVM_MIN_OPCODE )  &&
		   (jbByte <= JVM_MAX_OPCODE ));
}

JVM_BOOL IsJvmQuickOpcode(
 IN JVM_BYTE jbByte
)
{
    return (jbByte == 0xdc ) ? JVM_FALSE :
	       ((jbByte >= JVM_MIN_QUICK_OPCODE )  &&
		   (jbByte <= JVM_MAX_QUICK_OPCODE ));
}

JVM_BOOL  IsCPElementType( 
  IN       JVM_WORD  jwCPElementTag,
  IN      _JVM_Class_Class * pjccTheClass,
  IN       JVM_WORD jwCPIndex
)
{
  JVM_ASSERT_VALID( pjccTheClass );
  JVM_ASSERT_VALID( pjccTheClass->ppjcpConstantPool );
  JVM_ASSERT_VALID( jwCPIndex < pjccTheClass->jwCPEntryCount );

  return jwCPElementTag == CP_TAG( pjccTheClass->ppjcpConstantPool[ jwCPIndex ] ); 
}

/**   Is S a Superclass of T  **/
JVM_BOOL IsSuperclassOf(  
 IN 	_JVM_Class_Class * pjccS,
 IN     _JVM_Class_Class * pjccT
 )
{	
	return IsSubclassOf( pjccT, pjccS );  
}


/** Is S a Subclass of T **/
JVM_BOOL IsSubclassOf( 
 IN 	_JVM_Class_Class * pjccS,
 IN     _JVM_Class_Class * pjccT
)
{
  return IsCPClassType( pjccT->jsClassName, JVM_TRUE, pjccS);
}

/** Does S implements Interface T ?? **/
JVM_BOOL IsInterfaceImplementedBy(
 IN 	_JVM_Class_Class * pjccS,
 IN     _JVM_Class_Class * pjccT
)
{
   register JVM_WORD jwCount;
   register JVM_WORD jwIntfCount; 
   JVM_ASSERT( pjccS && pjccT );

   jwIntfCount = pjccS->jwIntfCount;

   for ( jwCount = 0; jwCount < jwIntfCount; jwCount ++ )
   {
	   if ( pjccT == pjccS->ppjoInterfaces[ jwCount ]
		                ->joiObjectID.pjccClass )
			   return JVM_TRUE;
   }

   return JVM_FALSE;
}


JVM_BOOL HasInterface(
  IN     _JVM_Class_Class * pjccSubjectClass,
  IN     _JVM_Class_Class * pjccTargetClass
)
{
	/** TODO **/
	return FALSE;
}



JVM_BOOL  IsKindOfEx( 
  IN       _JVM_Class_Class * pjccSubjectClass,
  IN       _JVM_Class_Class * pjccTargetClass
)
{
   return IsCPClassType( pjccTargetClass->jsClassName,JVM_TRUE, pjccSubjectClass);
}



/** Assumes that classes have been loaded already by the 
    same class loader joClassLoader 
	 **/
JVM_BOOL  IsKindOf( 
  IN       JVM_STRING  jsSubjectClassName,
  IN       JVM_STRING  jsTargetClassName,
  IN       JVM_OBJREF  joClassLoader
  
)
{
  _JVM_Object * joSubjClassObject;
  if ( !JvmLdrGetClassObject( jsSubjectClassName, joClassLoader, &joSubjClassObject ))
     return JVM_FALSE;

  return   IsCPClassType( jsTargetClassName,
	   JVM_TRUE, joSubjClassObject->joiObjectID.pjccClass);
}



JVM_BOOL  IsCPClassType( 
  IN       JVM_STRING  jsClassName,
  IN       JVM_BOOL  jbCheckIfSubclass,
  IN      _JVM_Class_Class * pjccTheClass
)
{
   _JVM_Class_Class *pjccAClass = JVM_NULL;

	JVM_ASSERT_VALID( pjccTheClass );

	pjccAClass = pjccTheClass;

	/** Given string exactly matches the class name of pjccTheClass **/
	if ( !jbCheckIfSubclass )
		return (0 == JvmSpringStrCmp( jsClassName, pjccTheClass->jsClassName ));

	/** Loop thro' until we reach class "Object" **/
	do
	{
		if ( 0 == JvmSpringStrCmp( jsClassName, pjccAClass->jsClassName ))
			return JVM_TRUE;
    if (!pjccAClass->joSuperClassObject)
       break;
	    pjccAClass = pjccAClass->joSuperClassObject->joiObjectID.pjccClass;
	} while ( pjccAClass != JVM_NULL );

	return JVM_FALSE;
}



/** CHECK,CHECK Do Code Walkthro' this function tree **/
/** CHECK,CHECK all exit points for memory leaks **/
/** CHECK,CHECK basic logical flow and stupidities **/

/** Constant Pool is assumed to be present by now **/
JVM_BOOL  JvmLdrReadFields( 
  IN OUT  JVM_PBYTE * ppjbClassStream,
  IN      _JVM_Class_Class * pjccTheClass,
  IN      JVM_WORD jwFieldsCount
)
{
   JVM_WORD  jwCount  = 0;
   JVM_PBYTE pjbyUtf8 = JVM_NULL;
   JVM_WORD  jwDescLen = 0;
   JVM_STRING jsFieldDesc = JVM_NULL;
   JVM_PBYTE  pjbyFieldDesc = JVM_NULL;
   _JVM_Field * pjfField = JVM_NULL;
   JVM_WORD  jwAttributesCount = 0;

   DECLARE_LOG_DATA("c:\\jvm\\out\\class.log")

   JVM_ASSERT_VALID( ppjbClassStream );
   JVM_ASSERT_VALID( pjccTheClass );
   JVM_ASSERT( jwFieldsCount != 0 );

   /** CHECK,CHECK position of the stream before returning from this function**/
   pjccTheClass->ppjfData = 
	   ( _JVM_Field ** ) JvmMemCalloc( sizeof( _JVM_Field *) *  jwFieldsCount );
   JVM_ALLOC_CHECK( pjccTheClass->ppjfData )

   LOG_WRITE( "%s...", "Fields")

   do
   {
 	 pjfField = pjccTheClass->ppjfData[ jwCount ] = JvmMemCalloc( sizeof( _JVM_Field ) );
	 JVM_ALLOC_CHECK( pjfField )
     
	 /** Access Flags **/
	 pjfField->jwFieldAccessFlags = LD_WORD( *ppjbClassStream );
	 MV_WORD(*ppjbClassStream);
	 /** Field Name CP Index **/
	 pjfField->jwFieldCPIndex  = 	LD_WORD( *ppjbClassStream );

	 /* store the field name */
	 JvmLdrGetUtf8Bytes( (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ pjfField->jwFieldCPIndex ], 
		 &pjfField->jsFieldName, JVM_NULL );


	 MV_WORD( *ppjbClassStream );

	 JVM_ASSERT_VALID( pjccTheClass->ppjcpConstantPool );

	 /** Field Descriptor CP Index **/
	 pjbyUtf8 = (JVM_PBYTE)
		 pjccTheClass->ppjcpConstantPool[ LD_WORD( *ppjbClassStream ) ];

	 if ( JVM_CP_TAG_UTF8  != CP_TAG( pjbyUtf8 ) )
	 {
		   JvmCleanUpFields( pjccTheClass->ppjfData, jwFieldsCount );
		   pjccTheClass->ppjfData = JVM_NULL;
           return JVM_FALSE;
	 }

	 JvmLdrGetUtf8Bytes( pjbyUtf8, &pjfField->jsFieldDesc, JVM_NULL );
	 pjbyFieldDesc = pjfField->jsFieldDesc;

	/*  jwDescLen = *(JVM_WORD *)(pjbyUtf8 + sizeof( _JVM_Constant_Pool ));
	 pjbyFieldDesc = jsFieldDesc = JvmMemCalloc( jwDescLen + 1);
	 JvmMemCpy( jsFieldDesc, pjbyUtf8 + sizeof( _JVM_Constant_Pool ) + 
		                                sizeof( JVM_WORD ),
							   jwDescLen );
	 jsFieldDesc[ jwDescLen ] = 0; */

	 LOG_WRITE1( "   %s - %s", pjfField->jsFieldName, pjbyFieldDesc )
	 JvmMemSet( &pjfField->jfiFieldType, 0, sizeof( _JVM_Field_Info ));
	 /** Parse the field descriptor right now **/
	 if ( !JvmParseFieldDescriptor( &pjbyFieldDesc, &pjfField->jfiFieldType ) )
	 {
		  JvmCleanUpFields( pjccTheClass->ppjfData, jwFieldsCount );
		  pjccTheClass->ppjfData = JVM_NULL;
     	  return JVM_FALSE;
	 }

	 MV_WORD( *ppjbClassStream );
	 jwAttributesCount = LD_WORD( *ppjbClassStream );
	 MV_WORD( *ppjbClassStream );

	 if ( jwAttributesCount != 0)
	 {
		/** Field Must have a constant attribute **/
		/** So we add in ACC_STATIC to field access flags anyway **/
	    
		pjfField->jwFieldAccessFlags |= ACC_STATIC;
		if ( !JvmLdrReadFieldAttributes( jwAttributesCount, pjfField, 
								   ppjbClassStream, jwFieldsCount,
								   pjccTheClass
								 )
           )
		{
		  JvmCleanUpFields( pjccTheClass->ppjfData, jwFieldsCount );
		  pjccTheClass->ppjfData = JVM_NULL;
    	  return JVM_FALSE;
		}
	 }
	 jwCount ++;
	 /** Check,Check **/
   }while ( jwCount < jwFieldsCount );

   pjccTheClass->jwFieldsCount = jwFieldsCount;

   return JVM_TRUE;
}

/** Refer to ConstantValue_attribute in JVM Spec 1.0.2 **/

JVM_BOOL  JvmLdrReadFieldAttributes( 
  IN      JVM_WORD    jwAttribCount, 
  IN     _JVM_Field * pjfField, 
  IN      JVM_PBYTE * ppjbClassStream,
  IN      JVM_WORD    jwFieldsCount,
  IN OUT _JVM_Class_Class *pjccTheClass
)
{
	JVM_ASSERT_VALID( pjccTheClass );
	JVM_ASSERT_VALID( ppjbClassStream );

	if ( JvmGenericAttribHandler( ppjbClassStream, 
			           pjfField, pjccTheClass, 
					   jwAttribCount,
					   ATTRIB_HOST_FIELD
					   )
    		 )
      return JVM_TRUE;

	/** TODO : Check,Check Should we have CleanUpField?? **/
     /**  JvmCleanUpField( pjfField ); **/
	return JVM_FALSE;

}

JVM_BOOL JvmHdlAttrConstantValue( 
  IN      JVM_PBYTE * ppjbClassStream,
  IN      JVM_PVOID   pjvGeneric,
  IN OUT _JVM_Class_Class *pjccTheClass,
  IN      JVM_WORD   jwAttrHost
  )
{
     JVM_WORD jwPoolIndex = 0xffff;
	_JVM_Field * pjfField =  ( _JVM_Field *)pjvGeneric;
  	 JAVA_INT jai = 0xffffffff;

	if (  jwAttrHost != ATTRIB_HOST_FIELD )
		return JVM_FALSE;

	if ( pjfField->jfiFieldType.jwFieldType & JVM_ARRAY_TYPE ) 
		return JVM_TRUE;

	/** Must be 2 according to JVM spec 1.0.2 **/
	if ( LD_DWORD( *ppjbClassStream ) != 0x00000002 )
		return JVM_FALSE;

	MV_DWORD( *ppjbClassStream );

	jwPoolIndex = LD_WORD( *ppjbClassStream );

	/** Array fields cannot have constant value attributes **/

	if ( pjfField->jfiFieldType.jwFieldType & JVM_BASE_TYPE ) 
	{
	   switch ( pjfField->jfiFieldType.jcBaseType  )
	   {
	     case 'L' :	
			   if ( !JvmLdrCVProviderLong(pjccTheClass, jwPoolIndex, &pjfField->u.jal) )
				   return JVM_FALSE;
                break;
		 case 'F' :	
			   if ( !JvmLdrCVProviderFloat(pjccTheClass, jwPoolIndex, &pjfField->u.jaf ))
				   return JVM_FALSE;
	            break;
		 case 'D' :	pjfField->u.jad = 
				    JvmLdrCVProviderDouble(pjccTheClass, jwPoolIndex, &pjfField->u.jad); break;
		 default :
			 {
			  
			  JvmLdrCVProviderInteger(pjccTheClass, jwPoolIndex, &jai);
			  switch ( pjfField->jfiFieldType.jcBaseType )
			  {
			     case 'I' :	 pjfField->u.jai  =  (JAVA_INT  ) jai; break;
		         case 'S' :  pjfField->u.jas  =  (JAVA_SHORT) jai; break;
		         case 'C' :  pjfField->u.jac  =  (JAVA_CHAR ) jai; break;
		         case 'B' :	 pjfField->u.jaby =  (JAVA_BYTE ) jai; break;
           		 case 'Z' :	 pjfField->u.jab  =  (JAVA_BOOL ) jai; break;
				 default  :  JVM_ASSERT( JVM_FALSE ); break;
			  }
			 }
		 break;
	   }
	}
	else
	if ( pjfField->jfiFieldType.jwFieldType & JVM_OBJECT_TYPE ) 
	{
		if (( pjfField->jfiFieldType.jiObjectNameLen == (JVM_INT)LEN_JAVA_LANG_STRING )
		 &&( 0 == JvmMemCmp( pjfField->jfiFieldType.jsObjectName,
			  S_JAVA_LANG_STRING , LEN_JAVA_LANG_STRING) )
			)
		{
    		 /** JvmLdrCVProviderString(pjccTheClass, jwPoolIndex, &pjfField->u.jaoref); **/
			  pjfField->u.js = JvmLdrGetStringName( pjccTheClass, jwPoolIndex, NULL);
		}
	}

	MV_WORD( *ppjbClassStream );

	return JVM_TRUE;
}


#ifndef __JDEPENDS__

JVM_BOOL JvmLdrCVProviderString(
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_OBJREF * pjaoObjrefValue     
)
{
	JAVA_OBJREF jaorObjectRef = JVM_NULL;
	JVM_WORD jwStringLen = 0;
	JVM_BOOL jbReturnValue = JVM_FALSE;
	JVM_STRING jsStringName;

	JVM_ASSERT_VALID( pjccTheClass );

	 *pjaoObjrefValue = JVM_NULL;
	jsStringName = JvmLdrGetStringName( pjccTheClass, jwPoolIndex, &jwStringLen );

	 if ( jsStringName )
	 {
		  if ( JvmRtCreateObject_JavaLangString_v0( jsStringName, &jaorObjectRef ) )
		  {
			    *pjaoObjrefValue = jaorObjectRef;
  				jbReturnValue = JVM_TRUE;
		  }

	 }

    return jbReturnValue;

}

#endif

/** CHECK,CHECK check again , any optimization possible at all??**/
/** Converts an encoded Utf8 bytes to Utf8 bytes , 0 terminates 
    the resulting string . Caller needs to free the string **/

/** final Utf8 string representation is ( Sniper String format )
    See Also jvmsprng.c

    <n1>bytes	 	  <n2> bytes    	       <1>byte
	+--------------+---------------------------+-----+
    |UTF8 Node ptr |bytes...                   |0    |
	+--------------+---------------------------+-----+

    n1 -  sizeof( JVM_PVOID )
	n2 -  number of bytes in 2 byte granularity i.e each character occupies
	      2 bytes irrespective of ascii or not.

 **/

JVM_BOOL JvmLdrGetUtf8Bytes( 
  IN   JVM_PBYTE  pjbyUtf8, 
  OUT  JVM_STRING * ppjbyString,
  OUT  JVM_WORD   * pjwStringLen
)
{
	JVM_PBYTE pjbyBytes = 0;
	JVM_BYTE  jby1,jby2,jby3;
	JVM_WORD  jw        = 0;
	JVM_PBYTE pjbyString = JVM_NULL;
	JVM_PBYTE pjbyNewString = JVM_NULL;
	JVM_WORD  jwLen       = 0;
	JVM_WORD  jwNextIndex = 0;
	JVM_BOOL  jbReturnValue = JVM_TRUE;
	JVM_BOOL  jbAnsiFlag    = JVM_TRUE;

	JVM_ASSERT_VALID( pjbyUtf8 );
	JVM_ASSERT_VALID( ppjbyString );

	if ( !JvmGetUtf8Info( pjbyUtf8, &pjbyBytes, &jwLen ) )
		return JVM_FALSE;

	/** This allocation will grow based on need **/
	pjbyString = JvmMemCalloc( CHUNK );
	JVM_ALLOC_CHECK( pjbyString )

	for ( ; ; )	{
	  /** exit **/
	  if ( jwLen <= 0 )
	  {  break;  }

	  jby1 = *pjbyBytes;
	  /** We have ASCII i.e \u0001 to \u007f **/
      if ( !( jby1 & 0x80 ) )  {
		if ( jwNextIndex && (!((jwNextIndex + 3) % (CHUNK + 1))))
		{
		   pjbyString = JvmMemRealloc( pjbyString, jwNextIndex + CHUNK + 3);
		   JVM_ALLOC_CHECK( pjbyString )
		}
		pjbyString[ jwNextIndex++ ] = 0x00;
		pjbyString[ jwNextIndex++ ] = jby1;
		++pjbyBytes;
		jwLen--;
		continue;
	  }

	  if ( jby1 & 0xe0 ) {
		  /** utf8 values \u0800 to \uffff **/
		  jby2 = *(pjbyBytes + 1);
		  jby3 = *(pjbyBytes + 2);

   		  if ( ( jby2 & 0x80 ) && ( jby3 & 0x80 ) )
		  {
				if ( jwNextIndex && (!((jwNextIndex + 3) % (CHUNK + 1))))
				{
				pjbyString = JvmMemRealloc( pjbyString, jwNextIndex + CHUNK + 3);
				JVM_ALLOC_CHECK( pjbyString )
				}
				jw = (JVM_WORD)((jby1 & 0x0f) << 12) + (jby2 & 0x3f) + 
											(jby3 & 0x3f);
				pjbyString[ jwNextIndex++ ] = (JVM_BYTE)(jw >> 8);
				pjbyString[ jwNextIndex++ ] = (JVM_BYTE)jw;
				pjbyBytes += 3;
				jwLen -= 3;
				jbAnsiFlag = JVM_FALSE;
				continue;
		  } 
	  } else if ( jby1 & 0xc0 ) {
		  /** utf8 values \u0000 , \u0080 to \u07ff **/
		  jby2 = *(pjbyBytes + 1);
	 	  if ( jby2 & 0x80 )  {
				if ( jwNextIndex && (!((jwNextIndex + 3) % (CHUNK + 1))))
				{
				pjbyString = JvmMemRealloc( pjbyString, jwNextIndex + CHUNK + 3);
				JVM_ALLOC_CHECK( pjbyString )
				}
				jw = (JVM_WORD)((jby1 & 0x1f) << 6) + (jby2 & 0x3f);
				pjbyString[ jwNextIndex++ ] = (JVM_BYTE)(jw >> 8);
				pjbyString[ jwNextIndex++ ] = (JVM_BYTE)jw;
				pjbyBytes += 2;
				jwLen -= 2;
				jbAnsiFlag = JVM_FALSE;
				continue;
		  } 
	  }

      // Something wrong 
      return JVM_FALSE;
	}

	/** Remove leading 0x00's if ascii **/
	if ( jbAnsiFlag )
	{
		JVM_PBYTE pjbyAnsiBytes;
		JVM_WORD  jwIndex,jwAnsiIndex;
		jwIndex = jwAnsiIndex  = 0;
		pjbyAnsiBytes = (JVM_PBYTE)JvmMemCalloc(1 + ( jwNextIndex / 2 ));
		JVM_ALLOC_CHECK( pjbyAnsiBytes )

		for ( jwIndex = 0; jwIndex < jwNextIndex; jwIndex++ )
		{
		  if ( pjbyString[jwIndex] )
		     pjbyAnsiBytes[jwAnsiIndex ++] = pjbyString[jwIndex];
		}
		pjbyAnsiBytes[ jwAnsiIndex ] = 0;
		JvmMemFree( pjbyString );

		pjbyString  = pjbyAnsiBytes;
		jwNextIndex = jwAnsiIndex + 1;
	}

	/** Create a Read/Write Spring **/
	pjbyNewString = JvmSpringFromBytes( pjbyString, jwNextIndex, jbAnsiFlag, FALSE  );

	JvmMemFree( pjbyString );

	/** insert length before the actual bytes 
	LD_WORD( pjbyString - WORDLEN ) =  jwNextIndex;
	**/
	*ppjbyString  = pjbyNewString;
	if ( JVM_VALID( pjwStringLen ) )
	    *pjwStringLen = JvmSpringGetLen( pjbyNewString );

	return jbReturnValue;
}  




JVM_BOOL JvmGetUtf8Info(
  IN   JVM_PBYTE pjbyCPUtf8, 
  OUT  JVM_PBYTE * pjbyUtf8bytes,
  OUT  JVM_WORD  * pjwLen 
)
{
	JVM_ASSERT_VALID( pjbyCPUtf8 );

 	if ( JVM_CP_TAG_UTF8 != CP_TAG( pjbyCPUtf8 ) )
		return JVM_FALSE;
	*pjwLen   = CP_LEN( pjbyCPUtf8 );
	*pjbyUtf8bytes =  pjbyCPUtf8 + sizeof( _JVM_Constant_Pool ) 
		   + sizeof( JVM_WORD );

	return JVM_TRUE;
}

/** CHECK,CHECK Optimization possible?? **/

JVM_BOOL JvmLdrCVProviderLong( 
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_LONG * pjalLongValue     
)
{
   	JVM_PBYTE pjbyLong = JVM_NULL;
    JVM_ASSERT_VALID( pjccTheClass );

	/** 64bits **/
	*pjalLongValue = (JAVA_LONG)0;

 	pjbyLong = 	( JVM_PBYTE )pjccTheClass->ppjcpConstantPool[ jwPoolIndex ];

	if (JVM_CP_TAG_LONG  !=  CP_TAG( pjbyLong ))  
		return JVM_FALSE;

	pjbyLong += sizeof( _JVM_Constant_Pool );

	*pjalLongValue = ((JAVA_LONG)(LD_DWORD( pjbyLong ))
						   << 32 ) |(LD_DWORD( pjbyLong + 4));
	
	return JVM_TRUE;
}

JVM_BOOL JvmLdrCVProviderInteger( 
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_INT * pjaiIntValue     
)
{
   	JVM_PBYTE pjbyInt = JVM_NULL;
    JVM_ASSERT_VALID( pjccTheClass );

	*pjaiIntValue = (JAVA_INT)0;

 	pjbyInt = (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ jwPoolIndex ];

	if ( JVM_CP_TAG_INTEGER !=  CP_TAG( pjbyInt ))  
		return JVM_FALSE;

	pjbyInt += sizeof( _JVM_Constant_Pool );

	*pjaiIntValue = (JAVA_INT) LD_DWORD( pjbyInt );
	
	return JVM_TRUE;
}



JVM_BOOL JvmLdrCVProviderFloat( 
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_FLOAT * pjalFloatValue     
)
{
   	JVM_PBYTE pjbyFloat = JVM_NULL;
	JVM_DWORD jdwBytes  = 0;
    JVM_ASSERT_VALID( pjccTheClass );

	/** 32bits **/
	*pjalFloatValue = (JAVA_FLOAT)0.0;

 	pjbyFloat = (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ jwPoolIndex ];

	if (JVM_CP_TAG_FLOAT  != CP_TAG( pjbyFloat ))  
		return JVM_FALSE;

	pjbyFloat += sizeof( _JVM_Constant_Pool );
	jdwBytes = LD_DWORD( pjbyFloat );

    *pjalFloatValue = (JAVA_FLOAT)jdwBytes;

	if ( jdwBytes != IEEE754_F_POS_INFINITY  &&
	   	 jdwBytes != IEEE754_F_NEG_INFINITY
	   )
    {
    	if  ((( jdwBytes >  IEEE754_F_POS_INFINITY && 
	 	       jdwBytes <= IEEE754_F_POS_MAX ) ) ||
	    	 ( jdwBytes >  IEEE754_F_NEG_INFINITY &&
		       jdwBytes <= IEEE754_F_NEG_MAX ) ) 
           *pjalFloatValue = IEEE754_F_NaN;
	    else
		{
			JAVA_INT jaiSign = (( jdwBytes >> 0x1f ) == 0 ) ? 1 : -1;
			JAVA_INT jaiExponent = ( jdwBytes >> 0x17 ) & 0xff;
			JAVA_INT jaiMantissa = (jaiExponent == 0 ) ? 
				( jdwBytes & 0x7fffff ) << 1 :
			    ( jdwBytes & 0x7fffff ) | 0x800000;
		   *pjalFloatValue = ( JAVA_FLOAT )
			    jaiSign * jaiMantissa * ( 2 ^ ( jaiExponent	- 150 ) );
		}
	}

  return JVM_TRUE;
}

JVM_BOOL JvmLdrCVProviderDouble( 
   IN    _JVM_Class_Class *pjccTheClass,
   IN     JVM_WORD  jwPoolIndex,
   OUT    JAVA_DOUBLE * pjalDoubleValue     
)
{
   	JVM_PBYTE pjbyDouble = JVM_NULL;
	JAVA_LONG jalBytes  = 0;
    JVM_ASSERT_VALID( pjccTheClass );

	/** 64bits **/
	*pjalDoubleValue = (JAVA_DOUBLE)0.0;

 	pjbyDouble = (JVM_PBYTE)pjccTheClass->ppjcpConstantPool[ jwPoolIndex ];

	if ( JVM_CP_TAG_DOUBLE  != CP_TAG( pjbyDouble ))  
		return JVM_FALSE;

	pjbyDouble += sizeof( _JVM_Constant_Pool );
	jalBytes = ((JAVA_LONG)(LD_DWORD( pjbyDouble ))
						   << 32 ) |(LD_DWORD( pjbyDouble + 4));

    *pjalDoubleValue = (JAVA_DOUBLE)jalBytes;

	if ( jalBytes != IEEE754_D_POS_INFINITY  &&
	   	 jalBytes != IEEE754_D_NEG_INFINITY
	   )
    {
		/** CHECK,CHECK - do we need to use 0x7f80000000000000L or
		                                    0x7ff0000000000000L
		 **/

    	if  (( jalBytes >  IEEE754_D_POS_INFINITY && 
	 	       jalBytes <= IEEE754_D_POS_MAX )  ||
	    	 ( jalBytes >  IEEE754_D_NEG_INFINITY &&
		       jalBytes <= IEEE754_D_NEG_MAX ) ) 
           *pjalDoubleValue = ( JAVA_LONG)IEEE754_D_NaN;
	    else
		{
			JAVA_INT  jaiSign = (( jalBytes >> 63 ) == 0 ) ? 1 : -1;
			JAVA_INT  jaiExponent = (JAVA_INT)(( jalBytes >> 52 ) & 0x7ff);
			JAVA_LONG jalMantissa = (jaiExponent == 0 ) ? 
				( jalBytes & 0xfffffffffffffff ) << 1 :
			    ( jalBytes & 0xfffffffffffffff ) | 0x10000000000000;
		   *pjalDoubleValue = ( JAVA_DOUBLE )
			    jaiSign * jalMantissa * ( 2 ^ ( jaiExponent	- 1075 ) );
		}
	}

  return JVM_TRUE;
}




JVM_BOOL JvmLdrCheckVersion(
  IN  	 JVM_PBYTE * ppjbClassStream 
)
{
  JVM_WORD w;
  w = LD_WORD(*ppjbClassStream);
  // Just skip now
 // if (w != JVM_MINOR_VERSION )
  	//  return JVM_FALSE;
  MV_WORD( *ppjbClassStream );
 w =  LD_WORD(*ppjbClassStream);
//  if (w != JVM_MAJOR_VERSION && w != 46 && w != 47 && w != 48)
  //	  return JVM_FALSE;
  MV_WORD( *ppjbClassStream );
  return JVM_TRUE;
}

JVM_BOOL JvmLdrCheckMagic( 
  IN     JVM_PBYTE * ppjbClassStream
) 
{
  if ( LD_DWORD(*ppjbClassStream) != 0xCAFEBABE )
	  return JVM_FALSE;
  MV_DWORD( *ppjbClassStream );
  return JVM_TRUE;
}

JVM_VOID JvmCPTypeConstUtf8(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN     JVM_WORD *  pjwCurPoolSize, 
  IN     JVM_DWORD * pjdwStatus
)
{
#ifdef __CP_TRACE_ON__
   DECLARE_LOG_DATA("c:\\jvm\\out\\class.log")
#endif
   register JVM_PBYTE            pjbyUtf8 = JVM_NULL;
   JVM_WORD             jwLength = 0;

   JVM_ASSERT_VALID( ppjbClassStream   );
   JVM_ASSERT_VALID( pppjcpTheCPVector );
   JVM_ASSERT_VALID( pjdwStatus        );

   (*ppjbClassStream)++;

   *pjdwStatus = JVM_NO_GOOD;

   jwLength = LD_WORD( *ppjbClassStream );
   
   pjbyUtf8 =   (JVM_PBYTE) JvmMemCalloc( sizeof( _JVM_Constant_Pool ) + 
						WORDLEN + BYTELEN * ( jwLength + 1) );
   JVM_ALLOC_CHECK( pjbyUtf8 )
   CP_TAG( pjbyUtf8 ) = JVM_CP_TAG_UTF8;
   CP_LEN( pjbyUtf8 ) = jwLength;
   *((JVM_WORD *)(pjbyUtf8 + sizeof( _JVM_Constant_Pool ) ))
	    = jwLength;
   MV_WORD( *ppjbClassStream );

   if ( jwLength != 0 ) {
#ifdef __CP_TRACE_ON__
     JVM_WORD jwIndex;
     JVM_BUFFER info, name_info;
     info[0] = 0;
     for ( jwIndex = 0; jwIndex < jwLength; jwIndex++){
      name_info[ jwIndex] = *(*ppjbClassStream + jwIndex);
      sprintf( info + JvmStrLen( info ), " 0x%02x",  *(*ppjbClassStream + jwIndex));
     }
     name_info[ jwIndex ] = 0;
     LOG_WRITE1("0x%02x -> %s", *pjwCurPoolSize, info);
     LOG_WRITE("%s", name_info);
#endif
	      /* Modified-Utf8 format characters */
     JvmMemCpy( pjbyUtf8 + sizeof( _JVM_Constant_Pool ) + WORDLEN,
		       *ppjbClassStream,jwLength );
     (*ppjbClassStream) += jwLength;
   } 

   *pppjcpTheCPVector =   JvmLdrAddNewCPInfoToCPVector( 
	                          *pppjcpTheCPVector,
							   pjwCurPoolSize,
							   pjbyUtf8 
						  );

   *pjdwStatus = JVM_GOOD;
}

JVM_VOID JvmCPTypeConstInteger(
  IN OUT JVM_PBYTE * ppjbClassStream,
  IN OUT _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD *  pjwCurPoolSize, 
  IN OUT JVM_DWORD * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					JVM_CP_TAG_INTEGER, 4
                  );
}


JVM_VOID JvmCPTypeConstFloat(
  IN OUT JVM_PBYTE            * ppjbClassStream,
  IN OUT _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD             * pjwCurPoolSize, 
  IN OUT JVM_DWORD            * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					        JVM_CP_TAG_FLOAT,   4
                  );
}

JVM_VOID JvmCPTypeConstLong(
  IN     JVM_PBYTE           * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD            * pjwCurPoolSize, 
  IN     JVM_DWORD           * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					        JVM_CP_TAG_LONG,    8
                  );
   /* Double and long has one more wierd dummy entry */
   JvmAddDummyCPEntry( pppjcpTheCPVector, pjwCurPoolSize );
}

JVM_VOID JvmCPTypeConstDouble(
  IN     JVM_PBYTE           * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD            * pjwCurPoolSize, 
  IN     JVM_DWORD           * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					        JVM_CP_TAG_DOUBLE,   8
                  );
   /* Double and long has one more wierd dummy entry */
   JvmAddDummyCPEntry( pppjcpTheCPVector, pjwCurPoolSize );
}

JVM_VOID JvmCPTypeConstClass(
  IN     JVM_PBYTE           * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD           * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					JVM_CP_TAG_CLASS,   2
                  );


}

JVM_VOID JvmCPTypeConstString(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					JVM_CP_TAG_STRING,    2
                  );

}

JVM_VOID JvmCPTypeConstFieldRef(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					JVM_CP_TAG_FIELDREF,   4
                  );
}

JVM_VOID JvmCPTypeConstMethodRef(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					JVM_CP_TAG_METHODREF,   4
                  );
}

JVM_VOID JvmCPTypeConstInterfaceMethodRef(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					JVM_CP_TAG_INTERFACEMETHODREF,  4
                  );


}

JVM_VOID JvmCPTypeConstNameAndType(
  IN     JVM_PBYTE * ppjbClassStream,
  IN    _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD            * pjwCurPoolSize,
  IN     JVM_DWORD * pjdwStatus
)
{
   JvmAddNewCPType( ppjbClassStream, pppjcpTheCPVector,
	                pjwCurPoolSize,  pjdwStatus,
					JVM_CP_TAG_NAMEANDTYPE,   4
                  );
}


JVM_VOID JvmAddNewCPType(
  IN OUT JVM_PBYTE *  ppjbClassStream,
  IN OUT _JVM_Constant_Pool *** pppjcpTheCPVector,
  IN OUT JVM_WORD  *  pjwCurPoolSize, 
  IN OUT JVM_DWORD *  pjdwStatus,
  IN     JVM_WORD     jwTag,
  IN     JVM_WORD     jwBytesToCopy
)
{
#ifdef __CP_TRACE_ON__
   DECLARE_LOG_DATA("c:\\jvm\\out\\class.log")
   JVM_BUFFER           info; 
   JVM_WORD             jwIndex;
#endif

   register JVM_PBYTE   pjbyCPType = JVM_NULL;
   JVM_WORD             jwLength = 0;


   JVM_ASSERT_VALID( ppjbClassStream );
   JVM_ASSERT_VALID( pppjcpTheCPVector   );
   JVM_ASSERT_VALID( pjdwStatus      );

   (*ppjbClassStream)++;

   pjbyCPType    =   (JVM_PBYTE) JvmMemCalloc( sizeof( _JVM_Constant_Pool ) + 
						sizeof( JVM_BYTE ) * jwBytesToCopy );

   JVM_ALLOC_CHECK( pjbyCPType )

   /* These members will be used to represent the actual
      _JVM_Object * for a class and is also used to
	   represent direct references to _JVM_Method or
	   _JVM_Field of the class if jbyTag is 
	   09 (CONSTANT_FieldRef) or 10 (CONSTANT_MethodRef)
	   for quick cp resolution */
   ((_JVM_Constant_Pool *)pjbyCPType)->pjvClassRef = JVM_NULL;
   ((_JVM_Constant_Pool *)pjbyCPType)->pjvAttrRef = JVM_NULL;
   ((_JVM_Constant_Pool *)pjbyCPType)->jbyTag = (JVM_BYTE)jwTag;
   ((_JVM_Constant_Pool *)pjbyCPType)->jwLen = jwBytesToCopy;

   JvmMemCpy( pjbyCPType + sizeof( _JVM_Constant_Pool ),
	          *ppjbClassStream, jwBytesToCopy );

#ifdef __CP_TRACE_ON__
   info[0] = 0;
   for ( jwIndex = 0; jwIndex < jwBytesToCopy; jwIndex++){
     sprintf( info + JvmStrLen( info ), " 0x%02x",  *(*ppjbClassStream + jwIndex));
   }
   LOG_WRITE1("0x%02x -> %s", *pjwCurPoolSize, info);
#endif

   (*ppjbClassStream) += jwBytesToCopy;

   *pppjcpTheCPVector =   JvmLdrAddNewCPInfoToCPVector( 
	                          *pppjcpTheCPVector,
							   pjwCurPoolSize,
							   pjbyCPType 
   						 );

   *pjdwStatus = JVM_GOOD;
}



_JVM_Constant_Pool ** JvmLdrAddNewCPInfoToCPVector( 
  IN     _JVM_Constant_Pool ** ppjcpVector,
  IN OUT  JVM_WORD          *  pjwCurCPSize, 
  IN      JVM_PBYTE            pjbCPInfo
)
{
  ppjcpVector = JvmMemRealloc( ppjcpVector,
	  ((*pjwCurCPSize) + 1) * sizeof( _JVM_Constant_Pool * ));
  JVM_ALLOC_CHECK( ppjcpVector )

  ppjcpVector[ *pjwCurCPSize ] = (_JVM_Constant_Pool *)pjbCPInfo;

  (*pjwCurCPSize)++;

  return  ppjcpVector;
}



/**

  typedef struct _JVM_Class_Class
{
  JVM_DWORD     jdwClassFlags;
  JVM_STRING    jsClassName;
  JVM_STRING    jsSourceFileName;
  JVM_WORD      jwSourceFileNameLen;
  JVM_WORD      jwFieldCount;
  JVM_WORD      jwMethodsCount;
  _JVM_Field  ** ppjfData;
  _JVM_Method ** ppjmMethods;
  Dictionary  * pdCNCPDict;
  JVM_WORD     jwCPEntryCount;

  _JVM_Constant_Pool       ** ppjcpConstantPool;
  struct _JVM_Class_Class   * pjccClassLoader;
  struct _JVM_Class_Class   * pjccSuperClass;
  struct _JVM_Class_Class   * pjccClassClass;
  struct _JVM_Class_Class   * pjccObjectClass;
  struct _JVM_Class_Class  ** ppjccInterfaces;
}_JVM_Class_Class;

**/

JVM_BOOL JvmConstructObjectOfClass(
  IN     JVM_STRING jsClassName,
  OUT    JAVA_OBJREF * pjaoObjRef,
  IN     JAVA_OBJREF   pjaoLdrRef
)
{
   _JVM_Object *pjoClassObject  = JVM_NULL;
    JVM_BOOL jbCreateState    = JVM_FALSE;

	Hash_Table * phtClass;
	Hash_Type htkey,htval;

	JvmSysAcquireMutex( _jmoGCLdrDictMutex );
	htkey.u.pv = pjaoLdrRef;
	htval = GetFromDictionary( _pdictGCLdrClass, htkey );
	phtClass = ( Hash_Table *)htval.u.pv;

	JVM_ASSERT_VALID( phtClass );

	htkey.u.pv =  jsClassName;
	htval = GetFromHashTable( phtClass, htkey );
	pjoClassObject = ( _JVM_Object * ) htval.u.pv;

    JvmSysReleaseMutex( _jmoGCLdrDictMutex );
	    
    if ( pjoClassObject != JVM_NULL )
    {
     *pjaoObjRef   = JvmRtCreateNewInstance( pjoClassObject->joiObjectID.pjccClass );
	 jbCreateState = JVM_TRUE;
    }

    return jbCreateState;
}


/** Added for runtime module support **/
JVM_BOOL JvmLdrGetDeferredException( 
 IN     JVM_STRING jsClassName,
 IN     JVM_OBJREF joClassLoader,
 IN OUT eJVMExceptionType  * pejeException 
 )
{
   _JVM_Class_State * pjcsClassState = JVM_NULL;
   Hash_Table *phtClassState;
   Hash_Type  htkey,htval;

   JvmSysAcquireMutex( _jmoGCLdrStateMutex );

   htkey.u.pv =  joClassLoader;
   htval =  GetFromDictionary( _pdictGCLdrClassState, htkey );
   phtClassState = ( Hash_Table * )htval.u.pv;

   if ( phtClassState == JVM_NULL )
	   return JVM_FALSE;

   htkey.u.pv = jsClassName;
   htval = GetFromHashTable( phtClassState, htkey );
   pjcsClassState = (_JVM_Class_State * )htval.u.pv;

   if ( JVM_NULL == pjcsClassState )
	   return JVM_FALSE;

   *pejeException = pjcsClassState->ejeException;

   return JVM_TRUE;
}

/** CHECK,CHECK - need to add deffered exception removal functions too 
    because a class might be loaded successfully subsequent to an error load.
**/


/** Used for defferring an exception to be thrown later **/
JVM_VOID JvmDeferException( 
  IN     JVM_STRING jsClassName,
  IN     JVM_OBJREF joClassLoader,
  IN    eJVMExceptionType ejeException 
)
{
   _JVM_Class_State * pjcsClassState = JVM_NULL;
   Hash_Table *phtClassState;
   Hash_Type htkey,htval,htinvalid;

   if (!jsClassName ) return;
   JvmSysAcquireMutex( _jmoGCLdrStateMutex );

   htkey.u.pv =  joClassLoader;
   htval = GetFromDictionary( _pdictGCLdrClassState, htkey );
   phtClassState = ( Hash_Table * )htval.u.pv;

   if ( phtClassState == JVM_NULL )
   {
     htinvalid.u.pv = JVM_NULL;
#ifdef __DEBUG__
     if ( !CreateHashTable(
		 "ClassStateHash",
		 &phtClassState,
		 htinvalid,
		 HASH_TYPE_PTR,
		 HASH_TYPE_PTR,
		 HashClassStateRundown
	 ))
#else
   if ( !CreateHashTable(
		 &phtClassState,
		 htinvalid,
		 HASH_TYPE_PTR,
		 HASH_TYPE_PTR,
		 HashClassStateRundown
	 ))
#endif
      JVM_THROW_EXCEPTION( EXC_JVMNOMEMORY );

     /* Add the hash table to the dictionary */
     htval.u.pv = phtClassState;
     AddToDictionary( _pdictGCLdrClassState, htkey, htval );
   }

   htkey.u.pv = jsClassName;
   htval = GetFromHashTable( phtClassState, htkey);
   pjcsClassState = (_JVM_Class_State * )htval.u.pv;

   if ( 0 == pjcsClassState )
   {
	 pjcsClassState = JvmMemCalloc( sizeof( _JVM_Class_State ));
     JVM_ALLOC_CHECK( pjcsClassState )
	 htkey.u.pv = JVM_SPRING_TO_CSTR( jsClassName );
	 htval.u.pv = pjcsClassState;
     AddToHashTable( phtClassState, htkey, htval);
   }

   pjcsClassState->ejeException = ejeException;
   pjcsClassState->jiState      = JVM_NO_GOOD;

   JvmSysReleaseMutex( _jmoGCLdrStateMutex );
}

JVM_VOID HashClassStateRundown( 
  IN     Hash_Type htype
)
{
  _JVM_Class_State * pjcsClassState = ( _JVM_Class_State * ) htype.u.pv;
  if ( pjcsClassState )
    JvmMemFree( pjcsClassState );
}


_JVM_Class_Class * JvmGetNewClass()
{
   _JVM_Class_Class * pjccAClass = JVM_NULL;
   Hash_Type httype;

   pjccAClass = (_JVM_Class_Class *) JvmMemCalloc( sizeof( _JVM_Class_Class ) );
   JVM_ALLOC_CHECK( pjccAClass )

   pjccAClass->jdwClassFlags       = 0;
   pjccAClass->jsClassName         = JVM_NULL;
   pjccAClass->jsSourceFileName    = JVM_NULL;
   pjccAClass->jwSourceFileNameLen = 0;
   pjccAClass->jwFieldsCount        = 0;
   pjccAClass->jwMethodsCount      = 0;
   pjccAClass->ppjfData            = JVM_NULL;
   pjccAClass->ppjmMethods         = JVM_NULL;
   httype.u.pv = JVM_NULL;
#ifdef __DEBUG__
   if ( !CreateDictionary(
	    "NamePoolIndex_Dict",
	    &pjccAClass->pdNCPDict,
      httype,
      HASH_TYPE_INT,
      HASH_TYPE_PTR,
      NULL,
	   )
     )
#else
   if ( !CreateDictionary(
	    &pjccAClass->pdNCPDict,
      httype,
      HASH_TYPE_INT,
      HASH_TYPE_PTR,
      NULL,
	   )
    )
#endif
     JVM_THROW_EXCEPTION( EXC_JVMNOMEMORY );

   pjccAClass->jwCPEntryCount        = 0;
   pjccAClass->ppjcpConstantPool     = JVM_NULL;
   pjccAClass->joClassLoaderObject   = JVM_NULL;  
   pjccAClass->joSuperClassObject    = JVM_NULL;  
   pjccAClass->joClassClassObject    = JVM_NULL;  
   pjccAClass->joObjectClassObject	 = JVM_NULL;   
   pjccAClass->joArrayBaseTypeObject = JVM_NULL;
   pjccAClass->ppjoInterfaces        = JVM_NULL;  
   pjccAClass->pjfiArrayInfo         = JVM_NULL;
   pjccAClass->pju16InterfacesCPIndex = JVM_NULL;
   
   return pjccAClass;
}

									     
JVM_VOID JvmLdrDefineClass(
 IN      JAVA_OBJREF joThisObject,
 IN 	 JVM_PBYTE pjbyBytes,
 IN      JAVA_INT jaiOffset,
 IN      JAVA_INT jaiLength,
 OUT     JAVA_OBJREF * pjoClassObject
)
{
   JAVA_OBJREF joClassRef;
   /** Call JvmLdrLoadNewClass with joThisObject as the class loader **/
   JvmLdrLoadNewClass( JVM_NULL, &pjbyBytes[jaiOffset],  joThisObject, &joClassRef );
   *pjoClassObject = joClassRef;
}

void JvmLdrFindSystemClass(JVM_STRING pszName)
{
}




/**	JVM_BOOL JvmRtInvokeMethod(  
  IN     JVM_STRING jsMethodName, 
  IN     JVM_OBJREF joObjRef,
  IN     JAVA_WORD  * pjawArgs,
  IN     JVM_INT      jiArgCount,
  OUT   _JVM_Field * pjfiReturn
  IN     JVM_Exec_Env * pjeeEnvCaller
 )
 **/

JVM_BOOL InvokeLoadClass(
  IN	 JVM_OBJREF * pjoObjRef,
  IN     JVM_OBJREF joLoader, 
  IN     JVM_STRING jsClassName,
  IN     JAVA_BOOL   jabLinkAndInit
)
{
	JAVA_OBJREF jaoRef;
	JAVA_WORD jawArgs[2];

	JVM_ASSERT( joLoader != PRIMITIVE_LOADER );

#ifndef __JDEPENDS__

	if ( JvmRtCreateObject_JavaLangString_v0( jsClassName, &jaoRef) )
	{
	   jawArgs[0] = (JAVA_WORD)jaoRef;
	   jawArgs[1] = (JAVA_WORD)jabLinkAndInit;
	}

   return JvmRtInvokeMethod( S_METHOD_LOADCLASS, S_DESC_JLCLASS_LOADCLASS, joLoader, jawArgs, 2, JVM_NULL, 
     JvmGetThreadState()->pjeeEnvCaller );
#else
   return JVM_TRUE;
#endif /* __JDEPENDS__ */

}


_JVM_Class_Class * JvmLdrGetExceptionClass( 
  IN 	eJVMExceptionType ejetException, 
  IN    _JVM_Object * pjoClassLoader
)
{ 
  _JVM_Object *pjoClassObject;

  if(ejetException ==  EXC_JAVAVIRTUALMACHINEERROR) {
    JvmLdrLoadNewClass(JVM_SPRING_CREATE("java\\lang\\VirtualMachineError"),
       JVM_NULL, PRIMITIVE_LOADER, &pjoClassObject);
    
   return ((_JVM_Object *)pjoClassObject)->joiObjectID.pjccClass;
  } 
  
	/** TODO **/
	return JVM_NULL;
}

 
 

JVM_FILE JvmOpenClassFile( 
  IN     JVM_STRING jsClassFileName 
)
{
	JVM_FILE jfFile = JvmSysOpenFile( jsClassFileName, JVM_FILE_READ );

    if (!JVM_VALID( jfFile ) )
	{
      JVM_THROW_EXCEPTION( EXC_JVMFILEIOERROR )
	  JVM_ASSERT( JVM_FALSE );
	}

   return jfFile;
}

JVM_PBYTE JvmReadClassFile( 
  IN     JVM_FILE jfiClassFile
)
{
	JVM_PBYTE pjbyStreamBuf = JVM_NULL;
	JVM_DWORD jdwFileSize   = 0;

	if (!JvmSysIsValidFileHandle( jfiClassFile ))
		JVM_THROW_EXCEPTION( EXC_JVMFILEIOERROR )
	   

	pjbyStreamBuf = 
		JvmMemCalloc( jdwFileSize = JvmSysGetFileSize( jfiClassFile ));

	JVM_TRY
	{
	  	JvmSysReadFile( jfiClassFile, &pjbyStreamBuf, jdwFileSize );
	}
	JVM_CATCH_ALL_EXCEPTIONS()
	{
		JvmMemFree( pjbyStreamBuf );
		return JVM_NULL;
	}
	JVM_END_TRY

   return pjbyStreamBuf;
}


JVM_VOID JvmCloseClassFile( 
  IN     JVM_FILE  jfiClassFile,
  IN     JVM_PBYTE * ppjbyClassStream
)
{
	 if ( JvmSysIsValidFileHandle( jfiClassFile ) )
	     JvmSysCloseFile( jfiClassFile );

	 if ( JVM_VALID( ppjbyClassStream ) )
	 {
		 JvmMemFree( *ppjbyClassStream );
		 *ppjbyClassStream = JVM_NULL;
	 }

}

JVM_VOID JvmFreeThePool(
 IN  _JVM_Constant_Pool ** ppjcpThePool, 
 IN  JVM_WORD              jwCPSize
)
{
  register JVM_WORD jwCount = 0;
  if ( !JVM_VALID(ppjcpThePool) )
  {
	  TRACE_PRINT("Empty Constant Pool Passed")
	  return;
  }

  for ( ;jwCount < jwCPSize; jwCount++ )
  {
     if ( ppjcpThePool[ jwCount ] )
		 JvmMemFree( ppjcpThePool[ jwCount ] );
  }


  JvmMemFree( ppjcpThePool );
}



JVM_VOID JvmFreeTheClass(
 IN  _JVM_Class_Class * pjccTheClass 
)
{
    if ( !JVM_VALID(pjccTheClass))
	{
		TRACE_PRINT("Empty Class_Class passed");
		return;
	}

	if ( pjccTheClass->jsClassName )
		JvmSpringFree( &pjccTheClass->jsClassName );


	if ( pjccTheClass->jsSourceFileName )
		JvmSpringFree( &pjccTheClass->jsSourceFileName );


	if ( pjccTheClass->ppjfData )
	{
	    JvmCleanUpFields( 
		   pjccTheClass->ppjfData, 
		   pjccTheClass-> jwFieldsCount
		);
	    pjccTheClass->ppjfData = JVM_NULL; 
	}

	if ( pjccTheClass->ppjmMethods )
	{
	    JvmCleanUpMethods( 
		   pjccTheClass->ppjmMethods, 
		   pjccTheClass-> jwMethodsCount
	   	 );
	    pjccTheClass->ppjmMethods = JVM_NULL; 
	}

	if ( pjccTheClass->pdNCPDict )
	{
		DestroyDictionary( pjccTheClass->pdNCPDict );
		pjccTheClass->pdNCPDict = JVM_NULL;
	}

	if ( pjccTheClass->ppjcpConstantPool )
	{
		JvmFreeThePool(
			pjccTheClass->ppjcpConstantPool,
			pjccTheClass->jwCPEntryCount
        );
 	    pjccTheClass->ppjcpConstantPool = JVM_NULL;
	}

	if ( pjccTheClass->pjfiArrayInfo )
	{
	   JvmParseCleanAFieldInfo( pjccTheClass->pjfiArrayInfo );
	   JvmMemFree( pjccTheClass->pjfiArrayInfo );
	    pjccTheClass->pjfiArrayInfo = JVM_NULL;
    }

	JvmMemFree( pjccTheClass );

}


JVM_VOID JvmCleanUpMethods(
 IN     _JVM_Method ** ppjmMethod, 
 IN      JVM_WORD      jwMethodsCount
)
{
  register JVM_WORD jwCount = 0;

  for ( ; jwCount < jwMethodsCount ; jwCount ++ )
  {
	  if ( ppjmMethod[ jwCount ] )
		  JvmLdrFreeAMethod( ppjmMethod[jwCount] );
  }

  JvmMemFree( ppjmMethod );
}


JVM_VOID JvmLdrFreeAMethod( 
 IN 	_JVM_Method * pjmMethod 
)
{
  register JVM_WORD jwCount = 0;
  if ( !pjmMethod ) return;

  if ( pjmMethod->pjmiMethodInfo )
  {
	if ( pjmMethod->pjmiMethodInfo->pjfiParameterFields &&
			 pjmMethod->pjmiMethodInfo->jiParameterCount > 0)
	{
	   JvmParseCleanupFields(
		   &pjmMethod->pjmiMethodInfo->pjfiParameterFields,
		   pjmMethod->pjmiMethodInfo->jiParameterCount
    	);
	}

  JvmParseCleanAFieldInfo( &pjmMethod->pjmiMethodInfo->jfiReturnField );

	JvmMemFree( pjmMethod->pjmiMethodInfo );
    pjmMethod->pjmiMethodInfo  = JVM_NULL;
  }

  if ( pjmMethod->pjciCodeInfo )
  {
	JvmCleanUpCode(  pjmMethod->pjciCodeInfo );
	pjmMethod->pjciCodeInfo = JVM_NULL;
  }

  if (pjmMethod->pjwMthdExcTable)
  {
    JvmMemFree( pjmMethod->pjwMthdExcTable );
	pjmMethod->pjwMthdExcTable = JVM_NULL;
  }

  JvmMemFree( pjmMethod );

}




JVM_VOID JvmCleanUpFields( 
 IN     _JVM_Field ** ppjfData ,
 IN      JVM_WORD     jwFieldsCount
)
{
  register JVM_INT jwCount = 0;
  if (!ppjfData )
	  return;

  for ( ; jwCount < jwFieldsCount ; jwCount ++ )
  {
	  if ( ppjfData[ jwCount ] )
		  JvmLdrFreeAField( ppjfData[jwCount] );
  }

  JvmMemFree( ppjfData );
}


JVM_VOID JvmCleanUpCode(
 IN		_JVM_ByteCode_Info * pjbiCodeInfo 
)
{
  register JVM_INT jwCount = 0;
  if ( !JVM_VALID(pjbiCodeInfo))
  {
	  TRACE_PRINT("Empty Byte code info passed");
	  return;
  }

  /** Free up ExceptionTable **/
  if ( pjbiCodeInfo->jwExceptionTableCount  && 
	    pjbiCodeInfo->ppjetiExceptionTable )
   {
       for ( ; jwCount <  pjbiCodeInfo->jwExceptionTableCount ; jwCount ++ )
	   {
	   	   if ( pjbiCodeInfo->ppjetiExceptionTable[jwCount] )
			   JvmMemFree( pjbiCodeInfo->ppjetiExceptionTable[jwCount] );
	   }

	   JvmMemFree( pjbiCodeInfo->ppjetiExceptionTable );
	   pjbiCodeInfo->ppjetiExceptionTable = JVM_NULL;
   }

   /** Free up LineNumberTable **/
   if ( pjbiCodeInfo->jwLineNumTableCount &&
        pjbiCodeInfo->ppjltiLineNumTable )
   {
       for ( jwCount = 0; jwCount < pjbiCodeInfo->jwLineNumTableCount; jwCount ++ )
	   { 
		   if ( pjbiCodeInfo->ppjltiLineNumTable[jwCount] )
			   JvmMemFree( pjbiCodeInfo->ppjltiLineNumTable[jwCount] );
	   }

       JvmMemFree( pjbiCodeInfo->ppjltiLineNumTable );
	   pjbiCodeInfo->ppjltiLineNumTable = JVM_NULL;
	   
   }

   /** Free Up LocalVarTable **/
   if ( pjbiCodeInfo->jwLocalVarTableCount &&
        pjbiCodeInfo->ppjltiLocalVarTable )
   {
       for ( jwCount = 0; jwCount < pjbiCodeInfo->jwLocalVarTableCount; jwCount ++ )
	   { 
		   if ( pjbiCodeInfo->ppjltiLocalVarTable[jwCount] )
			   JvmLdrFreeAnLVTable( pjbiCodeInfo->ppjltiLocalVarTable[jwCount] );
	   }      
	   JvmMemFree( pjbiCodeInfo->ppjltiLocalVarTable );
	   pjbiCodeInfo->ppjltiLocalVarTable = JVM_NULL;
   }

   JvmMemFree( pjbiCodeInfo );

}

JVM_VOID JvmLdrFreeAnLVTable( 
 IN     _JVM_LV_Table_Info * pjltiLVTable 
)
{
     if ( JVM_VALID( pjltiLVTable ))
	 {
		 if ( pjltiLVTable->pjfField )
			 JvmLdrFreeAField( pjltiLVTable->pjfField );

		 JvmMemFree( pjltiLVTable );
	 }
}

JVM_VOID JvmLdrFreeAField(
 IN     _JVM_Field * pjfField
)
{
	if ( JVM_VALID( pjfField ))
	{
       if ( pjfField->jsFieldName )
		   JvmSpringFree( &pjfField->jsFieldName );

	   if ( pjfField->jfiFieldType.jsObjectName )
		   JvmSpringFree( &pjfField->jfiFieldType.jsObjectName );

	   /** TODO : Free the data values if necessary here **/

	   JvmMemFree( pjfField );
	}
}


#if defined( __JVM_CHECKTOOL_TEST_ENV__ )

#if 0 

#if !defined(__JVM_RUNTIME_PRESENT__)

JVM_THREAD JvmGetCurrentThread()
{
  JVM_THREAD jtHandle = NULL;
  jtHandle = JvmSysGetCurrentThread();
  return jtHandle;
}

_JVM_Thread_State * JvmGetThreadState()
{
   JVM_THREAD jtHandle = JvmGetCurrentThread();
   _JVM_Thread_State *pjtsState = NULL;

   JVM_ASSERT( jtHandle != NULL);
   JvmSysAcquireMutex( _jmoGGlobalDataMutex );

   /* Access the Thread list */
   pjtsState = _pjtsGStartThread;

   /* Loop thro' until we found the right thread state */
   while ( pjtsState != NULL )
   {
     if ( pjtsState->jtHandle == jtHandle ) break;
     else
  	    pjtsState = pjtsState->pjtsNext;
   }

   JvmSysReleaseMutex( _jmoGGlobalDataMutex );

   /* New Thread */
   if ( pjtsState == NULL )
   	  pjtsState = JvmThreadInit(jtHandle);

   return pjtsState;
}

_JVM_Thread_State * JvmThreadInit(JVM_THREAD jtHandle)
{
  _JVM_Thread_State *pjtsThreadState = JVM_NULL;
  _JVM_Thread_State *pjtsState       = JVM_NULL;

  JVM_ASSERT_VALID( jtHandle );

  /* Can't use JVM-TRY-CATCH just yet */
   pjtsThreadState = (_JVM_Thread_State * )
	                 JvmMemCalloc( sizeof(_JVM_Thread_State) );
   pjtsThreadState->jtHandle = jtHandle;
   pjtsThreadState->jnThreadPriority =  JvmSysSetThreadPriority(jtHandle,
									      JVM_THREAD_NORMAL_PRIORITY);
   

   // should be NULL now ; will be initialized
   // by appropriate functions
   pjtsThreadState->pjefTopFrame          = JVM_NULL;
   pjtsThreadState->pjeiErrorStack        = JVM_NULL;
   pjtsThreadState->pjdehDefExHandler     = JvmDefaultExceptHandler;
   pjtsThreadState->pjtwmThdWorkingMemory = JVM_NULL;
   pjtsThreadState->pjmmMainMemory        = JVM_NULL;
   pjtsThreadState->pjsStack              = JVM_NULL;
   pjtsThreadState->jbTempBuf[ 0 ]        = 0;
   pjtsThreadState->jwThreadState         = JVM_THREAD_RUNNING;
 
   JvmSysAcquireMutex( _jmoGGlobalDataMutex );

   pjtsThreadState->pjtsNext = _pjtsGStartThread;

   _pjtsGStartThread    = 	pjtsThreadState;

   JvmSysReleaseMutex( _jmoGGlobalDataMutex );

   return pjtsThreadState;
}

JVM_VOID JvmThreadExit( _JVM_Thread_State *pjtsThread )
{
  JVM_ASSERT_VALID( pjtsThread );

  /* To be continued after runtime implementation
      becomes clear 
  */

}


JVM_OBJREF JvmRtCreateNewInstanceEx( _JVM_Class_Class *pjccTheClass )
{
    _JVM_Object * pjoObject = JVM_NULL;
  	 JVM_INT jiIndex = 0;
	_JVM_Class_Class * pjccAClass = JVM_NULL;
	 JVM_STRING jsName = JVM_NULL;

	 JVM_ASSERT_VALID( pjccTheClass );

    if ( pjccTheClass->jdwClassFlags & ACC_ABSTRACT  ||
       pjccTheClass->jdwClassFlags & ACC_INTERFACE )
    {
       pjoObject = JvmMemCalloc( sizeof( _JVM_Object ));
	     JVM_STATE_CHECK( pjoObject, EXC_JAVAINSTANTIATIONERROR )

       pjoObject->joiObjectID.jbyID = REF_TYPE_CLASS; 
       pjoObject->joiObjectID.pjccClass    = pjccTheClass;

       pjoObject->pjwsWaitSet  = JvmMemCalloc( sizeof( _JVM_Wait_Set ));
       JVM_STATE_CHECK( pjoObject->pjwsWaitSet, EXC_JAVAINSTANTIATIONERROR )

       pjoObject->pjesEntrySet = JvmMemCalloc( sizeof( _JVM_Entry_Set ));
       JVM_STATE_CHECK( pjoObject->pjesEntrySet, EXC_JAVAINSTANTIATIONERROR )

	   pjoObject->pjlLock      = JvmMemCalloc( sizeof( _JVM_Lock ));
       JVM_STATE_CHECK( pjoObject->pjesEntrySet, EXC_JAVAINSTANTIATIONERROR )
	   pjoObject->pjtsOwner    = JvmGetThreadState();
    }
    else
	   return JvmRtCreateNewInstance( pjccTheClass );

    return pjoObject;
}


JVM_OBJREF JvmRtCreateNewInstance( _JVM_Class_Class *pjccTheClass )
{
	_JVM_Object * pjoObject = JVM_NULL;
	 JVM_INT jiIndex = 0;
	 JVM_U16 ju16ClassesCount = 0;
	 JVM_U16 ju16FieldCount = 0,ju16FieldIndex = 0;
	_JVM_Field ** ppjfFields = JVM_NULL;
	_JVM_Field *  pjfField = JVM_NULL;
	_JVM_Class_Class * pjccAClass = JVM_NULL;
	JVM_STRING jsName = JVM_NULL;

	JVM_ASSERT_VALID( pjccTheClass );

	if ( pjccTheClass->jdwClassFlags & ACC_ABSTRACT )
	{
		JAVA_THROW_EXCEPTION( EXC_JAVAINSTANTIATIONERROR );
		JVM_ASSERT( JVM_FALSE );
	}

	pjoObject = JvmMemCalloc( sizeof( _JVM_Object ));
	JVM_STATE_CHECK( pjoObject, EXC_JAVAINSTANTIATIONERROR )
    pjoObject->joiObjectID.pjccClass    = pjccTheClass;
    pjoObject->pjwsWaitSet  = JvmMemCalloc( sizeof( _JVM_Wait_Set ));
    JVM_STATE_CHECK( pjoObject->pjwsWaitSet, EXC_JAVAINSTANTIATIONERROR )
	pjoObject->pjesEntrySet = JvmMemCalloc( sizeof( _JVM_Entry_Set ));
    JVM_STATE_CHECK( pjoObject->pjesEntrySet, EXC_JAVAINSTANTIATIONERROR )
	pjoObject->pjlLock      = JvmMemCalloc( sizeof( _JVM_Lock ));
    JVM_STATE_CHECK( pjoObject->pjesEntrySet, EXC_JAVAINSTANTIATIONERROR )
	pjoObject->pjtsOwner    = JvmGetThreadState();

	if ( pjccTheClass->jdwClassFlags & ACC_INTERFACE )
		pjoObject->joiObjectID.jbyID = REF_TYPE_INTERFACE;
	else 
		pjoObject->joiObjectID.jbyID = REF_TYPE_CLASS;


	/** CHECK,CHECK - What if a class has no super class?????? **/

	/** CHECK,CHECK - can we optimize this by redesigning?? **/
	/** Copy all fields to the object's own **/
    pjccAClass = pjccTheClass;

    for ( ;  pjccAClass  ;)
    {   
	  ju16ClassesCount++;
	  ju16FieldCount += pjccAClass->jwFieldsCount;
      pjccAClass =  pjccAClass->joSuperClassObject ?
     		pjccAClass->joSuperClassObject->joiObjectID.pjccClass : JVM_NULL;
    }

	if ( ju16ClassesCount )
      pjoObject->pppjfFields = ( _JVM_Field ***) JvmMemCalloc(
	      ju16ClassesCount * sizeof( _JVM_Field ** ) );
    JVM_ALLOC_CHECK( pjoObject->pppjfFields )

    jiIndex = 0;
    for ( pjccAClass = pjccTheClass;  pjccAClass  ; )
	{
       if (  pjccAClass->jwFieldsCount )
	   {
	     ppjfFields = pjoObject->pppjfFields[ jiIndex ] = (_JVM_Field **)
		     JvmMemCalloc( sizeof( _JVM_Field *) * pjccAClass->jwFieldsCount );
	     JVM_ALLOC_CHECK( ppjfFields )
	   }

	   for( ju16FieldIndex = 0 ; ju16FieldIndex < pjccAClass->jwFieldsCount;
   	        ju16FieldIndex ++ )
	   {
	       pjfField = ( _JVM_Field * )JvmMemCalloc( sizeof( _JVM_Field ));
           JVM_ALLOC_CHECK( pjfField ) 
           JvmMemCpy(pjfField, pjccAClass->ppjfData[ ju16FieldIndex],
		                       sizeof( _JVM_Field));
           if ( pjfField->jfiFieldType.jwFieldType & JVM_OBJECT_TYPE )
		   {
			 jsName = pjfField->jfiFieldType.jsObjectName;
             pjfField->jfiFieldType.jsObjectName = (JVM_STRING) 
  			  JvmMemCalloc( pjfField->jfiFieldType.jiObjectNameLen + 1 ); 
 			 JvmStrCpy( pjfField->jfiFieldType.jsObjectName, jsName);
		   }

		   ppjfFields[ ju16FieldIndex ] = pjfField;
	   }

      pjccAClass = pjccAClass->joSuperClassObject  ? 
		  pjccAClass->joSuperClassObject->joiObjectID.pjccClass : JVM_NULL;
	  ++ jiIndex;
	}

   /** Can't escape -- time to think about GC !! **/

   JvmGCAddNewObject( pjoObject );

   /** All set **/
   return pjoObject;

  /** TODO **/
}


JVM_BOOL JvmGCAddNewObject( JVM_OBJREF pjoObjRef )
{
	// return  BinTreeAddNode( JVM_GC_OBJECT_TREE, (JVM_U32)pjoObject ) );
//	JvmDisplay( "Invoking Dummy JvmGCAddNewObject for CHECKTOOL Env" );

	return JVM_TRUE;
}

JVM_BOOL JvmGCInit()
{
	JvmDisplay( "Invoking Dummy JvmGCInit for CHECKTOOL Env" );
//	JVM_GC_OBJECT_TREE = BinTreeGetNewTree();
//	JVM_ALLOC_CHECK( JVM_GC_OBJECT_TREE );
	return JVM_TRUE;
}

JVM_BOOL JvmRtCreateObject_JavaLangString_v0( 
 IN  JVM_STRING jsName,
 IN  JAVA_OBJREF * pjaoJavaLangStringRef 
)
{
   /** DUMMY for __JVM_CHECKTOOL_TEST_ENV__ **/

   JvmDisplay( "Invoking Dummy JvmRtCreateObject_JavaLangString_v0 for CHECKTOOL Env" );
   return JVM_TRUE;
}


JVM_VOID JvmRtDestroyStackFrame()
{
   JvmDisplay( "Invoking Dummy JvmRtDestroyStackFrame for CHECKTOOL Env" );
}


JVM_BOOL JvmRtInvokeMethod(  
  IN     JVM_STRING jsMethodName,
  IN     JVM_STRING jsMethodDesc,
  IN     JVM_OBJREF joObjRef,
  IN     JAVA_WORD  * pjawArgs,
  IN     JVM_INT      jiArgCount,
  OUT   _JVM_Field * pjfiReturn,
  IN    _JVM_Exec_Env * pjeeEnvCaller
)
{
	JvmDisplay( "Invoking Dummy JvmRtInvokeMethod for CHECKTOOL Env" );
	return JVM_TRUE;
}

#endif /** __JVM_RUNTIME_PRESENT__ **/

#endif /** 0 **/


JVM_VOID JvmInit()
{
	Hash_Type htinvalid;
	G_STATE  go_state;
	G_STATE_P state = &go_state;

	RESET_STATE( state );

	JvmSysOSInit( state);

	htinvalid.u.pv = JVM_NULL;

#ifdef __DEBUG__
	CreateDictionary(
		 "LdrClassDict",
		 &_pdictGCLdrClass,
     htinvalid,
     HASH_TYPE_PTR,
     HASH_TYPE_PTR,
     HashDictionaryRundown
	 );
   CreateDictionary(
	   "LdrClassStateDict",
	   &_pdictGCLdrClassState,
     htinvalid,
     HASH_TYPE_PTR,
     HASH_TYPE_PTR,
     HashDictionaryRundown
	 );
#else
   CreateDictionary(
		 &_pdictGCLdrClass,
     htinvalid,
     HASH_TYPE_PTR,
     HASH_TYPE_PTR,
     HashDictionaryRundown
	 );
   CreateDictionary(
	   &_pdictGCLdrClassState,
     htinvalid,
     HASH_TYPE_PTR,
     HASH_TYPE_PTR,
     HashDictionaryRundown
   );
#endif

  JvmExceptInit(state);
	JvmSysInitMutex( &_jmoGGlobalDataMutex );
  JvmSysInitMutex( &_jmoGExclCodeMutex ); 
  JvmSysInitMutex( &_jmoGClassStateMutex ); 
  JvmSysInitMutex( &_jmoGCLdrDictMutex ); 
	JvmSysInitMutex( &_jmoGCLdrStateMutex ); 
  JvmSysInitMutex( &_jmoGAttrVtblMutex); 
	JvmSysInitMutex( &_jmoGSpringMutex); 
	JvmSysInitMutex( &_jmoGConfigMutex);
  JvmSysInitMutex( &_jmoGFileHandleAccessMutex); 
	JvmSpringInit(state);
	JvmSysInit(state);
	JvmLdrInit(state);
	JvmRtInit( state );
	JvmConfigInit(state);

	JvmGetThreadState()->SystemState = go_state; 
  JvmSysInitComplete(state);

}

JVM_VOID HashDictionaryRundown( 
 IN 	 Hash_Type  htype
 )
{
  Hash_Table * phTable;
  phTable = (Hash_Table *)htype.u.pv;
  if ( phTable )
    DestroyHashTable( phTable );
}



JVM_VOID JvmExit()
{
	DestroyDictionary( _pdictGCLdrClass );
  DestroyDictionary( _pdictGCLdrClassState );
	JvmConfigExit();
	JvmLdrExit();
  JvmSpringExit();
  JvmSysKillMutex( &_jmoGGlobalDataMutex );
  JvmSysKillMutex( &_jmoGExclCodeMutex ); 
	JvmSysKillMutex( &_jmoGClassStateMutex ); 
  JvmSysKillMutex( &_jmoGCLdrDictMutex ); 
	JvmSysKillMutex( &_jmoGCLdrStateMutex ); 
  JvmSysKillMutex( &_jmoGAttrVtblMutex); 
	JvmSysKillMutex( &_jmoGSpringMutex); 
	JvmSysKillMutex( &_jmoGFileHandleAccessMutex); 

  JvmSysBeginExit();
	JvmSysEndExit();
	
}

/** Roll the Ball **/
void JvmStartTheMusic(int argc, char ** argv )
{
#if 0
  JVM_STRING jsHWClass;
  JVM_OBJREF joHWClassObject;
#endif
  int i=0;

  JvmInit();
  JvmConfigParse(argc,argv);

/*  for ( i = 1; i < 10002 ; i++ )
  {
	  p = (char *)JvmMemAlloc( 87 );
  }

  strcpy( p, "Hellllllllllllllllllllllllllllllllllll0");

  for ( i = 1; i < 10002 ; i++ )
  {
	  JvmMemRealloc( p, 15 );
  }
  JvmMemDump(); */
/*  JvmLdrLoadSystemClasses(&_jaoGPrimitiveLoader);

  jsHWClass = JVM_SPRING_CREATE("main.class");
  JvmLdrLoadNewClass( jsHWClass, JVM_NULL, 
    &_jaoGPrimitiveLoader, &joHWClassObject );

  JAVA_ARRAYREF jarArgs;
  JvmConfigGetJavaArgs( &jarArgs );
  
  JvmRtCallMethodByName(JVM_SPRING_CREATE("main"),
	  JVM_SPRING_CREATE("main.class"),
	  jarArgs, JvmConfigGetJavaArgCount() 
  ); */

/* JvmRtCallMethod( JVM_NULL,
 IN     _JVM_Method * pjmMethod,
 IN     _JVM_Class_Class *pjccClass,
 IN      JVM_OBJREF joObjRef,
 IN      JAVA_WORD  * pjawArgs,
 IN      JVM_INT      jiArgCount,
 IN OUT _JVM_Field  * pjfiReturn
)
*/

  JvmExit();
}




JvmCallMain(int argc, char ** argv)
{

}

#endif /** __JVM_CHECKTOOL_TEST_ENV__ **/


