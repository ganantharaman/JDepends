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
#include "split.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplit          

IMPLEMENT_DYNCREATE(CSplit, CSplitterWnd)

CSplit::CSplit()
{  
  m_csSplitTitle = "";
  m_dwSplitStyle = 0x00000002;
}

CSplit::~CSplit()
{
}               

void CSplit::SetSplitStyle(DWORD dwStyle)
{                          
 if (( dwStyle & SPLIT_BMP ) || ( dwStyle & SPLIT_TEXT ))
   m_dwSplitStyle = dwStyle;
}

BOOL CSplit::SetSplitBitmap(UINT nBmpResID)
{
  return m_Bitmap.LoadBitmap(nBmpResID);
} 


BEGIN_MESSAGE_MAP(CSplit, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplit)
	ON_WM_SIZE()
	ON_WM_CREATE()    
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplit message handlers

void CSplit::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
  CSplitterWnd::OnDrawSplitter(pDC,nType,rectArg);   
 
  CRect rect = rectArg;  
  
  if ( pDC )
   {
     if ( m_dwSplitStyle & SPLIT_BMP )
      {
        CDC dcMem;
        dcMem.CreateCompatibleDC(pDC);
        CBitmap *pOldBitmap = dcMem.SelectObject( &m_Bitmap );                   
        BITMAP bm;
        m_Bitmap.GetObject(sizeof(BITMAP),&bm);
        pDC->BitBlt(rect.left,rect.top,rect.Width(),rect.Height(),&dcMem,0,0,SRCCOPY);
        dcMem.SelectObject( pOldBitmap );
      }
      
      if ( m_dwSplitStyle & SPLIT_TEXT )
      {
        if ( !m_csSplitTitle.IsEmpty() )
        {                  
          pDC->PatBlt(rect.left, rect.top, rect.Width(), 1, WHITENESS);
	        pDC->PatBlt(rect.left, rect.top, 1, rect.Height(), WHITENESS);
	       // bottom or right shadow for 3D look
	        pDC->SelectStockObject(DKGRAY_BRUSH);
	        pDC->PatBlt(rect.right - 1, rect.top, 1, rect.Height(), PATCOPY);
	        pDC->PatBlt(rect.left, rect.bottom - 1, rect.Width(), 1, PATCOPY);
          pDC->SelectStockObject(LTGRAY_BRUSH);
          rect.InflateRect(-1, -1);
  	      // fill the middle
	        pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
          pDC->SetTextColor(RGB(255,0,0));
          pDC->SetBkMode(TRANSPARENT);
          pDC->DrawText(m_csSplitTitle,-1,rect,DT_CENTER|DT_VCENTER);
        } 
      }  
    }  
}    

BOOL CSplit::PreCreateWindow(CREATESTRUCT& cs)
{   
  
  return CSplitterWnd::PreCreateWindow(cs);
}


void CSplit::OnSize(UINT nType, int cx, int cy) 
{
   CSplitterWnd::OnSize(nType, cx, cy);
  // TODO: Add your message handler code here
}

void CSplit::SetSplitTitle(LPCSTR lpszTitle)
{  m_csSplitTitle = (LPSTR)lpszTitle;   }

int CSplit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{         
    if ( m_dwSplitStyle & SPLIT_BMP )
    {
      BITMAP bm;
      m_Bitmap.GetObject(sizeof(BITMAP),&bm);
      m_cySplitter = max(20,bm.bmHeight);
      m_cxSplitter = max(20,bm.bmWidth);
    }  
    if (CSplitterWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	return 0;
}


void CSplit::OnPaint() 
{
	// TODO: Add your message handler code here
  CSplitterWnd::OnPaint();	
 // Do not call CSplitterWnd::OnNcPaint() for painting messages
} 
