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

#include "stdafx.h"
#include "jdepends.h"
#include "ClassInfo.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CClassInfo

// Static Initialization
CPtrArray CClassInfo::m_stInfoArray;


IMPLEMENT_DYNAMIC( CClassInfo, CObject )
/*
 *  Construct a classinfo given a classname and path
 */
CClassInfo::CClassInfo(PSTR pszClassName,PSTR pszPath, BOOL bFlag)
:  	m_pszClassName(NULL),m_pszClassPath(NULL) 
{
	if ( pszClassName ){
		m_pszClassName = JvmDGetSpringFromCString(pszClassName);
	}
  

  if ( pszPath && *pszPath ) {
		m_pszClassPath = new char[strlen(pszPath ) + 1];
		strcpy( m_pszClassPath, pszPath );
	}		

	m_pChild = m_pSibling = NULL;
	m_pszJarName = NULL;
	m_hti = NULL;
  m_dwCFlags = 0L;
  m_pDInfo = NULL;
  m_nRefCount = 1;
  m_uiClassFlags = m_nInterfaceCount = 0;
}

/*
 *  Set the class path
 */
void CClassInfo::SetClassPath(PSTR path )
{
  ASSERT( path != NULL);
  /* set class path */
  if (!m_pszClassPath) {
    m_pszClassPath = new char[strlen( path ) + 1 ];
    strcpy ( m_pszClassPath, path);
  }
}





/*
 *  Set Jar file name && class name
 */
void CClassInfo::SetJarName(PSTR jar)
{
  ASSERT( jar != NULL);
  /* First set JAR name */
  
  if ( m_pszJarName ) JvmDFreeSpring(&m_pszJarName);
  m_pszJarName = JvmDGetSpringFromCString(jar);
  

  /* then set class name */
  if (m_pszClassName ) {
    if ( m_pszClassPath ) delete [] m_pszClassPath;
    m_pszClassPath = new char[strlen( jar ) + strlen( m_pszClassName ) + 5 ];
    sprintf(m_pszClassPath, "%s (%s)", m_pszClassName, jar);
  }
}

/*
 *  Return stripped class name
 */
PSTR CClassInfo::GetShortClassName(PSTR pszClassName)
{
  ASSERT( pszClassName != NULL );
  PSTR p = strrchr( pszClassName, '\\');
  if ( p )
    return p + 1;
  p = strrchr(pszClassName,'/');

  return  p ? p+1 :pszClassName ;
}

/*
 *  Get fields given an index
 */
char * CClassInfo::GetFields(int index)
{
  ASSERT( m_pDInfo != NULL);
  ASSERT( index >= 0 && index  < m_pDInfo->fields_count );
  return m_pDInfo->fields[index].field_type_name;
}

/*
 *  Get fields count
 */
int CClassInfo::GetFieldsCount()
{
  ASSERT( m_pDInfo != NULL);
  return m_pDInfo->fields_count;
}

/*
 *  Get method's bytecode count
 */
unsigned long CClassInfo::GetMethodByteCodeCount(int index)
{
   ASSERT( m_pDInfo != NULL);
   ASSERT( index >= 0 && index  < m_pDInfo->methods_count);

   return  m_pDInfo->methods[index].byte_code_count;
}

/*
 *  Extract byte codes for the method @ index
 */
void CClassInfo::ExtractMethodByteCodes( int index )
{
   ASSERT( m_pDInfo != NULL );
   ASSERT( index >= 0 && index  < m_pDInfo->methods_count );

   // Actually get formatted bytecodes, if necessary
   if (!m_pDInfo->methods[index].byte_codes )
     JvmDGetByteCodes( &m_pDInfo->methods[index] );
}


/*
 *  Extract and print byte codes 
 */
BOOL CClassInfo::GetMethodByteCodes( int index, CString& bcode )
{
   ExtractMethodByteCodes( index );
   // format and write to buffer
   bcode += m_pDInfo->methods[index].byte_codes;
   return TRUE;
}


/*
 *  Get Field attribute names
 */
