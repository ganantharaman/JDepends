// JarDialog.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "JarDialog.h"

extern "C" int file_cback(char * file, void * data);


typedef int (*LIST_CLASSES_CALLBACK)(char * file, void * info);
extern "C" 
int list_classes_in_jar(char * jar, LIST_CLASSES_CALLBACK cback) ;

CList<CString, CString&> CJarDialog::m_files;


// CJarDialog dialog

IMPLEMENT_DYNAMIC(CJarDialog, CDialog)
CJarDialog::CJarDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CJarDialog::IDD, pParent)
{
}

CJarDialog::~CJarDialog()
{
}

void CJarDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLASSLIST, m_classList);
}


BEGIN_MESSAGE_MAP(CJarDialog, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_CLASSLIST, OnLbnSelchangeClasslist)
	ON_LBN_DBLCLK(IDC_CLASSLIST, OnLbnDblclkClasslist)
END_MESSAGE_MAP()


// CJarDialog message handlers

void CJarDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CJarDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_classList.GetText(m_classList.GetCurSel(), m_csClass);
	
	AfxMessageBox(m_csClass);

	OnOK();
}

extern "C"
int file_cback(char * filename, void *test) {
  CString cs(filename);
  CJarDialog::m_files.AddTail(cs);
  return 0;
}

BOOL CJarDialog::OnInitDialog() {

	BOOL b = CDialog::OnInitDialog();
    m_files.RemoveAll();

	list_classes_in_jar((char *)(LPCTSTR)m_csZipFile, file_cback);

	POSITION pos = m_files.GetHeadPosition();
	for (int i=0;i < m_files.GetCount();i++) {
      m_classList.AddString( (LPCSTR) m_files.GetNext(pos));
	}

	return b;
}

void CJarDialog::OnLbnSelchangeClasslist()
{
	// TODO: Add your control notification handler code here
}

void CJarDialog::OnLbnDblclkClasslist()
{
	OnBnClickedOk();
	// TODO: Add your control notification handler code here
}
