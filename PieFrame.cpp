// MiniFrame.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "commonFrame.h"
#include "dclassview.h"
#include "pieview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT DM_VIEWUPDATE;

/////////////////////////////////////////////////////////////////////////////
// CPieFrame

IMPLEMENT_DYNCREATE(CPieFrame, CMDIChildWnd)

CPieFrame::CPieFrame()
{
}

CPieFrame::~CPieFrame()
{
}


BEGIN_MESSAGE_MAP(CPieFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CPieFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPieFrame message handlers

int CPieFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	return 0;
}

BOOL CCommonFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
  CWnd * pWnd;
  CCreateContext ctx;

  ctx.m_pNewViewClass = m_pViewRuntimeClass;
  ctx.m_pCurrentDoc   = m_pDoc;

  pWnd = CreateView( &ctx );

  pWnd->SendMessage( DM_VIEWUPDATE );
  ((CView *)pWnd)->OnInitialUpdate();
	
  return TRUE; //CMDIChildWnd::OnCreateClient( lpcs, pContext);//TRUE;
}

void CPieFrame::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
  *ptr = NULL;
	CMDIChildWnd::PostNcDestroy();
}
