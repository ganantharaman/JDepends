// NormalChildFrm.cpp : implementation file
//

#include "stdafx.h"
#include "jdepends.h"
#include "NormalChildFrm.h"


// CNormalChildFrm

IMPLEMENT_DYNCREATE(CNormalChildFrm, CMDIChildWnd)

CNormalChildFrm::CNormalChildFrm()
{
}

CNormalChildFrm::~CNormalChildFrm()
{
}


BEGIN_MESSAGE_MAP(CNormalChildFrm, CMDIChildWnd)
END_MESSAGE_MAP()


BOOL CNormalChildFrm::PreCreateWindow(CREATESTRUCT &cs)
{
    // Do default processing.
    if (CMDIChildWnd::PreCreateWindow(cs)==0) return
        FALSE;
     
   cs.style &= ~(LONG)FWS_ADDTOTITLE;
   return TRUE;
}



// CNormalChildFrm message handlers
