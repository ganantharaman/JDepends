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

#if !defined(AFX_CALLGRAPHVIEW_H__1C37EAC7_043C_11D4_95A3_00C04F72C184__INCLUDED_)
#define AFX_CALLGRAPHVIEW_H__1C37EAC7_043C_11D4_95A3_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CallGraphView.h : header file
//

#include "classinfo.h"
/////////////////////////////////////////////////////////////////////////////
// CCallGraphView view

class CCallGraphView : public CTreeView
{
  HTREEITEM m_htiCallGraph;
  CImageList  m_imList;
protected:
	CCallGraphView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCallGraphView)

// Attributes
public:
 
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCallGraphView)
 	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL


// Implementation
protected:
  void InsertCallGraph(HTREEITEM hParent, _JVM_DCall_Graph * pRoot);
  LONG OnDmViewUpdate( UINT, LONG );
	virtual ~CCallGraphView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CCallGraphView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALLGRAPHVIEW_H__1C37EAC7_043C_11D4_95A3_00C04F72C184__INCLUDED_)
