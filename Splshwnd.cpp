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
#include "splshwnd.h"
#include <io.h>
#include <errno.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif   

static BOOL bFromHeap = TRUE;

#pragma warning(disable : 4118)
#pragma intrinsic(strcpy)         

#define crL crClientRect.left
#define crT crClientRect.top
#define crR crClientRect.right
#define crB crClientRect.bottom
#define crW crClientRect.Width()
#define crH crClientRect.Height()

////////////////////////////////////////////////////////////
// CSplashWnd
////////////////////////////////////////////////////////////
CSplashWnd::CSplashWnd(BOOL& bSplashOver)
: m_nSplashSec(4),
  m_bError(FALSE),     
  m_bFromHeap(bFromHeap),
  m_b3DEnabled(0),
  m_nError(0),         
  m_nWidth(0),
  m_nHeight(0),
  m_pBmpInfoHeader(NULL),
  m_pBmpInfo(NULL),
  m_pBmpBytes(NULL), 
  m_hLocal(NULL),
  m_hGlobal(NULL),
  m_dwCurrentTime(0L),
  m_pSplashBitmap(NULL),
  m_lpszBmpFileName(NULL),
  m_lpszWindowTitle(NULL),
  m_lpszRegInfo(NULL),
  m_lpszOrgName(NULL),
  m_hSplashBitmap(NULL),
  m_bSplashOver(bSplashOver)
{                            
  
}

void CSplashWnd::Init( LONG lSplashSec,
    LPCSTR lpszWindowTitle,LPCSTR lpszRegInfo,LPCSTR lpszOrgName,BOOL b3DEnabled)
{
  m_b3DEnabled = b3DEnabled;
  m_nSplashSec  = lSplashSec;
  if ( lpszWindowTitle )
  {
     m_lpszWindowTitle = new char[ lstrlen( lpszWindowTitle ) + 1 ];
     if ( m_lpszWindowTitle )
      strcpy(m_lpszWindowTitle, lpszWindowTitle);
  }  
  
  if ( lpszRegInfo )
  {
     m_lpszRegInfo = new char[ lstrlen( lpszRegInfo ) + 1 ]; 
     if ( m_lpszRegInfo )
      strcpy(m_lpszRegInfo, lpszRegInfo);
  }
  
  if ( lpszOrgName ) 
  {  
     m_lpszOrgName = new char[ lstrlen( lpszOrgName ) + 1 ];
     if ( m_lpszOrgName )
      strcpy(m_lpszOrgName ,lpszOrgName);
  }
}

CSplashWnd::~CSplashWnd()
{
   CleanUpObject();
}


BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplashWnd message handlers


int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    // Create our window first
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	// We create a HBITMAP here and attach it to our
	// CBitmap *.
	if (m_pBmpInfoHeader && m_hGlobal && m_hLocal)	
	{
	  m_pBmpBytes = (LPBYTE)::GlobalLock(m_hGlobal);
	  m_pBmpInfo  = (PBITMAPINFO)::LocalLock (m_hLocal);
	  
	  m_hSplashBitmap = ::CreateDIBitmap(GetDC()->m_hDC,m_pBmpInfoHeader,
		 CBM_INIT,m_pBmpBytes,m_pBmpInfo,DIB_RGB_COLORS);
	
	  if (!m_hSplashBitmap)
	    {   return -1;   }	 
	  
      if (m_pSplashBitmap)
	    m_pSplashBitmap->Attach(m_hSplashBitmap);	
	   
	   // memory not needed any more 
        FreeBmpMemory(); 
	}    
	
	// Move the window to the Bitmap size
	if (m_pSplashBitmap)
	{
	  BITMAP bm;
      m_pSplashBitmap->GetObject(sizeof(BITMAP),(LPVOID)&bm);
      m_nWidth  = min(::GetSystemMetrics(SM_CXSCREEN) - 14,bm.bmWidth);
      m_nHeight = min(::GetSystemMetrics(SM_CYSCREEN) - 14,bm.bmHeight);
	}  
	// Move it!
	MoveWindow(lpCreateStruct->x,lpCreateStruct->y,m_nWidth + 12,m_nHeight + 12,FALSE);
	
	return 0;  
}   



