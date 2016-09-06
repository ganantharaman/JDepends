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
#if !defined(AFX_CLASSINFO_H__3500EC41_3FC0_11D3_A73F_90FA46C10127__INCLUDED_)
#define AFX_CLASSINFO_H__3500EC41_3FC0_11D3_A73F_90FA46C10127__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ClassInfo.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CClassInfo window
#include "../cmn/jdp.h"


enum {
  CLASS_NOT_FOUND = 0,
  JARRED_CLASS,
  REGULAR_CLASS
};


class CClassInfo : public CObject
{
  // Data members
	CClassInfo * m_pChild;
	CClassInfo * m_pSibling;
	PSTR         m_pszClassName;
	PSTR         m_pszClassPath;
	STREAM       m_ByteCodeStream;
	UINT         m_uiClassMask;
  DWORD        m_dwCFlags;
 _JVM_DClass_Info * m_pDInfo;
  HTREEITEM    m_hti;
  PSTR         m_pszJarName;
  int          m_nInterfaceCount;
  unsigned long m_uiClassFlags;

  // List of pointers
  static CPtrArray m_stInfoArray;
  
  enum {
    JDC_IS_INTERFACE = 0x00000001,
    JDC_IS_FINAL     = 0x00000002,
    JDC_IS_ABSTRACT  = 0x00000004
  };

  
// Construction
public:
	CClassInfo(PSTR pszClassName,PSTR pszClassPath = NULL, BOOL bFlag = 0);

// Operations
public:
  // class load
	int LoadClass();

  // class statics
  // init && exit
  static void Init();
	static void Exit();
  // List Management
  static void AddClassInfoToList(CClassInfo *);
  static void FreeClassInfoFromList();
  // Ref counting
  static void DecreaseRefCount( CClassInfo * ptr );
  static void IncreaseRefCount( CClassInfo * ptr );
  LONG         m_nRefCount;

  // RefCount free function
  void DeleteClassInfo(CClassInfo * pInfo);
  // helper
  static long   FindClassPath( PSTR pszClassName, PSTR pszPath, CClassInfo * pInfo = NULL );

  // Getters
	CClassInfo * GetChild(){ return m_pChild; }
	CClassInfo * GetSibling() { return m_pSibling; }
  PSTR   GetClassName(){ return m_pszClassName; }
  PSTR   GetShortClassName(PSTR pszLongName);
  PSTR   GetClassPath(){ return m_pszClassPath; }
  PSTR   GetJar() { return m_pszJarName; }
	STREAM GetByteCodeStream();

  // Fields
  int GetFieldsCount();
  char * GetFields(int index);
  BOOL GetFieldAttributeNames(  int index, CString& attr );

  // Methods 
  int GetMethodsCount();
  char * GetMethodName(int index);
  BOOL GetMethodFullName(int index, CString& mname);
  BOOL GetMethodByteCodes( int index, CString& bcode );
  BOOL GetMethodAttributeNames(  int index, CString& attr );
  BOOL GetMethodExceptions( int index, CString& exc_names );
  BOOL GetMethodExceptionsCount( int index );
  // for convenience
  void ExtractMethodByteCodes(int index);
  unsigned long GetMethodByteCodeCount(int index);

  // Method Call Graph
  _JVM_DCall_Graph * GetCallGraph(int index); 
  _JVM_DCall_Graph * GetCallGraph(_JVM_DCall_Graph * graph, _JVM_DClass_Info * pclass, _JVM_DMethod_Info * pmethod);
  char * GetClassNameFromCG( _JVM_DCall_Graph * p ) { return  p? p->jclass->class_name: NULL; }
  char * GetMethodNameFromCG( _JVM_DCall_Graph * p) { return  p? p->jmethod->method_name : NULL; }
  void GetCGMethodSignature( _JVM_DCall_Graph * p, CString& cs);
  
  // Contained Classes 
  int GetContainedClassesCount();
  char * GetContainedClass(int index);

  // Referred Classes
  int GetReferredClassesCount();
  char * GetReferredClass(int index);

  // Constant Pool info
  int GetCPInfoCount();
  char * GetCPInfoString(int index);
  char * GetCPInfoType(int index);
  int GetCPInfoLength(int index);
  BOOL GetCPInfoBytes(int index, CString& pinfo);

  // Interface
  int GetInterfacesCount() { return m_nInterfaceCount; }
  char * GetInterfaceName(int index);


  // Flags
  BOOL  IsInterface() { return m_dwCFlags & JDC_IS_INTERFACE; }
  BOOL  IsFinal() { return m_dwCFlags & JDC_IS_FINAL; }
  BOOL  IsAbstract() { return m_dwCFlags & JDC_IS_ABSTRACT; }
  unsigned long GetClassAttributes(){  return m_uiClassFlags; }

  // Setters
  void SetChild( CClassInfo * pChild );
  void SetSibling(CClassInfo * pSibling );
  void SetClassName(PSTR pszClassName);
  void SetClassPath(PSTR pszClassPath);
  void SetJarName( PSTR pszJarName);
 

  void SetTreeItem(HTREEITEM hti){ m_hti = hti; }
  HTREEITEM GetTreeItem() { return m_hti; }


	BOOL WriteToStream( char * info , unsigned int len );
	char * ReadFromStream(unsigned int& count); 
  BOOL GetCommonAttributeNames( unsigned long flags, CString& attr);
  char * GetSourceFileName();

// restricted access
private:
  // Other helpers
  void FillFullMethodName(_JVM_DMethod_Info * pmi, CString& mname );

public:
  // Destruction
	virtual ~CClassInfo();

  DECLARE_DYNAMIC( CClassInfo );

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSINFO_H__3500EC41_3FC0_11D3_A73F_90FA46C10127__INCLUDED_)
