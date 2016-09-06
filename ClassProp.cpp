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
#include "ClassProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClassProp dialog


CClassProp::CClassProp(CClassInfo * pInfo, CWnd* pParent /*=NULL*/)
	: CDialog(CClassProp::IDD, pParent)
{
  if (pInfo) {
     CClassInfo::IncreaseRefCount( pInfo );
     m_pInfo = pInfo;
  }
  m_clrBkgnd = RGB(255,255,255);
  m_brBkgnd.CreateSolidBrush( m_clrBkgnd );
	//{{AFX_DATA_INIT(CClassProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CClassProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClassProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClassProp, CDialog)
	//{{AFX_MSG_MAP(CClassProp)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassProp message handlers

BOOL CClassProp::OnInitDialog() 
{
	CDialog::OnInitDialog();

  SetDlgItemText( IDC_CLASSNAME, m_pInfo->GetClassName());
  CClassInfo * pChild = m_pInfo->GetChild();
  if ( pChild )
   SetDlgItemText( IDC_BASECLASS, pChild->GetClassName());
  else 
   SetDlgItemText( IDC_BASECLASS, "-Nil-");
  char tmp[64];
  sprintf( tmp, "%d", m_pInfo->GetMethodsCount());
  SetDlgItemText( IDC_METHODSCOUNT,tmp );
  sprintf( tmp, "%d", m_pInfo->GetFieldsCount());
  SetDlgItemText( IDC_FIELDSCOUNT, tmp );
  int iCount = m_pInfo->GetInterfacesCount();
  CListBox * pList = (CListBox *)GetDlgItem(IDC_INTERFACELIST);
  if ( !iCount && pList ) {
     pList->AddString("-Nil-");
  }
   
  if ( pList ) {
    for ( int i = 0; i < iCount; i++ )
      pList->AddString( m_pInfo->GetInterfaceName(i));
  }
  
  CString cs;
  m_pInfo->GetCommonAttributeNames(m_pInfo->GetClassAttributes(), cs);
  SetDlgItemText(IDC_CLASSFLAGS,cs);
  SetDlgItemText(IDC_SOURCEFILE,
    m_pInfo->GetSourceFileName());

	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CClassProp::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

  if ( nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX ) {
    pDC->SetTextColor( RGB(0,128,0));
    pDC->SetBkColor(RGB(255,255,255));
  } else if ( nCtlColor == CTLCOLOR_STATIC) {
    pDC->SetTextColor( RGB(255,0,0));
    pDC->SetBkColor(RGB(255,255,255));
  }
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return m_brBkgnd;
}