BOOL CClassInfo::GetFieldAttributeNames(  int index, CString& attr )
{
   ASSERT( m_pDInfo != NULL);
   ASSERT( index >= 0 && index  < m_pDInfo->fields_count);
   unsigned long flags;
   // Get Access attributes
   flags = JvmDGetFieldAttributes( &m_pDInfo->fields[index] );
   return GetCommonAttributeNames( flags, attr );
}

/*
 *  Helper for getting Common attribute names
 */
BOOL CClassInfo::GetCommonAttributeNames( unsigned long flags, CString& attr)
{
   if ( flags & FLAG_DEPRECATED )
      attr += "DEPRECATED ";
   if ( flags & ACC_PUBLIC )
      attr += "public ";
   else if ( flags & ACC_PRIVATE )
      attr += "private ";
   else if ( flags & ACC_PROTECTED )
      attr += "protected ";

   // Get qualifiers
   if ( flags & ACC_SYNCHRONIZED )
     attr += "synchronized ";
   if ( flags & ACC_VOLATILE )
     attr += "volatile ";
   if ( flags & ACC_FINAL )
     attr += "final ";
   if ( flags & ACC_STATIC )
     attr += "static ";
   if ( flags & ACC_ABSTRACT )
     attr += "abstract ";
   if ( flags & ACC_TRANSIENT )
     attr += "transient ";
   if ( flags & ACC_NATIVE )
     attr += "native ";
   if ( flags & ACC_STRICT )
     attr += "strict ";
   if ( flags & FLAG_SYNTHETIC )
     attr += "synthetic ";

   return TRUE;
}

/*
 *  Get Method attribute names
 */
BOOL CClassInfo::GetMethodAttributeNames( int index, CString& attr )
{
   ASSERT( m_pDInfo != NULL);
   ASSERT( index >= 0 && index  < m_pDInfo->methods_count);
   // Declarations
   unsigned long flags;
   // Get Access attributes
   flags = JvmDGetMethodAttributes( &m_pDInfo->methods[index] );

   return GetCommonAttributeNames( flags, attr );
}

/*
 *  Get Thrown exception count
 */
int CClassInfo::GetMethodExceptionsCount(int index)
{
   ASSERT( m_pDInfo != NULL);
   ASSERT( index >= 0 && index  < m_pDInfo->methods_count);

   return m_pDInfo->methods[index].exceptions_count;
}

/*
 *  Get Method Exception names
 */
BOOL CClassInfo::GetMethodExceptions( int index, CString& exc )
{
   ASSERT( m_pDInfo != NULL);
   ASSERT( index >= 0 && index  < m_pDInfo->methods_count);

   _JVM_DMethod_Info * pmi = &m_pDInfo->methods[index];
   
   // Loop and obtain exception classes
   // NOTE,NOTE add stripping of full names later based on menu selection
   for ( int i = 0; i < pmi->exceptions_count; i++) {
      exc += (i == 0)? "" :", ";
      exc += pmi->exception_classes[i];
   }

   return TRUE;
}

/*
 *  Get short method name
 */
char * CClassInfo::GetMethodName( int index )
{
   ASSERT( m_pDInfo != NULL);
   ASSERT( index >= 0 && index  < m_pDInfo->methods_count);  

   return m_pDInfo->methods[index].method_name;
}

/*
 *  Get Full method name. 
 *  Method name finally looks like <ret_type> <method_name>( <p1>, <p2>, ... ).
 */
BOOL CClassInfo::GetMethodFullName(int index, CString& mname )
{
   ASSERT( m_pDInfo != NULL);
   ASSERT( index >= 0 && index  < m_pDInfo->methods_count);

   _JVM_DMethod_Info * pmi =  &m_pDInfo->methods[index];

   FillFullMethodName ( pmi, mname );

   return TRUE;
}

/*
 *  Get Call Graph method display string
 *  This has a class prefix
 */
void CClassInfo::GetCGMethodSignature( _JVM_DCall_Graph * graph, CString& cs)
{
  _JVM_DMethod_Info * pmi = graph->jmethod;

  char * p = strrchr(pmi->ret_info.field_type_name, '/');
  cs += p? p+1: pmi->ret_info.field_type_name;
  cs += " ";
  cs += GetShortClassName( graph->jclass->class_name );
  cs += "::";
  cs += pmi->method_name;

  cs += " ( ";
  // Add parameters
  for ( int i = 0; i < pmi->param_count ; i++)
  {
    if ( i )
      cs += ", ";
    p = strrchr(pmi->parameters[i].field_type_name, '/');
    cs += p ? p + 1 : pmi->parameters[i].field_type_name;
  }
  cs += " )";

}

