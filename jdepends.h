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

#if !defined(AFX_JDEPENDS_H__F92BE3D4_2E79_11D3_A73E_90FA46C10121__INCLUDED_)
#define AFX_JDEPENDS_H__F92BE3D4_2E79_11D3_A73E_90FA46C10121__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CJdependsApp:
// See jdepends.cpp for the implementation of this class
//


enum {
   PMID_SHOW1 =2345,
   PMID_SHOW2 =2346,
   PMID_CHGCLR =2347,
   PMID_CPROP  = 2348,
   PMID_CODEDIST = 2349,
   PMID_CALLGRAPH= 2350,
   PMID_FULLPATH = 2351,
   PMID_LOADCLASS=2352
};

// global functions
HTREEITEM  InsertTreeItem(CTreeCtrl&, HTREEITEM hParent, PSTR pszText, int lparam, int iImage );

//  CJdependsApp
class CJdependsApp : public CWinApp
{


  CMultiDocTemplate *m_pDefaultTemplate, *m_pMiniTemplate, 
                    *m_pJarDefaultTemplate,*m_pJarClassDefaultTemplate ;
  CString m_csClassFile;
  
public:
	CJdependsApp();
	CMultiDocTemplate * GetClassTemplate() { return m_pDefaultTemplate; }
  CMultiDocTemplate * GetMiniTemplate() { return m_pMiniTemplate; }
  CMultiDocTemplate * GetJarTemplate() { return m_pJarDefaultTemplate; }
  CMultiDocTemplate * GetJarClassTemplate() { return m_pJarClassDefaultTemplate; }
  
  BOOL OnOpenRecentFile(UINT nID);

  DWORD GetDSFrameStyle() {
     return ( WS_CHILD | WS_VISIBLE | WS_BORDER
                       | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION
                       | WS_SYSMENU| WS_THICKFRAME );
  }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJdependsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CJdependsApp)
	afx_msg void OnAppAbout();
	afx_msg void OnClassFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
  afx_msg void OnOptionsSetclasspath();
  afx_msg void OnOptionsSetupclasspath();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JDEPENDS_H__F92BE3D4_2E79_11D3_A73E_90FA46C10121__INCLUDED_)
