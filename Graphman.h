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

#ifndef __GRAPH_DATA_INCLUDE__
#define __GRAPH_DATA_INCLUDE__

#include <new.h>

#define BCXDATA      m_giGraphInfo.pgtBarOrCurve->pgdHorizGraphData
#define BCYDATA      m_giGraphInfo.pgtBarOrCurve->pgdVertGraphData
#define BCXMAX       m_giGraphInfo.pgtBarOrCurve->gdHorizMax
#define BCYMAX       m_giGraphInfo.pgtBarOrCurve->gdVerticalMax
#define BCXMIN       m_giGraphInfo.pgtBarOrCurve->gdHorizMin
#define BCYMIN       m_giGraphInfo.pgtBarOrCurve->gdVerticalMin
#define BCXTITLE     m_giGraphInfo.pgtBarOrCurve->lpszHorizTitle
#define BCYTITLE     m_giGraphInfo.pgtBarOrCurve->lpszVertTitle
#define BCXDIVISION  m_giGraphInfo.pgtBarOrCurve->gdHorizDivision
#define BCYDIVISION  m_giGraphInfo.pgtBarOrCurve->gdVerticalDivision
#define BARWIDTH     m_giGraphInfo.pgtBarOrCurve->gdBarWidth         
#define BCGRIDCOLOR  m_giGraphInfo.pgtBarOrCurve->crGridColor
#define BCPLOTCOLOR  m_giGraphInfo.pgtBarOrCurve->pgcdPlotColor
#define BCXPTEXT     m_giGraphInfo.pgtBarOrCurve->pgpHorizPtText
#define BCYPTEXT     m_giGraphInfo.pgtBarOrCurve->pgpVertPtText


#define GRAPHTYPE            m_giGraphInfo.gtGraphType
#define GRAPHDATATYPE        m_giGraphInfo.gdtGraphDataType
#define GRAPHLEGEND          m_giGraphInfo.lpszLegendInfo
#define GRAPHTITLEINFO       m_giGraphInfo.pTitleInfo
#define GRAPHTITLEFONT       m_giGraphInfo.pTitleInfo->lfLogFont
#define GRAPHTITLE           m_giGraphInfo.pTitleInfo->lpszTitle
#define GRAPHTITLEFGCOLOR    m_giGraphInfo.pTitleInfo->crTitleFgColor
#define GRAPHTITLEBKCOLOR    m_giGraphInfo.pTitleInfo->crTitleBkColor
#define GRAPHLGWINTITLE      m_giGraphInfo.lpszLegendWinTitle
#define GRAPHITEMNUM         m_giGraphInfo.nNumberOfItems
#define GRAPHDATAFONT        m_giGraphInfo.pGDataDisplayInfo->lfLogFont
#define GRAPHDATACOLOR       m_giGraphInfo.pGDataDisplayInfo->crDataDisplayColor
#define GRAPHDATADISPLAYINFO m_giGraphInfo.pGDataDisplayInfo
#define GRAPHCOLORDATA       m_giGraphInfo.pgcdColorValues
#define GRAPHDATA            m_giGraphInfo.pgdPieData
#define GRAPHBARORCURVE      m_giGraphInfo.pgtBarOrCurve



// Supported graph styles
#define  GS_PIE              0x00000001
#define  GS_BAR              0x00000002
#define  GS_CURVE            0x00000004
#define  GS_PIEREGULAR       0x00000008
#define  GS_PIEELLIPTIC      0x00000010
#define  GS_BC3DFRAME        0x00000020
#define  GS_GRAPHTITLERIGHT  0x00000040
#define  GS_GRAPHTITLELEFT   0x00000080
#define  GS_FLOATING		 0x00000100
#define  GS_BC3DBARS		 0x00000200
#define  GS_GRID  			 0x00000400
#define  GS_LEGENDWIN		 0x00000800
#define  GS_LGREFLECTSHADE   0x00001000
#define  GS_BCDISPLAYINT     0x00002000
#define  GS_BCDISPLAYLONG    0x00004000
#define  GS_TRANSPARENTTITLE 0x00008000
#define  GS_SIGNED           0x00010000
#define  GS_BCMARKPOINTX     0x00020000
#define  GS_BCMARKPOINTY     0x00040000
#define  GS_BCMARKPTANDDIVX  0x00080000
#define  GS_BCMARKPTANDDIVY  0x00100000
#define  GS_CURVEBOLDPOINTS  0x00200000