/*
 *  Helper that actually fills full method name
 */
void CClassInfo::FillFullMethodName(_JVM_DMethod_Info * pmi, CString& mname )
{
   // Strip path information from return type and parameter names
   // Aesthetics sake.
   char * p = strrchr(pmi->ret_info.field_type_name, '/');
   // Method name finally looks like <ret_type> <method_name>( <p1>, <p2>, ... )
   mname += p? p+1: pmi->ret_info.field_type_name;
   mname += " ";
   mname += pmi->method_name;
   mname += " ( ";

   // Add parameters
   for ( int i = 0; i < pmi->param_count ; i++)
   {
      if ( i )
        mname += ", ";
      p = strrchr(pmi->parameters[i].field_type_name, '/');
      mname += p ? p + 1 : pmi->parameters[i].field_type_name;
   }

   mname += " )";
}


/*
 *  Gets Number of methods
 */
int CClassInfo::GetMethodsCount()
{
  ASSERT( m_pDInfo != NULL);
  return m_pDInfo->methods_count;
}

/*
 *  Get referred class @ specified index
 */
char * CClassInfo::GetReferredClass(int index)
{
  ASSERT( m_pDInfo != NULL);
  return m_pDInfo->all_referred_classes[index];
}

/*
 *  Get number of referred classes
 */
int CClassInfo::GetReferredClassesCount()
{
  ASSERT( m_pDInfo != NULL);
  return m_pDInfo->referred_classes_count;
}

/*
 *  Get contained class @ index
 */
char * CClassInfo::GetContainedClass(int index)
{
  ASSERT( m_pDInfo != NULL);
  return m_pDInfo->contained_classes[index];
}

/*
 *  Gets number of contained classes, which is 
 *  actually the number of fields that are references
 */
int CClassInfo::GetContainedClassesCount()
{
  ASSERT( m_pDInfo != NULL);
  return m_pDInfo->contained_classes_count;
}

/*
 *  Get no of items in constant pool
 */
int CClassInfo::GetCPInfoCount() 
{
  ASSERT( m_pDInfo != NULL);
  return m_pDInfo->cp_info_count;
}

/*
 *  Get Constant pool string @ specified index
 */
char * CClassInfo::GetCPInfoString(int index)
{
  ASSERT( m_pDInfo != NULL);
  return m_pDInfo->cp_info[ index ].string;
}

/*
 *  Get Constant pool item type @ specified index
 *  This is basically the 'CONSTANT_<xxx>' types 
 *  defined in JVM Spec.
 */
char * CClassInfo::GetCPInfoType(int index )
{
  ASSERT( m_pDInfo != NULL);
  return (char *)m_pDInfo->cp_info[ index ].type;
}

/*
 *  Get the actual bytes of the constant pool item @ specified index
 */
BOOL CClassInfo::GetCPInfoBytes(int index, CString& pinfo )
{
  ASSERT( m_pDInfo != NULL);
  CString tmp;

  // Loop thro and write byte - by -byte 
  // May be need to optimize, if necessary  
  for (unsigned int i = 0; i < m_pDInfo->cp_info[index].len_bytes; i++){
    tmp.Format(" %02x", m_pDInfo->cp_info[index].bytes[i] ); 
    pinfo += tmp;
  }

  return TRUE;
}

/*
 *  Gets the length of the bytes of CP item @ specified index
 */
int CClassInfo::GetCPInfoLength( int index )
{
   ASSERT( m_pDInfo != NULL);
   return m_pDInfo->cp_info[index].len_bytes;
}

/*
 *  Returns a method call graph, given a method index
 */
_JVM_DCall_Graph * CClassInfo::GetCallGraph(int index)
{
  ASSERT( m_pDInfo != NULL);
  ASSERT( index >= 0 && index  < m_pDInfo->methods_count);
  // call helper
  return GetCallGraph(NULL, m_pDInfo, &m_pDInfo->methods[index]) ;
}


