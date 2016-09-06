#pragma once


// CClasspathDialog dialog

class CClasspathDialog : public CDialog
{
	DECLARE_DYNAMIC(CClasspathDialog)

public:
	CClasspathDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClasspathDialog();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_CLASSPATHDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedBrowsebutton();
  afx_msg void OnBnClickedOk();
};
