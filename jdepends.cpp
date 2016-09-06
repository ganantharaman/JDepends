/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*  This code and any software associated is covered by                     */
/*  GNU Lesser Public License (LGPL)                                        */
/*                                                                          */
/*  Author: Gopal Ananthraman                                               */
/*                                                                          */
/*  Desc  :                                                                 */
/*                                                                          */
/****************************************************************************/

#include "stdafx.h"
#include "jdepends.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "NormalChildFrm.h"
#include "jdependsDoc.h"
#include "JarDoc.h"
#include "JarListView.h"
#include "JarClassListView.h"
#include "jdependsView.h"
#include "DClassView.h"
#include "titlewnd.h"
#include "pieview.h"
#include "splshwnd.h"
#include "commonframe.h"
#include "classpathdialog.h"
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char BASED_CODE g_szFilter[] =
 "Class Files (*.class)|*.class|Jar Files (*.jar)|*.jar|All Files (*.*)|*.*||";

UINT DM_VIEWUPDATE= RegisterWindowMessage("dm_ViewUpdate");


/////////////////////////////////////////////////////////////////////////////
// CJdependsApp

BEGIN_MESSAGE_MAP(CJdependsApp, CWinApp)
	//{{AFX_MSG_MAP(CJdependsApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnClassFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
  ON_COMMAND(ID_OPTIONS_SETCLASSPATH, OnOptionsSetclasspath)
  ON_COMMAND(ID_OPTIONS_SETUPCLASSPATH, OnOptionsSetupclasspath)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJdependsApp construction

CJdependsApp::CJdependsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CJdependsApp object

CJdependsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CJdependsApp initialization

BOOL CJdependsApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.


	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("JDepends"));
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	m_pDefaultTemplate = new CMultiDocTemplate(
		IDR_JDEPENTYPE,
		RUNTIME_CLASS(CJdependsDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CDClassView));
	AddDocTemplate(m_pDefaultTemplate);

    m_pMiniTemplate = new CMultiDocTemplate(
		IDR_JDEPENTYPE,
		RUNTIME_CLASS(CJdependsDoc),
		RUNTIME_CLASS(CCommonFrame), // custom MDI child frame
		RUNTIME_CLASS(CPieView));
	AddDocTemplate(m_pMiniTemplate);
	
	m_pJarDefaultTemplate = new CMultiDocTemplate(
		IDR_JDEPENTYPE,
		RUNTIME_CLASS(CJarDoc),
		RUNTIME_CLASS(CNormalChildFrm), // custom MDI child frame
		RUNTIME_CLASS(CJarListView));
	AddDocTemplate(m_pJarDefaultTemplate);
	
	m_pJarClassDefaultTemplate = new CMultiDocTemplate(
		IDR_JDEPENTYPE,
		RUNTIME_CLASS(CJarDoc),
		RUNTIME_CLASS(CNormalChildFrm), // custom MDI child frame
		RUNTIME_CLASS(CJarClassListView));
	AddDocTemplate(m_pJarClassDefaultTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
   //  pMainFrame->CreateTitleWnd();

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

  // Do not display the startup MDI child window
  cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

  BOOL bSplashOver = FALSE;

	// The main window has been initialized, so show and update it.
  CSplashWnd * pSplashWnd = new CSplashWnd(bSplashOver);
  pSplashWnd->Init(4, "JDepends", "Registered To: Gopal", "JDepends", TRUE); 

  int ret = pSplashWnd->Display(IDB_BITMAP4);
  if ( ret == SPLWND_OK )
  {
    int init = 0;
    // Init ClassInfo
    while ( !bSplashOver) {
      pSplashWnd->Idle();
      if ( !init ){ CClassInfo::Init(); init = 1; }
    }

  }else { 
         CClassInfo::Init(); 
  }


  pMainFrame->CenterWindow();
  pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// App command to run the dialog
void CJdependsApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
	
}


BOOL CJdependsApp::OnOpenRecentFile(UINT nID) {
  ASSERT_VALID(this);
	ASSERT(m_pRecentFileList != NULL);

	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

	TRACE(traceAppMsg, 0, _T("MRU: open file (%d) '%s'.\n"), (nIndex) + 1,
			(LPCTSTR)(*m_pRecentFileList)[nIndex]);

	CString csFile ((*m_pRecentFileList)[nIndex]);
	CDocument * pDoc = NULL;
	if  ( !csFile.IsEmpty()) {
    if(csFile.Right(4) == ".jar") {
      pDoc = m_pJarDefaultTemplate->OpenDocumentFile( csFile );
    } else {
      pDoc = m_pDefaultTemplate->OpenDocumentFile( csFile );
    }
  }
  
  if(pDoc == NULL) {
    m_pRecentFileList->Remove(nIndex);
  }


	return (pDoc != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CJdependsApp commands

void CJdependsApp::OnClassFileOpen() 
{
	// TODO: Add your command handler code here
	CFileDialog fd(TRUE, ".class", NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		g_szFilter);
	fd.m_ofn.lpstrTitle = "Open Class File";
	fd.DoModal();
  m_csClassFile = fd.GetPathName();
  if  ( !m_csClassFile.IsEmpty()) {
    if(m_csClassFile.Right(4) == ".jar") {
      m_pJarDefaultTemplate->OpenDocumentFile( m_csClassFile );
    } else {
      m_pDefaultTemplate->OpenDocumentFile( m_csClassFile );
    }
  }
}

int CJdependsApp::ExitInstance() 
{
	CClassInfo::Exit();
	return CWinApp::ExitInstance();
}

// Global
HTREEITEM  InsertTreeItem(CTreeCtrl& ctrl, HTREEITEM hParent, PSTR pszText, int lparam, int iImage )
{
  if ( !pszText || !*pszText ) return NULL;

  TV_INSERTSTRUCT ts;
  ts.hParent = hParent;
  ts.hInsertAfter = TVI_LAST;
	memset ( &ts.item, 0, sizeof( TV_ITEM ));
	ts.item.mask |= ( TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM);
	ts.item.pszText = pszText;
  ts.item.iImage =  iImage;
  ts.item.iSelectedImage = iImage;
	ts.item.cchTextMax = strlen( ts.item.pszText );
  ts.item.lParam = lparam;

	return ctrl.InsertItem( &ts );
}

void CJdependsApp::OnOptionsSetclasspath()
{
  CClasspathDialog ccpDialog;
  ccpDialog.DoModal();
 
}

void CJdependsApp::OnOptionsSetupclasspath()
{
  CClasspathDialog ccpDialog;
  ccpDialog.DoModal();
  // TODO: Add your command handler code here
}
