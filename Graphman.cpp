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
#include "graphman.h"
#include "math.h"


#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))

#ifdef WIN32
#define STRING_COPY lstrcpy
#else
#define STRING_COPY _fstrcpy
#endif  

// also calls constructor
#define ALLOC_BLOCK(x)  new x;
#define FREE_BLOCK(x)   delete x;

#ifdef WIN32
#define QUECRITSECTION 1
#define RTGCRITSECTION 2
#define GRDCRITSECTION 3
#endif
                               
_GRAPH_DATA ::_GRAPH_DATA()
{ 
  ugd.dDoubleValue     = 0.0;
  ugd.dwDwordValue     = 0L;
  pNext                = NULL;
}                   


_GRAPH_TITLE_INFO::_GRAPH_TITLE_INFO()
{                                     
  lpszTitle            = NULL; 
  crTitleFgColor       = RGB(255,0,0);
  crTitleBkColor       = RGB(255,255,255);
  memset (&lfLogFont,0,sizeof lfLogFont);
}

_GRAPH_TITLE_INFO::_GRAPH_TITLE_INFO(LPSTR lpTitle,LOGFONT * pLogFont,
                                     COLORREF crFg,COLORREF crBk ) 
{                                     
  lpszTitle            = lpTitle;
  lfLogFont            = *pLogFont;
  crTitleFgColor       = crFg;
  crTitleBkColor       = crBk;
}

_GRAPH_DATA_DISPLAY_INFO ::_GRAPH_DATA_DISPLAY_INFO ()
{
  crDataDisplayColor   = RGB(255,0,0);
  memset (&lfLogFont,0,sizeof lfLogFont);
}

_GRAPH_DATA_DISPLAY_INFO::_GRAPH_DATA_DISPLAY_INFO(COLORREF crData,LOGFONT *plfDataFont)
{
  crDataDisplayColor   = crData;
  lfLogFont            = *plfDataFont;
}


_BAR_CURVE_INFO::_BAR_CURVE_INFO()
{
  pgdVertGraphData     = NULL;
  lpszHorizTitle       = NULL;
  lpszVertTitle        = NULL;
  pgdHorizGraphData    = NULL;
  pgpHorizPtText       = NULL;
  pgpVertPtText        = NULL;
  crGridColor          = RGB(255,255,255); // Default Grid Color
}

_GRAPH_INFO::_GRAPH_INFO()
{
  lpszLegendInfo       = NULL;
  pTitleInfo           = NULL;
  lpszLegendWinTitle   = NULL;
  pGDataDisplayInfo    = NULL;
  pgcdColorValues      = NULL;
  pgdPieData           = NULL;
  pgtBarOrCurve        = NULL;
  nNumberOfItems       = 0;
} 


CGraphManager::CGraphManager()
{  
  m_pLegendWnd         = NULL;
  m_pMemBitmap         = NULL;
  m_pDataFont          = NULL;
  m_pTitleFont         = NULL;
  m_pSavedRects        = NULL;
  m_nLegendLenMax      = 0;
  m_xDisplayRange      = 0;
  m_yDisplayRange      = 0;
  m_dwGraphStyle       = 0;
  m_dTotal             = -1;
  m_crOldClientRect.SetRect(0,0,0,0);
}              

CGraphManager::~CGraphManager()
{         
  if( m_pMemBitmap )
  {  delete m_pMemBitmap; m_pMemBitmap = NULL;  }

  if ( m_pLegendWnd )
  {  m_pLegendWnd->DestroyWindow(); m_pLegendWnd = NULL; }

  FreeCollisionRects();
  
  if ( GRAPHDATADISPLAYINFO )
   { FREE_BLOCK( GRAPHDATADISPLAYINFO ); GRAPHDATADISPLAYINFO = NULL; }
   
  FreeGraphDataList( GRAPHDATA  );
  FreeString ( GRAPHLGWINTITLE ); 
  FreeGraphColorDataList( GRAPHCOLORDATA );     
  FreeFont( &m_pDataFont );
  FreeFont( &m_pTitleFont ); 
  
  if ( GRAPHBARORCURVE )
  {
    FreeGraphDataListNodes( BCXDATA );
    FreeGraphDataListNodes( BCYDATA );
    FreeGraphPtTextListNodes( BCXPTEXT );
    FreeGraphPtTextListNodes( BCYPTEXT );
    FreeString( BCXTITLE ); 
    FreeString( BCYTITLE ); 
    FreeBarOrCurveSpecificInfo( GRAPHBARORCURVE );
    GRAPHBARORCURVE = NULL;
  } 
  
  if ( GRAPHTITLEINFO )
  {
   FreeString( GRAPHTITLE );
   FREE_BLOCK( GRAPHTITLEINFO );
   GRAPHTITLEINFO = NULL;
  } 
   
  if ( GRAPHLEGEND )
  {
    for ( int nPos = 0;nPos < GRAPHITEMNUM;nPos ++)
       FreeString( GRAPHLEGEND[nPos] );
    delete [] GRAPHLEGEND;
    GRAPHLEGEND = NULL;
  }      
} 

void CGraphManager::FreeBarOrCurveSpecificInfo( _BAR_CURVE_INFO *pBarOrCurve)
{  FREE_BLOCK( pBarOrCurve );  } 

CFont * CGraphManager::AllocFont()
{        
  GraphSetNewHandler();
  CFont *p = new CFont;
  GraphClearNewHandler();
  return p;
}  

void CGraphManager::FreeFont(CFont **pFont)
{
  if ( *pFont )
   { delete *pFont ; *pFont = NULL; }
}

void CGraphManager::GraphSetNewHandler()
{
  // causes 'new' to return NULL on failure
  // to allocate memory
//  m_pnhOldNewHandler = set_new_handler(NULL);
}

void CGraphManager::GraphClearNewHandler()
{
  // reset the old new handler
// set_new_handler(m_pnhOldNewHandler);
}


void CGraphManager::FreeGraphDataList( _GRAPH_DATA * pGraphData)
{
  _GRAPH_DATA *p = pGraphData;
  _GRAPH_DATA *q = p;  
  
  if ( !p ) return;
  
  // Go thro' the list and Free
  do { q = p->pNext;   FreeGraphData( p ); } while (p = q);
}

void CGraphManager::FreeGraphColorDataList( _GRAPH_COLOR_DATA * pGraphData)
{
  _GRAPH_COLOR_DATA *p = pGraphData;
  _GRAPH_COLOR_DATA *q = p;     
  
  if ( !p ) return;              

  // Go thro' the list and Free
  do { q = p->pNext;   FreeGraphColorData( p ); } while (p = q);
}                      

void CGraphManager::FreeGraphDataListNodes( _GRAPH_DATA **pGD)
{  
  int nNumLists = 1;
  
  // For curves we need to get the
  // number of curves
  // for Bar diagram it is one 
  // For pie this function never gets called
  if ( m_dwGraphStyle & GS_CURVE )
   nNumLists = GetNumberOfGraphItems();
   
  if ( !pGD ) return;                                 
  
  // Go thro' the list one by one and 
  // delete it
  for ( int i = 0; i < nNumLists; i++)
     FreeGraphDataList( pGD[i] );
  delete pGD;
}  

void CGraphManager::FreeGraphTitleInfo(_GRAPH_TITLE_INFO *pGTI)
{             
   if ( pGTI )
   {  FreeString( GRAPHTITLE );  delete pGTI; }  
} 

void CGraphManager::FreeDataDisplayInfo(_GRAPH_DATA_DISPLAY_INFO *pGDDI)
{  if ( pGDDI ) delete pGDDI;   }      

BOOL CGraphManager::InitGraph(int nNoOfItems, DWORD dwStyle,
      _GRAPH_TITLE_INFO *pGTInfo,_GRAPH_DATA_DISPLAY_INFO *pGDDInfo )
{ 
   BOOL bRetCode = FALSE;      
   
   // init style for the graph
   // there is no default style
   SetGraphStyle( dwStyle );
   GraphSetNewHandler();
/*
   if ( m_dwGraphStyle & GS_REALTIME )
   {
	   m_pCSH = new CCSHandler;
	   // we use 3 critical section objects
	   if ( m_pCSH )
		  m_pCSH->InitCS( 3 );

	   m_pRTGData = new _RTG_DATA;
	   if ( m_pRTGData )
		 bRetCode = TRUE;


   }
*/   
   // For pie we need not initialize Bar/Curve
   // specific data structures
   if (!(m_dwGraphStyle & GS_PIE)) 
   {
      FreeBarOrCurveSpecificInfo( GRAPHBARORCURVE );
      GRAPHBARORCURVE = ALLOC_BLOCK( _BAR_CURVE_INFO );
      if ( GRAPHBARORCURVE )
        bRetCode = TRUE;
   }  
  // Number of items - Could be
  // 1) Number of curves each with a set of co-ordinate points
  // 2) Number of bars in one bar graph
  // 3) Number of sectors for the pie 
  SetNumberOfItems( nNoOfItems ); 

  if ( m_dwGraphStyle & GS_PIE )
    InitCollisionRects( nNoOfItems );

  GraphClearNewHandler();        
      
  // Title and other display info
  SetGraphTitleInfo( pGTInfo );
  SetGraphDataDisplayInfo( pGDDInfo ); 
  return bRetCode;  
}                                

void CGraphManager::InitCollisionRects( int nNoOfItems )
{
  m_pSavedRects = nNoOfItems ? new CRect*[nNoOfItems] : NULL;
  for ( int i = 0; i < nNoOfItems ; i++ )
      m_pSavedRects[i] = new CRect( 0,0,0,0);
}

void CGraphManager::FreeCollisionRects()
{
  if ( m_pSavedRects ) {
   for ( int i = 0; i < GRAPHITEMNUM ; i++ )
      delete m_pSavedRects[i];
    delete [] m_pSavedRects;
    m_pSavedRects = NULL;
  }
}



BOOL CGraphManager::SetNumberOfItems(int nNoOfItems)
{                   
  BOOL bRetCode = FALSE;
  GRAPHITEMNUM = nNoOfItems;

  nNoOfItems =  ( m_dwGraphStyle & GS_BAR ) ? 1 :
                ( m_dwGraphStyle & GS_PIE ) ? 0 : nNoOfItems;
   
   // if it is not a pie graph
   // allocate space for data
   if ( nNoOfItems )     
   {            
    FreeGraphDataListNodes( BCXDATA );
    BCXDATA = NULL;                   
   
    FreeGraphDataListNodes( BCYDATA );
    BCYDATA = NULL; 
   
    if ( BCYDATA = AllocGraphDataVector( nNoOfItems ))
      if ( BCXDATA = AllocGraphDataVector( nNoOfItems ))
      {
          for ( int n = 0; n < nNoOfItems;n++)
           BCYDATA[n] = BCXDATA[n] = NULL;
          bRetCode = TRUE;
      }            

    // We also need to maintain a set of point-mark-texts
    // based on the style
    // Point markers Just for X
    if ( m_dwGraphStyle & GS_BCMARKPOINTX ||
         m_dwGraphStyle & GS_BCMARKPTANDDIVX )
    {     
        FreeGraphPtTextListNodes( BCXPTEXT ) ;
        BCXPTEXT = NULL;
        
      if ( BCXPTEXT = AllocGraphPtTextVector( nNoOfItems ))
      {
          for ( int n = 0; n < nNoOfItems;n++)
           BCXPTEXT[ n ] = NULL;
          bRetCode = TRUE;
      }
    }
    
    // Point markers for Y     
    if ( m_dwGraphStyle & GS_BCMARKPOINTY ||
         m_dwGraphStyle & GS_BCMARKPTANDDIVY )
    {
        FreeGraphPtTextListNodes( BCYPTEXT );
        BCYPTEXT = NULL;
        
      if ( BCYPTEXT = AllocGraphPtTextVector( nNoOfItems ))
      {
         for ( int n = 0; n < nNoOfItems;n++)
           BCYPTEXT[ n ] = NULL;
         bRetCode = TRUE;
      } // if           
    } // if 
  } // if 
  
  return bRetCode; // all set
}

void CGraphManager::SetGraphStyle(DWORD dwStyle)
{  m_dwGraphStyle = dwStyle; }

DWORD CGraphManager::GetGraphStyle()
{  return m_dwGraphStyle;   }


BOOL CGraphManager::SetGraphTitleInfo(_GRAPH_TITLE_INFO *pGTInfo)
{                     
  if ( !pGTInfo )
   return FALSE;
  GraphSetNewHandler();
  // allocate title data structures
  FreeGraphTitleInfo( GRAPHTITLEINFO );
  // intentional =
  GRAPHTITLEINFO = ALLOC_BLOCK( _GRAPH_TITLE_INFO );
  if ( GRAPHTITLEINFO ) 
    *GRAPHTITLEINFO = *pGTInfo;
    
  GraphClearNewHandler();
  // copy title
  if ((GRAPHTITLE = AllocString(lstrlen(pGTInfo->lpszTitle) + 1)) != NULL)
     STRING_COPY(GRAPHTITLE,pGTInfo->lpszTitle);
  return TRUE;
}


