// hmdoc.cpp : implementation of the CHmDoc class
//

#include "stdafx.h"
#include "hm.h"
#include "graphdat.h"
#include "hmdoc.h"
#include "pieview.h"
#include "graphvie.h"
#include "curvevie.h"




#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHmDoc

IMPLEMENT_DYNCREATE(CHmDoc, CDocument)

BEGIN_MESSAGE_MAP(CHmDoc, CDocument)
	//{{AFX_MSG_MAP(CHmDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHmDoc construction/destruction

CHmDoc::CHmDoc()
{
	// TODO: add one-time construction code here
	m_pBarGDM   = NULL;
	m_pCurveGDM = NULL;
	m_pPieGDM   = NULL;
}

CHmDoc::~CHmDoc()
{        
  if ( m_pBarGDM )
   delete m_pBarGDM ;
   
  if ( m_pCurveGDM )
   delete m_pCurveGDM ; 
   
  if ( m_pPieGDM )
   delete m_pPieGDM ; 
}           

CGraphManager * CHmDoc::GetGraphManager(CView *pView)
{   
  if ( pView->IsKindOf(RUNTIME_CLASS(CPieView)) )
    return m_pPieGDM;
    
  if ( pView->IsKindOf(RUNTIME_CLASS(CCurveView)) )
    return m_pCurveGDM;
    
  if ( pView->IsKindOf(RUNTIME_CLASS(CGraphView)) )
    return m_pBarGDM; 
    
  return NULL;
} 

