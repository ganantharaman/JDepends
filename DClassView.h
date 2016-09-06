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

#if !defined(AFX_DCLASSVIEW_H__5C3456C3_2E7D_11D3_A73E_90FA46C10121__INCLUDED_)
#define AFX_DCLASSVIEW_H__5C3456C3_2E7D_11D3_A73E_90FA46C10121__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DClassView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDClassView view

#include "classinfo.h"
#include "commonframe.h"

class CDClassView : public CTreeView
{
  CMenu * m_pRCMenu;
  CMenu * m_pLoadClassMenu;
	HTREEITEM m_hti, m_htiHier, m_htiCC, m_htiRC;
  CImageList  m_imList;
  CCommonFrame * m_pPieFrame;
  CString  m_csClass;
  PSTR  m_pJar;
protected:
	CDClassView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDClassView)

// Attributes
public:
  HTREEITEM InsertTreeItem(HTREEITEM hParent, PSTR pszText, int iImage );
	void InsertClassHierarchy(CClassInfo * p); 
  void InsertReferredClasses( CClassInfo * pInfo );
  void InsertContainedClasses( CClassInfo * pInfo );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDClassView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL
	afx_msg LONG OnDmViewUpdate( UINT, LONG );
  void OnPopupMenuHandlerCodeDist();
  void OnPopupMenuHandlerClassProp();
  void OnPopupMenuHandlerLoadClass();

// Implementation
protected:
	virtual ~CDClassView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDClassView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCLASSVIEW_H__5C3456C3_2E7D_11D3_A73E_90FA46C10121__INCLUDED_)