BOOL CGraphManager::SetNewLegendData(LPSTR lpszLegend,int nItemNum)
{ 
  // Sets up legend data for each
  // item 
  int nPos = nItemNum - 1;
  if ( GRAPHLEGEND == NULL )
  {
   if ((GRAPHLEGEND = AllocCharVector( GRAPHITEMNUM )) != NULL)
      for (int n = 0; n < GRAPHITEMNUM; n++)
        GRAPHLEGEND[ n ] = NULL;
  }    
  FreeString( GRAPHLEGEND[nPos] );
  
  // Allocates string 
  if (( GRAPHLEGEND[nPos] =
      AllocString(lstrlen(lpszLegend) + 1)) != NULL)
  {
     STRING_COPY(GRAPHLEGEND[nPos],lpszLegend);
     
     int nLen =  lstrlen(lpszLegend);
     m_nLegendLenMax  =( nLen > m_nLegendLenMax ) ?
                              nLen : m_nLegendLenMax;
  } 
 
  return TRUE;  
}

BOOL CGraphManager::AddNewGraphColorData(COLORREF rgbColor)
{
   _GRAPH_COLOR_DATA *p = GRAPHCOLORDATA;
   _GRAPH_COLOR_DATA *q = NULL;
   // allocate
   if ((q = AllocNewColorData()) != NULL)
   {
     q->rgbColor = rgbColor;
     q->pNext = NULL;
   }              
   // first entry?
   if ( !p )
   {  return  ((GRAPHCOLORDATA = q) != NULL);   }              
   // walk thro' the list
   for (;p && (p->pNext != NULL);p = p->pNext);
   // append it at the end
   p->pNext = q;
   return q ? TRUE : FALSE;
}                 

int CGraphManager::GetPointsInCurve(int nCurveNum )
{
   int nIndex = 1;                              
   // same number of points in X or Y
   _GRAPH_DATA *p = BCXDATA[ nCurveNum - 1 ] ;
   // walk thro' the list
   for (;p->pNext;p = p->pNext,nIndex ++);
   // return 
   return nIndex;
}    

double CGraphManager::GetCurveDisplayStartYPoint(int nCurveNum,int nPointNum)
{
  return  nCurveNum ? 
     GetBCDisplayStartYPoint(nCurveNum - 1,nPointNum) : 0.0;
}

double CGraphManager::GetCurveDisplayStartXPoint(int nCurveNum,int nPointNum)
{ 
  return nCurveNum ? 
     GetBCDisplayStartXPoint(nCurveNum - 1,nPointNum) : 0.0;
}         

double CGraphManager::GetBCDisplayStartYPoint(int nItemNum,int nPointNum)
{                  
  _GRAPH_DATA *p =  BCYDATA[nItemNum];
  double dYPoint =  0.0;
  int    index   =  1;
  for (; p && index < nPointNum; p = p->pNext,index++);
  if ( p )                        
    dYPoint = GetDisplayPoint( *p , FALSE );
         
  return dYPoint;
} 

LPSTR CGraphManager :: GetBarHorizPointText(int nPointNum)
{   return GetBarOrCurvePointText( BCXPTEXT[0], nPointNum );                }
LPSTR CGraphManager :: GetBarVertPointText(int nPointNum)
{   return GetBarOrCurvePointText( BCYPTEXT[0], nPointNum );                }                      
LPSTR CGraphManager :: GetCurveHorizPointText(int nCurveNum,int nPointNum)
{   return GetBarOrCurvePointText( BCXPTEXT[nCurveNum - 1], nPointNum );    } 
LPSTR CGraphManager :: GetCurveVertPointText(int nCurveNum,int nPointNum)
{   return GetBarOrCurvePointText( BCYPTEXT[nCurveNum - 1], nPointNum );    } 
BOOL  CGraphManager :: AddNewBarData(_GRAPH_DATA *pGDX,_GRAPH_DATA *pGDY)
{   return AddNewNonPieData(pGDX,pGDY,0);                                   }                                      
BOOL  CGraphManager :: AddNewCurveData(_GRAPH_DATA *pGDX,_GRAPH_DATA *pGDY,int nCurveIndex)
{   return AddNewNonPieData(pGDX,pGDY,nCurveIndex - 1);                     }
int   CGraphManager :: GetNumberOfGraphItems()
{   return GRAPHITEMNUM;                                                    }             
int   CGraphManager :: GetNumberOfBars()
{   return GetNumberOfGraphItems();                                         }                     
int   CGraphManager :: GetNumberOfCurves()
{   return GetNumberOfGraphItems();                                         } 
void  CGraphManager :: FreeGraphData(_GRAPH_DATA * p)
{   if ( p ) delete p;                                                      }     
void  CGraphManager :: FreeGraphColorData(_GRAPH_COLOR_DATA *p)
{   if ( p ) delete p;                                                      }        
void  CGraphManager :: FreeString( LPSTR p )
{   if ( p ) delete [] p ;                                                  }              

LPSTR CGraphManager::GetBarOrCurvePointText(_GRAPH_PTEXT *p,int nPointNum)
{
  int  index   =  1;
  for (; p && index < nPointNum; p = p->pNext,index++);
  return p ? p->lpszText : NULL;
}          

// 
double CGraphManager::GetBCDisplayStartXPoint(int nItemNum,int nPointNum)
{
  _GRAPH_DATA * p  = BCXDATA[ nItemNum ];
  double dXPoint   = 0.0;
  int index        = 1;
  for (; p && index < nPointNum; p = p->pNext,index++);
  if ( p )                              
    dXPoint = GetDisplayPoint( *p ,TRUE);     
  return dXPoint;
} 

double CGraphManager::GetDisplayPoint(_GRAPH_DATA gd,BOOL bX)
{
	_GRAPH_DATA *pgdXYMin = &BCXMIN;
	double dXYPoint = 0.0;
	double dRatio = (double)m_xDisplayRange  / GetHorizontalRange(); 
	if ( !bX )
	{
	  dRatio = (double)m_yDisplayRange  / GetVerticalRange(); 
	  pgdXYMin = &BCYMIN;
	} 

	dXYPoint  =  ( IsFloat() ) ? gd.ugd.dDoubleValue 
                : gd.ugd.dwDwordValue ;        
    (dXYPoint -= (IsFloat()) ? pgdXYMin->ugd.dDoubleValue : 
          pgdXYMin->ugd.dwDwordValue ) *= dRatio;       

    return dXYPoint;
}

 
BOOL CGraphManager::AddNewPieData(_GRAPH_DATA *pGD)
{   
    BOOL bTypeDouble = IsFloat();
    _GRAPH_DATA * p  = GRAPHDATA ;
    _GRAPH_DATA * q  = NULL;      
    
    // Allocate new graph data object
    if ((q = AllocNewGraphData()) != NULL)
     bTypeDouble ?(q->ugd.dDoubleValue = pGD->ugd.dDoubleValue):
                  (q->ugd.dwDwordValue = pGD->ugd.dwDwordValue);
    
    // if First entry update 'head' 
    // and return
    if ( !p )
    {  return ((GRAPHDATA = q) != NULL);    }    
    
    // else append it
    for (;p && (p->pNext != NULL);p = p->pNext);
    
    p->pNext = q;
    
    return q ? TRUE : FALSE;
}                                   

/* Any Non-pie data has an x and y co-ordinate */
BOOL CGraphManager::AddNewNonPieData(_GRAPH_DATA *pGDX,_GRAPH_DATA *pGDY,int nItem )
{
    BOOL bTypeDouble = IsFloat();
    _GRAPH_DATA * pX = BCXDATA[nItem];
    _GRAPH_DATA * pY = BCYDATA[nItem];
    _GRAPH_DATA * x  = NULL;
    _GRAPH_DATA * y  = NULL;
    
    // Alloc for x
    if ((x = AllocNewGraphData()) != NULL)
     bTypeDouble ? (x->ugd.dDoubleValue = pGDX->ugd.dDoubleValue)
                 : (x->ugd.dwDwordValue = pGDX->ugd.dwDwordValue);
        
    // Alloc for y    
    if ((y = AllocNewGraphData()) != NULL)
     bTypeDouble ? (y->ugd.dDoubleValue = pGDY->ugd.dDoubleValue)
                 : (y->ugd.dwDwordValue = pGDY->ugd.dwDwordValue);     

    // Append both to the list    
    if ( !pX ) {  BCXDATA[nItem] = x; }
    else
    { for (;pX && (pX->pNext != NULL);pX = pX->pNext); pX->pNext = x; }
    if ( !pY ) {  BCYDATA[nItem] = y; }                
    else
    { for (;pY && (pY->pNext != NULL);pY = pY->pNext); pY->pNext = y; }

 
    return x ? y ? TRUE : FALSE : FALSE;
}
 
// Allocates for one GRAPH_COLOR_DATA object and returns
// a pointer to it
_GRAPH_COLOR_DATA * CGraphManager::AllocNewColorData()
{             
  _GRAPH_COLOR_DATA * p = AllocGraphColorData();
  
  if ( p )
  {
    p->pNext = NULL;
    p->rgbColor = RGB(255,0,0);
  }
  return p;
} 


// Allocates a new graph data
_GRAPH_DATA *CGraphManager::AllocNewGraphData()
{ 
  _GRAPH_DATA *p =  AllocGraphData();
   
  if ( p )
  {
    p->pNext   = NULL;
    p->ugd.dwDwordValue = 0;
    p->ugd.dDoubleValue = 0.0;
  }  
  
  return p;
}  

// GRAPH_COLOR_DATA allocation primitive
_GRAPH_COLOR_DATA * CGraphManager::AllocGraphColorData()
{
   GraphSetNewHandler();
   _GRAPH_COLOR_DATA *p = new _GRAPH_COLOR_DATA;
   GraphClearNewHandler();
   return p;
}

// GRAPH_DATA allocation primitive
_GRAPH_DATA * CGraphManager::AllocGraphData()
{
   GraphSetNewHandler();
   _GRAPH_DATA *p = new _GRAPH_DATA;
   GraphClearNewHandler();
   return p;
}

// string allocation primitive
LPSTR CGraphManager::AllocString(size_t nSize)
{
  GraphSetNewHandler();
  LPSTR p =  new char[nSize];
  GraphClearNewHandler();
  return p;
}

// Allocates a Graphdata vector
_GRAPH_DATA ** CGraphManager::AllocGraphDataVector(size_t nSize)
{
  GraphSetNewHandler();  
  
  _GRAPH_DATA **p = new _GRAPH_DATA*[nSize];          
  
  GraphClearNewHandler();
  
  return p;
}                

// char array vector
LPSTR * CGraphManager::AllocCharVector(size_t nSize)
{
  GraphSetNewHandler();
  LPSTR *p = new char*[nSize];
  GraphClearNewHandler();
  return p;
}

_GRAPH_PTEXT ** CGraphManager::AllocGraphPtTextVector(size_t nSize)
{
  GraphSetNewHandler();  
  _GRAPH_PTEXT **p = new _GRAPH_PTEXT*[nSize];          
  GraphClearNewHandler();
  return p;
}


LPSTR CGraphManager::GetGraphTitle()
{         
  BOOL bTitleInfo   =  GRAPHTITLEINFO ? TRUE : FALSE;  
  return bTitleInfo ? GRAPHTITLE : FALSE;  
}  

BOOL CGraphManager::IsFloat()
{
  return (int)( m_dwGraphStyle & GS_FLOATING );
}


double CGraphManager::GetPieSectorPercentage(int nSectorNum)
{
  if (GRAPHITEMNUM == 0)
   return -1;
   
  BOOL bTypeDouble = IsFloat();
  _GRAPH_DATA * p  = GRAPHDATA;
  double dPercent  = -1;
  
  if ( m_dTotal == -1)
  {                     
    m_dTotal = 0.0;
    for (; p ; p = p->pNext)
     m_dTotal += bTypeDouble ? p->ugd.dDoubleValue : p->ugd.dwDwordValue;
  }
     
  p = GRAPHDATA;
  int index = 1; 
  double dVal = 0.0;
  for (; p && index < nSectorNum; p = p->pNext,index ++);
  
  if ( p )
  {
    dVal = bTypeDouble ? p->ugd.dDoubleValue : p->ugd.dwDwordValue;
    dPercent = (dVal / m_dTotal) * 100.0 ;
  } 
  
  return dPercent;
}   



LPSTR CGraphManager::GetGraphLegendString(int nItemNum)
{             
  if ( GRAPHLEGEND )                               
   if ( GRAPHLEGEND[nItemNum - 1] )
     return GRAPHLEGEND[nItemNum - 1];
  
  return NULL;
}        