// Overloaded Display function for 
// taking care of different presentations of 
// Bitmap
// Bitmap presented as a Handle 
int CSplashWnd::Display(HBITMAP hBitmap)
{  
  // if object not in heap return error
  if (!HeapSplashObject())
   return ErrorExit(SPLWND_ERROBJNOTCREATEDINHEAP);
  if (!AllocBitmap())
   return ErrorExit(SPLWND_ERRMEMALLOCFAIL);
  m_pSplashBitmap->Attach(hBitmap); 
  return SplashCreate();
} 

// Bitmap as a .bmp file name
// limited support for now => No RLE formats supported
int CSplashWnd::Display(LPCSTR lpBmpFileName)
{   
  // if object not in heap return error
  if (!HeapSplashObject())
   return ErrorExit(SPLWND_ERROBJNOTCREATEDINHEAP);
  // check file existance
  if (_access(lpBmpFileName,00) == -1)
    return ErrorExit(SPLWND_ERRFILENOTFOUND);
  // Allocate memory  
  if (!AllocBitmap())
   return ErrorExit(SPLWND_ERRMEMALLOCFAIL);  
  // Store the file name    
  m_lpszBmpFileName = new char[lstrlen(lpBmpFileName) + 1];
  if (!m_lpszBmpFileName)
    return ErrorExit(SPLWND_ERRMEMALLOCFAIL);  
  lstrcpy(m_lpszBmpFileName,lpBmpFileName);
  // Now read in the bitmap
  if (!ReadBitmapFile())
   return ErrorExit();
  // and Create the window 
  return SplashCreate();
} 


// Bitmap from ResourceID 
int CSplashWnd::Display(UINT uIDBitmap)
{  
  // if object not in heap return error
  if (!HeapSplashObject())
   return ErrorExit(SPLWND_ERROBJNOTCREATEDINHEAP);
  ASSERT(uIDBitmap != NULL);
  // Allocate memory          
  if (!AllocBitmap())
   return ErrorExit(SPLWND_ERRMEMALLOCFAIL);  
  // Load bitmap from resource 
  if (!m_pSplashBitmap->LoadBitmap(uIDBitmap))
   return ErrorExit(SPLWND_ERRBMPLOADFAIL);
  // Create window 
  return SplashCreate(); 
}   

// Allocate a CBitmap type object
int CSplashWnd::AllocBitmap()
{
  m_pSplashBitmap = new CBitmap;
  if (!m_pSplashBitmap)
    return FALSE;
    
  return TRUE;  
}


