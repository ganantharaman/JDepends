// JarClassListView.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "jdependsdoc.h"
#include "jardoc.h"
#include "JarClassListView.h"

extern "C" int view_file_cback(char * file, void * data);

extern UINT DM_VIEWUPDATE;


typedef int (*LIST_CLASSES_CALLBACK)(char * file, void * info);
extern "C" 
int list_classes_in_jar(char * jar, LIST_CLASSES_CALLBACK cback) ;

CList<CString, CString&> CJarClassListView::m_files;



// CJarClassListView

IMPLEMENT_DYNCREATE(CJarClassListView, CListView)

CJarClassListView::CJarClassListView()
{
}

CJarClassListView::~CJarClassListView()
{
}

BEGIN_MESSAGE_MAP(CJarClassListView, CListView)
ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate)
	//{{AFX_MSG_MAP(CCPListView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern "C"
int view_file_cback(char * filename, void *test) {
  CString cs(filename);
  CJarClassListView::m_files.AddTail(cs);
  return 0;
}

int CJarClassListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  lpCreateStruct->style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SORTASCENDING;

	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	
	return 0;
}



LONG CJarClassListView::OnDmViewUpdate( UINT, LONG )
{
	CJarDoc * pDoc= (CJarDoc *)GetDocument();
  CRect rect;
  char    		szTitle[5][64] = {"Classes"};

	GetWindowRect(&rect);
  GetListCtrl().SetTextColor(RGB(0,0,255));
  
  GetParent()->SetWindowText( pDoc->GetPathName()+	" Classes");

  InsertListColumn(0, rect.Width(), "Classes");  // assumes return value is OK.
  
  list_classes_in_jar((char *)(LPCTSTR)pDoc->GetPathName(), view_file_cback);
  
  POSITION pos = m_files.GetHeadPosition();
	for (int i=0;i < m_files.GetCount();i++) {
	    InsertListItem( i, 0, (char *) (LPCSTR) m_files.GetNext(pos));
	}
  

	return 1UL;
}

void CJarClassListView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
	
  GetListCtrl().SetColumnWidth(0, cx);
}
	
	// TODO: Add your message handler code here
	


void CJarClassListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) {
  
  CListCtrl& ctrl = GetListCtrl();
  CString cs;
  POSITION n = ctrl.GetFirstSelectedItemPosition();
  if(n == NULL)
    return;
    
  GetListItem((int)--n, cs);
  CJdependsApp * pApp = (CJdependsApp *)AfxGetApp();
  CMultiDocTemplate * pTemplate = pApp->GetClassTemplate();
  if(pTemplate) {
     CJarDoc * pDoc= (CJarDoc *)GetDocument();     
     CJdependsDoc::SetClass(cs);
     pTemplate->OpenDocumentFile((LPCTSTR)pDoc->GetPathName());
  }
}

void CJarClassListView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) {
}

void CJarClassListView::GetListItem(int row, CString& ref) {
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

int  CJarClassListView::InsertListColumn( int column, int width, char * text )
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
int  CJarClassListView::InsertListItem(int index, int entry, char * text )
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



// CJarClassListView diagnostics

#ifdef _DEBUG
void CJarClassListView::AssertValid() const
{
	CListView::AssertValid();
}

void CJarClassListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// CJarClassListView message handlers