/*
 *  Returns a method call graph, given a tree,class and a method depth level 2
 */
_JVM_DCall_Graph *  CClassInfo::GetCallGraph(_JVM_DCall_Graph * graph, 
                                             _JVM_DClass_Info * pclass,
                                             _JVM_DMethod_Info * pmethod)
{
  _JVM_DCall_Graph * pGraph = NULL; 
  // Get the root
  pGraph = JvmDGetCallGraph( graph, pclass, pmethod );
  // And each of its nodes
  for ( int i = 0; i < pGraph->nodes_len; i++ )
    JvmDGetCallGraph( pGraph->nodes[i], pGraph->nodes[i]->jclass, pGraph->nodes[i]->jmethod);
  return pGraph;
}

/*
 *  Gets interface names based on a given index
 */
char * CClassInfo::GetInterfaceName(int index)
{
  return m_pDInfo ? JvmDGetInterfaceName( m_pDInfo, index) : NULL;
}

char * CClassInfo::GetSourceFileName()
{
  return m_pDInfo ? JvmDGetSourceFileName( m_pDInfo ) : NULL;
}


/*
 *  Loads a class ( initialized during construction )
 *  Recursively loads super classes and all the interfaces
 */  
int CClassInfo::LoadClass()
{
  char path[_MAX_PATH] = {0};
  CClassInfo *pChild, *pSibling, *pNext;
  pChild = pSibling = NULL;
  int jiCount = 0;
  char *pSuperClassName, *pInterfaceName;

  if ( !m_pszClassName )
	  return FALSE;
  m_pDInfo = m_pszJarName ? JvmLoadDependsClassFromJAR((char *)m_pszJarName, m_pszClassName) : 
                         JvmLoadDependsClassFromFile((char *)m_pszClassName );
  if ( m_pDInfo )
  {
    if (JvmDIsAnInterface( m_pDInfo ))
       m_dwCFlags |=  JDC_IS_INTERFACE;
    if ( JvmDIsFinal( m_pDInfo ) )
       m_dwCFlags |=  JDC_IS_FINAL;
    if (JvmDIsAbstract( m_pDInfo ))
       m_dwCFlags |=  JDC_IS_ABSTRACT;

    // Load super class
	  if (JvmDHasSuperClass( m_pDInfo )) 
	  {
       pSuperClassName = JvmDGetSuperClassName( m_pDInfo );
       path[0] = 0;
  	   pChild =  new CClassInfo(  pSuperClassName );
       CClassInfo::AddClassInfoToList( pChild );
	     CClassInfo::FindClassPath(pSuperClassName, path, pChild); 
       CClassInfo::IncreaseRefCount(pChild);
	     SetChild( pChild );
	  }

	  // Load interfaces
	  if (jiCount = JvmDHasInterface( m_pDInfo )) 
	  {
 	    CClassInfo * pFirst = pChild;
	    for ( int i = 0; i < jiCount; i++ ) 
	    {
        pInterfaceName = JvmDGetInterfaceName( m_pDInfo, i );
        path[0] = 0;
  		  pNext = new CClassInfo( pInterfaceName );
        CClassInfo::AddClassInfoToList( pNext );
	      CClassInfo::FindClassPath( pInterfaceName , path, pNext );
        if ( pFirst && pNext ) {
             CClassInfo::IncreaseRefCount(pNext);
             pFirst->SetSibling( pNext );
        }
		    pFirst = pNext;
	    }
      m_nInterfaceCount = jiCount;
    } // if
    
    JvmDGetFieldsInfo( m_pDInfo );
    JvmDGetMethodsInfo( m_pDInfo, NULL, NULL, NULL );
    JvmDGetContainedClasses( m_pDInfo );
    JvmDGetReferredClasses( m_pDInfo );
    JvmDGetCPInfo( m_pDInfo ); 
    m_uiClassFlags = JvmDGetClassAttributes( m_pDInfo );
   
    if ( m_pChild )
	    m_pChild->LoadClass();

    // Recurse on sibling
    if( m_pSibling )
	    m_pSibling->LoadClass();

    return TRUE;
  } // if

  return FALSE;
}

