// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__6158AF49_8BFE_4CD1_B623_8778ED91BC76__INCLUDED_)
#define AFX_MAINFRM_H__6158AF49_8BFE_4CD1_B623_8778ED91BC76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InterfaceView.h"
#include "../YATH/clientclasses.h"
#include "../YATH/commandparser.h"


class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void SubmitCommand( CString Command);
	void MakeTest( );

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	// CDialogBar  m_wndDlgBar;
	// CCommandDialog m_wndDlgBar;
	CInterfaceView  m_wndView;
	yathCConnectionManager m_Connection;
	yathCCommandParser m_CommandParser;

	// CEdit		m_wndView;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnUpdateConfigbttn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConnectbttn(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnConnectbttn();
	afx_msg void OnConfigbttn();
	afx_msg void OnClearoutputbttn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__6158AF49_8BFE_4CD1_B623_8778ED91BC76__INCLUDED_)
