#pragma once
#include "afxwin.h"


// CJarDialog dialog

class CJarDialog : public CDialog
{
	DECLARE_DYNAMIC(CJarDialog)

public:
	CJarDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CJarDialog();
 
// Dialog Data
	enum { IDD = IDD_CLASSDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// Classes from jar
	CListBox m_classList;
	static CList<CString, CString&> m_files;
	CString m_csClass;
	CString m_csZipFile;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnSelchangeClasslist();
	afx_msg void OnLbnDblclkClasslist();
};