BOOL CHmDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;                
	
   m_pBarGDM = new CGraphManager;
   ASSERT( m_pBarGDM != NULL);
   
   m_pCurveGDM = new CGraphManager;
   ASSERT( m_pCurveGDM != NULL);
   
   m_pPieGDM = new CGraphManager;
   ASSERT( m_pPieGDM != NULL);
	
   _GRAPH_DATA *pGDX = new _GRAPH_DATA;
   ASSERT( pGDX != NULL);	        
	 
   _GRAPH_DATA *pGDY = new _GRAPH_DATA;
   ASSERT( pGDY != NULL);
	  
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
   
    // used to set min x,y values
    // we stick with 0.0
    _GRAPH_DATA gdMinLimit;
    gdMinLimit.ugd.dDoubleValue  = -3.0;
   
   // BAR DATA          
   _GRAPH_TITLE_INFO  gtInfo("Investment Returns", &m_logfont,RGB(255,0,0),RGB(255,255,255));
   	m_logfont.lfHeight = -10;
   	m_logfont.lfWeight = FW_DONTCARE;
   _GRAPH_DATA_DISPLAY_INFO  gddInfo( RGB(0,0,255), &m_logfont );
   
    m_pBarGDM->InitGraph(6,GS_GRID|GS_SIGNED|GS_BAR|GS_BC3DFRAME|GS_BC3DBARS|GS_FLOATING|
    GS_TRANSPARENTTITLE|GS_LEGENDWIN|GS_LGREFLECTSHADE,
            &gtInfo,&gddInfo);                                
            
    m_pBarGDM->SetYAxisDescText("Revenues");
	m_pBarGDM->SetXAxisDescText("Expenses in Millions");        
            
    pGDX->ugd.dDoubleValue = 30.0;
    m_pBarGDM->SetHorizontalLimits( pGDX, &gdMinLimit );
 	
    pGDY->ugd.dDoubleValue = 30.0;
    m_pBarGDM->SetVerticalLimits( pGDY, &gdMinLimit );
	
	pGDX->ugd.dDoubleValue = 3.0;
	m_pBarGDM->SetBarWidth( pGDX );
	
	pGDX->ugd.dDoubleValue = 14.0;
	pGDY->ugd.dDoubleValue = 14.0;
	m_pBarGDM->AddNewBarData(  pGDX,pGDY );
	pGDX->ugd.dDoubleValue = 10.0;
	pGDY->ugd.dDoubleValue = 10.0;
	m_pBarGDM->AddNewBarData(  pGDX,pGDY );
	pGDX->ugd.dDoubleValue = 2.0;
	pGDY->ugd.dDoubleValue = 3.0;
	m_pBarGDM->AddNewBarData(  pGDX,pGDY );
	pGDX->ugd.dDoubleValue = 20.0;
	pGDY->ugd.dDoubleValue = 25.0;
	m_pBarGDM->AddNewBarData(  pGDX,pGDY );
	pGDX->ugd.dDoubleValue = 25.0;
	pGDY->ugd.dDoubleValue = 30.0;
	m_pBarGDM->AddNewBarData(  pGDX,pGDY );
	pGDX->ugd.dDoubleValue = 6.0;
	pGDY->ugd.dDoubleValue = 11.0;
	m_pBarGDM->AddNewBarData(  pGDX,pGDY ); 
	
	pGDX->ugd.dDoubleValue = 3.0;
	m_pBarGDM->SetHorizDivisionUnit(pGDX);
	
	pGDY->ugd.dDoubleValue = 3.0;
	m_pBarGDM->SetVertDivisionUnit(pGDY);
	
	m_pBarGDM->AddNewGraphColorData( RGB(255,0,0) );
	m_pBarGDM->AddNewGraphColorData( RGB(0,255,0) );
	m_pBarGDM->AddNewGraphColorData( RGB(0,0,255) );
	m_pBarGDM->AddNewGraphColorData( RGB(255,255,0) );
	m_pBarGDM->AddNewGraphColorData( RGB(0,255,255) );
	m_pBarGDM->AddNewGraphColorData( RGB(23,25,25) );
	
	m_pBarGDM->SetNewLegendData("Compaq",1);
	m_pBarGDM->SetNewLegendData("Intel",2);
	m_pBarGDM->SetNewLegendData("Microsoft",3);
	m_pBarGDM->SetNewLegendData("IBM",4);
	m_pBarGDM->SetNewLegendData("Cisco",5);
	m_pBarGDM->SetNewLegendData("Fujitsu",6);
	
   // CURVE DATA	
    gtInfo.lpszTitle  = "Sales Return graphs";       
   
    m_pCurveGDM->InitGraph(3,GS_GRID|GS_SIGNED|GS_CURVE|GS_BC3DFRAME|GS_FLOATING|GS_TRANSPARENTTITLE,
            &gtInfo,&gddInfo);           
            
    m_pCurveGDM->SetYAxisDescText("Profit");
	m_pCurveGDM->SetXAxisDescText("Sales in Millions");                
	
    pGDX->ugd.dDoubleValue = 130.0;        
    m_pCurveGDM->SetHorizontalLimits( pGDX, &gdMinLimit );
 	
    pGDY->ugd.dDoubleValue = 220.0;
    m_pCurveGDM->SetVerticalLimits( pGDY, &gdMinLimit );        
            
    pGDX->ugd.dDoubleValue = 11.2;
	pGDY->ugd.dDoubleValue = 145.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,1 );
	pGDX->ugd.dDoubleValue = 40.2;
	pGDY->ugd.dDoubleValue = 200.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,1 );
	pGDX->ugd.dDoubleValue = 60.234;
	pGDY->ugd.dDoubleValue = 200.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,1 );
	pGDX->ugd.dDoubleValue = 100.4;
	pGDY->ugd.dDoubleValue = 100.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,1 );
	pGDX->ugd.dDoubleValue = 120.234;
	pGDY->ugd.dDoubleValue = 150.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,1 );
	pGDX->ugd.dDoubleValue = 125.112;
	pGDY->ugd.dDoubleValue = 177.3;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,1 );
	
	pGDX->ugd.dDoubleValue = 11.2;
	pGDY->ugd.dDoubleValue = 120.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,2 );
	pGDX->ugd.dDoubleValue = 40.2;
	pGDY->ugd.dDoubleValue = 100.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,2 );
	pGDX->ugd.dDoubleValue = 60.234;
	pGDY->ugd.dDoubleValue = 120.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,2 );
	pGDX->ugd.dDoubleValue = 100.4;
	pGDY->ugd.dDoubleValue = 65.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,2 );
	pGDX->ugd.dDoubleValue = 120.234;
	pGDY->ugd.dDoubleValue = 34.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,2 );  
	
	pGDX->ugd.dDoubleValue = 11.2;
	pGDY->ugd.dDoubleValue = 156.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,3 );
	pGDX->ugd.dDoubleValue = 50.0;
	pGDY->ugd.dDoubleValue = 76.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,3 );
	pGDX->ugd.dDoubleValue = 65.0;
	pGDY->ugd.dDoubleValue = 140.000;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,3 );
	pGDX->ugd.dDoubleValue = 100.4;
	pGDY->ugd.dDoubleValue = 200.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,3 );
	pGDX->ugd.dDoubleValue = 120.234;
	pGDY->ugd.dDoubleValue = 110.234;
	m_pCurveGDM->AddNewCurveData(  pGDX,pGDY,3 );         
	
	pGDX->ugd.dDoubleValue = 10.11;
	m_pCurveGDM->SetHorizDivisionUnit(pGDX);
	
	pGDY->ugd.dDoubleValue = 20.23;
	m_pCurveGDM->SetVertDivisionUnit(pGDY);
	
	m_pCurveGDM->AddNewGraphColorData( RGB(255,0,0) );
	m_pCurveGDM->AddNewGraphColorData( RGB(0,255,0) );
	m_pCurveGDM->AddNewGraphColorData( RGB(0,0,255) );
	
	m_pCurveGDM->SetNewLegendData("Compaq",1);
	m_pCurveGDM->SetNewLegendData("Intel",2);
	m_pCurveGDM->SetNewLegendData("Microsoft",3);
	
    // PIE DATA           
            
    gtInfo.lpszTitle  = "IT Corporate Net Profit Analysis 1996-97";
   
    m_pPieGDM->InitGraph(6,GS_PIE|GS_FLOATING,
            &gtInfo,&gddInfo);           
            
   	pGDY->ugd.dDoubleValue = 14.0;
	m_pPieGDM->AddNewPieData( pGDY );
	pGDY->ugd.dDoubleValue = 10.0;
	m_pPieGDM->AddNewPieData( pGDY );
	pGDY->ugd.dDoubleValue = 3.0;
	m_pPieGDM->AddNewPieData( pGDY );
	pGDY->ugd.dDoubleValue = 25.0;
	m_pPieGDM->AddNewPieData( pGDY );
	pGDY->ugd.dDoubleValue = 30.0;
	m_pPieGDM->AddNewPieData( pGDY );
	pGDY->ugd.dDoubleValue = 11.0;
	m_pPieGDM->AddNewPieData( pGDY );          
	
	m_pPieGDM->AddNewGraphColorData( RGB(255,0,0) );
	m_pPieGDM->AddNewGraphColorData( RGB(0,255,0) );
	m_pPieGDM->AddNewGraphColorData( RGB(0,0,255) );
	m_pPieGDM->AddNewGraphColorData( RGB(255,255,0) );
	m_pPieGDM->AddNewGraphColorData( RGB(0,255,255) );
	m_pPieGDM->AddNewGraphColorData( RGB(23,25,25) );
	
	m_pPieGDM->SetNewLegendData("Compaq",1);
	m_pPieGDM->SetNewLegendData("Intel",2);
	m_pPieGDM->SetNewLegendData("Microsoft",3);
	m_pPieGDM->SetNewLegendData("IBM",4);
	m_pPieGDM->SetNewLegendData("Cisco",5);
	m_pPieGDM->SetNewLegendData("Fujitsu",6);
	
	m_pPieGDM->SetLegendWinTitle("Legend");//orate Competitors"); 
	
	if ( pGDX)
	 delete pGDX;
	
	if ( pGDY )
	 delete pGDY; 
