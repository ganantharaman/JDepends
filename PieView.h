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

#if !defined(AFX_PIEVIEW_H__68DD966E_0022_11D4_95A3_00C04F72C184__INCLUDED_)
#define AFX_PIEVIEW_H__68DD966E_0022_11D4_95A3_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PieView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPieView view
#include "graphman.h"

class CPieView : public CScrollView
{
  LOGFONT m_logfont;
  CFont * m_pFont;
  CGraphManager * m_pGraphManager;
	CSize sizeTotal;
  BOOL m_bReady,m_bNotEmpty;

protected:
	CPieView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPieView)

// Attributes
public:

// Operations
public:
   void CenterText(CDC& dc,CFont* pFont,
				           CString& String,
				           int nWidth, int nHeight
				          );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPieView)
	public:
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
  LONG OnDmViewUpdate( UINT, LONG );
	virtual ~CPieView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPieView)
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIEVIEW_H__68DD966E_0022_11D4_95A3_00C04F72C184__INCLUDED_)
