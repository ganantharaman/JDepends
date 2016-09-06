#if !defined(AFX_BYTECODEVIEW_H__1214FF0C_FC71_11D3_95A3_00C04F72C184__INCLUDED_)
#define AFX_BYTECODEVIEW_H__1214FF0C_FC71_11D3_95A3_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ByteCodeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CByteCodeView view
#include "formatedit.h"

class CByteCodeView : public CView
{
  CFormatEdit m_ByteCodeEdit;
protected:
	CByteCodeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CByteCodeView)

// Attributes
public:

// Operations
public:
  BOOL DisplayCode();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CByteCodeView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	 //}}AFX_VIRTUAL
  BOOL DrawCode( CDC * pDC, char * bytes );
// Implementation
protected:
  LONG OnDmViewUpdate( UINT wParam, LONG );
	virtual ~CByteCodeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CByteCodeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BYTECODEVIEW_H__1214FF0C_FC71_11D3_95A3_00C04F72C184__INCLUDED_)
