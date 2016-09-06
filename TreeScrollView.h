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

#if !defined(AFX_TREESCROLLVIEW_H__BB346A82_8E50_11D3_9596_00C04F72C184__INCLUDED_)
#define AFX_TREESCROLLVIEW_H__BB346A82_8E50_11D3_9596_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TreeScrollView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeScrollView view

class CTreeScrollView : public CScrollView
{
protected:
	CTreeScrollView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeScrollView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeScrollView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTreeScrollView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTreeScrollView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREESCROLLVIEW_H__BB346A82_8E50_11D3_9596_00C04F72C184__INCLUDED_)
