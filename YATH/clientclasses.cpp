#include "clientclasses.h"
#include <string.h>

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCUserInterface::yathCUserInterface()
{
}

/////////////////////////////////////////////////////////////

yathCUserInterface::~yathCUserInterface()
{
}

/////////////////////////////////////////////////////////////

void yathCUserInterface::WriteOutput( const char *Str)
{
}

/////////////////////////////////////////////////////////////

void yathCUserInterface::WriteOutputToCatch( const char *Str)
{
}

/////////////////////////////////////////////////////////////

void yathCUserInterface::FlushOutputCatch()
{
}

/////////////////////////////////////////////////////////////

void yathCUserInterface::ClearOutput()
{
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCTransaction::yathCTransaction()
{
	m_StopRequested = FALSE;
}

/////////////////////////////////////////////////////////////

yathCTransaction::~yathCTransaction()
{
}

/////////////////////////////////////////////////////////////

BOOL yathCTransaction::Do( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	return FALSE;
}

/////////////////////////////////////////////////////////////

void yathCTransaction::StopRequest()
{
	m_CriticalSection.Enter();
	m_StopRequested = TRUE;
	m_CriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

BOOL yathCTransaction::IsStopRequested()
{
	BOOL Result;
	m_CriticalSection.Enter();
	Result = m_StopRequested;
	m_CriticalSection.Leave();
	return Result;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCConnectionActor::yathCConnectionActor( int Port, char *Address, yathCConnectionManager *Manager)
:yathCThread( 1000)
{
	m_ListCriticalSection.Enter();
	m_Manager = Manager;
	m_Connection = new yathCTCPConnection( Port, Address);
	m_FirstWaitingTransaction = NULL;
	m_LastWaitingTransaction = NULL;
	m_FirstDoneTransaction = NULL;
	m_LastDoneTransaction = NULL;
	m_WorkingTransaction = NULL;
	m_ListCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

yathCConnectionActor::yathCConnectionActor( int Port, DWORD Address, yathCConnectionManager *Manager)
:yathCThread( 1000)
{
	m_ListCriticalSection.Enter();
	m_Manager = Manager;
	m_Connection = new yathCTCPConnection( Port, Address);
	m_FirstWaitingTransaction = NULL;
	m_LastWaitingTransaction = NULL;
	m_FirstDoneTransaction = NULL;
	m_LastDoneTransaction = NULL;
	m_WorkingTransaction = NULL;
	m_ListCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

yathCConnectionActor::~yathCConnectionActor()
{
	m_ListCriticalSection.Enter();

	RemoveAllTransactions();

	if ( m_Connection != NULL) {
		m_Connection->Free();
		m_Connection = NULL;
	}

	m_ListCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

void yathCConnectionActor::RemoveAllTransactions(  BOOL EvenWorking)
{
	while ( m_FirstWaitingTransaction != NULL){
		yathTRANSACTION_LIST *ScanItem = m_FirstWaitingTransaction;
		m_FirstWaitingTransaction = ScanItem->Next;
		if ( ScanItem->Transaction != NULL) ScanItem->Transaction->Free();
		delete ScanItem;
	}

	m_LastWaitingTransaction = NULL;

	while ( m_FirstDoneTransaction != NULL){
		yathTRANSACTION_LIST *ScanItem = m_FirstDoneTransaction;
		m_FirstDoneTransaction = ScanItem->Next;
		if ( ScanItem->Transaction != NULL) ScanItem->Transaction->Free();
		delete ScanItem;
	}

	m_LastDoneTransaction = NULL;

	if ( EvenWorking == TRUE) {
		if ( m_WorkingTransaction != NULL) {
			m_WorkingTransaction->Free();
			m_WorkingTransaction = NULL;
		}
	}
}

/////////////////////////////////////////////////////////////

void yathCConnectionActor::StopRequest()
{
	yathCThread::StopRequest();
	m_ListCriticalSection.Enter();
	if ( m_WorkingTransaction != NULL) m_WorkingTransaction->StopRequest();
	if ( m_Connection != NULL) m_Connection->Done();
	RemoveAllTransactions( FALSE);
	m_ListCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

void yathCConnectionActor::AppendTransaction( yathCTransaction *Transaction)
{
	if ( Transaction == NULL) return;

	m_ListCriticalSection.Enter();
	yathTRANSACTION_LIST *NewItem = new yathTRANSACTION_LIST;

	NewItem->Transaction = Transaction;
	NewItem->Next = NULL;
	
	if (( m_FirstWaitingTransaction != NULL) && ( m_LastWaitingTransaction != NULL)){
		NewItem->Prev = m_LastWaitingTransaction;
		m_LastWaitingTransaction->Next = NewItem;
		m_LastWaitingTransaction = NewItem;
	} else {
		NewItem->Prev = NULL;
		m_FirstWaitingTransaction = NewItem;
		m_LastWaitingTransaction = NewItem;
	}

	m_ListCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

BOOL yathCConnectionActor::ShiftToNextWaitingTransaction()
{
	BOOL Result;
	yathTRANSACTION_LIST *ShiftedListItem = NULL;
	
	m_ListCriticalSection.Enter();
	
	yathCTransaction *LastWorkingTransaction = m_WorkingTransaction;
	m_WorkingTransaction = NULL;

	do {
		if ( m_FirstWaitingTransaction != NULL){
			if ( ShiftedListItem != NULL) delete ShiftedListItem;
			ShiftedListItem = m_FirstWaitingTransaction;
			m_FirstWaitingTransaction = ShiftedListItem->Next;
			if ( m_FirstWaitingTransaction != NULL) m_FirstWaitingTransaction->Prev = NULL;
			else m_LastWaitingTransaction = NULL;
			m_WorkingTransaction = ShiftedListItem->Transaction;
		} else {
			m_WorkingTransaction = NULL;
			break;
		}
	} while ( m_WorkingTransaction == NULL);

	if ( LastWorkingTransaction != NULL){
		if ( ShiftedListItem == NULL) ShiftedListItem = new yathTRANSACTION_LIST;
		ShiftedListItem->Transaction = LastWorkingTransaction;
		ShiftedListItem->Next = NULL;
		if (( m_FirstDoneTransaction != NULL) && ( m_LastDoneTransaction != NULL)){
			ShiftedListItem->Prev = m_LastDoneTransaction;
			m_LastDoneTransaction->Next = ShiftedListItem;
			m_LastDoneTransaction = ShiftedListItem;
		} else {
			ShiftedListItem->Prev = NULL;
			m_FirstDoneTransaction = ShiftedListItem;
			m_LastDoneTransaction = ShiftedListItem;
		}
	} else if ( ShiftedListItem != NULL) delete ShiftedListItem;

	Result = ( m_WorkingTransaction != NULL);
	
	m_ListCriticalSection.Leave();
	
	return Result;
}

/////////////////////////////////////////////////////////////

yathCTransaction *yathCConnectionActor::GetDoneTransaction()
{
	yathCTransaction *Result;
	m_ListCriticalSection.Enter();
	if ( m_FirstDoneTransaction != NULL){
		yathTRANSACTION_LIST *RemovedItem = m_FirstDoneTransaction;
		m_FirstDoneTransaction = RemovedItem->Next;
		if ( m_FirstDoneTransaction != NULL) m_FirstDoneTransaction->Prev = NULL;
		else m_LastDoneTransaction = NULL;
		Result = RemovedItem->Transaction;
		delete RemovedItem;
	} else Result = NULL;
	m_ListCriticalSection.Leave();
	return NULL;
}

/////////////////////////////////////////////////////////////

DWORD yathCConnectionActor::Execute()
{
	m_ListCriticalSection.Enter();
	if ( m_Connection == NULL){
		m_Manager->WriteOutput( "Unable to connect to remote server, internal error.\r\n\r\n");
		m_ListCriticalSection.Leave();
		return -1;
	}
	if (m_Connection->Initialize()){
		m_Manager->WriteOutput( "Unable to connect to remote server.\r\n\r\n");
		m_ListCriticalSection.Leave();
		return -1;
	} else m_Manager->WriteOutput( "Connection established successfully.\r\n\r\n");

	m_ListCriticalSection.Leave();

	while ( !IsStopRequested()) {
		while ( !ShiftToNextWaitingTransaction()) {
			Sleep( 50);
			if (IsStopRequested()) goto Skip;
		}
		m_WorkingTransaction->Do( m_Connection, m_Manager);
	};

Skip:
	// m_Manager->WriteOutput( "Connection closed.\r\n\r\n");
	m_Connection->Done();

	return 0;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCConnectionManager::yathCConnectionManager()
{
	m_Actor = NULL;
	m_OutputView = NULL;
}

/////////////////////////////////////////////////////////////

yathCConnectionManager::~yathCConnectionManager()
{
	Disconnect();
}

/////////////////////////////////////////////////////////////

void yathCConnectionManager::SetOutputView( yathCUserInterface *OutputView)
{
	m_OutputCriticalSection.Enter();
	m_OutputView = OutputView;
	m_OutputCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

void yathCConnectionManager::WriteOutput( const char *Str)
{
	m_OutputCriticalSection.Enter();
	if ( m_OutputView != NULL) m_OutputView->WriteOutput( Str);
	m_OutputCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

void yathCConnectionManager::WriteOutputToCatch( const char *Str)
{
	m_OutputCriticalSection.Enter();
	if ( m_OutputView != NULL) m_OutputView->WriteOutputToCatch( Str);
	m_OutputCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

void yathCConnectionManager::FlushOutputCatch()
{
	m_OutputCriticalSection.Enter();
	if ( m_OutputView != NULL) m_OutputView->FlushOutputCatch();
	m_OutputCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

void yathCConnectionManager::ClearOutput()
{
	m_OutputCriticalSection.Enter();
	if ( m_OutputView != NULL) m_OutputView->ClearOutput();
	m_OutputCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

BOOL yathCConnectionManager::Connect( char *Address, int Port)
{
	Disconnect();

	m_Actor = new yathCConnectionActor( Port, Address, this);
	m_Actor->DestroyLock();
	m_ThreadsContainer.AddThread( m_Actor, TRUE);
	return TRUE;
}

/////////////////////////////////////////////////////////////

BOOL yathCConnectionManager::Connect( DWORD Address, int Port)
{
	Disconnect();

	m_Actor = new yathCConnectionActor( Port, Address, this);
	m_Actor->DestroyLock();
	m_ThreadsContainer.AddThread( m_Actor, TRUE);
	return TRUE;
}

/////////////////////////////////////////////////////////////

void yathCConnectionManager::Disconnect()
{
	if ( m_Actor != NULL) {
		m_Actor->DestroyUnlock();
		m_Actor->StopRequest();
		Sleep( 300);
		m_ThreadsContainer.PeacefullyClearIdleList();
		m_Actor = NULL;
	}
}

/////////////////////////////////////////////////////////////

void yathCConnectionManager::NewTransaction( yathCTransaction *Transaction)
{
	if ( Transaction != NULL){
		if ( m_Actor != NULL) m_Actor->AppendTransaction( Transaction);
		else Transaction->Free();
	}
}

/////////////////////////////////////////////////////////////