BOOL CGraphManager::InitGraphManager( CWnd *pWnd )
{ 
  // for speed display
  CDC *pDC = pWnd->GetDC(); 
  int nCXScreen = ::GetSystemMetrics(SM_CXSCREEN);
  int nCYScreen = ::GetSystemMetrics(SM_CYSCREEN);
  
  if ( !m_pMemBitmap )
  {
     m_pMemBitmap = new CBitmap;
     if ( !m_pMemBitmap ) goto _exit;
     m_pMemBitmap->CreateCompatibleBitmap(pDC,nCXScreen,nCYScreen);
     CDC dcMem;
     dcMem.CreateCompatibleDC(pDC);
     CBitmap *pOldBitmap = dcMem.SelectObject( m_pMemBitmap );
     dcMem.PatBlt(0,0,nCXScreen,nCYScreen,WHITENESS);
     dcMem.SelectObject( pOldBitmap );
  } 
  
_exit:                   
  pWnd->ReleaseDC(pDC);
  return CreateLegendWindow( pWnd );
}


BOOL CGraphManager::CreateLegendWindow(CWnd *pView)
{                       
 if (!( m_dwGraphStyle & GS_LEGENDWIN ))
   return FALSE;
   
  CRect crClientRect;
  pView->GetClientRect(crClientRect);
  
  int top    =  crClientRect.top +   crClientRect.Height() / 2 - 20;
  int width  =  CalcMaxWidth(pView);
  int left   =  crClientRect.right - (crClientRect.Width() / 6 + width);
  int height =  CalcMaxHeight(pView);

  if ( top <= 0 )
    top = 10;
  if ( left <= 0)
    left = 10;
  
  CPoint pt(left,top);
  
  pView->ClientToScreen(&pt);
  
  if ( m_pLegendWnd )
    return TRUE;
  
  TRY
  {
    m_pLegendWnd = new CLegendWnd(this) ;
  }
  CATCH_ALL(e)
  {
    m_pLegendWnd = NULL;
  } 
  END_CATCH_ALL
  
  
  ASSERT ( m_pLegendWnd != NULL);

  const char *pszClass = 
   AfxRegisterWndClass(CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|CS_SAVEBITS,
     0,(HBRUSH)::GetStockObject(WHITE_BRUSH));
   
  m_pLegendWnd->CreateLegendWindow(pszClass,
   CRect(pt.x,pt.y,pt.x + width,pt.y + height),pView->m_hWnd);
     
  return TRUE;
} 

int CGraphManager::CalcMaxWidth(CWnd *pWnd)
{ 
  ASSERT( pWnd != NULL);
  int nWidth = 0;
  TEXTMETRIC tm;
  CDC *pDC = pWnd->GetDC();
  pDC->GetTextMetrics(&tm);
  nWidth =  4 * (tm.tmAveCharWidth * m_nLegendLenMax + 2) / 3;
  pWnd->ReleaseDC(pDC);
  return nWidth;
}

int CGraphManager::CalcMaxHeight(CWnd *pWnd)
{                                                                       
  int nWidgetHeight = ::GetSystemMetrics(SM_CYCAPTION) + 
                       2 * ::GetSystemMetrics(SM_CYFRAME) + 1;
  return  16 * GetNumberOfGraphItems() + nWidgetHeight;
}  

BOOL CGraphManager::SetLegendWinTitle(LPSTR lpszLegendTitle)
{
  FreeString ( GRAPHLGWINTITLE );
  
  if ((GRAPHLGWINTITLE  = AllocString(lstrlen(lpszLegendTitle) + 1)) != NULL)
    STRING_COPY(GRAPHLGWINTITLE,lpszLegendTitle);
    
  m_nLegendLenMax = max(m_nLegendLenMax,lstrlen(lpszLegendTitle)); 
  
  return TRUE;
  
}      


LPSTR CGraphManager::GetLegendWinTitle()
{
  return GRAPHLGWINTITLE;
}

CFont * CGraphManager::GetTitleFont()
{
   return m_pTitleFont;
}                     

COLORREF CGraphManager::GetTitleFgColor()
{
  BOOL bTitleInfo   =  GRAPHTITLEINFO ? TRUE : FALSE;  
  return bTitleInfo ? GRAPHTITLEFGCOLOR : RGB(255,0,0);
}                      

COLORREF CGraphManager::GetTitleBkColor()
{
  BOOL bTitleInfo   =  GRAPHTITLEINFO ? TRUE : FALSE;  
  return bTitleInfo ? GRAPHTITLEBKCOLOR : RGB(255,255,255);
}

CFont * CGraphManager::GetPiePercentDisplayFont()
{         
  // for now return  title font
   return GetDataDisplayFont();
}                              

COLORREF CGraphManager::GetPiePercentDisplayFgColor()
{                        
  // for now return title fg color
  return GetDataDisplayColor();
} 

COLORREF CGraphManager::GetPiePercentDisplayBkColor()
{                        
  // for now return title Bk color
  return GetTitleBkColor();
} 


BOOL CGraphManager::IsPie()
{
  return (int)( m_dwGraphStyle & GS_PIE );
}

/////////////////////////////////////////////////////////////////////////////
// Bar

BOOL CGraphManager::SetDisplayRange(int xDisplayRange,int yDisplayRange)
{             
  m_xDisplayRange = xDisplayRange;
  m_yDisplayRange = yDisplayRange;
  return TRUE;
}                                 

BOOL CGraphManager::GetDisplayRange(int& xDisplayRange,int& yDisplayRange)
{
  xDisplayRange = m_xDisplayRange;
  yDisplayRange = m_yDisplayRange;
  return TRUE;
}


double CGraphManager::CalcBarDisplayWidth()
{ 
  double dRatio        =  GetBarWidth() / GetHorizontalRange();  
  double dDisplayWidth =  (double)m_xDisplayRange * dRatio;
  
  while( dDisplayWidth < 5.0 )
  {
    m_xDisplayRange ++;
    dDisplayWidth =   (double)m_xDisplayRange * dRatio;
  }
  
  return dDisplayWidth;
}  

double CGraphManager::CalcBarDisplayHeight( int nBarNum )
{ 
  return GetBCDisplayStartYPoint(0,nBarNum);
}                              

double CGraphManager::GetBarWidth()
{        
  return  (IsFloat()) ? BARWIDTH.ugd.dDoubleValue 
             : BARWIDTH.ugd.dwDwordValue;
}       

double CGraphManager::GetHorizontalRange()
{
  return  IsFloat()  ? (BCXMAX.ugd.dDoubleValue - BCXMIN.ugd.dDoubleValue):
        (BCXMAX.ugd.dwDwordValue - BCXMIN.ugd.dwDwordValue);
}       


double CGraphManager::GetVerticalRange()
{        
  return IsFloat() ? (BCYMAX.ugd.dDoubleValue -  BCYMIN.ugd.dDoubleValue):
        (BCYMAX.ugd.dwDwordValue -  BCYMIN.ugd.dwDwordValue);
}       

double CGraphManager::GetBarDisplayStartXPoint( int nBarNum )
{
  return (GRAPHITEMNUM == 0) ? 0.0 :
     GetBCDisplayStartXPoint(0,nBarNum);
}       

BOOL CGraphManager::SetBarWidth( _GRAPH_DATA *pGD )
{
  BARWIDTH = *pGD;
  
  return TRUE;
}                               

BOOL CGraphManager::SetVerticalLimits(_GRAPH_DATA *pGDMax,_GRAPH_DATA *pGDMin)
{             
  BCYMAX = *pGDMax;
  BCYMIN = *pGDMin;
  return TRUE;
}


BOOL CGraphManager::SetHorizontalLimits(_GRAPH_DATA *pGDMax,_GRAPH_DATA *pGDMin)
{
  BCXMAX = *pGDMax;
  BCXMIN = *pGDMin;
  return TRUE;
}

COLORREF CGraphManager::GetPlotColor(int nCurveNum)
{
  return GetGraphItemColor( nCurveNum );
} 

COLORREF CGraphManager::GetBarColor(int nBarNum)
{
  return GetGraphItemColor( nBarNum );
}


COLORREF CGraphManager::GetGraphItemColor(int nItemNum)
{
  COLORREF cr = RGB(255,0,0);
  if (GRAPHITEMNUM == 0)
   return cr;
  _GRAPH_COLOR_DATA * p = GRAPHCOLORDATA;
  int index = 1;
  for (; p && index < nItemNum; p = p->pNext,index++);
  if ( p )
    cr = p->rgbColor;
  return cr;  
}                    

BOOL CGraphManager::IsBar()
{
  return (int)( m_dwGraphStyle & GS_BAR );
}    

BOOL CGraphManager::IsCurve()
{ 
  return (int)( m_dwGraphStyle & GS_CURVE );
}

int CGraphManager::SetHorizDivisionUnit(_GRAPH_DATA *pGD)
{
  BCXDIVISION = *pGD;
  return TRUE;
}      

int CGraphManager::SetVertDivisionUnit(_GRAPH_DATA *pGD)
{
  BCYDIVISION = *pGD;
  return TRUE;
}

double CGraphManager::GetHorizRangeDisplayDivisionUnit()
{                  
  double dRetVal     = 0.0;
  double dRangeRatio = (double)m_xDisplayRange / GetHorizontalRange();
  
  (dRetVal =  (IsFloat()) ? BCXDIVISION.ugd.dDoubleValue :
     BCXDIVISION.ugd.dwDwordValue) *= dRangeRatio ;
     
  return dRetVal;
}                     

double CGraphManager::GetVertRangeDisplayDivisionUnit()
{
  double dRetVal     = 0.0;
  double dRangeRatio = (double)m_yDisplayRange / GetVerticalRange();
  
  (dRetVal = (IsFloat()) ? BCYDIVISION.ugd.dDoubleValue
         : BCYDIVISION.ugd.dwDwordValue ) *= dRangeRatio;
   
  return dRetVal;
}

double CGraphManager::GetHorizDivisionValue(int nMarkerIndex)
{
  BOOL bFloat     = IsFloat();
  double dDivVal  = 0.0;
  ( dDivVal = bFloat ? BCXDIVISION.ugd.dDoubleValue : 
     BCXDIVISION.ugd.dwDwordValue ) *= nMarkerIndex;
  dDivVal += bFloat ? BCXMIN.ugd.dDoubleValue : BCXMIN.ugd.dwDwordValue;   
  dDivVal = max(dDivVal,bFloat ? BCXMIN.ugd.dDoubleValue : BCXMIN.ugd.dwDwordValue);
  return min(dDivVal,bFloat ? BCXMAX.ugd.dDoubleValue : BCXMAX.ugd.dwDwordValue);
}

double CGraphManager::GetVertDivisionValue(int nMarkerIndex)
{ 
  BOOL bFloat    = IsFloat();
  double dDivVal = 0.0;
  ( dDivVal = bFloat ? BCYDIVISION.ugd.dDoubleValue : 
     BCYDIVISION.ugd.dwDwordValue ) *= nMarkerIndex;
  dDivVal +=  bFloat ? BCYMIN.ugd.dDoubleValue : BCYMIN.ugd.dwDwordValue;  
  dDivVal = max(dDivVal,bFloat ? BCYMIN.ugd.dDoubleValue : BCYMIN.ugd.dwDwordValue);   
  return min(dDivVal,bFloat ? BCYMAX.ugd.dDoubleValue : BCYMAX.ugd.dwDwordValue); 
}    

CFont * CGraphManager::GetDataDisplayFont()
{
   return m_pDataFont;
}      

COLORREF CGraphManager::GetDataDisplayColor()
{
  BOOL bDataInfo =  GRAPHDATADISPLAYINFO ? TRUE : FALSE;   
  return bDataInfo ? GRAPHDATACOLOR : RGB(255,0,0);
}

BOOL CGraphManager::SetGraphDataDisplayInfo(_GRAPH_DATA_DISPLAY_INFO *pGDDI)
{  
  if ( !pGDDI )
   return FALSE;
  
  BOOL bRetCode = FALSE;  
  FreeDataDisplayInfo( GRAPHDATADISPLAYINFO );
  GraphSetNewHandler();
  
  GRAPHDATADISPLAYINFO = ALLOC_BLOCK( _GRAPH_DATA_DISPLAY_INFO );
  
  if ( GRAPHDATADISPLAYINFO )
  {
   *GRAPHDATADISPLAYINFO = *pGDDI;
   bRetCode = TRUE;
  } 
  GraphClearNewHandler();
  return bRetCode;
}  


LPSTR CGraphManager::GetYAxisDescText()
{ 
  return BCYTITLE;
}

LPSTR CGraphManager::GetXAxisDescText()
{ 
  return BCXTITLE;
}                            

