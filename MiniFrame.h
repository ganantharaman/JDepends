#if !defined(AFX_MINIFRAME_H__465ED22C_FF61_11D3_95A3_00C04F72C184__INCLUDED_)
#define AFX_MINIFRAME_H__465ED22C_FF61_11D3_95A3_00C04F72C184__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MiniFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMiniFrame frame

class CMiniFrame : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CMiniFrame)

  CMiniFrame** ptr;

protected:
  CMiniFrame(CMiniFrame ** frame) { ptr = frame; }           // protected constructor used by dynamic creation
  CMiniFrame();

// Attributes
public:

  static void CreateFrame(CMiniFrame ** frame) { *frame = new CMiniFrame(frame) ; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiniFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMiniFrame();

	// Generated message map functions
	//{{AFX_MSG(CMiniFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINIFRAME_H__465ED22C_FF61_11D3_95A3_00C04F72C184__INCLUDED_)
