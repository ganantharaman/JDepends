// JarDoc.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "JarDoc.h"
#include "jarlistview.h"
#include "jarclasslistview.h"

extern UINT DM_VIEWUPDATE;
// CJarDoc

IMPLEMENT_DYNCREATE(CJarDoc, CDocument)



CJarDoc::CJarDoc()
{
}

BOOL CJarDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

BOOL CJarDoc::OnOpenDocument(LPCTSTR lpszPathName) {
  m_csPath = lpszPathName;
  
  POSITION pos = GetFirstViewPosition(); 

  // Iterate thro' all views and send DM_VIEWUPDATE message
  while (pos != NULL) {
     CView* pView = GetNextView(pos);
     if ( pView->IsKindOf(RUNTIME_CLASS( CJarListView )) 
          || pView->IsKindOf(RUNTIME_CLASS( CJarClassListView ))){
          pView->PostMessage(DM_VIEWUPDATE);
     }
  }   
  
  return TRUE;
}

CString& CJarDoc::GetPathName() {
  return m_csPath;
}

CJarDoc::~CJarDoc()
{
}


BEGIN_MESSAGE_MAP(CJarDoc, CDocument)
END_MESSAGE_MAP()


// CJarDoc diagnostics

#ifdef _DEBUG
void CJarDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CJarDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CJarDoc serialization

void CJarDoc::Serialize(CArchive& ar)
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


// CJarDoc commands
