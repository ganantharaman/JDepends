/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*  This code and any software associated is covered by                     */
/*  GNU Lesser General Public License (LGPL)                                        */
/*                                                                          */
/*  Author: Gopal Ananthraman                                               */
/*                                                                          */
/*  Desc  :                                                                 */
/*                                                                          */
/****************************************************************************/

#if !defined(AFX_MINIFRAME_H__465ED22C_FF61_11D3_95A3_00C04F72C184__INCLUDED_)
#define AFX_MINIFRAME_H__465ED22C_FF61_11D3_95A3_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MiniFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPieFrame frame

class CPieFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPieFrame)

  CPieFrame ** ptr;
  CDocument * m_pDoc;

protected:
  // protected constructor used by dynamic creation
  CPieFrame(CPieFrame ** frame, CDocument * pDoc ) { ptr = frame; m_pDoc = pDoc; }
  CPieFrame();

// Attributes
public:
  static void CreateFrame(CPieFrame *& frame, CDocument * pDoc) { frame = new CPieFrame(&frame, pDoc) ; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPieFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPieFrame();

	// Generated message map functions
	//{{AFX_MSG(CPieFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINIFRAME_H__465ED22C_FF61_11D3_95A3_00C04F72C184__INCLUDED_)