/*
 *  Set child info
 */
inline void CClassInfo::SetChild( CClassInfo * pChild )
{  this->m_pChild = pChild;  }
/*
 *  Set sibling info
 */
inline void CClassInfo::SetSibling( CClassInfo * pSibling )
{  this->m_pSibling = pSibling; }


/*
 *  static function to establish a class path for a file
 */
long CClassInfo::FindClassPath(PSTR pszClass, PSTR pszPath, CClassInfo * pInfo)
{
  char  szFullName[_MAX_PATH + 1];	 
  char  * pszRetName = szFullName;
  char * pszName;
  int len = _MAX_PATH;
  long ret;
  int r = REGULAR_CLASS;

  ASSERT(pszClass != NULL );

  pszName = new char[strlen(pszClass) + 10];
  add_file_ext(pszClass, ".class", pszName );
  
  /* Try locating the class file */
  ret = locate_class_file(getenv("CLASSPATH"), pszName, pszRetName, &len);
  if ( ret == AP_CLASS_NOT_FOUND ) {  /* Class not found */
     if ( pszName ) 
        delete [] pszName;
     return CLASS_NOT_FOUND;
  } else if ( ret == AP_INVALID_LEN) { /* insufficient length */
     pszRetName = new char[len + 1];
     ret = locate_class_file(getenv("CLASSPATH"), pszName, pszRetName, &len);   
     ASSERT( ret == AP_CLASS_FOUND);
  } else if ( ret == AP_CLASS_IN_JAR) { /* pszRetName has JAR name in it */
    if ( pInfo ) { pInfo->SetJarName( pszRetName ); }
    r = JARRED_CLASS;
  }

  strcpy( pszPath, pszRetName );
  if ( pInfo ) pInfo->SetClassPath( pszRetName );

  if ( pszRetName != szFullName )
     delete [] pszRetName;

  if ( pszName ) 
    delete [] pszName;

  return r;
}

/*
 *  Kick Start
 */
void CClassInfo::Init() 
{
  m_stInfoArray.SetSize( 10, 10); 
  JvmDependsInit();
}

/*
 *  Shut down
 */
void CClassInfo::Exit() 
{
  CClassInfo::FreeClassInfoFromList();
  JvmDependsExit();
}

/*
 *  Destroy
 */
CClassInfo::~CClassInfo()
{
  CClassInfo::DecreaseRefCount( m_pChild );
  CClassInfo::DecreaseRefCount( m_pSibling );

  if ( m_pszClassName ) {
     JvmDFreeSpring(&m_pszClassName);
	 m_pszClassName = NULL; 
  }

  if ( m_pszClassPath )
	delete [] m_pszClassPath;
	    
  if(m_pszJarName) {
	  JvmDFreeSpring(&m_pszJarName);
  }

	JvmFreeDependsClass( m_pDInfo );
}

/*
 *  Add the class info from list 
 */ 
void CClassInfo::AddClassInfoToList( CClassInfo * pInfo )
{
  if ( pInfo )
    m_stInfoArray.Add( pInfo );
}

/*
 *  Free the class info list
 */
void CClassInfo::FreeClassInfoFromList()
{
  int size = m_stInfoArray.GetSize();
  CObject * ptr = NULL;

  for (int i = 0 ;i < size ; i++)
  {
    ptr = (CObject *)m_stInfoArray.GetAt(i);

    if (ptr && ptr->IsKindOf(RUNTIME_CLASS(CClassInfo)))
       CClassInfo::DecreaseRefCount((CClassInfo *)ptr );
  }

  m_stInfoArray.RemoveAll();
}

void CClassInfo::DecreaseRefCount( CClassInfo * ptr )
{
  if ( ptr ) {
    if ( 0 == InterlockedDecrement(&ptr->m_nRefCount))
      delete ptr;
  }
}

void CClassInfo::IncreaseRefCount( CClassInfo * ptr )
{
  if ( ptr ) 
    InterlockedIncrement(&ptr->m_nRefCount);
}



/////////////////////////////////////////////////////////////////////////////
// CClassInfo message handlers

