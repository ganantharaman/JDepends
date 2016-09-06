#pragma once


// CJarListView view

class CJarListView : public CListView
{
	DECLARE_DYNCREATE(CJarListView)

protected:
	CJarListView();           // protected constructor used by dynamic creation
	virtual ~CJarListView();
	LONG OnDmViewUpdate( UINT, LONG );
	int  InsertListColumn( int column, int width, char * text );
	int  InsertListItem(int index, int entry, char * text );
	void GetListItem(int index, CString& ref);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	
	//{{AFX_MSG(CJarListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
};


