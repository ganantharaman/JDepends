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

#if !defined(AFX_JDEPENDSDOC_H__F92BE3DC_2E79_11D3_A73E_90FA46C10121__INCLUDED_)
#define AFX_JDEPENDSDOC_H__F92BE3DC_2E79_11D3_A73E_90FA46C10121__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "classinfo.h"

class CJdependsDoc : public CDocument
{
  enum { _COUNT_MAX = 512 };
  CClassInfo * m_pRoot;
  int m_nMethodIndex;
  CString m_csMethodName;
  CView * m_pCodeView;
  LOGFONT m_logfont;
  static CString m_csCls;

protected: // create from serialization only
	CJdependsDoc();
	DECLARE_DYNCREATE(CJdependsDoc)

// Attributes
public:
	CClassInfo * GetRoot(){ return m_pRoot; }
  void SetSelMethodIndex( int index) { m_nMethodIndex = index; }
  int GetSelMethodIndex() { return m_nMethodIndex; }
  void SetSelMethodName( CString& name, BOOL bRefresh = TRUE );
  char * GetSelMethodName() { return m_csMethodName.GetBuffer(m_csMethodName.GetLength());  }

  void SetRedrawFlag( BOOL bVal );
  BOOL LoadClassFromJar(CString& csClass, 
		                LPCTSTR lpszPathName);

  static void SetClass(CString& csClass) { m_csCls = csClass; }
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJdependsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

	virtual void OnOptions();

// Implementation
public:
	virtual ~CJdependsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CJdependsDoc)
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMenu163();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JDEPENDSDOC_H__F92BE3DC_2E79_11D3_A73E_90FA46C10121__INCLUDED_)
