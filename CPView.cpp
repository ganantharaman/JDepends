// CPView.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "Codeview.h"
#include "jdependsdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT DM_VIEWUPDATE;

/////////////////////////////////////////////////////////////////////////////
// CCodeView

IMPLEMENT_DYNCREATE(CCodeView, CFormView)

CCodeView::CCodeView()
	: CFormView(CCodeView::IDD)
{
	//{{AFX_DATA_INIT(CCodeView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCodeView::~CCodeView()
{
}



void CCodeView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodeView)
	DDX_Control(pDX, IDC_FORMATEDIT, m_ByteCodeEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCodeView, CFormView)
ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate)
	//{{AFX_MSG_MAP(CCodeView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodeView diagnostics
LONG CCodeView::OnDmViewUpdate( UINT wParam, LONG )
{
  CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
  CClassInfo * pInfo = pDoc->GetRoot();
  char buffer[512 + 1];
  char * bytes = buffer, * str = NULL;
  void * tmp = NULL;
  int i = 0, len;

  int index = pDoc->GetSelMethodIndex();
  if ( index >= 0 && index <  pInfo->GetMethodsCount() )
  {
    str = pDoc->GetSelMethodName();
    if (!str ) {
      len = JD_BUFSIZE;
      str = new char[len];
      if ( !pInfo->GetMethods( index, str, len) )
      {
        delete [] str;
        str = new char[len + 1];
        ASSERT( pInfo->GetMethods( index, str, len) );
      }
    }
    
    len = 512;
    bytes = buffer;
    if ( !pInfo->GetMethodByteCodes( index, str, bytes, len ) )
    {
      bytes = new char[len + 1];
      ASSERT( pInfo->GetMethodByteCodes( index, str, bytes, len ) );
    }
    m_ByteCodeEdit.SetWindowText("");
    m_ByteCodeEdit.SetWindowText( bytes );
    if ( bytes != buffer ) delete [] bytes;
  }

  return TRUE;
}

#ifdef _DEBUG
void CCodeView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCodeView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCodeView message handlers

void CCodeView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

  if ( m_ByteCodeEdit.m_hWnd )
     m_ByteCodeEdit.MoveWindow( 0, 0, cx, cy );
	
	// TODO: Add your message handler code here
	
}