// Read the Bitmap file into memory
// and Lock it
BOOL CSplashWnd::ReadBitmapFile()
{  
  HFILE hBmpFile      = NULL;                   
  UINT nBytesRead     = 0;
  BOOL bRetCode       = FALSE;
  BITMAPFILEHEADER bmpFileHeader;
  UINT nRGBQuadSize   = 0;
  DWORD dwBmpByteSize = 0;
  BYTE *pByte    = NULL; 
  WORD wBitCount      = 0;
  int nRGBQuads       = 0;
  const UINT MAX_READ = 65534;
  
  ASSERT(m_lpszBmpFileName !=  NULL);
  hBmpFile      = _lopen(m_lpszBmpFileName,OF_READ);
  if (hBmpFile == HFILE_ERROR)
   { m_nError   = SPLWND_ERRBMPFILEOPENERROR; goto exit;}
  nBytesRead    = _lread(hBmpFile,&bmpFileHeader,sizeof(BITMAPFILEHEADER));
  if (nBytesRead == HFILE_ERROR)
   {  m_nError  = SPLWND_ERRREADFILEFAILURE; goto exit; }
  if (nBytesRead < sizeof(BITMAPFILEHEADER))
   { m_nError   = SPLWND_ERRINVALIDBMPFILE; goto exit;   }
  // Check for valid BMP file
  if (bmpFileHeader.bfType != (WORD)(('M' << 8 )|'B'))
   { m_nError   = SPLWND_ERRINVALIDBMPFILE; goto exit; }
  m_pBmpInfoHeader = new BITMAPINFOHEADER; 
  ASSERT(m_pBmpInfoHeader != NULL);
  nBytesRead    = _lread(hBmpFile,m_pBmpInfoHeader,sizeof(BITMAPINFOHEADER));
  if (nBytesRead == HFILE_ERROR)
    {  m_nError  = SPLWND_ERRREADFILEFAILURE; goto exit; }
  if (nBytesRead < sizeof(BITMAPINFOHEADER))
   { m_nError   = SPLWND_ERRINVALIDBMPFILE;goto exit;   }
  if (m_pBmpInfoHeader->biCompression != BI_RGB)
  {  m_nError      = SPLWND_ERRRLECMPRSSNOTSUPPORTED;bRetCode = FALSE;goto exit; }                                                          
  wBitCount     = m_pBmpInfoHeader->biBitCount;
  nRGBQuads = (wBitCount == 1)  ? 2:(wBitCount == 4)? 16:
                  (wBitCount == 8)  ? 256:0;
  if (!nRGBQuads)
   { m_nError   = SPLWND_ERR24BITNOTSUPPORTED;goto exit;  }                                                                          
  nRGBQuadSize  =  sizeof(RGBQUAD) * nRGBQuads;
  m_hLocal = ::LocalAlloc(LHND,sizeof(BITMAPINFOHEADER) + nRGBQuadSize);
  if (!m_hLocal)
   { m_nError   = SPLWND_ERRMEMALLOCFAIL; goto exit;}
  m_pBmpInfo    = (PBITMAPINFO)::LocalLock(m_hLocal);
  m_pBmpInfo->bmiHeader = *m_pBmpInfoHeader;          
  nBytesRead    = _lread(hBmpFile,m_pBmpInfo->bmiColors,nRGBQuadSize);
  if (nBytesRead == HFILE_ERROR)
    {  m_nError  = SPLWND_ERRREADFILEFAILURE; goto exit; }
  if (nBytesRead < nRGBQuadSize)
   {  m_nError  = SPLWND_ERRINVALIDBMPFILE;goto exit; }
  dwBmpByteSize = bmpFileHeader.bfSize - sizeof(BITMAPFILEHEADER)
                  - sizeof(BITMAPINFOHEADER) - nRGBQuadSize;
  m_hGlobal   = ::GlobalAlloc(GHND,dwBmpByteSize); 
  m_pBmpBytes = (LPBYTE)::GlobalLock(m_hGlobal);
  if (!m_pBmpBytes)
   {  m_nError  = SPLWND_ERRMEMALLOCFAIL; goto exit; }
   
  // Now read the data bytes
  pByte =  m_pBmpBytes;
  do
  {                         
   // No of bytes read
   nBytesRead    = _lread(hBmpFile,pByte,MAX_READ);
   if (nBytesRead == HFILE_ERROR)
    {  m_nError  = SPLWND_ERRREADFILEFAILURE; goto exit; }
   pByte  +=  nBytesRead;
  } while (nBytesRead >= MAX_READ );
                                                      
  ::GlobalUnlock(m_hGlobal);
  ::LocalUnlock(m_hLocal);
  
  bRetCode = TRUE;
  
exit:
  // Free Allocated memory on error
  if (!bRetCode)
    FreeBmpMemory();
  // close the file  
  if (hBmpFile)
   _lclose(hBmpFile); 
   
  return bRetCode; 
} 

int CSplashWnd::FreeBmpMemory()
{
  if (m_hLocal)             
    { ::LocalUnlock(m_hLocal);::LocalFree(m_hLocal);m_pBmpInfo = NULL;m_hLocal = NULL;  }
     
  if (m_pBmpInfoHeader)
    { delete m_pBmpInfoHeader; m_pBmpInfoHeader = NULL ;} 
  
  if (m_hGlobal)
    { ::GlobalUnlock(m_hGlobal); ::GlobalFree(m_hGlobal); m_pBmpBytes = NULL;m_hGlobal = NULL; }  
  
  return TRUE;  
}

int CSplashWnd::ErrorExit(int nError)
{
  CleanUpObject();
  m_nError  = nError ? TRUE : m_nError;
  return nError ? nError : m_nError;
}

