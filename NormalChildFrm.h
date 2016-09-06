#pragma once


// CNormalChildFrm frame

class CNormalChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CNormalChildFrm)
protected:
	CNormalChildFrm();           // protected constructor used by dynamic creation
	virtual ~CNormalChildFrm();
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
	DECLARE_MESSAGE_MAP()
};