struct _GRAPH_DATA
{
   union{
     float   dDoubleValue;
     DWORD   dwDwordValue;
   } ugd;
  _GRAPH_DATA  *pNext; 
  _GRAPH_DATA();
};

struct _GRAPH_PTEXT
{                           
   LPSTR        lpszText;
  _GRAPH_PTEXT *pNext;
};               


struct _GRAPH_COLOR_DATA
{
  COLORREF rgbColor;
  _GRAPH_COLOR_DATA *pNext;
};                         

struct _GRAPH_TITLE_INFO
{
  LPSTR       lpszTitle;
  LOGFONT     lfLogFont;
  COLORREF    crTitleFgColor;
  COLORREF    crTitleBkColor;
   _GRAPH_TITLE_INFO();
  _GRAPH_TITLE_INFO(LPSTR,LOGFONT *,COLORREF,COLORREF);
};         

struct _GRAPH_DATA_DISPLAY_INFO
{
  COLORREF    crDataDisplayColor;
  LOGFONT     lfLogFont;
  _GRAPH_DATA_DISPLAY_INFO();
  _GRAPH_DATA_DISPLAY_INFO(COLORREF,LOGFONT *);
};

struct _BAR_CURVE_INFO
 { 
  _GRAPH_DATA  gdHorizMax;      
  _GRAPH_DATA  gdVerticalMax;
  _GRAPH_DATA  gdHorizMin;      
  _GRAPH_DATA  gdVerticalMin;
  _GRAPH_DATA  gdBarWidth;
  _GRAPH_DATA  gdHorizDivision;
  _GRAPH_DATA  gdVerticalDivision;
  _GRAPH_DATA  **pgdVertGraphData;
  _GRAPH_DATA  **pgdHorizGraphData;
  _GRAPH_PTEXT **pgpHorizPtText;
  _GRAPH_PTEXT **pgpVertPtText;
  LPSTR        lpszHorizTitle;
  LPSTR        lpszVertTitle;
  COLORREF     crGridColor;
  _BAR_CURVE_INFO();
 };                     
 

struct _GRAPH_INFO
{
  _GRAPH_TITLE_INFO        * pTitleInfo;
  _GRAPH_DATA_DISPLAY_INFO * pGDataDisplayInfo;
  _GRAPH_COLOR_DATA        * pgcdColorValues;
  _GRAPH_DATA              * pgdPieData;
  LPSTR                    * lpszLegendInfo;
  LPSTR  lpszLegendWinTitle;
  int    nNumberOfItems;
  // BarDiagram or Curve Fitting specific
  // info
  _BAR_CURVE_INFO          * pgtBarOrCurve;
  // Constructor to initialize  
  _GRAPH_INFO();
};            

struct _PIE_CORE_DATA
{
 double b,a,x1,y1,x,y,theetah,sintheetah,costheetah,denom;
 int iStartX,iStartY;
};

struct _BC_CORE_DATA
{
 int xVertMax,yVertMax,xOrigin,yOrigin,xHorizMax,yHorizMax;
 int xDisplayRange,yDisplayRange; 
};

#if 0
struct _RTG_DATA
{
   CWnd *pDrawWnd;
   int nCurveNum;
  _BC_CORE_DATA *pBCD;
  _GRAPH_DATA gdX,gdY;
   LPRTDATAFUNC lpfnRTDataFunc;
};

#endif

/////////////////////////////////////////////////////////////////////////////
// CLegendWnd window

