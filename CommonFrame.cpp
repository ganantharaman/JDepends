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
#include "Commonframe.h"
#include "CallGraphView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT DM_VIEWUPDATE;




/////////////////////////////////////////////////////////////////////////////
// CCommonFrame

IMPLEMENT_DYNCREATE(CCommonFrame, CMDIChildWnd)

LPCSTR CCommonFrame::m_lpszClassName = NULL;

CCommonFrame::CCommonFrame()
{
  m_pWnd = NULL;
  m_pViewRuntimeClass = NULL;
}

CCommonFrame::~CCommonFrame()
{
}


BEGIN_MESSAGE_MAP(CCommonFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CCommonFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommonFrame message handlers

BOOL CCommonFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  // TODO: Add your specialized code here and/or call the base class
  CCreateContext ctx;

  ctx.m_pNewViewClass = m_pViewRuntimeClass;
  ctx.m_pCurrentDoc   = m_pDoc;

  m_pWnd = CreateView( &ctx );

  ((CView *)m_pWnd)->OnInitialUpdate();
  m_pWnd->SendMessage( DM_VIEWUPDATE );
	
  return TRUE; 
	
}

BOOL CCommonFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL ret = CMDIChildWnd::PreCreateWindow(cs);
	
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
	
	return ret;
}


void CCommonFrame::PostViewUpdate()
{
   if ( m_pWnd ) m_pWnd->PostMessage( DM_VIEWUPDATE ); 
}

void CCommonFrame::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
  *ptr = NULL;
	CMDIChildWnd::PostNcDestroy();
}
