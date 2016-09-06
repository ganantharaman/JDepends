// ByteCodeView.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "ByteCodeView.h"
#include "jdependsdoc.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT DM_VIEWUPDATE;
/////////////////////////////////////////////////////////////////////////////
// CByteCodeView

IMPLEMENT_DYNCREATE(CByteCodeView, CView)

CByteCodeView::CByteCodeView()
{
}

CByteCodeView::~CByteCodeView()
{
}


BEGIN_MESSAGE_MAP(CByteCodeView, CView)
  ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate)
	//{{AFX_MSG_MAP(CByteCodeView)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CByteCodeView drawing


LONG CByteCodeView::OnDmViewUpdate( UINT wParam, LONG )
{
  DisplayCode();
	return 1UL;
}

char buffer[512 + 1];
  char * bytes = buffer, * str = NULL;

BOOL CByteCodeView::DisplayCode()
{
  CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
  CClassInfo * pInfo = pDoc->GetRoot();
  //char buffer[512 + 1];
  //char * bytes = buffer, * str = NULL;
  void * tmp = NULL;
  int i = 0, len;

  int index = pDoc->GetSelMethodIndex();

  if ( index >= 0 && index <  pInfo->GetMethodsCount() )
  {
    str = pDoc->GetSelMethodName();
    if (!str ) {
      len = JD_BUFSIZE;
      str = new char[len];
      if ( !pInfo->GetMethodFullName( index, str, len) )
      {
        delete [] str;
        str = new char[len + 1];
        ASSERT( pInfo->GetMethods( index, str, len) );
      }
    }
    
    len = 512;
    if ( bytes != buffer ) delete [] bytes;
    bytes = buffer;
    if ( !pInfo->GetMethodByteCodes( index, str, bytes, len ) )
    {
      bytes = new char[len + 1];
      ASSERT( pInfo->GetMethodByteCodes( index, str, bytes, len ) );
    }
     Invalidate();
  //  m_ByteCodeEdit.SetWindowText("");
  //  m_ByteCodeEdit.SetWindowText( bytes );
   
  }

  return TRUE;
}


void CByteCodeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

  DrawCode( pDC, bytes );
	// TODO: add draw code here
}

BOOL CByteCodeView::DrawCode( CDC * pDC, char * bytes )
{
  TEXTMETRIC tm;
  pDC->GetTextMetrics( &tm );
  void * ptr = NULL;
  char * p;
  int y = 0;
  while ( p = str_tok( bytes, "\r\n", &ptr)) {
    if ( !p ) break;
    if ( *p == '\n' ) p++;
    pDC->TextOut( 0,y, p );
    y += (tm.tmHeight + tm.tmExternalLeading);
  }

  return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CByteCodeView diagnostics

#ifdef _DEBUG
void CByteCodeView::AssertValid() const
{
	CView::AssertValid();
}

void CByteCodeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CByteCodeView message handlers

int CByteCodeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
 // CRect rect;
 // GetWindowRect( &rect );

//  m_ByteCodeEdit.Create(WS_CHILD|WS_VISIBLE|WS_DISABLED, rect, this, 12345);
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
