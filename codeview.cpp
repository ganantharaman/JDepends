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
#include "codeview.h"
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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCodeView, CFormView)
ON_REGISTERED_MESSAGE(DM_VIEWUPDATE, OnDmViewUpdate)
	//{{AFX_MSG_MAP(CCodeView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
  ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodeView diagnostics
LONG CCodeView::OnDmViewUpdate( UINT wParam, LONG )
{
  CJdependsDoc * pDoc= (CJdependsDoc *)GetDocument();
  CClassInfo * pInfo = pDoc->GetRoot();
  CString csText;
  
  int index = pDoc->GetSelMethodIndex();
  if ( index >= 0 && index <  pInfo->GetMethodsCount() )
  {
    // Try getting the selected method name 
    csText = pDoc->GetSelMethodName();
    // Initially this might be NULL
    // Because the methods view may not hav been initialized
    // So try to construct one 
    if (csText.IsEmpty()) {
      pInfo->GetMethodAttributeNames( index, csText );
      VERIFY( pInfo->GetMethodFullName( index, csText) );
      // Add attributes
    }
    // Get exceptions thrown ,if any
    if ( pInfo->GetMethodExceptionsCount( index )) {
      csText += " throws\r\n";
      pInfo->GetMethodExceptions( index, csText );
    }
    csText += "\r\n";
    // Finally get the byte codes
    VERIFY( pInfo->GetMethodByteCodes( index, csText ));

    // And update the window text
    m_ByteCodeEdit.SetWindowText("");
    m_ByteCodeEdit.SetWindowText( csText );
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

void CCodeView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

  CRect rect;
  GetWindowRect(&rect);
  ScreenToClient(&rect);

  m_ByteCodeEdit.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | 
                        ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_LEFT
                        , rect, this, 1324);
}



void CCodeView::OnEnChangeEdit1()
{
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CFormView::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
}
