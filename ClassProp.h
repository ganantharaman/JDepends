#if !defined(AFX_CLASSPROP_H__C86546A1_5916_11D4_95AB_00C04F72C184__INCLUDED_)
#define AFX_CLASSPROP_H__C86546A1_5916_11D4_95AB_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ClassProp.h : header file
//
#include "classinfo.h"
/////////////////////////////////////////////////////////////////////////////
// CClassProp dialog

class CClassProp : public CDialog
{
// Construction
  CClassInfo * m_pInfo;
  CBrush m_brBkgnd;
  COLORREF m_clrBkgnd;
public:
	CClassProp(CClassInfo * pInfo = NULL,CWnd* pParent = NULL);   // standard constructor
  ~CClassProp() { if ( m_pInfo ) CClassInfo::DecreaseRefCount( m_pInfo ); } 

// Dialog Data
	//{{AFX_DATA(CClassProp)
	enum { IDD = IDD_CLASSPROP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  
	// Generated message map functions
	//{{AFX_MSG(CClassProp)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSPROP_H__C86546A1_5916_11D4_95AB_00C04F72C184__INCLUDED_)
