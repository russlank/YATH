// ServerOutputView.cpp : implementation file
//

#include "stdafx.h"
#include "YATHClient.h"
#include "InterfaceView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInterfaceView

CInterfaceView::CInterfaceView()
{
	m_MainFrame = NULL;
}

CInterfaceView::~CInterfaceView()
{
}


BEGIN_MESSAGE_MAP(CInterfaceView, CWnd)
	//{{AFX_MSG_MAP(CInterfaceView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInterfaceView message handlers

BOOL CInterfaceView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW), NULL);

	return TRUE;


}

void CInterfaceView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	CRect Rect;
	/*
	CPen *Pen = new CPen( PS_SOLID, 1, RGB(0,0,0));
	CPen *OldPen = dc.SelectObject( Pen);
	*/

	GetClientRect( Rect);

	if ( Rect.Height() > 60){
		dc.MoveTo( 0, Rect.Height() - 60);
		dc.LineTo( Rect.Width(), Rect.Height() - 60);
	}

	/*
	dc.SelectObject( OldPen);

	delete Pen;
	*/
	
	// Do not call CWnd::OnPaint() for painting messages
}

int CInterfaceView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	m_OutputEditFont.CreateFont( 0, 0, 0, 0, FW_DONTCARE, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "courier");
	m_CommandEditFont.CreateFont( 0, 0, 0, 0, FW_DONTCARE, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "courier");

	m_OutputEdit.Create( ES_READONLY | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, CRect( 0, 0, 0, 0), this, 0);
	m_OutputEdit.SetFont( &m_OutputEditFont, TRUE);
	m_CommandEdit.Create( ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_CHILD | WS_VISIBLE | WS_VSCROLL, CRect( 0, 0, 0, 0), this, 0);
	m_CommandEdit.SetFont( &m_CommandEditFont, TRUE);
	m_OutputEdit.LimitText( 0);
	return 0;
}

char MyData[] = "New line of data\n";
char *DataPtr;

void CInterfaceView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	int DefaultCommandEditSizeY = 60;

	if ( cy > DefaultCommandEditSizeY){
		m_OutputEdit.ShowWindow( SW_SHOW);
		m_OutputEdit.MoveWindow( CRect(0, 0, cx, cy - DefaultCommandEditSizeY), TRUE);
		m_CommandEdit.MoveWindow( CRect(0, (cy - DefaultCommandEditSizeY + 1), cx, cy), TRUE);
	} else {
		m_OutputEdit.ShowWindow( SW_HIDE);
		m_CommandEdit.MoveWindow( CRect(0, 0, cx, cy), TRUE);
	}
}

void CInterfaceView::ClearOutput()
{
	m_OutputBuffer.Empty();
	m_OutputEdit.SetWindowText( "");
}

void CInterfaceView::WriteOutput( const char *Str)
{
	FlushOutputCatch();
	
	m_OutputEdit.SetSel( -1, -1);
	m_OutputEdit.ReplaceSel( Str, FALSE);
	m_OutputEdit.LineScroll( m_OutputEdit.GetLineCount(), 0);
}

void CInterfaceView::WriteOutputToCatch( const char *Str)
{
	m_OutputBuffer = m_OutputBuffer + Str;
}

void CInterfaceView::FlushOutputCatch()
{
	if ( m_OutputBuffer.GetLength() > 0){
		m_OutputEdit.SetSel( -1, -1);
		m_OutputEdit.ReplaceSel( m_OutputBuffer, FALSE);
		m_OutputEdit.LineScroll( m_OutputEdit.GetLineCount(), 0);
		m_OutputBuffer.Empty();
	}
}

BOOL CInterfaceView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL Processed = FALSE;
	
	if ( pMsg->hwnd == m_CommandEdit.m_hWnd){
		if (( pMsg->message == WM_CHAR) && ( pMsg->wParam == 13)){
			SubmitCommand();
			Processed = TRUE;
		}
	}

	if ( !Processed) return CWnd::PreTranslateMessage(pMsg);
	else return TRUE;
}

void CInterfaceView::SubmitCommand()
{	
	if ( m_MainFrame != NULL){
		CString Command;
		m_CommandEdit.GetWindowText( Command);
		m_MainFrame->SubmitCommand( Command);
	}
	
	m_CommandEdit.SetWindowText( "");
	m_CommandEdit.SetFocus();
}

void CInterfaceView::SetMainFrame( CMainFrame *MainFrame)
{
	m_MainFrame = MainFrame;
}
