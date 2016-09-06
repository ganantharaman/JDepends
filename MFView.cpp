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
#include "MFView.h"
#include "classinfo.h"
#include "jdependsdoc.h"
#include "callgraphview.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFView

extern UINT DM_VIEWUPDATE;


IMPLEMENT_DYNCREATE(CMFView, CTreeView)

CMFView::CMFView()
{
  m_pCGFrame = NULL;
}

CMFView::~CMFView()
{
}


BEGIN_MESSAGE_MAP(CMFView, CTreeView)
  ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate) 
	//{{AFX_MSG_MAP(CMFView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CMFView drawing

void CMFView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CMFView diagnostics

#ifdef _DEBUG
void CMFView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CMFView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMFView message handlers

void CMFView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();

  m_imList.Create(IDB_BITMAP2, 16, 0, RGB(255,0,255));
  GetTreeCtrl().SetImageList( &m_imList, TVSIL_NORMAL);

  m_htiFields = ::InsertTreeItem( GetTreeCtrl(), TVI_ROOT, "Fields", -1, 8);
  m_htiMethods = ::InsertTreeItem(GetTreeCtrl(), TVI_ROOT, "Methods", -1, 8);


}

LONG CMFView::OnDmViewUpdate( UINT, LONG )
{
	CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
	CClassInfo * pRoot = pDoc->GetRoot();
  
  // Insert class hierarchy first
  pRoot->SetTreeItem( m_htiFields );  
  InsertFields( pRoot );
  GetTreeCtrl().Expand(m_htiFields, TVE_EXPAND);

  pRoot->SetTreeItem( m_htiMethods );  
  InsertMethods( pRoot );
  GetTreeCtrl().Expand(m_htiMethods, TVE_EXPAND);

	return 1UL;
}




void CMFView::InsertFields( CClassInfo * pInfo )
{
  ASSERT( pInfo != NULL);
  int i, nCount = pInfo->GetFieldsCount();
  CTreeCtrl& ctrl = GetTreeCtrl();
  for ( i = 0; i < nCount; i++) 
  {
       CString csAttr;
       // Add attributes
       pInfo->GetFieldAttributeNames( i, csAttr );
       // Then the field types
       csAttr += pInfo->GetFields(i);
       ::InsertTreeItem(ctrl, pInfo->GetTreeItem(), 
                      csAttr.GetBuffer(csAttr.GetLength()), i,
                      4);
  } 
}


void CMFView::InsertMethods( CClassInfo * pInfo )
{
  ASSERT( pInfo != NULL);
  CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
  int nCount = pInfo->GetMethodsCount();
  CTreeCtrl& ctrl = GetTreeCtrl();

  if ( nCount && !m_pCGFrame )
  {
  // get Parent Frame's co-ordinates
  // use it to create the new PieFrame
  // we adjust the rect to get a 'square'
  CRect rect;
  CWnd * pWnd = ((CMDIFrameWnd *)AfxGetMainWnd())->MDIGetActive();
  if ( pWnd ) 
     pWnd->GetClientRect( &rect );
  else
     GetParent()->GetClientRect( &rect );

  rect.right = rect.left + rect.Height();

  // Make up a title
  CString csTitle = pInfo->GetShortClassName(pInfo->GetClassName());
  csTitle += " ( Call Graph )";

  // Create a Pie Frame Object and a window
  CCommonFrame::CreateFrame(m_pCGFrame, pDoc, RUNTIME_CLASS( CCallGraphView ), csTitle, rect);

  // Center it and show
  m_pCGFrame->CenterWindow();
  m_pCGFrame->ShowWindow(SW_SHOW);
  m_pCGFrame->UpdateWindow();

  }

  // For all methods do,
  for (int i = 0; i < nCount; i++) {
    CString csName;
    // Get method attributes first 
    pInfo->GetMethodAttributeNames( i, csName );
    // Get method name next
    VERIFY( pInfo->GetMethodFullName(i, csName));

    // Initialize currently selected index to be 0
    if ( i == 0 ) {
       pDoc->SetSelMethodIndex(0); 
       pDoc->SetSelMethodName(csName, 0L);
    }
    // insert
    ::InsertTreeItem(ctrl, pInfo->GetTreeItem(),
          csName.GetBuffer(csName.GetLength()),i,4);
  } 
}


int CMFView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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




void CMFView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  CJdependsDoc * pDoc;
  CTreeCtrl& ctrl = GetTreeCtrl();

  HTREEITEM hSelItem = ctrl.GetSelectedItem();

  if ( m_htiMethods == ctrl.GetParentItem( hSelItem ))
  {
    pDoc = (CJdependsDoc *)GetDocument();
    //  no redrawing
    pDoc->SetRedrawFlag(FALSE);
    // Set the selected index in document
    // views will make use of it.
    int index = ctrl.GetItemData( hSelItem );
    pDoc->SetSelMethodIndex(index);
    // set the Item text
    CString& str = ctrl.GetItemText(hSelItem);
    pDoc->SetSelMethodName(str);
    // Enable drawing
    pDoc->SetRedrawFlag(TRUE);
    // Update call graph view
    if ( m_pCGFrame )
      m_pCGFrame->PostViewUpdate();
 
  }
	// TODO: Add your control notification handler code here
	*pResult = 0;
}



void CMFView::OnDestroy() 
{
  if ( m_pCGFrame )
    m_pCGFrame->DestroyWindow();
	CTreeView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}
