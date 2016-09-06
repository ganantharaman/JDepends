// MiniFrame.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "MiniFrame.h"
#include "dclassview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMiniFrame

IMPLEMENT_DYNCREATE(CMiniFrame, CMiniFrameWnd)

CMiniFrame::CMiniFrame()
{
}

CMiniFrame::~CMiniFrame()
{
}


BEGIN_MESSAGE_MAP(CMiniFrame, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CMiniFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiniFrame message handlers

int CMiniFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	return 0;
}

BOOL CMiniFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

/*  CRuntimeClass * pClass = RUNTIME_CLASS( CDClassView );
  CWnd * pWnd;
  pWnd = (CWnd *)pClass->CreateObject();
  ASSERT ( pWnd != NULL );
 
  
	if (!pWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0, lpcs->cx,lpcs->cy), this, 123, pContext)) 
    return FALSE;

  ((CView *)pWnd)->OnInitialUpdate(); */
	
  return CMiniFrameWnd::OnCreateClient( lpcs, pContext);//TRUE;
}

void CMiniFrame::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
 // *ptr = NULL;
	CMiniFrameWnd::PostNcDestroy();
}
