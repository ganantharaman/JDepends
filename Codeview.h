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

#if !defined(AFX_CPVIEW_H__9824BEBE_F9E9_11D3_95A2_00C04F72C184__INCLUDED_)
#define AFX_CPVIEW_H__9824BEBE_F9E9_11D3_95A2_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Codeview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCodeView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "formatedit.h"

class CCodeView : public CFormView
{
protected:
	CCodeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCodeView)
  LONG OnDmViewUpdate( UINT wParam, LONG );

// Form Data
public:
	//{{AFX_DATA(CCodeView)
	enum { IDD = IDD_CPDIALOG };
	CFormatEdit	m_ByteCodeEdit;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCodeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCodeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCodeView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnEnChangeEdit1();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPVIEW_H__9824BEBE_F9E9_11D3_95A2_00C04F72C184__INCLUDED_)
