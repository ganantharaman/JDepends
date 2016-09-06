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

#if !defined(AFX_MFVIEW_H__F1F2684C_F9D3_11D3_95A2_00C04F72C184__INCLUDED_)
#define AFX_MFVIEW_H__F1F2684C_F9D3_11D3_95A2_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MFView.h : header file
//

#include "classinfo.h"
#include "commonframe.h"

/////////////////////////////////////////////////////////////////////////////
// CMFView view

class CMFView : public CTreeView
{
	HTREEITEM m_htiFields, m_htiMethods;
  CImageList  m_imList;
  CCommonFrame * m_pCGFrame;
  const char * m_lpszClassName;

protected:
	CMFView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMFView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL
  void InsertFields( CClassInfo * pInfo );
  void InsertMethods( CClassInfo * pInfo );
  LONG OnDmViewUpdate( UINT, LONG );

  HTREEITEM InsertTreeItem(HTREEITEM hParent, PSTR pszText, int lparam, int iImage );
// Implementation
protected:
	virtual ~CMFView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMFView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFVIEW_H__F1F2684C_F9D3_11D3_95A2_00C04F72C184__INCLUDED_)