class CGraphManager;

class CLegendWnd : public CWnd
{
// Construction 
   LPSTR * m_pszLegendData;        
   CGraphManager *m_pGMParent;
   CListBox m_ListBox;
   BOOL m_bInCapture;
   CPoint m_POffsetPoint;
   
public:
	CLegendWnd(CGraphManager *p);

// Attributes
public:

// Operations
public:
  BOOL CreateLegendWindow(LPCSTR lpszClasName,CRect &,HWND hWnd);

// Overrides
public:

// Implementation
public:
	virtual ~CLegendWnd();
  virtual void PostNcDestroy();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLegendWnd)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CGraphManager
/*

class CCSHandler
{
	CCriticalSection *m_pMFCCS;
	int nNoOfCS;
public :
	CCSHandler();
	~CCSHandler();
	InitCS( int nNoOfCritSections );
	Lock (  int nCritSectionIndex );
	Unlock( int nCritSectionIndex );
	FreeCS();
};

*/

class CGraphManager
{  
    // Common
   _GRAPH_INFO  m_giGraphInfo;
   double       m_dTotal;   
   
   // Font specific
   CFont*       m_pTitleFont;
   CFont*       m_pDataFont;
   
   // Legend Window specific
   CLegendWnd*  m_pLegendWnd;     
   int          m_nLegendLenMax;   
   
   // old 'new' handler
   _PNH         m_pnhOldNewHandler;
   
   // Bar and Curve Specific
   int          m_xDisplayRange;  
   int          m_yDisplayRange; 
   DWORD        m_dwGraphStyle;   
/*   int          m_nStartPoint
   int          m_nEndPoint
 */  
   
   // To Optimize drawing and improve
   // speed and to avoid despicable 
   // flicker
   CBitmap *m_pMemBitmap;
   CRect m_crOldClientRect;

   // Currently Used for Pie Only
   // Percent display collision detect
   CRect  ** m_pSavedRects;

#if 0
   CCSHandler *m_pCSH;
#endif
   
  public :  
    // constructor
            CGraphManager();
    // destructor        
            ~CGraphManager();    
    // One Self-Init interface
    BOOL     InitGraphManager(CWnd *pWnd);
    // One and only drawing interface
    BOOL     DrawGraph(CWnd *pWnd,CDC *pDC,CRect *pBoundsRect = NULL);
    
    
    // One Graph-Init interface
    BOOL     InitGraph(int nNumberOfItems,DWORD dwStyle,
                   _GRAPH_TITLE_INFO *pgtInfo = NULL,
                   _GRAPH_DATA_DISPLAY_INFO *pgddInfo = NULL);
    
    // Many Common-to-all-type-of-graph interfaces
    BOOL     SetNumberOfItems(int nNoOfItems);
    BOOL     SetGraphTitleInfo(_GRAPH_TITLE_INFO *pgtInfo);
    BOOL     SetGraphDataDisplayInfo(_GRAPH_DATA_DISPLAY_INFO *pgddInfo);
    void     SetGraphStyle(DWORD dwGraphStyle);
    
    CFont *  GetTitleFont();
    COLORREF GetTitleFgColor();
    COLORREF GetTitleBkColor();
    LPSTR    GetGraphTitle();
    COLORREF GetDataDisplayColor();
    CFont *  GetDataDisplayFont();
    int      GetNumberOfGraphItems();
    COLORREF GetGraphItemColor(int nItemIndex);
    BOOL     GetColorRectFlag();
    LPSTR    GetGraphLegendString(int nItemNum);
    DWORD    GetGraphStyle();
	double   GetDisplayPoint(_GRAPH_DATA gd,BOOL bX);
	void     DrawLinkLine(CDC *pDC,CPoint ptStart,CPoint ptEnd);
    
    
    BOOL     IsPie();
    BOOL     IsBar();
    BOOL     IsCurve();
    BOOL     IsFloat();
    