BOOL CGraphManager::SetYAxisDescText(LPSTR lpszYDesc)
{                
  FreeString( BCYTITLE ); 
  
  BOOL bRetVal = TRUE;
  
  if ((BCYTITLE = AllocString(lstrlen(lpszYDesc) + 1)) != NULL)
    STRING_COPY(BCYTITLE,lpszYDesc);
  else
    bRetVal = FALSE;
  
  return bRetVal;
}          

BOOL CGraphManager::SetXAxisDescText(LPSTR lpszXDesc)
{                
  FreeString( BCXTITLE ); 
  
  BOOL bRetVal = TRUE;
  
  if (( BCXTITLE = AllocString(lstrlen(lpszXDesc) + 1) ) != NULL)
    STRING_COPY(BCXTITLE,lpszXDesc);
  else
    bRetVal = FALSE;  
  
  return bRetVal;
}          

void CGraphManager::SetGridColor(COLORREF crGridColor)
{
  BCGRIDCOLOR = crGridColor;
}

BOOL CGraphManager::GetGridFlag()
{
  return (int)( m_dwGraphStyle & GS_GRID );
} 

COLORREF CGraphManager::GetGridColor()
{
  return BCGRIDCOLOR;
}

BOOL CGraphManager::GetColorRectFlag()
{
  return (int)( m_dwGraphStyle & GS_LGREFLECTSHADE );
}             

BOOL CGraphManager::DrawBarOrCurve(CRect& crRefRect,BOOL bBar,CDC *pDC)
{
   CRect crClientRect   = crRefRect;
   CBrush *pOldBrush    =
    pDC->SelectObject(CBrush::FromHandle((HBRUSH)::GetStockObject(LTGRAY_BRUSH)));
   pDC->PatBlt(crClientRect.left,crClientRect.top,crClientRect.Width(),crClientRect.Height(),PATCOPY);
 
   pDC->SelectObject(pOldBrush);
   pDC->SetBkMode(( m_dwGraphStyle & GS_TRANSPARENTTITLE ) ? TRANSPARENT: OPAQUE);
   CRect crTitleRect; 
   DrawGraphTitle(pDC,crTitleRect,&crClientRect);
   
   int nWidth           = crClientRect.Width();
   int nHeight          = crClientRect.Height();             
   int nOneFifthWidth   = nWidth  / 7;
   int nOneFifthHeight  = nHeight / 7; 
   double dBarWidth     = 0; 
   _BC_CORE_DATA *pBCD  = new _BC_CORE_DATA;
   ASSERT( pBCD != NULL);
   
   pBCD->xVertMax       = crClientRect.left + nOneFifthWidth;
   pBCD->yVertMax       = crClientRect.top +  crTitleRect.Height() + 10;//nOneFifthHeight;
   
   pBCD->xOrigin        = pBCD->xVertMax;
   pBCD->yOrigin        = pBCD->yVertMax + 5 * nOneFifthHeight;
   
   pBCD->xHorizMax      = crClientRect.right - nOneFifthWidth;
   pBCD->yHorizMax      = pBCD->yOrigin;
   
   pBCD->xDisplayRange  = pBCD->xHorizMax - pBCD->xVertMax;
   pBCD->yDisplayRange  = pBCD->yHorizMax - pBCD->yVertMax;
   
   SetDisplayRange(pBCD->xDisplayRange,pBCD->yDisplayRange);
   if ( bBar )   
     dBarWidth = CalcBarDisplayWidth();
   
   GetDisplayRange(pBCD->xDisplayRange,pBCD->yDisplayRange);
   
  // pDC->MoveTo(pBCD->xVertMax,pBCD->yVertMax);
  // pDC->LineTo(pBCD->xOrigin,pBCD->yOrigin);
  // pDC->LineTo(pBCD->xHorizMax,pBCD->yHorizMax);
   pDC->SetBkMode(TRANSPARENT);
   
   CFont *pDataFont      = NULL;                
   CFont *pOldFont       = NULL;

   pDC->SetTextColor(GetDataDisplayColor());
   if ( (pDataFont = GetDataDisplayFont()) != NULL) 
     pOldFont = pDC->SelectObject(pDataFont);
     
     DrawGridLines(pDC,pBCD);  
   
   int nMaxDisplayLen = 6;        
   
   if ( m_dwGraphStyle & GS_BCMARKPOINTX )
       DrawXPointMarks(pDC,pBCD);       
   else 
   if ( m_dwGraphStyle & GS_BCMARKPTANDDIVX )
   {   DrawXPointMarks(pDC,pBCD); DrawXDivisionMarks(pDC,pBCD); } 
   else
   {   DrawXDivisionMarks(pDC,pBCD); }
   
   if ( m_dwGraphStyle & GS_BCMARKPOINTY )
       DrawYPointMarks(pDC,pBCD,nMaxDisplayLen);       
   else 
   if ( m_dwGraphStyle & GS_BCMARKPTANDDIVY )
   {
       DrawYPointMarks(pDC,pBCD,nMaxDisplayLen);
       DrawYDivisionMarks(pDC,pBCD,nMaxDisplayLen);
   } 
   else
   {   DrawYDivisionMarks(pDC,pBCD,nMaxDisplayLen); }
     
   DrawAxesInfo(pDC,pBCD,nMaxDisplayLen);  
   
   pDC->SelectObject( pOldFont );
   
   if ( bBar )   
     DrawBars(pDC,pBCD,dBarWidth);
   else
     DrawCurves(pDC,pBCD);
   Draw3DFrame(pDC,CRect(pBCD->xOrigin - 4,pBCD->yVertMax - 4,pBCD->xHorizMax + 4,pBCD->yOrigin + 4),TRUE);
   
   if ( pBCD )
     delete pBCD;
     
   return TRUE;  
}

BOOL CGraphManager::DrawBars(CDC *pDC,_BC_CORE_DATA *pBCD,double dBarWidth)
{
  int nNumberOfBars = GetNumberOfBars();
  CPen *pOldPen = NULL;
  CBrush *pOldBrush = NULL;
  
  for (int nRunCount = 1; nRunCount <= nNumberOfBars;nRunCount ++)
    {
      double dBarHeight  = CalcBarDisplayHeight( nRunCount );
      double dStartPoint = GetBarDisplayStartXPoint( nRunCount );
      COLORREF cr = GetBarColor( nRunCount );
      CBrush brush(cr);       
      pOldBrush = pDC->SelectObject(&brush);
      CRect crBarRect((int)((double)pBCD->xOrigin + dStartPoint - (dBarWidth / 2.0)), (int)((double)pBCD->yOrigin - dBarHeight), 
                    (int)((double)pBCD->xOrigin + dStartPoint + (dBarWidth / 2.0)),pBCD->yOrigin - 1);
      pDC->PatBlt(crBarRect.left + 4,crBarRect.top + 4,crBarRect.Width() - 8,crBarRect.Height() - 8,PATCOPY);                  
      if ( m_dwGraphStyle & GS_BC3DBARS )
        Draw3DFrame(pDC,crBarRect);
      pDC->SelectObject(pOldBrush);
    }
    
  return TRUE;  
}             

BOOL CGraphManager::DrawCurves(CDC *pDC,_BC_CORE_DATA *pBCD)
{  
  int nNoOfCurves = GetNumberOfCurves();
  
  CPen *pOldPen = NULL;
  CBrush *pOldBrush = NULL;
  BOOL   bBoldPoints = m_dwGraphStyle & GS_CURVEBOLDPOINTS;
                                      
  for ( int nCurveNum = 1;nCurveNum <= nNoOfCurves;nCurveNum ++)
  {
    COLORREF cr = GetPlotColor( nCurveNum );
    CPen pen(PS_SOLID,0,cr);
    CBrush brush(cr);       
    pOldPen   = pDC->SelectObject(&pen);      
    pOldBrush = pDC->SelectObject(&brush);              
    int nNoOfPoints = GetPointsInCurve( nCurveNum );

    int nLineStartX,nLineStartY;
    nLineStartX = nLineStartY = 0;
    
    for ( int nRunCount = 1;nRunCount <= nNoOfPoints ;nRunCount ++)
    {
     double dXPoint = GetCurveDisplayStartXPoint( nCurveNum,nRunCount );
     double dYPoint = GetCurveDisplayStartYPoint( nCurveNum,nRunCount );

	 if ( bBoldPoints )	 
        pDC->Ellipse((int)(pBCD->xOrigin + dXPoint - 3.0),(int)(pBCD->yOrigin - dYPoint - 3.0),
         (int)(pBCD->xOrigin + dXPoint + 3.0),(int)(pBCD->yOrigin - dYPoint + 3.0));
        
     if ( nLineStartX || nLineStartY )
     {
	   DrawLinkLine(pDC,
		      CPoint(nLineStartX,nLineStartY),
			  CPoint((int)(pBCD->xOrigin + dXPoint),
	         (int)((double)pBCD->yOrigin - dYPoint)));
     }
     
      nLineStartX =  (int)(pBCD->xOrigin + dXPoint);
      nLineStartY =  (int)(pBCD->yOrigin - dYPoint);
    }  
    
    pDC->SelectObject(pOldBrush);              
    pDC->SelectObject(pOldPen);
  } 

/*  if ( IsRealTimeGraph() )
  { 
	 m_pCSH->Lock( RTGCRITSECTION );
	 if ( m_pRTGData )
	    memcpy(m_pRTGData->pBCD,pBCD,sizeof(_BC_CORE_DATA));
	 m_pCSH->Unlock( RTGCRITSECTION );
  }
 */
 return TRUE; 
} 


void CGraphManager::DrawLinkLine(CDC *pDC,CPoint ptStart,CPoint ptEnd)
{
   ASSERT(pDC != NULL); 	 
   pDC->MoveTo(ptStart.x,ptStart.y);
   pDC->LineTo(ptEnd.x,ptEnd.y);
}


BOOL CGraphManager::DrawGridLines(CDC *pDC,_BC_CORE_DATA *pBCD)
{  
   if (!GetGridFlag())
    return FALSE;
         
   double dDiv,dInterval; 
   dDiv =  dInterval = 0.0;
   
   if ((dInterval = GetHorizRangeDisplayDivisionUnit()) == 0.0)
    return FALSE;
   
   CPen pen(PS_DOT,1,GetGridColor());
   CPen *pOldPen = pDC->SelectObject(&pen);
   
   int nIndex = 0;
   int nGridPos = 0;
   
   
   for (  nGridPos = pBCD->xOrigin; nGridPos <= pBCD->xHorizMax;
           dDiv = (( nIndex++ ) * dInterval),
           dDiv += ((dDiv - (int)dDiv) > 0.6) ? 1:0,
           nGridPos = pBCD->xOrigin + (int)dDiv)
     {
         if (( nGridPos != pBCD->xOrigin ) && (nGridPos != pBCD->xHorizMax))
         {
          pDC->MoveTo(nGridPos,pBCD->yOrigin);
          pDC->LineTo(nGridPos,pBCD->yVertMax);
         } 
     }   
     
   nIndex = 0;
   
   if ((dInterval =  GetVertRangeDisplayDivisionUnit()) == 0)
        return FALSE;                      
   
   for (nGridPos = pBCD->yOrigin;nGridPos >= pBCD->yVertMax;
       dDiv = (( nIndex++ ) * dInterval),
       dDiv += ((dDiv - (int)dDiv) > 0.6) ? 1.0 : 0.0,
       nGridPos = pBCD->yOrigin - (int)dDiv)
      {  
        if (( nGridPos != pBCD->yOrigin ) && ( nGridPos != pBCD->yVertMax))
        {
          pDC->MoveTo(pBCD->xOrigin,nGridPos);
          pDC->LineTo(pBCD->xHorizMax,nGridPos);
        } 
        
      } 
      
   pDC->SelectObject(pOldPen);    
   
   return TRUE;
}  

BOOL CGraphManager::DrawXPointMarks(CDC *pDC,_BC_CORE_DATA *pBCD)
{  
 int nNumOfItems    = 0;   
 int nDisplayStrLen = 0;
 TEXTMETRIC tm;
 pDC->GetTextMetrics(&tm);
 
 if ( IsBar() )
 {                                 
    nNumOfItems = GetNumberOfBars();
    
    for ( int nIndex = 1;nIndex <= nNumOfItems;nIndex ++)
    {
      int nXPos = (int)GetBarDisplayStartXPoint(nIndex);
      pDC->MoveTo(nXPos,pBCD->yOrigin);
      pDC->LineTo(nXPos,pBCD->yOrigin + 10);                        
      LPSTR lpszText = GetBarHorizPointText(nIndex);
      if ( lpszText )
      {
        nDisplayStrLen = strlen(lpszText) * tm.tmAveCharWidth + 2;
        pDC->TextOut(nXPos - nDisplayStrLen / 2,pBCD->yOrigin + 11 ,lpszText);
      } 
    } 
 }         
 else
 {
    nNumOfItems = GetNumberOfCurves();
    for ( int nCurveNum = 1;nCurveNum <= nNumOfItems ; nCurveNum ++)
    {
      int nNoOfPoints = GetPointsInCurve( nCurveNum );
      for ( int nPoint = 1; nPoint <= nNoOfPoints; nPoint ++)
      {
        int nXPoint = (int)GetCurveDisplayStartXPoint( nCurveNum,nPoint ); 
        pDC->MoveTo(nXPoint,pBCD->yOrigin);
        pDC->LineTo(nXPoint,pBCD->yOrigin + 10);
        LPSTR lpszText = GetCurveHorizPointText(nCurveNum,nPoint);
        if ( lpszText )
        { 
          nDisplayStrLen = strlen(lpszText) * tm.tmAveCharWidth + 2;
          pDC->TextOut(nXPoint - nDisplayStrLen / 2,pBCD->yOrigin + 11 ,lpszText);
        }
      }  
    } 
 }
 return TRUE;
}

