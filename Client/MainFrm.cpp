// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "YATHClient.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../YATH/transactions.h"
#include "InterfaceView.h"
#include "connectdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(ID_CONFIGBTTN, OnUpdateConfigbttn)
	ON_UPDATE_COMMAND_UI(ID_CONNECTBTTN, OnUpdateConnectbttn)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CONNECTBTTN, OnConnectbttn)
	ON_COMMAND(ID_CONFIGBTTN, OnConfigbttn)
	ON_COMMAND(ID_CLEAROUTPUTBTTN, OnClearoutputbttn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame

	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		 CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
 
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	m_Connection.SetOutputView( &m_wndView);

	m_wndView.SetMainFrame( this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnUpdateConfigbttn(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( TRUE);
	
}

void CMainFrame::OnUpdateConnectbttn(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( TRUE);
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CFrameWnd::OnClose();
}

void CMainFrame::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CFrameWnd::PreSubclassWindow();
}

void CMainFrame::SubmitCommand( CString Command)
{
	yathCTransaction *NewTransaction = m_CommandParser.Parse( Command);
	if ( NewTransaction != NULL){
		m_Connection.NewTransaction( NewTransaction);
	}
}

char Param1[] = "This is 1st parameter string";
char Param2[] = "This is 2nd parameter string";
char Param3[] = "This is 3rd parameter string";
char Param4[] = "This is 4th parameter string";
char Param5[] = "This is 5th parameter string";

void CMainFrame::MakeTest()
{
	void *CommandBlock = malloc( 2048);
	if ( CommandBlock != NULL){
		if ( !::yathInitChunk( CommandBlock, 2048, 100)) {
			m_wndView.WriteOutput( "Unable to init command block\r\n");
			goto Skip;
		}

		{
			yathFIELDREC Recs[10];

			Recs[0].FieldData = Param1;
			Recs[0].FieldDataSize = sizeof( Param1);
			Recs[1].FieldData = Param2;
			Recs[1].FieldDataSize = sizeof( Param2);
			Recs[2].FieldData = Param3;
			Recs[2].FieldDataSize = sizeof( Param3);
			Recs[3].FieldData = Param4;
			Recs[3].FieldDataSize = sizeof( Param4);
			Recs[4].FieldData = Param5;
			Recs[4].FieldDataSize = sizeof( Param5);

			if ( !::yathAppendFieldsToChunk( CommandBlock, 2048, Recs, 5)){
				m_wndView.WriteOutput( "Unable to add parameters\r\n");
				goto Skip;
			}

		}
		
		
		{
			yathFIELDREC Recs[10];
			int RecsCount = 10;

			if ( ::yathRetriveFields( CommandBlock, 2048, Recs, RecsCount)){
				int I;
				for ( I = 0; I < RecsCount; I++){
					m_wndView.WriteOutput( (( char *) Recs[I].FieldData));
					m_wndView.WriteOutput( "\r\n");
				}
			} else m_wndView.WriteOutput( "Unable to retrive the parameters\r\n");
		}
Skip:
		delete CommandBlock;
	}
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnConnectbttn() 
{
	// TODO: Add your command handler code here
	CConnectDlg Dlg;
	int DlgResult = Dlg.DoModal();

	if ( DlgResult == IDOK){
		m_Connection.Connect( Dlg.m_Address, Dlg.m_Port);
	}
}

void CMainFrame::OnConfigbttn() 
{
	// TODO: Add your command handler code here
	MakeTest();
}

void CMainFrame::OnClearoutputbttn() 
{
	// TODO: Add your command handler code here
	m_wndView.ClearOutput();	
}
