// JarListView.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "JarListView.h"
#include "jardoc.h"


extern UINT DM_VIEWUPDATE;


IMPLEMENT_DYNCREATE(CJarListView, CListView)

CJarListView::CJarListView()
{

}

CJarListView::~CJarListView()
{
}

BEGIN_MESSAGE_MAP(CJarListView, CListView)
  ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate)
	//{{AFX_MSG_MAP(CCPListView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CJarListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  lpCreateStruct->style |= LVS_REPORT | LVS_SHOWSELALWAYS;

	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	
	return 0;
}


// CJarListView diagnostics

#ifdef _DEBUG
void CJarListView::AssertValid() const
{
	CListView::AssertValid();
}

void CJarListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// CJarListView message handlers

LONG CJarListView::OnDmViewUpdate( UINT, LONG )
{
	CJarDoc * pDoc= (CJarDoc *)GetDocument();
  CRect rect;
  char    		szTitle[5][64] = {"Jar File", "Size", "Last created", "Last Modified", "No. of files in jar"};
  
  GetParent()->SetWindowText(pDoc->GetPathName());

	GetWindowRect(&rect);
  GetListCtrl().SetTextColor(RGB(0,0,255));


  InsertListColumn(0, rect.Width(), szTitle[0]);  // assumes return value is OK.
  InsertListItem( 0, 0, (char *)(LPCTSTR)pDoc->GetPathName());

	return 1UL;
}

void CJarListView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
	
  GetListCtrl().SetColumnWidth(0, cx);
}
	
	// TODO: Add your message handler code here
	


void CJarListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) {
  
  CListCtrl& ctrl = GetListCtrl();
  CString cs;
  POSITION n = ctrl.GetFirstSelectedItemPosition();
  if(n == NULL)
    return;
    
  GetListItem((int)--n, cs);
  CJdependsApp * pApp = (CJdependsApp *)AfxGetApp();
  CMultiDocTemplate * pTemplate = pApp->GetJarClassTemplate();
  if(pTemplate) {
     pTemplate->OpenDocumentFile(cs);
  }
  
}

void CJarListView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) {
}

void CJarListView::GetListItem(int row, CString& ref) {
    LVITEM lvitem;
    CHAR szBuff[_MAX_PATH + 1];

    memset( &szBuff, 0x0, sizeof(szBuff));
    lvitem.mask = LVIF_TEXT ;
    //pos is a 1-based index, so you must decrement
    lvitem.iItem      = row;
    lvitem.iSubItem   = 0;
    lvitem.pszText    = szBuff;
    lvitem.cchTextMax = sizeof(szBuff);

    GetListCtrl().GetItem(&lvitem);
    
    ref = (LPCTSTR)lvitem.pszText;
}

int  CJarListView::InsertListColumn( int column, int width, char * text )
{
  LV_COLUMN		lvcolumn;
  if ( !text ) return FALSE;

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.pszText = text;
	lvcolumn.iSubItem = column;
	lvcolumn.cx = width;  
	GetListCtrl().InsertColumn(column, &lvcolumn);  // assumes return value i

  return TRUE;
}

/*
 *   Insert one list item
 */ 
int  CJarListView::InsertListItem(int index, int entry, char * text )
{
 	LV_ITEM			lvitem;
 	lvitem.mask = LVIF_TEXT ;
	lvitem.iItem = index;
	lvitem.iSubItem = entry;
	lvitem.pszText = text;
  if(entry == 0)
		return GetListCtrl().InsertItem(&lvitem);
	return GetListCtrl().SetItem(&lvitem); 
}