/*	m_pGraphDataMan->SetPieChartTitle ( "Information Tech Corporate Profit Analysis 1996-97 ");
	
	pGD->ugd.dDoubleValue = 145.234;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dDoubleValue = 64.11;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dDoubleValue = 244.23;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dDoubleValue = 25.23;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dDoubleValue = 56.1627;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dDoubleValue = 156.567;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dDoubleValue = 14.78;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dwDwordValue = 56;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dwDwordValue = 56;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dwDwordValue = 56;
	m_pGraphDataMan->AddNewPieData( pGD );
*/	
/*    pGD->ugd.dwDwordValue = 56;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dwDwordValue = 56;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dwDwordValue = 56;
	m_pGraphDataMan->AddNewPieData( pGD );
	pGD->ugd.dwDwordValue = 56;
	m_pGraphDataMan->AddNewPieData( pGD );*/ 
	
	
	

//	m_pGraphDataMan->AddNewGraphColorData( RGB(100,100,255) );
//	m_pGraphDataMan->AddNewGraphColorData( RGB(192,192,192));
//	m_pGraphDataMan->AddNewGraphColorData( RGB(255,255,255) );
//	m_pGraphDataMan->AddNewGraphColorData( RGB(123,255,255) );
	
//	m_pGraphDataMan->SetYAxisDescText("Revenues");
//	m_pGraphDataMan->SetXAxisDescText("Expenses in Millions");
	

//	m_pGraphDataMan->SetNewLegendData("Digital",6);
	

//	m_pGraphDataMan->ShowLegendWindow(TRUE);
/*	m_pGraphDataMan->SetNewLegendData("Gateway",8);
	m_pGraphDataMan->SetNewLegendData("AT&T",9);
	m_pGraphDataMan->SetNewLegendData("Xerox Palo Alto",10);
	
	
*/
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHmDoc serialization

void CHmDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHmDoc diagnostics

#ifdef _DEBUG
void CHmDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHmDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHmDoc commands
