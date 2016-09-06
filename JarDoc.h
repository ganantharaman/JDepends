#pragma once


// CJarDoc document

class CJarDoc : public CDocument
{
	DECLARE_DYNCREATE(CJarDoc)
	
	CString m_csPath;

public:
	CJarDoc();
	virtual ~CJarDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

 virtual CString& GetPathName();

protected:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	
	

	DECLARE_MESSAGE_MAP()
	
	
};
