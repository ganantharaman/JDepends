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
#include "PieView.h"
#include "graphman.h"
#include "jdependsdoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPieView


static COLORREF clr[] = {
  RGB(255,0,0),RGB(0,0,255), RGB(0,255,0), RGB(255,255,0),
  RGB(195,195,195), RGB(121,0,0), RGB(0,255,255), RGB(255,255,0),
  RGB(0,121,0), RGB( 121,121,121), RGB( 25,25,25),RGB(0,240,180),
  RGB(255,128,128), RGB(255,255,128),  RGB(255,128,64), RGB(64,0,0),
  RGB(128,255,255), RGB(255,128,192), RGB(121,0,121), RGB(128,128,64),
  RGB(128,128,255),RGB(128,64,64), RGB(0,64,128), RGB(0,0,0) 
};

extern UINT DM_VIEWUPDATE;



IMPLEMENT_DYNCREATE(CPieView, CScrollView)

CPieView::CPieView()
{
  m_pGraphManager = NULL;
  m_bNotEmpty = m_bReady = FALSE;
}



CPieView::~CPieView()
{
}


BEGIN_MESSAGE_MAP(CPieView, CScrollView)
  ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate)
	//{{AFX_MSG_MAP(CPieView)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPieView drawing  LONG OnDmViewUpdate( UINT, LONG );

struct arr_info {
    DWORD val;
    int index;
};

int arr_compare( const void *v1, const void *v2)
{
  struct arr_info * e1 = (struct arr_info *)v1;
  struct arr_info * e2 = (struct arr_info *)v2;
  
  if ( e1->val == e2->val )
    return 0;
  else if ( e1->val < e2->val )
    return -1;

  return 1;
}


LONG CPieView::OnDmViewUpdate(UINT,LONG)
{
  CJdependsDoc* pDoc = (CJdependsDoc *)GetDocument();
  CClassInfo * pInfo = pDoc->GetRoot();
  m_bReady = FALSE;
  _GRAPH_DATA gdx;
	  
  // PIE DATA           
  m_logfont.lfHeight = -12;              
	m_logfont.lfWeight = FW_NORMAL;
  _GRAPH_TITLE_INFO  gtInfo("Code distribution", &m_logfont,RGB(255,0,0),RGB(255,255,255));
  m_logfont.lfHeight = -10;
  m_logfont.lfWeight = FW_DONTCARE;
  _GRAPH_DATA_DISPLAY_INFO  gddInfo( RGB(0,0,255), &m_logfont );

  int i, nCount = pInfo->GetMethodsCount();
  int nActualCount = 0;
  CString cs = "";

  if ( !nCount ) return 0L;

  if (m_pGraphManager )
     delete m_pGraphManager;
  m_pGraphManager = new CGraphManager;

  struct arr_info * arr = new arr_info[nCount];
  int m = 0;
  for ( i = 0; i < nCount; i++) {
      DWORD dwVal;
      /* Extract byte code if necessary */
      pInfo->ExtractMethodByteCodes( i );
      dwVal = pInfo->GetMethodByteCodeCount(i);
      if ( dwVal )  {
        arr[m].val = dwVal;
        arr[m++].index = i;
        nActualCount++;
        m_bNotEmpty = TRUE;
      }
  }

  if ( nActualCount )
  {
    qsort( arr, nActualCount, sizeof( struct arr_info ), arr_compare);
 //   m_pGraphManager->InitGraph(nActualCount, GS_PIE|GS_PIEELLIPTIC|GS_LEGENDWIN|GS_LGREFLECTSHADE,
   //         &gtInfo,&gddInfo);   
  m_pGraphManager->InitGraph(nActualCount, GS_BAR | GS_LEGENDWIN | GS_LGREFLECTSHADE | 
	GS_BC3DFRAME | GS_BC3DBARS | GS_TRANSPARENTTITLE,
		         &gtInfo,&gddInfo);   
    int m = 0;
	_GRAPH_DATA gdxMin, gdxMax, gdxIndex, gdxWidth, gdxDivUnit, gdyDivUnit;

	
	gdxMin.ugd.dwDwordValue = 0;
	gdxMax.ugd.dwDwordValue = 2*nActualCount;
	m_pGraphManager->SetHorizontalLimits(&gdxMax, &gdxMin);
	gdxMin.ugd.dwDwordValue = 1;
	gdxMax.ugd.dwDwordValue = 100;
	m_pGraphManager->SetVerticalLimits(&gdxMax, &gdxMin);
	gdxWidth.ugd.dwDwordValue = 1;
	m_pGraphManager->SetBarWidth(&gdxMin);
	CString csX = "Method";
	CString csY = "Number of Bytecode";
	m_pGraphManager->SetXAxisDescText(csX.GetBuffer(csX.GetLength()));
	m_pGraphManager->SetYAxisDescText(csY.GetBuffer(csY.GetLength()));
	gdxDivUnit.ugd.dwDwordValue = 2;
	gdyDivUnit.ugd.dwDwordValue = 5;
	m_pGraphManager->SetHorizDivisionUnit(&gdxDivUnit);
	m_pGraphManager->SetVertDivisionUnit(&gdyDivUnit);

    for ( i = 0; i < nActualCount; i++) 
    {
		gdxIndex.ugd.dwDwordValue = arr[i].index + 1;
        gdx.ugd.dwDwordValue =  pInfo->GetMethodByteCodeCount(arr[i].index);
		gdx.ugd.dwDwordValue = (gdx.ugd.dwDwordValue > 100) ? 100 : gdx.ugd.dwDwordValue;
       // m_pGraphManager->AddNewPieData( &gdx );

	    m_pGraphManager->AddNewBarData( &gdxIndex, &gdx );
        m_pGraphManager->AddNewGraphColorData(clr[i % 24]); 
        m_pGraphManager->SetNewLegendData(pInfo->GetMethodName(arr[i].index),i+1);
    }

    CString csTitle = pInfo->GetShortClassName(pInfo->GetClassName());
    csTitle += " ( Legend )";
	  m_pGraphManager->SetLegendWinTitle(csTitle.GetBuffer(csTitle.GetLength()));//orate Competitors"); 
  }

  m_bReady = TRUE;

  if ( m_pGraphManager && m_bReady && m_bNotEmpty )
    m_pGraphManager->InitGraphManager( this );
  if( arr ) delete [] arr;

  return 1L;
}

