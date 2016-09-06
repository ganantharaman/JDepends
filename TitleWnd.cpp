// TitleWnd.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "TitleWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleWnd

CTitleWnd::CTitleWnd(char * title)
: m_strTitle(title),m_pEdit(NULL)
{
  
}

CTitleWnd::~CTitleWnd()
{
}


BEGIN_MESSAGE_MAP(CTitleWnd, CWnd)
	//{{AFX_MSG_MAP(CTitleWnd)
	ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CTitleWnd message handlers


BOOL CTitleWnd::Display(CWnd * pWnd, CRect& rect)
{
  LPCSTR lpszClassName = 
    AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,0,(HBRUSH)::GetStockObject(WHITE_BRUSH)); 
  // Create the window
  if (!CWnd::CreateEx(WS_EX_TOPMOST| WS_EX_STATICEDGE,lpszClassName,m_strTitle,
          WS_VISIBLE | WS_CHILD  | WS_DISABLED  |
		      WS_CLIPCHILDREN | WS_CLIPSIBLINGS ,rect, pWnd,NULL))
     return 0L;

  m_pEdit = new CEdit;
  m_pEdit->Create( WS_CHILD|WS_VISIBLE|WS_BORDER | ES_READONLY, 
    CRect(rect.left, rect.top + 40, rect.right, rect.bottom), 
    pWnd, 345);

 
  ShowWindow(SW_SHOW);
  return TRUE;
}

void CTitleWnd::Move_Window( const CRect& rect)
{
  if ( m_pEdit ) m_pEdit->MoveWindow( CRect( rect.left, rect.top + 21, rect.right, rect.bottom));
  MoveWindow( rect );
  m_pEdit->SetWindowText("I'm Here");
}

void CTitleWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
  GetClientRect(&rect);
  rect.bottom = 20;
  dc.FillSolidRect(rect,RGB(0,0,195));
	dc.SetTextColor( RGB(0,255,255));
  dc.SetBkMode(TRANSPARENT);
  dc.DrawText(m_strTitle, rect, DT_CENTER|DT_VCENTER); 
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CTitleWnd::PostNcDestroy() 
{
  	delete this;
}

