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

#if !defined(AFX_FORMATEDIT_H__185F3BF2_FEB0_11D3_95A3_00C04F72C184__INCLUDED_)
#define AFX_FORMATEDIT_H__185F3BF2_FEB0_11D3_95A3_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FormatEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormatEdit window

class CFormatEdit : public CEdit
{
  DWORD m_clrText;
  DWORD m_clrBkgnd;
  CBrush m_brBkgnd;
  LOGFONT m_logfont;
  CFont m_font, *m_pOldFont;

// Construction
public:
	CFormatEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormatEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFormatEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFormatEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATEDIT_H__185F3BF2_FEB0_11D3_95A3_00C04F72C184__INCLUDED_)