void CGraphManager::FreeGraphPtTextListNodes(_GRAPH_PTEXT ** pGP)
{
  int nNumCurves = 1;
  if ( m_dwGraphStyle & GS_CURVE )
   nNumCurves = GetNumberOfGraphItems();
  if ( !pGP ) return;                                 
  for ( int i = 0; i < nNumCurves; i++)
     FreeGraphPtTextList( pGP[i] );
  delete pGP;
}

void CGraphManager::FreeGraphPtTextList( _GRAPH_PTEXT *pGPStart)
{  
  _GRAPH_PTEXT *p = pGPStart;
  _GRAPH_PTEXT *q = p;  
  if ( !p ) return;
  do { q = p->pNext;   FreeGraphPtText( p ); } while (p = q);
}                                   

void CGraphManager::FreeGraphPtText(_GRAPH_PTEXT * pGPtText)
{           
  if ( pGPtText )
  {
    FreeString( pGPtText->lpszText );
    delete pGPtText;
  }  
}          

_GRAPH_PTEXT * CGraphManager::AllocNewPointTextNode()
{      
   GraphSetNewHandler();
   _GRAPH_PTEXT *p = new _GRAPH_PTEXT;
   GraphClearNewHandler();
   return p;
}                             

BOOL CGraphManager::AddNewBarPtText(LPSTR lpszXText,LPSTR lpszYText)
{
  return AddNewPointText(lpszXText,lpszYText,0);
}                                               

BOOL CGraphManager::AddNewCurvePtText(LPSTR lpszXText,LPSTR lpszYText,int nCurveIndex)
{
  return AddNewPointText(lpszXText,lpszYText,nCurveIndex - 1);
}                                               


BOOL CGraphManager::AddNewPointText(LPSTR lpszXText,LPSTR lpszYText, int nItem)
{  
   _GRAPH_PTEXT * p  = NULL;  
   _GRAPH_PTEXT * q  = NULL;
         
   if ( m_dwGraphStyle & GS_BCMARKPOINTX ||
         m_dwGraphStyle & GS_BCMARKPTANDDIVX )
   {      
    _GRAPH_PTEXT * pX = BCXPTEXT[nItem];      
    
    if ((p = AllocNewPointTextNode()) != NULL) 
    {
      if ( p->lpszText = lpszXText )
       if ( p->lpszText = AllocString( lstrlen(lpszXText) + 1 ))
           STRING_COPY(p->lpszText,lpszXText);
      p->pNext = NULL;
    } 
     
    if ( !pX ) {  BCXPTEXT[nItem] = p; }
    else
     { for (;pX && (pX->pNext != NULL);pX = pX->pNext); pX->pNext = p; } 
    
   }     
   
   if ( m_dwGraphStyle & GS_BCMARKPOINTY ||
         m_dwGraphStyle & GS_BCMARKPTANDDIVY )
   {      
    _GRAPH_PTEXT * pY = BCYPTEXT[nItem];
    
    if ((q = AllocNewPointTextNode()) != NULL) 
    {
     if ( q->lpszText = lpszYText )
      if ( q->lpszText = AllocString( lstrlen(lpszYText) + 1 ))
          STRING_COPY(q->lpszText,lpszYText);
      q->pNext = NULL;
    }
        
    if ( !pY ) {  BCYPTEXT[nItem] = q; }                
    else
     { for (;pY && (pY->pNext != NULL);pY = pY->pNext); pY->pNext = q; }
   } 
    
   return p ? q ? TRUE : FALSE : FALSE;
}                                    

BOOL CGraphManager::DrawYPointMarks(CDC *pDC,_BC_CORE_DATA *pBCD,int& nMaxDisplayStrLen)
{                 
 int nNumOfItems    = 0;   
 int nDisplayStrLen = 0;
 TEXTMETRIC tm;
 pDC->GetTextMetrics(&tm);
 
 int nTextHeight    = tm.tmHeight + tm.tmExternalLeading;
 
 if ( IsBar() )
 {                                 
    nNumOfItems = GetNumberOfBars();
    for ( int nIndex = 1;nIndex <= nNumOfItems;nIndex ++)
    {
      int nYPos = (int)CalcBarDisplayHeight( nIndex );
      pDC->MoveTo(pBCD->xOrigin,nYPos);
      pDC->LineTo(pBCD->xOrigin - 10,nYPos);                        
      LPSTR lpszText = GetBarVertPointText(nIndex);
      if ( lpszText )
      {
        nDisplayStrLen = strlen( lpszText ) * tm.tmAveCharWidth + 2;
        nMaxDisplayStrLen  = max(nMaxDisplayStrLen,nDisplayStrLen);
        pDC->TextOut(pBCD->xOrigin - 10 - nDisplayStrLen ,
                     nYPos - nTextHeight / 2,lpszText);
      } 
    } 
 }         
 else
 {
    nNumOfItems = GetNumberOfCurves();
    for ( int nCurveNum = 1;nCurveNum <= nNumOfItems ; nCurveNum ++)
    {
      int nNoOfPoints  = GetPointsInCurve( nCurveNum );
      for ( int nPoint = 1; nPoint <= nNoOfPoints; nPoint ++)
      {
        int  nYPoint = (int)GetCurveDisplayStartYPoint( nCurveNum,nPoint ); 
        pDC->MoveTo(pBCD->xOrigin,nYPoint);
        pDC->MoveTo(pBCD->xOrigin - 10,nYPoint);
        LPSTR lpszText = GetCurveVertPointText(nCurveNum,nPoint);
        if ( lpszText )
        { 
           nDisplayStrLen = strlen( lpszText ) * tm.tmAveCharWidth + 2;
           nMaxDisplayStrLen  = max(nMaxDisplayStrLen,nDisplayStrLen);
           pDC->TextOut(pBCD->xOrigin - 10 - nDisplayStrLen ,
                     nYPoint - nTextHeight / 2,lpszText);
        }
      }  
    } 
 }
  
  return TRUE;
}                                    


BOOL CGraphManager::DrawXDivisionMarks(CDC *pDC,_BC_CORE_DATA *pBCD)
{                                                                
   double dDivisionRange = GetHorizRangeDisplayDivisionUnit();
   if ( !dDivisionRange ) return FALSE;
   
   // Horizontal markers
   char szText[ _MAX_EXT + 1 ]       = {0}; 
   int nDisplayStrLen    = 0;  
   int nIndex            = 0;
   double dDiv           = 0;                    
   double dNoOfDivisions = (double)pBCD->xDisplayRange / (double)dDivisionRange;
   int nNoOfDivisions    = (int)dNoOfDivisions + 1;
   
   if ((dNoOfDivisions - (int)dNoOfDivisions) != (double)0.0)
     nNoOfDivisions++;
     
    int nTotalDisplayStrLen = 0;
   
   TEXTMETRIC tm;
   int nOff          = 0;
   int nMinTextWidth = 32767;
   
   pDC->GetTextMetrics(&tm);
   do
   {
     nOff ++;
     nTotalDisplayStrLen = 0;
     nMinTextWidth = 32767;
     for (int n = 1 ;n <= nNoOfDivisions;n += nOff)
     {                        
       FormatDisplayValue(szText,GetHorizDivisionValue(n - 1));
       int nTextWidth =  strlen(szText) * tm.tmAveCharWidth;
       nMinTextWidth = min(nMinTextWidth,nTextWidth);
       nTotalDisplayStrLen += nTextWidth;
     }                                     
   }
   while ( nTotalDisplayStrLen > max(pBCD->xDisplayRange,nMinTextWidth) );     
   
   dDiv = 0.0; 
   nIndex = 0;
   
   for ( int nPosX = pBCD->xOrigin;nPosX <= pBCD->xHorizMax;
         dDiv = ((nIndex += nOff) * dDivisionRange),
         dDiv += ((dDiv - (int)dDiv) > 0.6) ? 1:0,
         nPosX = pBCD->xOrigin + (int)dDiv)
   {                                      
     pDC->MoveTo(nPosX,pBCD->yOrigin);
     pDC->LineTo(nPosX,pBCD->yOrigin + 10 );
     FormatDisplayValue(szText, GetHorizDivisionValue(nIndex));
     nDisplayStrLen = strlen(szText) * tm.tmAveCharWidth + 2;
     pDC->TextOut(nPosX - nDisplayStrLen / 2,pBCD->yOrigin + 11 ,szText);
   }                   
   
  return TRUE;
} 

BOOL CGraphManager::DrawYDivisionMarks(CDC *pDC,_BC_CORE_DATA *pBCD,int& nMaxDisplayStrLen )  
{ 
  double dDivisionRange    = 0.0;
  double dDiv              = 0.0;
  int nIndex                = 0;            
  int nOff                  = 0;      
  char szText[_MAX_EXT + 1] = { 0 };
  int nTotalDisplayStrLen   = 0;
  if ((dDivisionRange   =  GetVertRangeDisplayDivisionUnit()) == 0.0)
        return FALSE;                      
  TEXTMETRIC tm;
  pDC->GetTextMetrics(&tm);
  int nTextHeight       = tm.tmHeight + tm.tmExternalLeading;      
  double dNoOfDivisions = (double)pBCD->yDisplayRange / (double)dDivisionRange;
  int    nNoOfDivisions = (int)dNoOfDivisions + 1;
  if ((dNoOfDivisions - (int)dNoOfDivisions) != (double)0.0)
    nNoOfDivisions ++;
     
  do
  {
    nOff ++;                                            
    nTotalDisplayStrLen = 0;
    for ( int n = 1;n <= nNoOfDivisions; n += nOff )
       nTotalDisplayStrLen +=  nTextHeight;
  }
  while ( nTotalDisplayStrLen > max(pBCD->yDisplayRange,nTextHeight) );     
   
   // Y-Axis Drawing
  for (int nPosY = pBCD->yOrigin;nPosY >= pBCD->yVertMax;
       dDiv = ((nIndex += nOff) * dDivisionRange),
       dDiv += ((dDiv - (int)dDiv) > 0.6) ? 1.0 : 0.0,
       nPosY = pBCD->yOrigin - (int)dDiv)
   {              
    
    pDC->MoveTo(pBCD->xOrigin,nPosY);
    pDC->LineTo(pBCD->xOrigin - 8,nPosY);
    FormatDisplayValue(szText,GetVertDivisionValue(nIndex));
    int nDisplayStrLen = strlen(szText) * tm.tmAveCharWidth + 2;
    nMaxDisplayStrLen  = max(nMaxDisplayStrLen,nDisplayStrLen);
    pDC->TextOut(pBCD->xOrigin - 10 - nDisplayStrLen ,nPosY - nTextHeight / 2,
                 szText);
  }                  
  
  return TRUE;
}