void CPieView::CenterText(CDC& dc,CFont* pFont,
						                  CString& String,
						                  int nWidth, int nHeight
						                 ) 
{
   // Get Textmetrics
   TEXTMETRIC tm;
   CFont * pOldFont = dc.SelectObject(pFont);
   dc.GetTextMetrics(&tm);
   int  bw,bh,x,y;
   bw = tm.tmAveCharWidth * String.GetLength();
   bh = tm.tmHeight;
   // Center 
   x =  (nWidth  - bw)/2;
   y =  (nHeight - bh)/2;  
   // Paint
   dc.TextOut(x, y, String); 
   dc.SelectObject(pOldFont);
}



// call once
void CPieView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

  CJdependsDoc* pDoc = (CJdependsDoc *)GetDocument();

	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

     // Give the height in point units LOGFONT
  memset(&m_logfont, 0, sizeof m_logfont);
	m_logfont.lfHeight = -12;              
	m_logfont.lfWeight = FW_NORMAL;
	static char BASED_CODE szArial[] = "Arial";
	lstrcpy(m_logfont.lfFaceName, szArial);
	m_logfont.lfOutPrecision = OUT_TT_PRECIS;
	m_logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_logfont.lfQuality = PROOF_QUALITY;
	m_logfont.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;

  
}

void CPieView::OnDraw(CDC* pDC)
{
	CJdependsDoc* pDoc = (CJdependsDoc *)GetDocument();
	// TODO: add draw code here
 
  CRect rect(0,0,sizeTotal.cx,sizeTotal.cy);

  // Bypass all other draw notifications if not ready
  if ( m_pGraphManager && m_bReady && m_bNotEmpty )
    m_pGraphManager->DrawGraph(this,pDC);//,&rect);
  else if ( !m_bNotEmpty ) { // Empty
    CRect rct;
    GetClientRect(&rct);
    CFont font;
  	m_logfont.lfHeight = -20;              
    m_logfont.lfWeight = FW_NORMAL;
    font.CreateFontIndirect(&m_logfont);
    pDC->SetTextColor( RGB(255,0,0));
    CString csText("Empty Methods found!"); 
    CenterText( *pDC, &font, 
      csText, rct.Width(), rct.Height());
  }

}

/////////////////////////////////////////////////////////////////////////////
// CPieView diagnostics

#ifdef _DEBUG
void CPieView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPieView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPieView message handlers
/*
BOOL CPieView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
  return TRUE;
}
*/


void CPieView::OnDestroy() 
{
  delete m_pGraphManager;

	CScrollView::OnDestroy();
	
	// TODO: Add your message handler code here

}
