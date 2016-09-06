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
#include "DClassView.h"
#include "classinfo.h"
#include "jdependsDoc.h"
#include "commonframe.h"
#include "pieview.h"
#include "classprop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT DM_VIEWUPDATE;

/////////////////////////////////////////////////////////////////////////////
// CDClassView

IMPLEMENT_DYNCREATE(CDClassView, CTreeView)

CDClassView::CDClassView()
{
  m_pLoadClassMenu = m_pRCMenu = NULL;
  m_pPieFrame = NULL;
  m_pJar = NULL;
}

CDClassView::~CDClassView()
{

}


BEGIN_MESSAGE_MAP(CDClassView, CTreeView)
  ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate)
	//{{AFX_MSG_MAP(CDClassView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
  ON_COMMAND( PMID_CODEDIST, OnPopupMenuHandlerCodeDist )
  ON_COMMAND( PMID_CPROP,    OnPopupMenuHandlerClassProp )
   ON_COMMAND( PMID_LOADCLASS,    OnPopupMenuHandlerLoadClass )
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDClassView drawing

void CDClassView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();


	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CDClassView diagnostics

#ifdef _DEBUG
void CDClassView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CDClassView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDClassView message handlers

/*
 *  One time view initializing message
 */
void CDClassView::OnInitialUpdate() 
{
 	CTreeView::OnInitialUpdate();
  
  m_imList.Create(IDB_BITMAP1, 16, 0, RGB(255,0,255));
  CTreeCtrl& ctrl = GetTreeCtrl();
  ctrl.SetImageList( &m_imList, TVSIL_NORMAL);

  m_hti     = ::InsertTreeItem(ctrl, TVI_ROOT, "dummy", -1, 8);
  m_htiHier = ::InsertTreeItem(ctrl, m_hti, "Class Hierarchy", -1,  8);
  m_htiCC   = ::InsertTreeItem(ctrl, m_hti, "Contained Classes", -1, 8);
  m_htiRC   = ::InsertTreeItem(ctrl, m_hti, "Referred Classes", -1, 8);
   
	// TODO: Add your specialized code here and/or call the base class
}

/*
 *  Initial message called to update the tree view
 */
LONG CDClassView::OnDmViewUpdate( UINT, LONG )
{
	CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
	CClassInfo * pRoot = pDoc->GetRoot();
	
	CWnd * pWnd = GetParent();
	if(pWnd) {
	   pWnd->GetParent()->SetWindowText(pDoc->GetTitle() + " " + pRoot->GetClassName());
	}

  GetTreeCtrl().SetItemText( m_hti, pRoot->GetClassName() );
  // Insert class hierarchy first
	pRoot->SetTreeItem( m_htiHier );
  InsertClassHierarchy( pRoot );
  GetTreeCtrl().Expand(m_htiHier, TVE_EXPAND);
  
  pRoot->SetTreeItem( m_htiCC );  
  InsertContainedClasses( pRoot );
  GetTreeCtrl().Expand(m_htiCC, TVE_EXPAND);

  pRoot->SetTreeItem( m_htiRC );  
  InsertReferredClasses( pRoot );
  GetTreeCtrl().Expand(m_htiRC, TVE_EXPAND);

  GetTreeCtrl().Expand(m_hti, TVE_EXPAND);

  

	return 1UL;
}

/*
 *  Insert classes contained in a given class
 */
void CDClassView::InsertContainedClasses( CClassInfo * pInfo )
{
  int i, nCount = pInfo->GetContainedClassesCount();
  CTreeCtrl& ctrl = GetTreeCtrl();
  for ( i = 0; i < nCount; i++) {
    ::InsertTreeItem( ctrl, pInfo->GetTreeItem(),
                      pInfo->GetContainedClass(i),
                      i,
                      6);
  }
}

/*
 *  Inserts all the referred classes inside a given class
 */
void CDClassView::InsertReferredClasses( CClassInfo * pInfo )
{
  int i,b, nCount = pInfo->GetReferredClassesCount();
  char path[_MAX_PATH + 1];
  char str[_MAX_PATH + 1];
  CTreeCtrl& ctrl = GetTreeCtrl();
  
  for ( i = 0; i < nCount; i++) {
	  
      char * pclass = pInfo->GetReferredClass(i);
      b = CClassInfo::FindClassPath(pclass, path);
      if ( b == JARRED_CLASS ) {
        sprintf(str, "%s (%s)", pclass, path);
      } else if ( b == REGULAR_CLASS ) {
        sprintf(str, "%s", path);
      }
      ::InsertTreeItem( ctrl, pInfo->GetTreeItem(),
                      b ? str : pclass,
                      i,
                      b ? 9 : 5);
  }
}

/*
 *  Insert the class hierarchy upto java\lang\Object
 */