void CSplashWnd::CleanUpObject()
{
  if ( m_lpszBmpFileName )
   { delete [] m_lpszBmpFileName; m_lpszBmpFileName = NULL; }   
  if ( m_lpszWindowTitle )
   { delete [] m_lpszWindowTitle; m_lpszWindowTitle = NULL; } 
  if ( m_lpszRegInfo )
   { delete [] m_lpszRegInfo; m_lpszRegInfo = NULL;  }
  if ( m_lpszOrgName )
   { delete [] m_lpszOrgName; m_lpszOrgName = NULL;  }  
  if ( m_pSplashBitmap )
   { delete m_pSplashBitmap; m_pSplashBitmap = NULL; }    
  
}

int CSplashWnd::SplashCreate()
{   
  // register the window class                           
  LPCSTR lpszClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,0,(HBRUSH)::GetStockObject(WHITE_BRUSH));
  // Create the splash window
  if (!CWnd::CreateEx(0L /*WS_EX_TOPMOST*/,lpszClassName,m_lpszWindowTitle,WS_POPUP,0,0,360,310,NULL,NULL))
   return ErrorExit(SPLWND_ERRSPLWINDOWCREATEFAIL);
  // Get the current time
  m_dwCurrentTime = ::GetCurrentTime();
  // center window
  CenterWindow(); 
  // show it now
  ShowWindow(SW_SHOW);
  // Start timer 
  SetTimer(SPLASH_TIMER_ID,SPLASH_INTERVAL,NULL);
  return SPLWND_OK;
}

void CSplashWnd::OnPaint() 
{                      
    // device context for painting
	CPaintDC dc(this); 
	CRect crClientRect;
	GetClientRect(crClientRect);
	CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap *pOldBmp = memDC.SelectObject(m_pSplashBitmap);
    dc.StretchBlt(crL + 5,crT + 5,m_nWidth - 1,m_nHeight -1,&memDC,0,0,m_nWidth - 1 ,m_nHeight - 1, SRCCOPY);
    
    Draw3DFrame(&dc,crL,crT,crR,crB);
  
    if (m_lpszRegInfo || m_lpszOrgName )
    {
      CPen RegInfoPen(PS_SOLID,1,RGB(255,255,255));
      CPen *pOldGrayPen = dc.SelectObject(&RegInfoPen);
      dc.SelectObject(CFont::FromHandle((HFONT)::GetStockObject(SYSTEM_FONT)));
     // if (!m_b3DEnabled)
       // dc.SetROP2(R2_NOT);            
      dc.SetBkMode(TRANSPARENT);
      
      TEXTMETRIC tm;
      dc.GetTextMetrics(&tm);
      
      int xInfoText = crW/2;
      int yInfoText = crB - 2 * tm.tmHeight - 20; 
      int yOrgInfoText = yInfoText + tm.tmHeight + 3;
      int nRegLen   = lstrlen(m_lpszRegInfo);
      int nOrgLen   = lstrlen(m_lpszOrgName);
      
      CRect crInfoTextRect( xInfoText,yInfoText,xInfoText + 20,yInfoText + tm.tmHeight);
      CRect crOrgInfoRect(xInfoText ,yOrgInfoText, xInfoText + 20 ,yOrgInfoText + tm.tmHeight);
      
      dc.DrawText(m_lpszRegInfo ,nRegLen ,crInfoTextRect ,DT_CALCRECT);
      dc.DrawText(m_lpszOrgName ,nOrgLen ,crOrgInfoRect ,DT_CALCRECT);
      
      if (m_b3DEnabled)    
      {
       CRect crFrameRect(xInfoText - 7,yInfoText - 7,
         7 + max(crInfoTextRect.right,crOrgInfoRect.right),crOrgInfoRect.bottom + 7 );
       dc.Rectangle(crFrameRect);
       dc.FillRect(crFrameRect,CBrush::FromHandle((HBRUSH)::GetStockObject(LTGRAY_BRUSH)));
       Draw3DFrame(&dc,crFrameRect.left,crFrameRect.top,crFrameRect.right,crFrameRect.bottom);   
      } 
      dc.DrawText(m_lpszRegInfo ,nRegLen ,crInfoTextRect, DT_CENTER);
      dc.DrawText(m_lpszOrgName ,nOrgLen ,crOrgInfoRect ,DT_CENTER);
      dc.SelectObject(pOldGrayPen);
    }  
    // assuming the bitmap width and height is same as window width and height
    // paint the bitmap
    // Select the old bitmap
    dc.SelectObject(pOldBmp);     
    
}                                    

