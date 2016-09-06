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
#include "CallGraphView.h"
#include "jdependsdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCallGraphView

extern UINT DM_VIEWUPDATE;

IMPLEMENT_DYNCREATE(CCallGraphView, CTreeView)

CCallGraphView::CCallGraphView()
{
}

CCallGraphView::~CCallGraphView()
{
}


BEGIN_MESSAGE_MAP(CCallGraphView, CTreeView)
  ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate) 
	//{{AFX_MSG_MAP(CCallGraphView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCallGraphView drawing

void CCallGraphView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CCallGraphView diagnostics

#ifdef _DEBUG
void CCallGraphView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CCallGraphView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCallGraphView message handlers

int CCallGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

  HWND hTreeWnd = GetTreeCtrl().GetSafeHwnd();
  ASSERT( hTreeWnd != NULL);

  // Set Tree ctrl styles   
  DWORD dwStyle = GetWindowLong( hTreeWnd, GWL_STYLE );
  dwStyle |= ( TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_LINESATROOT);
  SetWindowLong( hTreeWnd, GWL_STYLE, dwStyle );
	
	// TODO: Add your specialized creation code here
	
	return 0;
}


void CCallGraphView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();

  m_imList.Create(IDB_BITMAP2, 16, 0, RGB(255,0,255));
  GetTreeCtrl().SetImageList( &m_imList, TVSIL_NORMAL);

  m_htiCallGraph = ::InsertTreeItem(GetTreeCtrl(), TVI_ROOT, "Call Graph", -1,  8);
	// TODO: Add your specialized code here and/or call the base class

}

LONG CCallGraphView::OnDmViewUpdate( UINT, LONG )
{
	CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
	CClassInfo * pInfo = pDoc->GetRoot();

  _JVM_DCall_Graph * pTree;
  pTree = pInfo->GetCallGraph( pDoc->GetSelMethodIndex());

  LockWindowUpdate();
  GetTreeCtrl().DeleteAllItems();
  UnlockWindowUpdate();
  m_htiCallGraph = ::InsertTreeItem(GetTreeCtrl(), TVI_ROOT, "Call Graph", -1,  8);
  InsertCallGraph( m_htiCallGraph, pTree );
  GetTreeCtrl().Expand(m_htiCallGraph, TVE_EXPAND);

  return 1UL;
 
}


void CCallGraphView::InsertCallGraph(HTREEITEM hParent, _JVM_DCall_Graph * pRoot)
{
  CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
  CClassInfo * pInfo = pDoc->GetRoot();
  HTREEITEM hti;

  CString cs;
  if ( ! pRoot ) return;

  // insert roots text
  cs += "";
  pInfo->GetCGMethodSignature( pRoot, cs );
  hti = ::InsertTreeItem( GetTreeCtrl(), hParent,
                        cs.GetBuffer(cs.GetLength()), (LPARAM)pRoot,
                        1 );

  // recursively insert children ( depth is usually 2 )
  for ( int i = 0; i < pRoot->nodes_len; i++)
     InsertCallGraph( hti, pRoot->nodes[i] );
    
  // Expand
  GetTreeCtrl().Expand(hti, TVE_EXPAND);
}




void CCallGraphView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
  CTreeCtrl& ctrl = GetTreeCtrl();

  POINT pt, ptClnt;
  ::GetCursorPos( &pt );

  ptClnt = pt;
  ScreenToClient( &ptClnt );

  UINT flags;
  HTREEITEM hHotItem = ctrl.HitTest( ptClnt, &flags );

  if ( hHotItem ) {
    // Select item first
    ctrl.SelectItem( hHotItem );
    // get the data associated with item
    LPARAM lp = ctrl.GetItemData( hHotItem );
    _JVM_DCall_Graph * pTree = (_JVM_DCall_Graph *)lp;

    // Only if its nodes are NULL proceed
    if ( pTree && pTree->nodes == NULL )
    {
     	CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
      CClassInfo * pInfo = pDoc->GetRoot();
      AfxGetApp()->DoWaitCursor( 1 );
      pInfo->GetCallGraph(pTree, pTree->jclass, pTree->jmethod);

      LockWindowUpdate();
      for ( int i =0; i < pTree->nodes_len; i++) 
         InsertCallGraph( hHotItem, pTree->nodes[i] );

      UnlockWindowUpdate();
      AfxGetApp()->DoWaitCursor( -1 );
    }
  }

	*pResult = 0;
}