void CDClassView::InsertClassHierarchy(CClassInfo * pRoot)
{
  CClassInfo * pNode;
  char * str, *p;
  HTREEITEM hti;
  
  if (!pRoot) return;

  str = p = pRoot->GetClassPath();
  if ( !str ) {
      str = pRoot->GetClassName();
  }
  CTreeCtrl& ctrl = GetTreeCtrl();
  // Actually Insert into tree
  hti = ::InsertTreeItem(ctrl, pRoot->GetTreeItem(),
                        str,
                        -1,
                        pRoot->IsInterface() ? (p ? 1 : 5) :(p ? 6 : 7) );


  // Recurse on child
  // Intentional '='
	if ( pNode = pRoot->GetChild()) {
     pNode->SetTreeItem( hti );
     InsertClassHierarchy( pNode );
	}

  // Recurse on sibling
  // Intentional '='
	if ( pNode = pRoot->GetSibling()) {
	   HTREEITEM rhti = pRoot->GetTreeItem();
       pNode->SetTreeItem( rhti );
       InsertClassHierarchy( pNode );
  }

  // Expand
  ctrl.Expand(hti, TVE_EXPAND);
}

/*
 *  Handle code distribution menu click -  displays pie chart of code size
 */
void CDClassView::OnPopupMenuHandlerCodeDist()
{
  CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
  CClassInfo * pInfo = pDoc->GetRoot();
  // get Parent Frame's co-ordinates
  // use it to create the new PieFrame
  // we adjust the rect to get a 'square'
  CRect rect;
  GetParent()->GetClientRect( &rect );
  rect.right = rect.left + rect.Height();

  // Make up a title
  CString csTitle = pInfo->GetShortClassName(pInfo->GetClassName());
  csTitle += " ( Code distribution )";

  // Create a Pie Frame Object with a pie view
  CCommonFrame::CreateFrame(m_pPieFrame, pDoc, RUNTIME_CLASS( CPieView), csTitle, rect);

  // Center it and show
  m_pPieFrame->CenterWindow();
  m_pPieFrame->ShowWindow(SW_SHOW);
  m_pPieFrame->UpdateWindow();

}


void CDClassView::OnPopupMenuHandlerClassProp()
{
  CClassProp dlg(((CJdependsDoc *)GetDocument())->GetRoot());
  dlg.DoModal();
}

void CDClassView::OnPopupMenuHandlerLoadClass() {

  CJdependsApp * pApp = (CJdependsApp *)AfxGetApp();
  POSITION pos = pApp->GetFirstDocTemplatePosition();
  CDocTemplate * pDocTemplate =  pApp->GetNextDocTemplate(pos);
  char path[_MAX_PATH + 1];
  CString cs = m_csClass;
  int index = m_csClass.Find(' ');
  if( index > 0) {
    cs = m_csClass.Left(index);
  }
  int type = CClassInfo::FindClassPath((PSTR)(LPCTSTR)cs, path);
  if(type == JARRED_CLASS) {
    CJdependsDoc::SetClass(cs);
    pDocTemplate->OpenDocumentFile(path);
  } else if ( type == REGULAR_CLASS) {
    pDocTemplate->OpenDocumentFile(path);
  }
}


/*
 *  Creates a tree control - sets initial styles
 */
int CDClassView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CDClassView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) {
// TODO: Add your control notification handler code here
  CTreeCtrl& ctrl = GetTreeCtrl();

  POINT pt, ptClnt;
  ::GetCursorPos( &pt );

  ptClnt = pt;
  ScreenToClient( &ptClnt );

  UINT flags;
  HTREEITEM hHotItem = ctrl.HitTest( ptClnt, &flags );

  if ( hHotItem )
    ctrl.SelectItem( hHotItem );
    
  m_csClass = ctrl.GetItemText(hHotItem);   
  OnPopupMenuHandlerLoadClass();
}

/*
 *  Right button click handler - tracks hotitem and displays menu
 */
void CDClassView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
  CTreeCtrl& ctrl = GetTreeCtrl();

  POINT pt, ptClnt;
  ::GetCursorPos( &pt );

  ptClnt = pt;
  ScreenToClient( &ptClnt );

  UINT flags;
  HTREEITEM hHotItem = ctrl.HitTest( ptClnt, &flags );

  if ( hHotItem )
    ctrl.SelectItem( hHotItem );

  

  if ( m_hti == hHotItem ) {
    if (!m_pRCMenu) {
		m_pRCMenu = new CMenu;
		m_pRCMenu->CreatePopupMenu();
		m_pRCMenu->AppendMenu(MF_ENABLED|MF_STRING, PMID_CPROP, "Class Properties");
		m_pRCMenu->AppendMenu(MF_ENABLED|MF_STRING, PMID_CODEDIST, "Code distribution");
		m_pRCMenu->AppendMenu(MF_ENABLED|MF_CHECKED|MF_STRING, PMID_FULLPATH, "Show Full Path");
    }
    m_pRCMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
        pt.x + 1,
        pt.y,
        this);    // owner is the main application window
  } else {
	  if(!m_pLoadClassMenu) {
	      m_pLoadClassMenu = new CMenu;
	      m_pLoadClassMenu->CreatePopupMenu();
		    m_pLoadClassMenu->AppendMenu(MF_ENABLED|MF_STRING, PMID_LOADCLASS, "Load Class on New Window");
	  }
	  m_csClass = ctrl.GetItemText(hHotItem);
		m_pLoadClassMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
             pt.x + 1,
             pt.y,
             this);    // owner is the main application window
  }
  *pResult = 0;
}

/*
 *  Destroy Tree control - Cleanup and exit
 */
void CDClassView::OnDestroy() 
{
  if ( m_pPieFrame )
    m_pPieFrame->DestroyWindow();

  if ( m_pRCMenu ) delete m_pRCMenu;

	CTreeView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}
