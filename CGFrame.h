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

#if !defined(AFX_CGFRAME_H__1C37EAC6_043C_11D4_95A3_00C04F72C184__INCLUDED_)
#define AFX_CGFRAME_H__1C37EAC6_043C_11D4_95A3_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CGFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommonFrame frame

class CCommonFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CCommonFrame)
  CCommonFrame ** ptr;
  CDocument * m_pDoc;
  CWnd * m_pWnd;
protected:

// Attributes
public:


protected:
  // protected constructor used by dynamic creation
  CCommonFrame(CCommonFrame ** frame, CDocument * pDoc ) { ptr = frame; m_pDoc = pDoc; }
  CCommonFrame();
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Attributes
public:
  static void CreateFrame(CCommonFrame *& frame, CDocument * pDoc) { frame = new CCommonFrame(&frame, pDoc) ; }
  void DisplayCallGraph();


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommonFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCommonFrame();

	// Generated message map functions
	//{{AFX_MSG(CCommonFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGFRAME_H__1C37EAC6_043C_11D4_95A3_00C04F72C184__INCLUDED_)