    BOOL     AddNewGraphColorData(COLORREF cr);
        
    // Pie Chart
    BOOL     SetNewLegendData(LPSTR lpszLegend,int nPos);
    BOOL     SetLegendWinTitle(LPSTR lpszTitle);
    double   GetPieSectorPercentage(int nSectorNum);
    LPSTR    GetLegendWinTitle();
    CFont *  GetPiePercentDisplayFont();
    COLORREF GetPiePercentDisplayFgColor();
    COLORREF GetPiePercentDisplayBkColor();
    BOOL     AddNewPieData(_GRAPH_DATA *pGD);
   
    // Bar Diagram
    BOOL     SetHorizontalLimits(_GRAPH_DATA *pGDMax,_GRAPH_DATA *pGDMin);
    BOOL     SetVerticalLimits(_GRAPH_DATA *pGDMax,_GRAPH_DATA *pGDMin);
    BOOL     SetBarWidth(_GRAPH_DATA *pGD);
    int      SetHorizDivisionUnit(_GRAPH_DATA *pGD);
    int      SetVertDivisionUnit(_GRAPH_DATA *pGD);
    BOOL     SetYAxisDescText(LPSTR);
    BOOL     SetXAxisDescText(LPSTR);
    void     SetGridColor(COLORREF crGridColor);
    
    LPSTR    GetYAxisDescText();
    LPSTR    GetXAxisDescText();
    double   GetBarWidth();   
    double   GetHorizontalRange();
    double   GetVerticalRange();
    double   GetBarHeight(int nBarNum );
    BOOL     GetGridFlag();
    COLORREF GetGridColor();
    int      GetNumberOfBars();
    COLORREF GetBarColor(int nBarNum);
    LPSTR    GetBarHorizPointText(int nBarNum);
    LPSTR    GetBarVertPointText (int nBarNum);
    
    BOOL     AddNewBarData(_GRAPH_DATA *pGDX,_GRAPH_DATA *pGDY);
    BOOL     AddNewBarPtText(LPSTR lpszXText,LPSTR lpszYText);
    
    
    // Curve Fitting
    
    COLORREF GetPlotColor( int nCurveNum );
    int      GetPointsInCurve( int nCurveNum );
    int      GetNumberOfCurves();
    BOOL     AddNewCurveData(_GRAPH_DATA *pGDX,_GRAPH_DATA *pGDY,int nCurveIndex);
    BOOL     AddNewCurvePtText(LPSTR lpszXText,LPSTR lpszYText, int nCurveIndex);

    // Misc
    void ResetLegendWindowObject();
    
 protected:                  
     // core drawing functions    
    BOOL     DrawPieChart(CRect & crRect,CDC *pDC = NULL);
   	BOOL     DrawBarOrCurve(CRect & crRect,BOOL bBar,CDC *pDC = NULL);
    BOOL     DrawGraphTitle(CDC *pDC,CRect&,const CRect *pClientRect);
	BOOL     DrawXDivisionMarks(CDC *pDC,_BC_CORE_DATA *pBC);
	BOOL     DrawYDivisionMarks(CDC *pDC,_BC_CORE_DATA *pBC,int& nMaxDisplayStrLen);
	BOOL     DrawXPointMarks(CDC *pDC,_BC_CORE_DATA *pBC);
	BOOL     DrawYPointMarks(CDC *pDC,_BC_CORE_DATA *pBC,int& nMaxDisplayStrLen);
	BOOL     DrawGridLines(CDC *pDC,_BC_CORE_DATA *pBC);
	void     Draw3DFrame(CDC *pDC,LPRECT pRect,BOOL bEtchedIn = FALSE);
	BOOL     DrawBars(CDC *pDC,_BC_CORE_DATA *pBC,double dBarWidth);
	BOOL     DrawCurves(CDC *pDC,_BC_CORE_DATA *pBC);
	BOOL     DrawAxesInfo(CDC *pDC,_BC_CORE_DATA *pBCD,int nMaxDisplayLen);

	
    BOOL     AddNewNonPieData(_GRAPH_DATA *pGDX,_GRAPH_DATA *pGDY,int nItem = 0);
    BOOL     AddNewPointText(LPSTR lpszXText,LPSTR lpszYText, int nItem);
    