BOOL CGraphManager::DrawAxesInfo(CDC *pDC,_BC_CORE_DATA *pBCD,int nMaxDisplayStrLen)
{
  // Arrow Marks
  CBrush *pOldBr = 
  pDC->SelectObject(CBrush::FromHandle((HBRUSH)::GetStockObject(DKGRAY_BRUSH)));
  CPoint ptArray[3] = {  CPoint(pBCD->xOrigin - 8,pBCD->yVertMax - 4),
                         CPoint(pBCD->xOrigin ,pBCD->yVertMax - 4),
                         CPoint(pBCD->xOrigin - 4,pBCD->yVertMax - 12) };
  pDC->Polygon(ptArray,3);
  ptArray[0] = CPoint(pBCD->xHorizMax + 4 ,pBCD->yOrigin );
  ptArray[1] = CPoint(pBCD->xHorizMax + 4 ,pBCD->yOrigin + 8);
  ptArray[2] = CPoint(pBCD->xHorizMax + 12,pBCD->yOrigin + 4);
  pDC->Polygon(ptArray,3);
  pDC->SelectObject(pOldBr);
  
  CFont *pOldFont = NULL;
  if ((pOldFont = GetTitleFont()) != NULL)
   pOldFont = pDC->SelectObject(pOldFont);
   
  TEXTMETRIC tm; 
  pDC->GetTextMetrics(&tm); 
  
  int nTextHeight = tm.tmHeight;
  
  // Y title
  LPSTR lpszText = GetYAxisDescText();
  if ( lpszText )
  {
   int nReqYTextHeight = nTextHeight * lstrlen(lpszText) + 1;
   LPSTR p = lpszText;                                 
   for (int nStartY = pBCD->yVertMax + (pBCD->yDisplayRange - nReqYTextHeight) / 2;
              *p ; *++p)
   {         
     char szCharText[2] = { *p , 0};                                        
     int nCharWidth = tm.tmMaxCharWidth;
     pDC->GetCharWidth(*p,*p,&nCharWidth);
     int nXPoint = pBCD->xOrigin - nMaxDisplayStrLen - 12 - 
                      tm.tmMaxCharWidth  + (tm.tmMaxCharWidth - nCharWidth) / 2;
     if ( nXPoint <= 0)
      nXPoint = pBCD->xHorizMax + 12 + tm.tmMaxCharWidth  + (tm.tmMaxCharWidth - nCharWidth) / 2;
      
     pDC->TextOut(nXPoint, nStartY,szCharText);
     nStartY += nTextHeight;
   }
  } 
  
  // X title
  lpszText = GetXAxisDescText();
  if ( lpszText )
  {
    int nDisplayStrLen = lstrlen(lpszText) * tm.tmAveCharWidth  + 2;
    pDC->TextOut(pBCD->xOrigin + (pBCD->xDisplayRange - nDisplayStrLen) / 2,
                 pBCD->yOrigin + 2 * nTextHeight ,lpszText);
  }
  
  if ( pOldFont )
    pDC->SelectObject(pOldFont);               
    
  return TRUE;  
}            


BOOL CGraphManager::FormatDisplayValue(LPSTR szFormatBuffer , double dValue)
{
 if ( m_dwGraphStyle  & GS_SIGNED )
 { 
   if ( m_dwGraphStyle & GS_BCDISPLAYINT )
     wsprintf(szFormatBuffer,"%d",(int)dValue);
   else            
   if ( m_dwGraphStyle & GS_BCDISPLAYLONG )
     wsprintf(szFormatBuffer,"%ld",(long)dValue); 
   else  // default
     sprintf(szFormatBuffer,"%+6.2f",dValue);         
 }
 else
 {                                   // default
   if ( m_dwGraphStyle & GS_BCDISPLAYINT )
     wsprintf(szFormatBuffer,"%u",(int)dValue);
   else            
   if ( m_dwGraphStyle & GS_BCDISPLAYLONG )
     wsprintf(szFormatBuffer,"%lu",(long)dValue); 
   else
     sprintf(szFormatBuffer,"%6.2f",dValue);
 }  
 return TRUE;
}


BOOL CGraphManager::DrawGraph(CWnd *pWnd,CDC *pDC,CRect *pBoundsRect)
{           
 if (pWnd == NULL)
  return FALSE;
 BOOL bRet   = TRUE; 
 BOOL bWinDC = (pDC == NULL); 
 if ( bWinDC )
     pDC = pWnd->GetDC();
 CRect crClientRect;
 if ( pBoundsRect == NULL)
   pWnd->GetClientRect(crClientRect);
 else
   crClientRect   = *pBoundsRect;    
   
 int nWidth       = crClientRect.Width();
 int nHeight      = crClientRect.Height();  
 CDC dcDupMem;
 dcDupMem.CreateCompatibleDC(pDC);
 CBitmap *pOldBmp = dcDupMem.SelectObject( m_pMemBitmap );
 
 // This portion is to speed up display
 // we save an old image of the screen
 // and display it as fast as we can
 // thus preventing a flicker
 // if there is no change in the rect
 
 if ( m_crOldClientRect == crClientRect)
 {
   pDC->BitBlt(crClientRect.left,crClientRect.top,nWidth,
               nHeight,&dcDupMem,0,0,SRCCOPY);  
   dcDupMem.SelectObject( pOldBmp );            
   if ( bWinDC )
    pWnd->ReleaseDC(pDC);
   return bRet;
 }   
 
 CDC dcMem;
 dcMem.CreateCompatibleDC(pDC);
 CBitmap bmp;
 bmp.CreateCompatibleBitmap(pDC,nWidth,nHeight);
 
 CBitmap *pOldBitmap = dcMem.SelectObject(&bmp);
 dcMem.PatBlt(0,0,nWidth,nHeight,WHITENESS);
 
 SetUpFonts(pDC);
 // Draw Pie 
 if (IsPie())
   bRet = DrawPieChart(crClientRect,&dcMem);
 
 // Draw Bar or Curve
 if ( IsBar() || IsCurve() )
   bRet = DrawBarOrCurve(crClientRect,IsBar(),&dcMem);   
 pDC->BitBlt(crClientRect.left,crClientRect.top,nWidth,
              nHeight,&dcMem,0,0,SRCCOPY);  
 dcDupMem.BitBlt(crClientRect.left,crClientRect.top,nWidth,
              nHeight,&dcMem,0,0,SRCCOPY);               
 m_crOldClientRect = crClientRect;             
 dcMem.SelectObject(pOldBitmap);    
 dcDupMem.SelectObject( pOldBmp );            
 if ( bWinDC )
  pWnd->ReleaseDC(pDC);
 return bRet;
}

BOOL CGraphManager::SetUpFonts(CDC *pDC)
{            
 BOOL bRetVal = FALSE;
 LOGFONT *pLf = NULL;
 
 if ( GRAPHTITLEINFO )
 {    
   if ( !m_pTitleFont )
   {
       pLf = &GRAPHTITLEFONT;
       // if height is given in point size
       // then we need to convert it
       if ( pLf->lfHeight < 0)      
          pLf->lfHeight = -MulDiv(-pLf->lfHeight,
                    pDC->GetDeviceCaps(LOGPIXELSY),72);
       if ( m_pTitleFont = AllocFont())
          m_pTitleFont->CreateFontIndirect(pLf);

       bRetVal = TRUE;
   }
 }
 
 if ( GRAPHDATADISPLAYINFO )
 {    
   if ( !m_pDataFont )
   {
       pLf = &GRAPHDATAFONT;
       // if height is given in point size
       // then we need to convert it
       if ( pLf->lfHeight < 0)      
         pLf->lfHeight = -MulDiv(-pLf->lfHeight,
                    pDC->GetDeviceCaps(LOGPIXELSY),72);
       if (m_pDataFont = AllocFont())
         m_pDataFont->CreateFontIndirect(pLf);
       
       bRetVal = TRUE;  
   }
 }               
 return bRetVal;
} 



BOOL CGraphManager::DrawPieChart(CRect & crRefRect,CDC *pDC)
{       
   int nNumOfSectors    = 0;
   if (!(nNumOfSectors  = GetNumberOfGraphItems()))
       return FALSE;                                   
   
   CRect crRect = crRefRect; 
   
   int nWidth  =  crRect.Width() ;
   int nHeight =  crRect.Height();
   
   if ( !nWidth || !nHeight )
    return FALSE;   
       
   int nRunCount = 1;  
   _PIE_CORE_DATA *pPie = new _PIE_CORE_DATA;
    ASSERT (pPie != NULL);
    
   CRect crTitleRect;
   
   DrawGraphTitle(pDC,crTitleRect,&crRect);
   
   int nLeft   = crRect.left + nWidth / 4;
   int nRight  = nLeft + nWidth / 2;
   int nTop    = crRect.top + abs(2 * nHeight - nWidth) / 4 + (crTitleRect.Height() + 5);//nHeight / 8;
   int nBottom = 0;
   (nBottom =  (m_dwGraphStyle & GS_PIEREGULAR) ? nWidth / 2 : nHeight / 2 )
     += nTop;
    
   CRect crClientRect(nLeft,nTop,nRight,nBottom);
   
   crClientRect.top    += 5;
   crClientRect.bottom += 5;
   
   CPen *pOldPen     = NULL;
   CBrush *pOldBrush = NULL;
   
   CPen pen(PS_SOLID,0,RGB(0,0,0));
   CBrush brush(RGB(192,192,192));
   pOldPen   = pDC->SelectObject(&pen);
   pOldBrush = pDC->SelectObject(&brush);
   pDC->Ellipse(crClientRect);
   pDC->SelectObject(pOldPen);           
   pDC->SelectObject(pOldBrush);
   
   crClientRect.top    -= 5;
   crClientRect.bottom -= 5;
   
  // crClientRect.InflateRect( -100,-100);
   
   //////////////////////////////////////////////////////////////
   //
   //  Graphical representation of an elliptical pie
   //  (x1,y1)+---(x,y)-  -  -
   //         |     - .   |     -
   //         |  -     .  |        -
   //         |-        . |b         - 
   //         -        @ .|           -
   //         ------------+------------
   //         -     a     |           -
   //          -          |          -
   //            -        |        - 
   //               -     |     -
   //                  -  -  -
   //                   
   //  @  = Theetah, a = Major radius, b = Minor Radius
   //  P  = Percentage , @ = (P * pi * 0.02) radians
   //  pi = 3.142
   // 
   //  x = x1 +  a(1 - b Cos@ / (b^2Cos^2 @ + a^2 Sin^2 @)^ 0.5) 
   //  denotes the horizontal co-ordinate of the ending point of 
   //  a pie-sector to be drawn based on the percentage P
   // 
   //  y = y1 +  b(1 - a Sin@ / (b^2Cos^2 @ + a^2 Sin^2 @)^ 0.5)
   //  denotes the vertical co-ordinate of the ending point of 
   //  a pie-sector to be drawn based on the percentage  P
   //
   //  Notations to follow(Variable names) are based on the above 
   //  Geometrical figure
   //
   ///////////////////////////////////////////////////////////////
   
   pPie->a = (double) crClientRect.Width() / 2.0;
   pPie->b = (double) crClientRect.Height() / 2.0;           
   
   pPie->iStartX = crClientRect.left;      
   pPie->iStartY = crClientRect.top + (int)pPie->b;
   
   pPie->x1 = (double)pPie->iStartX;
   pPie->y1 = (double)crClientRect.top ;
   
   int iLineStartX = pPie->iStartX;
   int iLineStartY = pPie->iStartY;
   
   double dTotalPercent = 0.0;

   for ( int i = 0; i < nNumOfSectors; i++ )
      m_pSavedRects[i]->SetRect( 0,0,0,0);
       
   for (;nRunCount <= nNumOfSectors ; nRunCount ++ )
   { 
     double percent = GetPieSectorPercentage(nRunCount);
     dTotalPercent += percent;
     pPie->theetah = 0.06283 * dTotalPercent;
     pPie->sintheetah = sin(pPie->theetah);   
     pPie->costheetah = cos(pPie->theetah);
     
     if ( dTotalPercent <= 12.5)
     {
       pPie->x = pPie->x1;
       pPie->y = pPie->y1 + pPie->b + ( pPie->a * pPie->sintheetah /pPie->costheetah);
     } 
     
     if ( dTotalPercent > 12.5 && dTotalPercent <= 37.5)
     {
       pPie->x = pPie->x1 + pPie->a - (pPie->b * pPie->costheetah / pPie->sintheetah);
       pPie->y = pPie->y1 + 2.0 * pPie->b;
     }                                    
     
     if ( dTotalPercent > 37.5 &&  dTotalPercent <= 62.5)
     {
       pPie->x = pPie->x1 + 2.0 * pPie->a;
       
       pPie->theetah = 3.1416 - pPie->theetah;
       pPie->sintheetah = sin(pPie->theetah);
       pPie->costheetah = cos(pPie->theetah);
       
       pPie->y = pPie->y1 + pPie->b + (pPie->a * pPie->sintheetah /pPie->costheetah);
     } 
     
     if (dTotalPercent > 62.5 &&  dTotalPercent <= 87.5)
     { 
       pPie->theetah -= 4.7123;
       pPie->sintheetah = sin(pPie->theetah);   
       pPie->costheetah = cos(pPie->theetah);
       pPie->x  = pPie->x1 + pPie->a - (pPie->b * pPie->sintheetah /pPie->costheetah);
       pPie->y  = pPie->y1;
     }                     
     
     if ( dTotalPercent > 87.5)
     {
       pPie->x = pPie->x1;
       
       pPie->theetah = 6.2832 - pPie->theetah;
       pPie->sintheetah = sin(pPie->theetah);   
       pPie->costheetah = cos(pPie->theetah);
       pPie->y = pPie->y1 + pPie->b - (pPie->a * pPie->sintheetah /pPie->costheetah);
     }
     
     COLORREF cr = GetGraphItemColor(nRunCount);
     
     CPen cpPen(PS_SOLID,0,RGB(0,0,0));
     CBrush cbBrush(cr);
     
     pOldPen = pDC->SelectObject(&cpPen);
     pOldBrush = pDC->SelectObject(&cbBrush);


     if ( pPie->x > 0 && pPie->y > 0 && pPie->iStartX > 0  &&   pPie->iStartY > 0 )
         pDC->Pie(crClientRect, CPoint(pPie->iStartX,pPie->iStartY),
              CPoint((int)pPie->x,(int)pPie->y));
             
     double dOldX = pPie->x;
     double dOldY = pPie->y;        
    
     pPie->theetah = 6.2831 - 0.06283 * dTotalPercent;
     pPie->sintheetah = sin(pPie->theetah);   
     pPie->costheetah = cos(pPie->theetah);

     pPie->denom  =  sqrt((pPie->b * pPie->b * pPie->costheetah * pPie->costheetah) +
                          (pPie->a * pPie->a * pPie->sintheetah * pPie->sintheetah));
     pPie->x = pPie->x1 + pPie->a * ( 1.0 - (( pPie->b * pPie->costheetah)  / pPie->denom ));
     pPie->y = pPie->y1 + pPie->b * ( 1.0 - (( pPie->a * pPie->sintheetah ) / pPie->denom ));                
             
     int iPLineX = (int)((double)iLineStartX + (pPie->x - iLineStartX) / 2);
     int iPLineY = (int)((double)iLineStartY + (pPie->y - iLineStartY) / 2);
     
     char szPercent[_MAX_EXT + 1] = {0};
   
     pDC->SetBkMode(TRANSPARENT);
     sprintf(szPercent,"%lg",percent);
     lstrcat(szPercent,"%");
     COLORREF crPercentColor = GetPiePercentDisplayFgColor();
     pDC->SetTextColor(crPercentColor);
     CFont *pf = GetPiePercentDisplayFont();
     CFont *pOldFont = NULL;
  
     if ( pf )
     {
         pOldFont = pDC->SelectObject(pf);
     }  
     
     TEXTMETRIC tm;
     int anXVal = 0;
     pDC->GetTextMetrics(&tm);
     
     int nPercentLength = tm.tmAveCharWidth * 10;
     int nPercentHeight = tm.tmHeight + tm.tmExternalLeading;
     
     int xVal = 0;
     
     if ( iPLineX < (crClientRect.left + pPie->a))
     {                     
      xVal =  crClientRect.left - 5;
      anXVal = xVal;
      xVal -= nPercentLength;
     }                                                     
     else
     {
        xVal = (int)(crClientRect.left + 2 * pPie->a + 5);//min(nWidth / 4 - nPercentLength,nWidth / 8);
        anXVal = xVal;
        xVal += 2;
     }   

     BOOL bCollides = FALSE;

     if ( m_pSavedRects )
     {
       CRect rect(xVal,iPLineY - (nPercentHeight / 2),
                      xVal + nPercentLength,
                     iPLineY + (nPercentHeight / 2));
       for ( int i = 0; i < nRunCount; i++) {
          if ( bCollides = CheckCollision( *m_pSavedRects[i], rect))
             break;
          if ( bCollides = CheckCollision( rect, *m_pSavedRects[i]))
             break;
         }
     }
     

     if ( !bCollides ) {
       pDC->MoveTo(iPLineX,iPLineY);
       pDC->LineTo(anXVal,iPLineY);
       pDC->TextOut(xVal,iPLineY - (nPercentHeight / 2),szPercent);
       if ( m_pSavedRects ) {
         if ( m_pSavedRects[nRunCount - 1] )
            m_pSavedRects[nRunCount - 1]->SetRect(xVal, iPLineY - (nPercentHeight / 2),
                                              xVal + nPercentLength,
                                              iPLineY + (nPercentHeight / 2)); 
       }
     }
     
     pDC->SelectObject(pOldFont);
     
     iLineStartX = (int)pPie->x;
     iLineStartY = (int)pPie->y;
     
     pPie->x = dOldX;
     pPie->y = dOldY;
     
     pPie->iStartX =  (int)pPie->x;
     pPie->iStartY =  (int)pPie->y;                   
     
     pDC->SelectObject(pOldBrush);
     pDC->SelectObject(pOldPen);
     
   }    
   
   
   delete pPie;
   return TRUE; 
}

