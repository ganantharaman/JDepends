// ClasspathDialog.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "ClasspathDialog.h"
#include "folderdialog.h"

//static char BASED_CODE g_szFolderFilter[] =
 //"Class Files (*.class)|*.class|Jar Files (*.jar)|*.jar|All Files (*.*)|*.*||";

CString classpathText("");

// CClasspathDialog dialog

IMPLEMENT_DYNAMIC(CClasspathDialog, CDialog)
CClasspathDialog::CClasspathDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CClasspathDialog::IDD, pParent)
{
}

CClasspathDialog::~CClasspathDialog()
{
}

void CClasspathDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CClasspathDialog, CDialog)
  ON_BN_CLICKED(IDC_BROWSEBUTTON, OnBnClickedBrowsebutton)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CClasspathDialog::OnInitDialog() {
  BOOL val = CDialog::OnInitDialog();
  CWnd * pEdit = this->GetDlgItem(IDC_CPEDITBOX);  
  pEdit->SetWindowText(classpathText);
  
  return val;
}


// CClasspathDialog message handlers

void CClasspathDialog::OnBnClickedBrowsebutton()
{
 CString path;
 CFolderDialog fd(&path);
 fd.DoModal();
 
 CWnd * pEdit = this->GetDlgItem(IDC_CPEDITBOX);
 CString csText;
 pEdit->GetWindowText(csText);
 if(csText.IsEmpty()) {
    pEdit->SetWindowText(path);
 } else {
   csText += ";" + path;
   pEdit->SetWindowText(csText);
 }
  
  // TODO: Add your control notification handler code here
}

void CClasspathDialog::OnBnClickedOk()
{
  // TODO: Add your control notification handler code here
  OnOK();
  
  CWnd * pEdit = this->GetDlgItem(IDC_CPEDITBOX);
  pEdit->GetWindowText(classpathText);
}
