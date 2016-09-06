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

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd window


class CSplashWnd : public CWnd
{          
//Variables                  
private:  
	LONG m_nSplashSec;  
 	BOOL m_bError;
 	BOOL m_bFromHeap;
 	BOOL m_b3DEnabled;
 	int m_nError;       
 	int m_nWidth;
 	int m_nHeight;
 	BITMAPINFOHEADER *m_pBmpInfoHeader;
 	BITMAPINFO* m_pBmpInfo;
 	LPBYTE m_pBmpBytes;
 	HBITMAP  m_hSplashBitmap; 
 	HLOCAL m_hLocal;
 	HGLOBAL m_hGlobal;
 	DWORD m_dwCurrentTime;
 	CBitmap* m_pSplashBitmap;
 	LPSTR   m_lpszBmpFileName;
 	LPSTR   m_lpszWindowTitle;
 	LPSTR   m_lpszRegInfo;
 	LPSTR   m_lpszOrgName;
 	BOOL& m_bSplashOver;
 	
// Helpers 
private:	
    BOOL AllocBitmap(); 
    int SplashCreate();
    int ErrorExit(int nError = 0);
    BOOL FreeBmpMemory();
    int ReadBitmapFile();
    void CleanUpObject();     
    BOOL HeapSplashObject();
    void Draw3DFrame(CDC *pDC,int nL,int nT,int nR,int nB);
    
// Construction
public:
	CSplashWnd(BOOL&);
	int Display(UINT nID);
	int Display(LPCSTR lpszFileName);
  int Display(HBITMAP hBitmap);
  void Idle();
  void Init(LONG, LPCSTR, LPCSTR lpRegInfo = NULL,LPCSTR OrgName = NULL,BOOL b3D = FALSE);
    
// overridable    
public :
	virtual ~CSplashWnd();
//	void* operator new(size_t nSize);
//	void  operator delete(void* p);
// overridable
protected:
    virtual void PostNcDestroy();	
// Generated message map functions    
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// Error codes
enum{
     SPLWND_ERRMEMALLOCFAIL = 0xF1, // 241
     SPLWND_ERRFILENOTFOUND,        // 242 
     SPLWND_ERRBMPLOADFAIL,         // 243
     SPLWND_ERRBMPFILEOPENERROR,    // 244
     SPLWND_ERRINVALIDBMPFILE,      // 245
     SPLWND_ERRREADFILEFAILURE,     // 246 
     SPLWND_ERRRLECMPRSSNOTSUPPORTED, // 247
     SPLWND_ERR24BITNOTSUPPORTED,     // 248
     SPLWND_ERRSPLWINDOWCREATEFAIL,   // 249
     SPLWND_ERROBJNOTCREATEDINHEAP    // 250
    };      
      
// OK return code      
#define SPLWND_OK TRUE      
#define SPLASH_TIMER_ID 10
#define SPLASH_INTERVAL 1000

/////////////////////////////////////////////////////////////////////////////