    double   CalcBarDisplayWidth();
    double   CalcBarDisplayHeight(int nBarNum);               
    int      CalcMaxHeight(CWnd *pWnd);
    int      CalcMaxWidth(CWnd *pWnd);
    BOOL     CreateLegendWindow(CWnd *pGView); 
    
    BOOL     FormatDisplayValue(LPSTR szFormatBuffer , double dValue);
    
    double   GetBCDisplayStartXPoint(int nItemNo,int PointNo);
    double   GetBCDisplayStartYPoint(int nItemNo,int PointNo);
    double   GetBarDisplayStartXPoint( int nIndex );
    double   GetHorizRangeDisplayDivisionUnit();
    double   GetVertRangeDisplayDivisionUnit();
    double   GetCurveDisplayStartXPoint(int nItemNo,int PointNo);
    double   GetCurveDisplayStartYPoint(int nItemNo,int PointNo);
    LPSTR    GetCurveHorizPointText(int nCurveNo,int nPointNum);
    LPSTR    GetCurveVertPointText(int nCurveNo,int nPointNum);
    LPSTR    GetBarOrCurvePointText(_GRAPH_PTEXT *p,int nPointNum);
    
    double   GetVertDivisionValue(int nMarkerIndex);
    double   GetHorizDivisionValue(int nMarkerIndex);
    BOOL     GetDisplayRange(int& xDisplayRange,int& yDisplayRange);
    
    BOOL     SetDisplayRange(int xDisplayRange,int yDisplayRange);
    BOOL     SetUpFonts(CDC *pDC);
    
    void     GraphSetNewHandler();
    void     GraphClearNewHandler();
    
    LPSTR *  AllocCharVector(size_t nSize);
    LPSTR    AllocString( size_t nSize);
    CFont *  AllocFont();
    
    void     FreeFont ( CFont ** pFont);
    void     FreeGraphDataList( _GRAPH_DATA * pGraphData);
    void     FreeGraphDataListNodes( _GRAPH_DATA** pGraphData);
    void     FreeGraphColorDataList(_GRAPH_COLOR_DATA * pGCD);
    void     FreeBarOrCurveSpecificInfo(_BAR_CURVE_INFO *pBarOrCurve);
    void     FreeGraphColorData(_GRAPH_COLOR_DATA *pGCD);
    void     FreeGraphData(_GRAPH_DATA * p);
    void     FreeString( LPSTR p );         
    void     FreeGraphTitleInfo(_GRAPH_TITLE_INFO *pGTInfo);
    void     FreeDataDisplayInfo(_GRAPH_DATA_DISPLAY_INFO *pGDDI);
    void     FreeGraphPtTextListNodes( _GRAPH_PTEXT ** pGP );
    void     FreeGraphPtTextList( _GRAPH_PTEXT *pGPStart);
    void     FreeGraphPtText(_GRAPH_PTEXT * pGPtText);

    // others
    void     InitCollisionRects( int nNoOfItems );
    void     FreeCollisionRects();
    BOOL     CheckCollision( CRect& r1, CRect& r2);
    
    
     
    _GRAPH_DATA *       AllocNewGraphData();      
    _GRAPH_DATA *       AllocGraphData();
    _GRAPH_DATA **      AllocGraphDataVector(size_t nSize);
    _GRAPH_PTEXT **     AllocGraphPtTextVector(size_t nSize);
    _GRAPH_COLOR_DATA * AllocNewColorData();
    _GRAPH_COLOR_DATA * AllocGraphColorData();
    _GRAPH_PTEXT      * AllocNewPointTextNode();
};        

#endif
