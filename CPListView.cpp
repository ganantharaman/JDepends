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
#include "CPListView.h"
#include "classinfo.h"
#include "jdependsdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT DM_VIEWUPDATE;


/////////////////////////////////////////////////////////////////////////////
// CCPListView

IMPLEMENT_DYNCREATE(CCPListView, CListView)

CCPListView::CCPListView()
{
  m_iStartItem = m_iEndItem = 0;
  m_pMoreMenu = NULL;
  m_clrText = RGB(255,0,255);
}

CCPListView::~CCPListView()
{
}


BEGIN_MESSAGE_MAP(CCPListView, CListView)
  ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate)
	//{{AFX_MSG_MAP(CCPListView)
  ON_COMMAND( PMID_CHGCLR, OnPopupMenuHandlerChgClr )
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LONG CCPListView::OnDmViewUpdate( UINT, LONG )
{
	CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
  char    		szTitle[5][10] = {"String", "Bytes", "Type", "Length", "CPIndex"};
  int width;
  CRect rect;
	CClassInfo * pRoot = pDoc->GetRoot();

  m_iStartItem = 0;
  m_iCount = pRoot->GetCPInfoCount();
  m_iEndItem = ( m_iCount > 999 ) ? 1000 : m_iCount;

	GetListCtrl().GetWindowRect(&rect);
  GetListCtrl().SetTextColor(m_clrText);

  for (int i = 0; i < 5; i++)  // add the columns to the list control
	{
    width = (i == 0 || i == 1) ? rect.Width() / 10 : rect.Width() * 8 / 10;
	  InsertListColumn(i, width, szTitle[i]);  // assumes return value is OK.
	}

  PopulateListCtrl( pRoot );
    
	return 1UL;
}

/////////////////////////////////////////////////////////////////////////////
// CCPListView drawing

void CCPListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CCPListView diagnostics

#ifdef _DEBUG
void CCPListView::AssertValid() const
{
	CListView::AssertValid();
}

void CCPListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCPListView message handlers

int CCPListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  lpCreateStruct->style |= LVS_REPORT | LVS_SHOWSELALWAYS;

	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	
	return 0;
}

int  CCPListView::InsertListColumn( int column, int width, char * text )
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
int  CCPListView::InsertListItem(int index, int entry, char * text )
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

/*
 *  Populate list control
 */
void CCPListView::PopulateListCtrl(CClassInfo * pInfo)
{
  char *    str;
  char buf[20];
  int iItem, iIndex, i = 0;
  const char * dummy = "..";
  CString csInfo;

  for (iItem = m_iStartItem; iItem < m_iEndItem; iItem++)
  {
     iIndex = iItem % 1000;
     // First Column 
     str = pInfo->GetCPInfoString( iItem );
     InsertListItem( iIndex, 0, str ? str : (char *)dummy);

     // Second
     csInfo = "";
     VERIFY( pInfo->GetCPInfoBytes(iItem, csInfo) );
     InsertListItem( iIndex, 1, csInfo.GetBuffer(csInfo.GetLength()));

     // Third     
     str = pInfo->GetCPInfoType( iItem );
     InsertListItem( iIndex, 2, str ? str : (char *)dummy);

     // Fourth
     i = pInfo->GetCPInfoLength( iItem );
     sprintf( buf, "%d", i );
     InsertListItem( iIndex, 3, buf);

     // Fifth
     sprintf( buf, "%d", iItem );
     InsertListItem(iIndex , 4, buf);     
  }

  if ( iItem == m_iEndItem && (m_iEndItem != m_iCount)) {
      for ( i = 0; i < 5; i ++)
         InsertListItem(1000, i, "--More--");
  }
}


void CCPListView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
  CRect rect;
  int width;
	for (int i = 0; i < 5; i++)  // add the columns to the list control
	{
    width = (i == 0 || i == 1) ? cx / 10 : cx * 8 / 30;
	  GetListCtrl().SetColumnWidth( i, width );
	}
	
	// TODO: Add your message handler code here
	
}

void CCPListView::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
  	
	*pResult = 0;
}

void CCPListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
  UINT state;
  CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
 	CClassInfo * pRoot = pDoc->GetRoot();

  // Count > CRITICAL
  if ( m_iCount > 1000 ) {
    state = GetListCtrl().GetItemState(1000, -1);
    if ( state & LVIS_SELECTED ) {
      m_iStartItem = m_iEndItem;
      if ( (m_iCount - m_iEndItem) > 1000 )
         m_iEndItem += 1000;
      else
         m_iEndItem = m_iCount;
      LockWindowUpdate();
      GetListCtrl().DeleteAllItems();
      PopulateListCtrl( pRoot );
	    UnlockWindowUpdate();
    }
  }
 	
	*pResult = 0;
}

void CCPListView::OnPopupMenuHandlerChgClr()
{
   CColorDialog cdg;

   // Set up initial color 
   cdg.m_cc.lStructSize = sizeof( CHOOSECOLOR );
   cdg.m_cc.rgbResult = m_clrText;
   cdg.m_cc.Flags |= (CC_RGBINIT);

   cdg.DoModal();

   m_clrText = cdg.GetColor();
   GetListCtrl().SetTextColor( m_clrText );

   // Redraw all the items 
   CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
 	 CClassInfo * pRoot = pDoc->GetRoot();

   // Lock window display
   LockWindowUpdate();

   // populate list ctrl
   GetListCtrl().DeleteAllItems();
   PopulateListCtrl( pRoot );

   // unlock 
   UnlockWindowUpdate();
}

void CCPListView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
  if (!m_pMoreMenu) {
    m_pMoreMenu = new CMenu;
    m_pMoreMenu->CreatePopupMenu();
    m_pMoreMenu->AppendMenu(MF_ENABLED|MF_STRING, PMID_SHOW1, "Show 0-1000");
    m_pMoreMenu->AppendMenu(MF_ENABLED|MF_STRING, PMID_SHOW2, "Show Last 1000");
    m_pMoreMenu->AppendMenu(MF_ENABLED|MF_STRING, PMID_CHGCLR, "Change Color");
  }

  POINT pt;
  ::GetCursorPos( &pt );

  m_pMoreMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
        pt.x + 3,
        pt.y,
        this);    // owner is the main application window
	
	*pResult = 0;
}

BOOL CCPListView::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

  if ( m_pMoreMenu ) delete m_pMoreMenu;
	
	return CListView::
    DestroyWindow();
}