void CSplashWnd::Draw3DFrame(CDC *pDC,int nL,int nT,int nR,int nB)
{                         
    
    CPen BlackPen(PS_SOLID,2,RGB(0,0,0));
    CPen* pPen = pDC->SelectObject(&BlackPen);
    
    pDC->MoveTo(nR - 1 ,nT + 1);  pDC->LineTo(nL + 1 ,nT + 1);
    pDC->LineTo(nL + 1 ,nB - 1);  pDC->LineTo(nR - 1 ,nB - 1);
    pDC->LineTo(nR - 1 ,nT + 1);    
    
    pDC->SelectObject(pPen);           
    
    CPen WhitePen(PS_SOLID,2,RGB(225,225,225));
    CPen* pOldPen = pDC->SelectObject(&WhitePen);
    
    pDC->MoveTo(nL + 2 ,nB - 2);
    pDC->LineTo(nL + 2 ,nT + 2);  pDC->LineTo(nR - 2 ,nT + 2);
    pDC->MoveTo(nR - 3 ,nT + 3);  pDC->LineTo(nL + 3 ,nT + 3);
    pDC->LineTo(nL + 3 ,nB - 3);  pDC->MoveTo(nL + 4 ,nB - 4);
    pDC->LineTo(nL + 4 ,nT + 4);  pDC->LineTo(nR - 4 ,nT + 4);
    pDC->MoveTo(nR - 5 ,nT + 5);  pDC->LineTo(nL + 5 ,nT + 5);
    pDC->LineTo(nL + 5 ,nB - 5);
    
    CPen GrayPen(PS_SOLID,2,GetSysColor(COLOR_BTNSHADOW));
    CPen* pOldWhitePen = pDC->SelectObject(&GrayPen);
    
    pDC->LineTo(nR - 5 ,nB - 5);  pDC->LineTo(nR - 5 ,nT + 5);
    pDC->MoveTo(nR - 4 ,nT + 4);  pDC->LineTo(nR - 4 ,nB - 4);
    pDC->LineTo(nL + 4 ,nB - 4);  pDC->MoveTo(nL + 3 ,nB - 3);
    pDC->LineTo(nR - 3 ,nB -3);   pDC->LineTo(nR - 3 ,nT + 3);
    pDC->MoveTo(nR - 2 ,nT + 2);  pDC->LineTo(nR - 2 ,nB - 2);
    pDC->LineTo(nL + 2 ,nB - 2);  //pDC->MoveTo(nL + 1 ,nB - 1);
    //pDC->LineTo(nR - 1 ,nB - 1);  pDC->LineTo(nR - 1 ,nT + 1);
    
    pDC->SelectObject(pOldWhitePen);
    pDC->SelectObject(pOldPen);
}


void CSplashWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == SPLASH_TIMER_ID)
	{
	  if (((double)(::GetCurrentTime() - m_dwCurrentTime) * 0.001) > m_nSplashSec )	  
	  {   KillTimer(nIDEvent);   DestroyWindow(); return; }     
	}
	CWnd::OnTimer(nIDEvent);
}        

void CSplashWnd::PostNcDestroy()
{                   
  m_bSplashOver = TRUE;
  
  delete this;
}
/*
void* CSplashWnd::operator new(size_t nSize)
{ 
  bFromHeap = TRUE;
  
  return ::operator new(nSize);       
}                    

void CSplashWnd::operator delete(void* p)
{
   bFromHeap = FALSE;
   
  ::operator delete(p);
}
*/

void CSplashWnd::Idle()
{
  MSG msg;
  while( ::PeekMessage(&msg, NULL, 0,0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
}

inline BOOL CSplashWnd::HeapSplashObject()
{   return m_bFromHeap;  }