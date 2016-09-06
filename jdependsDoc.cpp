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

#include "jdependsDoc.h"
#include "classinfo.h"
#include "dclassview.h"
#include "mfview.h"
#include "codeview.h"
#include "jardialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT DM_VIEWUPDATE;

extern CString classpathText;



/////////////////////////////////////////////////////////////////////////////
// CJdependsDoc

IMPLEMENT_DYNCREATE(CJdependsDoc, CDocument)

BEGIN_MESSAGE_MAP(CJdependsDoc, CDocument)
	//{{AFX_MSG_MAP(CJdependsDoc)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_Menu163, OnOptions)
END_MESSAGE_MAP()

CString CJdependsDoc::m_csCls;

/////////////////////////////////////////////////////////////////////////////
// CJdependsDoc construction/destruction

CJdependsDoc::CJdependsDoc()
{
  m_pRoot = NULL;
  m_nMethodIndex = 0;
  m_pCodeView = NULL;
	// TODO: add one-time construction code here
}

CJdependsDoc::~CJdependsDoc()
{
  if ( m_pRoot ) delete m_pRoot;
}

BOOL CJdependsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CJdependsDoc serialization

void CJdependsDoc::Serialize(CArchive& ar)
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
// CJdependsDoc diagnostics

#ifdef _DEBUG
void CJdependsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CJdependsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJdependsDoc commands


BOOL CJdependsDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
   if (!CDocument::OnOpenDocument(lpszPathName))
	 	return FALSE; 
	 	
	 	CString cs(lpszPathName);
	 	if(!cs.IsEmpty()) {
	 	  int index = cs.ReverseFind('\\');
	 	  if(index == -1) {
	 	    index = cs.ReverseFind('/');
	 	  }
	 	  
	 	  if(index > 0) {
	      CString tmp = cs.Left(index);
	      classpathText = tmp + ";" + CString(getenv("CLASSPATH")) 
	         + ";" + classpathText;
	      tmp = "CLASSPATH=" + classpathText;
	      _putenv(tmp.GetBuffer(tmp.GetLength()));
	      tmp = getenv("CLASSPATH");
	    }
	  }
	  
 
   if(m_csCls.IsEmpty()) {
	    CJarDialog cjd;
	    if(cs.Right(4) == _T(".jar")) {
    	   
	    //	 cjd.CenterWindow();
		    cjd.m_csZipFile = cs;
		    cjd.DoModal();
	    }
	    return LoadClassFromJar(cjd.m_csClass, lpszPathName);
   } else {
     CString csCls = m_csCls;
	   m_csCls.Empty();
     return LoadClassFromJar(csCls, lpszPathName);  
   }
   
  

   
}

BOOL CJdependsDoc::LoadClassFromJar(CString& csClass, LPCTSTR lpszPathName)
{
   // Construct new class info with path information
   CString cs(lpszPathName);
   if((cs.Right(4) == _T(".jar")) && 
      !csClass.IsEmpty()) {
     m_pRoot = new CClassInfo((PSTR)(LPCTSTR)csClass, 
		 (char *)(LPCTSTR)csClass, 1);
	   m_pRoot->SetJarName((PSTR)lpszPathName);
   } else {
     m_pRoot = new CClassInfo((char *)lpszPathName,
		                      (char *)lpszPathName);
   }

   VERIFY( m_pRoot != NULL );
   // Actually Load the class
   if ( m_pRoot->LoadClass() ) {
     POSITION pos = GetFirstViewPosition(); 

     // Iterate thro' all views and send DM_VIEWUPDATE message
	   while (pos != NULL) {
       CView* pView = GetNextView(pos);
       if ( pView->IsKindOf(RUNTIME_CLASS( CCodeView )))
          m_pCodeView = pView; 
       pView->PostMessage(DM_VIEWUPDATE);
     }   
     // Job done.
     return TRUE;
  }
 
  // Indicate failure
  MessageBox(NULL, "LoadClass Failed", "JDepends", MB_OK);
  return FALSE;
}




void CJdependsDoc::SetSelMethodName( CString& name, BOOL bRefresh )
{ 
    m_csMethodName = name;
    if ( m_pCodeView && bRefresh ) 
        m_pCodeView->SendMessage( DM_VIEWUPDATE);
}

void CJdependsDoc::SetRedrawFlag( BOOL bVal )
{
  /*    POSITION pos = GetFirstViewPosition(); 

    while (pos != NULL) {
       CView* pView = GetNextView(pos);
       pView->SetRedraw( bVal );
     }   */
}

void CJdependsDoc::OnOptions()
{
	// TODO: Add your command handler code here
}
