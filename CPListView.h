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

#if !defined(AFX_CPLISTVIEW_H__9824BEBF_F9E9_11D3_95A2_00C04F72C184__INCLUDED_)
#define AFX_CPLISTVIEW_H__9824BEBF_F9E9_11D3_95A2_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CPListView.h : header file
//

#include "classinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CCPListView view

class CCPListView : public CListView
{
  int m_iStartItem, m_iEndItem, m_iCount;
  CMenu * m_pMoreMenu;
  COLORREF m_clrText;
protected:
	CCPListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCPListView)

// Attributes
public:
  void PopulateListCtrl(CClassInfo * pInfo);
  int  InsertListColumn( int column, int width, char * text );
  int  InsertListItem(int index, int entry, char * text );
 

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCPListView)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
  void OnPopupMenuHandlerChgClr();
   LONG OnDmViewUpdate( UINT, LONG );
	virtual ~CCPListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CCPListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPLISTVIEW_H__9824BEBF_F9E9_11D3_95A2_00C04F72C184__INCLUDED_)
