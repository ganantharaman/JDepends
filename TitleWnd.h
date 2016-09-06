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

#if !defined(AFX_TITLEWND_H__A94A14E6_EE1A_11D3_959A_00C04F72C184__INCLUDED_)
#define AFX_TITLEWND_H__A94A14E6_EE1A_11D3_959A_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TitleWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTitleWnd window

class CTitleWnd : public CWnd
{
  CString m_strTitle;
  CEdit * m_pEdit;
// Construction
public:
	CTitleWnd(char * title = NULL);
  BOOL Display(CWnd * pWnd, CRect& rect);
  void Move_Window( const CRect& rect);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitleWnd)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTitleWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTitleWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITLEWND_H__A94A14E6_EE1A_11D3_959A_00C04F72C184__INCLUDED_)