BOOL CGraphManager::CheckCollision( CRect& r1, CRect& r2)
{
  BOOL bRet = FALSE;
   // check left, top
   bRet = r1.PtInRect(CPoint(r2.left, r2.top));
   if ( !bRet ) {
       // right,top
      bRet = r1.PtInRect(CPoint(r2.right,r2.top));
      if ( !bRet ) {
        bRet = r1.PtInRect(CPoint(r2.right,r2.bottom));
        if ( !bRet )
         bRet = r1.PtInRect(CPoint(r2.left,r2.bottom));
      }
   }

   return bRet;
}

void CGraphManager::ResetLegendWindowObject()
{
  m_pLegendWnd = NULL;
}


BOOL CGraphManager::DrawGraphTitle(CDC *pDC,CRect &cr,const CRect *pClientRect)
{                       
  int nWidth = pClientRect->Width();
  CRect crt(pClientRect->left + 2, pClientRect->top + 5,pClientRect->left + 10,pClientRect->top + 6);
  cr = crt;
  LPSTR lpTitle = GetGraphTitle();
  if ( !lpTitle )
   return TRUE;
  pDC->SetBkColor(GetTitleBkColor());
  pDC->SetTextColor(GetTitleFgColor());
  CFont *pf = GetTitleFont();
  CFont *pOldFont = NULL;
  if ( pf )
   pOldFont = pDC->SelectObject(pf);
  pDC->DrawText(lpTitle,-1,cr,DT_CALCRECT);
  int nTitleWidth = cr.Width();
  cr.left = pClientRect->left + (nWidth - nTitleWidth) / 2;
  cr.right = cr.left + nTitleWidth;
  UINT nTitleAlignment = 
  ( m_dwGraphStyle & GS_GRAPHTITLERIGHT ) ? DT_RIGHT :
  ( m_dwGraphStyle & GS_GRAPHTITLELEFT   ) ? DT_LEFT  : DT_CENTER;
  pDC->DrawText(lpTitle, -1,cr,
         DT_SINGLELINE|DT_VCENTER|nTitleAlignment);
  if ( pf )
   pDC->SelectObject(pOldFont);
  return TRUE;
}

void CGraphManager::Draw3DFrame(CDC *pDC,LPRECT pRect,BOOL bEtchedIn)
{                         
    int nL = pRect->left;
    int nT = pRect->top;
    int nR = pRect->right;
    int nB = pRect->bottom;
    
    CBrush brGray(GetSysColor(COLOR_BTNSHADOW));
    CBrush *pOldBr = NULL;
    
    if ( bEtchedIn )
    { 
     pOldBr = pDC->SelectObject(&brGray);
     pDC->PatBlt(nL + 1,nT + 1,1,nB - nT - 2,PATCOPY);
     pDC->PatBlt(nL + 1,nT + 1,nR - nL - 2,1,PATCOPY);
     pDC->PatBlt(nL + 2,nT + 2,nR - nL - 4,1,PATCOPY);
     pDC->PatBlt(nL + 2,nT + 2,1,nB - nT - 4,PATCOPY);
     pDC->SelectObject(pOldBr);
     pDC->PatBlt(nL + 2,nB - 2,nR - nL - 4,1,WHITENESS);
     pDC->PatBlt(nR - 2,nT + 2,1,nB - nT - 4,WHITENESS);
     pDC->PatBlt(nR - 1,nT + 1,1,nB - nT - 2,WHITENESS);
     pDC->PatBlt(nL + 1,nB - 1,nR - nL - 2,1,WHITENESS); 
     
     pDC->PatBlt(nL + 3,nT + 3,nR - nL - 6,1,BLACKNESS);
     pDC->PatBlt(nL + 3,nT + 3,1,nB - nT - 6,BLACKNESS);
     pDC->PatBlt(nL + 3,nB - 3,nR - nL - 6,1,BLACKNESS);
     pDC->PatBlt(nR - 3,nT + 3,1,nB - nT - 6,BLACKNESS);
     return;
   } 
    
    pDC->PatBlt(nL + 1, nT + 1, nR - nL - 2, 1, BLACKNESS);
    pDC->PatBlt(nL + 1, nT + 1, 1, nB - nT - 2, BLACKNESS);
    pDC->PatBlt(nL + 1, nB - 1, nR - nL - 2, 1, BLACKNESS);
    pDC->PatBlt(nR - 1, nT + 1, 1, nB - nT - 2, BLACKNESS);
        
    // Lower half of the perimeter of the rectangle is painted white
    pDC->PatBlt(nL + 2,nT + 2,1,nB - nT - 4,WHITENESS);
    pDC->PatBlt(nL + 2,nT + 2,nR - nL - 4,1,WHITENESS);
    pDC->PatBlt(nL + 3,nT + 3,nR - nL - 6,1,WHITENESS);
    pDC->PatBlt(nL + 3,nT + 3,1,nB - nT - 6,WHITENESS);
    
    // The rest DkGray to give a 3D Appearance
    pOldBr = pDC->SelectObject(&brGray);
    pDC->PatBlt(nL + 3,nB - 3,nR - nL - 6,1,PATCOPY);
    pDC->PatBlt(nR - 3,nT + 3,1,nB - nT - 6,PATCOPY);
    pDC->PatBlt(nR - 2,nT + 2,1,nB - nT - 4,PATCOPY);
    pDC->PatBlt(nL + 2,nB - 2,nR - nL - 4,1,PATCOPY);

    pDC->SelectObject(pOldBr);
}




/////////////////////////////////////////////////////////////////////////////
// CLegendWnd


CLegendWnd::CLegendWnd(CGraphManager *pGM)
: m_POffsetPoint(0,0)
{ 
  m_pGMParent = pGM;
  m_bInCapture = FALSE;
}

CLegendWnd::~CLegendWnd()
{
}


BEGIN_MESSAGE_MAP(CLegendWnd, CWnd)
	//{{AFX_MSG_MAP(CLegendWnd)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_SIZE()
	ON_WM_NCACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CLegendWnd::CreateLegendWindow(LPCSTR lpszClassName,CRect& crRect,HWND hWnd)
{   
  if (!CreateEx(0,lpszClassName,"",WS_POPUP|WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME,
    crRect.left,crRect.top,crRect.Width(),crRect.Height(), hWnd,0))
  {    
    return FALSE;
  }
  
  CRect crLegendRect;
  // some random ID
  const int ID_LEGENDLIST = 123;
  GetClientRect(crLegendRect);
  SetWindowText(m_pGMParent->GetLegendWinTitle());
  
  if (!m_ListBox.Create(WS_CHILD|WS_VISIBLE|WS_BORDER
       |WS_VSCROLL|LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT,
         crLegendRect,this,ID_LEGENDLIST))
    return FALSE;
  
 int nNumItems = m_pGMParent->GetNumberOfGraphItems();        
 for (int i = 1; i <= nNumItems; i++)
 {           
   LPSTR lp = m_pGMParent->GetGraphLegendString(i);
   m_ListBox.AddString(lp);
 } 
 
 ShowWindow(SW_SHOW);   
 m_ListBox.ShowWindow(SW_SHOW);
 
 return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLegendWnd message handlers

void CLegendWnd::PostNcDestroy()
{
   m_pGMParent->ResetLegendWindowObject();    
   delete this;
}


void CLegendWnd::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT( m_pGMParent != NULL);
	int iItemID  = lpDrawItemStruct->itemID;
	CDC *pItemDC =  CDC::FromHandle(lpDrawItemStruct->hDC);
	ASSERT (pItemDC != NULL);
	CRect rc( lpDrawItemStruct->rcItem );
	COLORREF cr =  m_pGMParent->GetGraphItemColor(iItemID + 1);
	CPen pen(PS_SOLID,1,cr);
	CBrush brush(cr);           
	CPen *pOldPen = pItemDC->SelectObject(&pen);
	CBrush *pOldBrush = pItemDC->SelectObject(&brush);
	pItemDC->SetTextColor(cr);
	if ( lpDrawItemStruct->itemData )
	{
	 CRect crTextRect(rc.left + rc.Width() / 4 + 1,rc.top,rc.right,rc.bottom);
	 pItemDC->DrawText((LPSTR)lpDrawItemStruct->itemData,-1,crTextRect,DT_VCENTER | DT_LEFT);
	}
	if ( m_pGMParent->GetColorRectFlag())
	  pItemDC->PatBlt(rc.left + 2,rc.top,rc.left + rc.Width() / 5,rc.bottom - rc.top,PATCOPY);
	pItemDC->SelectObject(pOldBrush);
	pItemDC->SelectObject(pOldPen);   
}

void CLegendWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
   ASSERT (m_pGMParent != NULL);
   int nNumItems = m_pGMParent->GetNumberOfGraphItems();
   if ( !nNumItems ) return;
   CRect crListRect;
   m_ListBox.GetClientRect(crListRect);
   lpMeasureItemStruct->itemWidth = crListRect.Width();
}



void CLegendWnd::OnSize(UINT nType, int cx, int cy) 
{
   CWnd::OnSize(nType, cx, cy);

   if (::IsWindow(m_ListBox.m_hWnd))
   {
     CRect crRect;
     GetClientRect(crRect);
     m_ListBox.MoveWindow(crRect.left,crRect.top,crRect.Width(),crRect.Height());
   }
}



BOOL CLegendWnd::OnNcActivate(BOOL bActive) 
{
#ifdef WIN32
	return CWnd::OnNcActivate(bActive);
#else
	return TRUE;
#endif
}



void CLegendWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
   // TODO: Add your message handler code here and/or call default
   CWnd::OnMouseMove(nFlags, point);	 
   
   #ifndef WIN32
   if ( m_bInCapture )
	{
	  CRect rect;
	  GetWindowRect(rect);
	  ClientToScreen(rect);
	  ClientToScreen(&point);
      MoveWindow(point.x - m_POffsetPoint.x,point.y - m_POffsetPoint.y
        ,rect.Width(),rect.Height());
	}
   #endif
} 


void CLegendWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	#ifndef  WIN32 
	if ( m_bInCapture )
	{
	  m_bInCapture = FALSE;
	  m_POffsetPoint = CPoint(0,0);
	  ReleaseCapture();    
	}                  
    #endif

	CWnd::OnLButtonUp(nFlags, point);
}

void CLegendWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	#ifdef  WIN32
    CWnd::OnNcLButtonDown(nHitTest,point);
    #else
    if (nHitTest == HTCAPTION)
	{
    CRect rect;                
    GetWindowRect(rect);
    ClientToScreen(rect);
    ClientToScreen(&point);
    m_POffsetPoint = CPoint(point.x - rect.left,point.y - rect.top);
    m_bInCapture    = TRUE;
    SetCapture();
	}   

	
   #endif
}

void CLegendWnd::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
  // Don't call the default

#ifdef WIN32
      CWnd::OnNcLButtonUp(nHitTest,point);   
#else
	if ( m_bInCapture )
	{
	  m_bInCapture = FALSE;
	  m_POffsetPoint = CPoint(0,0);
	  ReleaseCapture();    
	}                         


#endif
}

/*
BOOL CCallQueue::AddCall( DWORD dwCallBack, DWORD dwCurVal )
{    
   _CALL_NODE *p = AllocNewCallNode();
   
   BOOL bRetCode = FALSE;
   
   if ( p )
   {
     p->pNext      = NULL;
     p->dwCallBack = dwCallBack;
     p->dwCurVal   = dwCurVal;  
     
     if ( !m_pCallQueueHead ) m_pCallQueueHead = p; 
     else 
       m_pCallQueueHead->pNext  = p; 
       
     bRetCode = TRUE;  
   }    
   
   return bRetCode;
}

BOOL CCallQueue::DispatchCalls()
{
  BOOL bReturn = TRUE;

  if ( !m_pCallQueueHead )
    return FALSE;
   
  _CALL_NODE *p = NULL; 

  do
  {
     p = m_pCallQueueHead;   

     // Call the alarm function
     (*(LPFNALARMCALLBACK)p->dwCallBack)(p->dwCurVal); 
     
     m_pCallQueueHead = p->pNext;
     
     FreeCallNode( p );     
     
  } while ( m_pCallQueueHead );


  return TRUE;
}

void CCallQueue::FreeCallNode( _CALL_NODE *p )
{
  if ( p )
   delete p;
}           

_CALL_NODE * CCallQueue::AllocCallNode()
{
  GraphSetNewHandler();
  _CALL_NODE *p = new _CALL_NODE;
  GraphClearNewHandler();
  return p;
} 

BOOL CGraphManager::SetStartTime(DWORD dwCurTime)
{    
  if (!IsRealTimeGraph())
    return FALSE;        
    
  RTSTARTTIME = dwCurTime;
  return TRUE;
}                     

BOOL CGraphManager::IsRealTimeGraph()
{
  return ( m_dwGraphStyle & GS_REALTIME );
}

BOOL CGraphManager::SetSamplingInterval( DWORD dwCurInterval )
{
    if ( !IsRealTimeGraph())
     return FALSE;
     
   GRAPHRTINTERVAL = dwCurInterval;
   
   return TRUE;
}

BOOL CGraphManager::SetNewCriticalLimitAlarm( LPFNALARMCALLBACK lpfnNewCritAlarmCallBack, GRAPH_DATA gdCriticalLimit )
{
  _GRAPH_CRIT_LIM_INFO * p  = GRAPHCRITLIMINFO;
  BOOL bTypeDouble = IsFloat();
  if ( !p )
   return FALSE;
  for (;p ;p = p->pNext )
  {
    if ( bTypeDouble )
    {
     if ( p->gd.ugd.dDoubleValue == gdCritLim.ugd.dDoubleValue )
        break;
    }
    else
    {
      if ( p->gd.ugd.dwDwordValue == gdCritLim.ugd.dwDwordValue )
        break;
    }   
  }
  if ( p )
    p->lpfnAlarmCallBack = lpfnNewCritAlarmCallBack;
  return TRUE;
}         



BOOL CGraphManager::ProcessCriticalLimitState( _RTGDATA *pRTG )
{      
  BOOL bTypeDouble = IsFloat();
#ifdef WIN32
   ASSERT( m_pCSH != NULL );
   m_pCSH->Lock( QUECRITSECTION );
#endif
  for (GRAPH_DATA *p = GRAPHCRITLIMINFO ;p; p = p->pNext)
  {           
    bCallAlarm = bTypeDouble ? ( p->gd.ugd.dwdDoubleValue <= gdCurData.ugd.dDoubleValue )
              : ( p->gd.ugd.dwDwordValue   <= gdCurData.ugd.dwDwordValue  );
    if ( bCallAlarm )
      m_CallQueue.AddCall( p->lpfnAlarmCallBack, gdCurData );   
  }  
#ifdef WIN32
  m_pCSH->Unlock( QUECRITSECTION );
  m_hCallThread = AfxBeginThread(CallQueueFunc,this);
  return TRUE;
#else
  return m_CallQueue.DispatchCall();
#endif
} 

#if 0

UINT CallQueueFunc( LPVOID lpVoidParam )
{
	CGraphManager *pCGM = (CGraphManager *)lpVoidParam;
	ASSERT(pCGM != NULL);
   	m_pCSH->Lock( QUECRITSECTION );
	pCGM->m_CallQueue.DispatchCall();
 	m_pCSH->Unlock( QUECRITSECTION );
}

#endif


void CGraphManager::AddNewCriticalLimInfo(
                    int nHorizFlag,
                    GRAPH_DATA gdCritLim,
                    LPFNALARMCALLBACK lpfnAlarmCallBack
                  )
{
   BOOL bTypeDouble = IsFloat();

   _GRAPH_CRIT_LIM_INFO * p  = GRAPHCRITLIMINFO;
   _GRAPH_CRIT_LIM_INFO * q  = NULL;      
    
    // Allocate new critical limit info object
    if ((q = AllocNewCriticalLimitInfo()) != NULL)
    {
      bTypeDouble ? (q->gd.ugd.dwDwordValue = gdCritLim.ugd.dwDwordValue) :
        (q->gd.ugd.dDoubleValue = gdCritLim.ugd.dDoubleValue);
      q->nHorizFlag             = nHorizFlag ;
      q->lpfnAlarmCallBack      = lpfnAlarmCallBack;
    }    
    
    // if First entry update 'head' 
    // and return
    if ( !p )
    {  return ((GRAPHCRITLIMINFO = q) != NULL);    }
    

    // else append it
    for (;p && (p->pNext != NULL);p = p->pNext);

    p->pNext = q;
    
    return q ? TRUE : FALSE;
}    

BOOL CGraphManager::StartRealTimePlot(CWnd *pDrawWnd)
{
  // Bail out if not real time	  CWinThread
  if (!IsRealTimeGraph())
	  return FALSE;

#ifdef WIN32
  if ( m_pRTGThread )
	return FALSE;
  m_pRTGThread = AfxBeginThread(RTGraphControlfunc,m_pRTGData);
  ASSERT( m_pRTGThread != NULL);
#else
	m_pRTGData->pDrawWnd->SetTimer(ID_RTGTIMER, GRAPHRTINTERVAL, NULL );
#endif

    return TRUE;
}


#ifdef WIN32

UINT RTGraphControlFunc( LPVOID lpVoidParam )
{
	_RTGDATA *pRTG = (_RTGDATA *)lpVoidParam;
	ASSERT(pCGM != NULL);
	ASSERT( m_pCSH != NULL );
  	m_pCSH->Lock( RTGCRITSECTION );
	BOOL bIsY = pRTG->pCGM->IsYRealTime();

    while ( !pRTG->bStopRTPlot && pRTG->pDrawWnd )
	{
		pRTG->pCGM->GetRTData( pRTG );
	    pRTG->pCGM->ProcessCriticalLimitState(pRTG);
		pRTG->pCGM->PlotRTData( pRTG );
	}

    m_pCSH->Unlock( RTGCRITSECTION );
}

#endif

BOOL CGraphManager::PlotRTData(_RTGDATA *pRTG)
{
	ASSERT(m_pRTGData != NULL);
	CDC *pDC = pRTG->pDrawWnd->GetDC();
	BOOL bTypeDouble = IsFloat();
	if (!pDC )
	{
		pRTG->bStopRTPlot = TRUE;
		return FALSE;
	}
	DWORD dXPoint = bTypeDouble ? pRTG->gdX.ugd.dDoubleValue : 
	         pRTG->gdX.ugd.dwDwordValue;
	DWORD dYPoint = bTypeDouble ? pRTG->gdY.ugd.dDoubleValue : 
	         pRTG->gdY.ugd.dwDwordValue;

	AddNewNonPieData( pRTG->gdX, pRTG->gdY, pRTG->nCurveNum );

    DrawLinkLine(pDC,
		      CPoint(pRTG->nLineStartX,pRTG->nLineStartY),
			  CPoint((int)(pRTG->pBCD->xOrigin + dXPoint),
	         (int)((double)pRTG->pBCD->yOrigin - dYPoint)));

	pRTG->nLineStartX = dXPoint;
	pRTG->nLineStartY = dYPoint;

	pRTG->pDrawWnd->ReleaseDC(pDC);
	return TRUE;
}


 
BOOL CGraphManager::GetRTData( _RTGDATA *pRTG )
{
#ifdef WIN32
	m_pCSH->Lock( GRDCRITSECTION );
#endif
	double d = 0.0;

	// Caller defined Real time Data acquisition function
	// might be a call into a driver etc.
    BOOL   b = (*pRTG->lpfnRTDataFunc)( &pRTG->gdX, &pRTG->gdY);

	if ( !b )
	  goto _GRDExit;

	b  = IsFloat();
    d  = GetDisplayPoint( pRTG->gdX, TRUE  );

	if ( b ) pRTG->gdX.ugd.dDoubleValue = d;
	else
       pRTG->gdX.ugd.dDoubleValue  = d;

	d  = GetDisplayPoint( pRTG->gdY, FALSE );

	if ( b ) pRTG->gdY.ugd.dDoubleValue = d;
	else
       pRTG->gdY.ugd.dDoubleValue  = d;
 
_GRDExit:

#ifdef WIN32
 	m_pCSH->Unlock( GRDCRITSECTION );
#endif
	return bSuccess;
}


BOOL CGraphManager::ProcessLButtonDown(UINT nFlags, CPoint& point )
{
	 return TRUE;
}


BOOL CCSHandler::InitCS( int nNoOfCS )
{
   if ( !nNoOfCS )
	   return FALSE; 

   TRY
   {
     m_pMFCCS = new CCriticalSection[ nNoOfCS ];
   }
   CATCH
   {
	 m_pMFCCS = NULL;
	 return FALSE;
   }
   END_CATCH

   m_nNoOfCS = nNoOfCS;

   return TRUE;
}

BOOL CCSHandler::Lock( int nCSIndex )
{
	if (( nCSIndex < 1 ) || ( nCSIndex > m_nNoOfCS))
	  return FALSE;

	m_pMFCCS[ nCSIndex - 1].Lock();

	return TRUE;
}

BOOL CCSHandler::Unlock( int nCSIndex )
{
  	if (( nCSIndex < 1 ) || ( nCSIndex > m_nNoOfCS))
	  return FALSE;

	m_pMFCCS[ nCSIndex - 1].Unlock();

	return TRUE;
}

BOOL CCSHandler::Free() 
{
  if ( m_pMFCCS )
	  delete [] m_pMFCCS;
}

*/

