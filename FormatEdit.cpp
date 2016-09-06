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
#include "FormatEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormatEdit

CFormatEdit::CFormatEdit()
{
  m_pOldFont = NULL;
  m_clrText = RGB( 0, 0, 255 );    m_clrBkgnd = RGB( 255, 255, 255 );
  m_brBkgnd.CreateSolidBrush( m_clrBkgnd );
  // Give the height in point units LOGFONT
  memset(&m_logfont, 0, sizeof m_logfont);
	m_logfont.lfHeight = -12;              
	m_logfont.lfWeight = FW_NORMAL;
	static char BASED_CODE szArial[] = "Courier New";
	lstrcpy(m_logfont.lfFaceName, szArial);
	m_logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_logfont.lfQuality = PROOF_QUALITY;
	m_logfont.lfPitchAndFamily = FF_MODERN | FIXED_PITCH;

  m_font.CreateFontIndirect(&m_logfont);
}

CFormatEdit::~CFormatEdit()
{
}


BEGIN_MESSAGE_MAP(CFormatEdit, CEdit)
	//{{AFX_MSG_MAP(CFormatEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormatEdit message handlers

HBRUSH CFormatEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
  pDC->SetTextColor( m_clrText );
  pDC->SetBkColor( m_clrBkgnd );    // text bkgnd

  return m_brBkgnd;                // ctl bkgnd

}

int CFormatEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

  SetFont( &m_font );

	return 0;
}
