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

#include "stdafx.h"
#include "jdepends.h"

#include "ChildFrm.h"
#include "jdependsDoc.h"
#include "jdependsView.h"
#include "mfview.h"
#include "cplistview.h"
#include "codeview.h"
#include "pieview.h"
#include "pieframe.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
  // m_pMiniFrame = NULL;	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL ret = CMDIChildWnd::PreCreateWindow(cs);
	
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
	
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

  int cxText = max(lpcs->cx / 4 , 20 );
	// add the first splitter pane - the default view in column 0
	if (!m_wndSplitter.CreateView(0, 0,
		pContext->m_pNewViewClass, CSize(cxText, 0), pContext))
	{
		TRACE("Failed to create first pane\n");
		return FALSE;
	}

  if (!m_wndSplitter2.CreateStatic(
		&m_wndSplitter,     // our parent window is the first splitter
		2, 1,               // the new splitter has 2 row, 1 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter.IdFromRowCol(0, 1)
			// new splitter is in the first row, 2nd column of first splitter
	))
	{
		TRACE("Failed to create nested splitter\n");
		return FALSE;
	}

 int cyText = max(lpcs->cy / 2, 20);    // height of text pane

  
 if (!m_wndSplitter3.CreateStatic(
		&m_wndSplitter2,     // our parent window is the second splitter
		1, 2,               // the new splitter has 1 row, 2 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter2.IdFromRowCol(1, 0)
			// new splitter is in the first row, 2nd column of first splitter
	))
	{
		TRACE("Failed to create nested splitter\n");
		return FALSE;
	}

  if (!m_wndSplitter3.CreateView(0, 0,
		RUNTIME_CLASS(CMFView), CSize(cxText, cyText), pContext))
	{
		TRACE("Failed to create MFView\n");
		return FALSE;
	}


  if (!m_wndSplitter3.CreateView(0, 1,
		RUNTIME_CLASS(CCodeView), CSize(0, 0), pContext))
	{
		TRACE("Failed to create Code view \n");
		return FALSE;
	}

  if (!m_wndSplitter2.CreateView(0, 0,
		RUNTIME_CLASS(CCPListView), CSize(cxText, cyText), pContext))
	{
		TRACE("Failed to create third pane\n");
		return FALSE;
	}

 // m_wndSplitter2.SetRowInfo( 1, 20,40 );

 

  return TRUE;
}

void CChildFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

// if ( m_pMiniFrame )
  // m_pMiniFrame->SendMessage( WM_CLOSE );
	
	CMDIChildWnd::OnClose();
}
