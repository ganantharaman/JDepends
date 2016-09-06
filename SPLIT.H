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

/////////////////////////////////////////////////////////////////////////////
// CSplit frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif                    

#define SPLIT_BMP    0x00000001
#define SPLIT_TEXT   0x00000002

class CSplit : public CSplitterWnd
{            
  CBitmap m_Bitmap;
	DECLARE_DYNCREATE(CSplit)                 
	CString m_csSplitTitle;
	DWORD  m_dwSplitStyle;
public:
	CSplit();			// protected constructor used by dynamic creation

// Attributes
    virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:        
    void SetSplitTitle(LPCSTR lpszSplitTitle);
    void SetSplitStyle( DWORD dwStyle );
    BOOL SetSplitBitmap( UINT nBmpResID );

// Operations
public:

// Overrides
public:

protected:
// Implementation
public:
	virtual ~CSplit();

	// Generated message map functions
	//{{AFX_MSG(CSplit)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
