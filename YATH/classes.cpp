/////////////////////////////////////////////////////////////

#include "classes.h"

/////////////////////////////////////////////////////////////

#ifdef _YATH_DEBUG

#include <stdio.h>

#endif


/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCGeneric::yathCGeneric()
{
}

/////////////////////////////////////////////////////////////

yathCGeneric::~yathCGeneric()
{
}

/////////////////////////////////////////////////////////////

void yathCGeneric::Free()
{
	delete (yathCGeneric *)this;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCCriticalSection::yathCCriticalSection()
{
	::InitializeCriticalSection( &m_Section);
}

/////////////////////////////////////////////////////////////

yathCCriticalSection::~yathCCriticalSection()
{
	::DeleteCriticalSection( &m_Section);
}

/////////////////////////////////////////////////////////////

void yathCCriticalSection::Enter()
{
	::EnterCriticalSection( &m_Section);
}

/////////////////////////////////////////////////////////////

void yathCCriticalSection::Leave()
{
	::LeaveCriticalSection( &m_Section);
}

/////////////////////////////////////////////////////////////

/*
BOOL yathCCriticalSection::TryToEnter()
{
	return ::TryEnterCriticalSection( &m_Section);
}
*/

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCUDPListener::yathCUDPListener( int Port, int BufferSize)
{
	m_Port = Port;
	m_Buffer = NULL;	
	m_BufferSize = BufferSize;
	m_LastStepDone = 0;
	m_BufferSize = BufferSize;
	m_LastSockError = 0;
}

/////////////////////////////////////////////////////////////

yathCUDPListener::~yathCUDPListener()
{
	Done();
}

/////////////////////////////////////////////////////////////

int yathCUDPListener::Initialize()
{
	int Result = 1;

	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();
	m_LastStepDone = 0;
	m_Socket = socket( AF_INET, SOCK_DGRAM, 0);
	if (m_Socket == INVALID_SOCKET){
		m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
		printf( "ERROR: socket() failed; %d\n", m_LastSockError);
#endif
		goto Skip;
	}
	m_LastStepDone = 1;
	m_Local.sin_family = AF_INET;
	m_Local.sin_port = htons(( short) m_Port);
	m_Local.sin_addr.s_addr = htonl( INADDR_ANY); // Any interface 
	if ( bind( m_Socket, ( SOCKADDR *)&m_Local, sizeof( m_Local)) == SOCKET_ERROR){
		m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
		printf("ERROR: bind() failed; %d\n", m_LastSockError);
#endif
		goto Skip;
	}
	m_LastStepDone = 2;
	if (m_Buffer != NULL){
		GlobalFree( m_Buffer);
		m_Buffer = NULL;
	}
	m_Buffer = GlobalAlloc( GMEM_FIXED, m_BufferSize);

	if ( !m_Buffer){
#ifdef _YATH_DEBUG
		printf("ERROR: GlobalAlloc() failed; %d\n", GetLastError());
#endif
		goto Skip;
	}
	m_LastStepDone = 3;
	m_DataRecvd = FALSE;
	Result = 0;

Skip:
	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();

	return Result;
}

/////////////////////////////////////////////////////////////

int yathCUDPListener::Done()
{
	int Result = 0;

	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();

	if ( m_LastStepDone >= 1) {
		
		m_LastStepDone = 0;

		if ( closesocket( m_Socket)){
			m_LastSockError = WSAGetLastError();
			Result = 1;
		}
	}

	if (m_Buffer != NULL){
		GlobalFree( m_Buffer);
		m_Buffer = NULL;
	}

	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();

	return Result;
}

/////////////////////////////////////////////////////////////

int yathCUDPListener::Listen(void *Buffer, int BufferSize, int &DataSize)
{
	int Result = 1;
	
	DataSize = 0;

	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();
	
	if ( m_LastStepDone == 3){
		m_SenderSize = sizeof( m_Sender);
		m_LastStepDone = 4;
		// LeaveCriticalSection( 0);
		m_CriticalSection.Leave();
		m_RecvdSize = recvfrom( m_Socket, (char *)m_Buffer, m_BufferSize, 0, (SOCKADDR *)&m_Sender, (int *)&m_SenderSize);
		// EnterCriticalSection( 0);
		m_CriticalSection.Enter();
		m_LastStepDone = 3;
		if ( m_RecvdSize != SOCKET_ERROR){
			if ( m_RecvdSize <= BufferSize) DataSize = m_RecvdSize;
			else DataSize = BufferSize;
			memcpy( Buffer, m_Buffer, DataSize);
		} else {
			m_LastSockError = WSAGetLastError();
			
#ifdef _YATH_DEBUG			
			printf( "ERROR: recvfrom() failed; %d\n", m_LastSockError);
#endif
			goto Skip;
		}
	} else {
		goto Skip;
	}
	
	Result = 0;

Skip:
	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();
	return 0;
}

/////////////////////////////////////////////////////////////

int yathCUDPListener::GetLastError()
{
	int Result;

	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();
	Result = m_LastSockError;
	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();

	return Result;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCTCPConnection::yathCTCPConnection( SOCKET &Socket, SOCKADDR_IN &SenderAddr, int SenderAddrLen)
{
	m_ServerSide = TRUE;
	
	m_Socket = Socket;
	m_SenderAddr = SenderAddr;
	m_SenderAddrLen = SenderAddrLen;
	m_LastSockError = 0;
	m_LastStepDone = 2;
	m_Port = 0;
}

/////////////////////////////////////////////////////////////

yathCTCPConnection::yathCTCPConnection( int Port, char *Address)
{
	m_ServerSide = FALSE;
	
	m_Port = Port;
	m_Address = inet_addr( Address);
	// m_IP = _strdup( IP);
}

/////////////////////////////////////////////////////////////

yathCTCPConnection::yathCTCPConnection( int Port, unsigned long Address)
{
	m_ServerSide = FALSE;
	
	m_Port = Port;
	m_Address = htonl( Address);
}

/////////////////////////////////////////////////////////////

yathCTCPConnection::~yathCTCPConnection()
{
	Done();
	
	/*
	if ( m_IP != NULL){
		free( m_IP);	
	}
	*/
}

/////////////////////////////////////////////////////////////

int yathCTCPConnection::Initialize()
{
	int Result = 1;
	
	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();
	
	m_LastStepDone = 0;
	
	m_Socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if ( m_Socket == INVALID_SOCKET){
		m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
		printf( "ERROR: socket() failed; %d\n", m_LastSockError);
#endif
		goto Skip;
	}

	m_LastStepDone = 1;
	
	{
		SOCKADDR_IN ServerAddr;
	
		memset( &ServerAddr, 0, sizeof( ServerAddr));

		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons( m_Port);
		ServerAddr.sin_addr.s_addr = m_Address;

		if ( ServerAddr.sin_addr.s_addr == INADDR_NONE){
			m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
			printf( "ERROR: inet_addr() failed; %d\n", m_LastSockError);
#endif
			goto Skip;
		}

		if ( connect( m_Socket, ( SOCKADDR *) &ServerAddr, sizeof( ServerAddr))){
			m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
			printf( "ERROR: connect() failed; %d\n", m_LastSockError);
#endif
			goto Skip;
		}

		m_LastStepDone = 2;
	}

	Result = 0;

Skip:
	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();
	return Result;
}

/////////////////////////////////////////////////////////////

int yathCTCPConnection::Done()
{
	int Result = 0;

	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();
	
	if (m_LastStepDone >= 1) {	
		m_LastStepDone = 0;
		if ( closesocket( m_Socket)){
			m_LastSockError = WSAGetLastError();
			Result = 1;
		}
	}

	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();

	return Result;
}

/////////////////////////////////////////////////////////////

int yathCTCPConnection::SendData( void *Buffer, int DataSize)
{
	int Result = 1;
	int SizeToSend = DataSize;
	void *DataPtr = Buffer;

	// EnterCriticalSection( 1);
	m_SendCriticalSection.Enter();
	while ( SizeToSend > 0){
		int SentSize = send( m_Socket, (char *)DataPtr, SizeToSend, 0);

		if ( SentSize == SOCKET_ERROR){
			m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
			printf( "ERROR: send() failed; %d\n", m_LastSockError);
#endif
			goto Skip;
		}

		SizeToSend -= SentSize;
		DataPtr = (char *)DataPtr + SentSize;		
	}
	
	Result = 0;
Skip:
	// LeaveCriticalSection(1);
	m_SendCriticalSection.Leave();
	return Result;
}

/////////////////////////////////////////////////////////////

int yathCTCPConnection::ReadData( void *Buffer, int ReadSize, int &DataSize, BOOL WaitToFill)
{
	int Result = 1;
	void *BufferPtr = Buffer;
	int SizeToRead = ReadSize;

	// EnterCriticalSection( 2);
	m_ReadCriticalSection.Enter();
	DataSize = 0;
	while ( SizeToRead > 0){
		int ReadSize = recv( m_Socket, (char *)BufferPtr, SizeToRead, 0);

		if ( ReadSize == SOCKET_ERROR){
			m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
			printf( "ERROR: recv() failed; %d\n", m_LastSockError);
#endif
			goto Skip;
		} 
		else if ( ReadSize == 0){
			m_LastSockError = 0;
			Result = 0;
			goto Skip;
		}

		DataSize += ReadSize;
		SizeToRead -= ReadSize;
		BufferPtr = (char *)BufferPtr + ReadSize;

		if ( !WaitToFill) break;
	}
	
	Result = 0;

Skip:
	// LeaveCriticalSection( 2);
	m_ReadCriticalSection.Leave();
	return Result;
}

/////////////////////////////////////////////////////////////

int yathCTCPConnection::SendDataBlock( void *Buffer, int BufferSize)
{
	int Result = 1;
	int ChunkSize;

	if ( ::yathRetriveChunkSize( Buffer, BufferSize, ChunkSize)){
		Result = SendData( Buffer, ChunkSize);
	}

	return Result;
}

/////////////////////////////////////////////////////////////

int yathCTCPConnection::ReadDataBlock( void *Buffer, int BufferSize, int &DataSize)
{
	int Result = 1;
	int ReadSize;

	if ( BufferSize >= sizeof( yathDATAHDR)){
		if ( !ReadData( Buffer, sizeof( yathDATAHDR), ReadSize, TRUE)){
			if ( ReadSize == sizeof( yathDATAHDR)){
				int BlockDataSize = (( yathDATAHDR *)Buffer)->DataSize;
				DataSize = sizeof( yathDATAHDR);	
				if ( DataSize > 0){
					if ( BufferSize >= ( BlockDataSize + (int)sizeof( yathDATAHDR))){
						void *DataPtr = ((void *)(((unsigned char *)Buffer) + sizeof( yathDATAHDR)));
						if ( !ReadData( DataPtr, BlockDataSize, ReadSize, TRUE)){
							if ( BlockDataSize == ReadSize) {
								DataSize += ReadSize;
								Result = 0;
							}
						}
					}
				} else Result = 0;
			} else if ( ReadSize == 0){
				DataSize = 0;
				Result = 0;
			}
		}
	}

	return Result;
}

/////////////////////////////////////////////////////////////

int yathCTCPConnection::GetLastError()
{
	int Result;
	
	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();
	Result = m_LastSockError;
	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();

	return Result;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCTCPListener::yathCTCPListener( int Port)
{
	m_Port = Port;
	m_LastStepDone = 0;
	m_LastSockError = 0;
	m_Connection = NULL;
}

/////////////////////////////////////////////////////////////

yathCTCPListener::~yathCTCPListener()
{
	Done();
}

/////////////////////////////////////////////////////////////

int yathCTCPListener::Initialize()
{
	int Result = 1;
	
	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();
	m_LastStepDone = 0;
	m_Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_Socket == SOCKET_ERROR){
		m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
		printf( "ERROR: socket() failed; %d\n", m_LastSockError);
#endif
		goto Skip;
	}
	m_LastStepDone = 1;
	m_Local.sin_family = AF_INET;
	m_Local.sin_port = htons(( short) m_Port);
	m_Local.sin_addr.s_addr = htonl( INADDR_ANY); // Any interface 
	if ( bind( m_Socket, ( SOCKADDR *)&m_Local, sizeof( m_Local)) == SOCKET_ERROR){
		m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
		printf("ERROR: bind() failed; %d\n", m_LastSockError);
#endif
		goto Skip;
	}
	m_LastStepDone = 2;
	Result = 0;

Skip:
	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();

	return Result;
}

/////////////////////////////////////////////////////////////

int yathCTCPListener::Done()
{
	int Result = 0;

	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();
	
	if (m_LastStepDone >= 1) {	
		m_LastStepDone = 0;
		if ( closesocket( m_Socket)){
			m_LastSockError = WSAGetLastError();
			Result = 1;
		}
	}

	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();

	return Result;
}

/////////////////////////////////////////////////////////////

int yathCTCPListener::Listen( int Pendings)
{
	int Result = 1;

	// EnterCriticalSection( 0);
	m_CriticalSection.Enter();
	
	if ( m_Connection != NULL) {
		delete m_Connection;
		m_Connection = NULL;
	}

	if ( m_LastStepDone < 2) goto Skip;

	if ( listen( m_Socket, Pendings) == SOCKET_ERROR){
		// EnterCriticalSection( 0);
		m_CriticalSection.Enter();
		m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
		printf( "ERROR: listen() failed; %d\n", m_LastSockError);
#endif
		goto Skip;
	}
	{
		SOCKADDR_IN SenderAddr;
		int SenderAddrLen = sizeof( SenderAddr);
		// LeaveCriticalSection( 0);
		m_CriticalSection.Leave();
		SOCKET ConnectionSocket = accept( m_Socket, (SOCKADDR *) &SenderAddr, &SenderAddrLen);
		// EnterCriticalSection( 0);
		m_CriticalSection.Enter();

		if ( ConnectionSocket == INVALID_SOCKET){
			m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
			printf( "ERROR: accept() failed; %d\n", m_LastSockError);
#endif
			goto Skip;
		}
		m_Connection = new yathCTCPConnection ( ConnectionSocket, SenderAddr, SenderAddrLen);
	}
	Result = 0;

Skip:
	// LeaveCriticalSection( 0);
	m_CriticalSection.Leave();

	return Result;
}

/////////////////////////////////////////////////////////////

yathCTCPConnection *yathCTCPListener::GetConnection()
{
	m_CriticalSection.Enter();
	yathCTCPConnection *Connection = m_Connection;
	m_Connection = NULL;
	m_CriticalSection.Leave();

	return Connection;
}

/////////////////////////////////////////////////////////////

int yathCTCPListener::GetLastError()
{
	int Result;
	
	m_CriticalSection.Enter();
	Result = m_LastSockError;
	m_CriticalSection.Leave();

	return Result;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCThread::yathCThread( DWORD TimeBeforeTerminate)
{
	m_ThreadHandle = NULL;
	m_ThreadId = 0;
	m_LastThreadError = 0;
	m_FirstTimeResume = TRUE;
	m_TimeBeforeTerminate = TimeBeforeTerminate;
	m_Container = NULL;
	m_InActiveList = FALSE;
	m_InFinishedList = FALSE;
	m_DestroyLock = 0;
	m_StopRequest = FALSE;

	m_ThreadHandle = ::CreateThread( NULL, 0, yathCThread::_Runner, (LPVOID) this, CREATE_SUSPENDED, &m_ThreadId);
}

/////////////////////////////////////////////////////////////

yathCThread::~yathCThread()
{
	if ( m_ThreadHandle != NULL) {
		::CloseHandle( m_ThreadHandle);
	}
}

/////////////////////////////////////////////////////////////

DWORD WINAPI yathCThread::_Runner( LPVOID lpvParam)
{
	DWORD Result = -1;

	if (((yathCThread *)lpvParam) != NULL) {
		((yathCThread *)lpvParam)->m_CriticalSection.Enter();
		((yathCThread *)lpvParam)->m_StopRequest = FALSE;
		((yathCThread *)lpvParam)->m_CriticalSection.Leave();
		Result = ((yathCThread *)lpvParam)->Execute();
		((yathCThread *)lpvParam)->Finish();
	}

	return Result;
}

/////////////////////////////////////////////////////////////

DWORD yathCThread::Resume()
{
	DWORD Result = -1;
	
	if ( m_ThreadHandle != NULL){
		BOOL DoResume = TRUE;

		if ( m_FirstTimeResume) {
			DoResume = BeforeExecute();
			m_FirstTimeResume = FALSE;
		}
		
		if ( DoResume){
			Result = ::ResumeThread( m_ThreadHandle);
			if ( Result == -1) m_LastThreadError = ::GetLastError();
		}		
	}

	return Result;
}

/////////////////////////////////////////////////////////////

DWORD yathCThread::Suspend()
{
	DWORD Result = -1;
	
	if ( m_ThreadHandle != NULL){
		Result = ::SuspendThread( m_ThreadHandle);
		if ( Result == -1) m_LastThreadError = ::GetLastError();
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCThread::SoftTerminate()
{
	BOOL DoTerminate = FALSE;
	
	if ( m_ThreadHandle != NULL) {	
		DWORD ExitCode;
		if ( ::GetExitCodeThread( m_ThreadHandle, &ExitCode) != 0){	
			if ( ExitCode ==  STILL_ACTIVE) {
				StopRequest();
				DoTerminate = TRUE;
				while ( ::GetExitCodeThread( m_ThreadHandle, &ExitCode) != 0){
					if ( ExitCode ==  STILL_ACTIVE){
						if ( m_TimeBeforeTerminate > 100){
							m_TimeBeforeTerminate -= 100;
							Sleep( 100);
						} else if ( m_TimeBeforeTerminate > 0){
							m_TimeBeforeTerminate = 0;
							Sleep( m_TimeBeforeTerminate);
						} else break;
					} else {
						DoTerminate = FALSE;
						break;
					}
				}
			}
		}

		if ( DoTerminate){
			::TerminateThread( m_ThreadHandle, -1);
#ifdef _DEBUG
			::Beep( 4000, 3000);
#endif
			AfterTermination();
		}
	}

	return ( DoTerminate);
}

/////////////////////////////////////////////////////////////

BOOL yathCThread::IsStopRequested()
{
	BOOL Result;

	m_CriticalSection.Enter();
	Result = m_StopRequest;
	m_CriticalSection.Leave();

	return Result;
}

/////////////////////////////////////////////////////////////

void yathCThread::StopRequest()
{
#ifdef _DEBUG
	::Beep( 500, 200);
#endif
	m_CriticalSection.Enter();
	BOOL DoFinish = m_InActiveList;
	yathCThreadsContainer *Container = m_Container;
	m_StopRequest = TRUE;
	m_CriticalSection.Leave();
	
	if (( DoFinish) && ( Container != NULL)) Container->SetFinished( this);
}

/////////////////////////////////////////////////////////////

BOOL yathCThread::BeforeExecute()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////

BOOL yathCThread::AfterTermination()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////

DWORD yathCThread::Execute()
{
#ifdef _YATH_DEBUG
	printf("The theed (%d) is started ...\n", m_ThreadId);
	Sleep( 1000);
	printf("The theed (%d) is finished ...\n", m_ThreadId);
#endif
	return 0;
}

/////////////////////////////////////////////////////////////

BOOL yathCThread::IsActive()
{

	BOOL Result = FALSE;

	if ( m_ThreadHandle != NULL){
		DWORD ExitCode;
		if ( ::GetExitCodeThread( m_ThreadHandle, &ExitCode) != 0){
			if ( ExitCode == STILL_ACTIVE) Result = TRUE;
		} else m_LastThreadError = ::GetLastError();
	}

	return Result;
}

/////////////////////////////////////////////////////////////

void yathCThread::AddPeerThread( yathCThread *Thread, BOOL AutoStart)
{
	if ( Thread != NULL){
		m_CriticalSection.Enter();
		if ( m_Container != NULL){
			m_Container->AddThread( Thread, AutoStart);
		} else Thread->Free();
		m_CriticalSection.Leave();
	}
}

/////////////////////////////////////////////////////////////

void yathCThread::Finish()
{
	m_CriticalSection.Enter();
	BOOL InActiveList = m_InActiveList;
	yathCThreadsContainer *Container = m_Container;
	m_CriticalSection.Leave();
	if (( InActiveList == TRUE) && ( Container != NULL)) {
		Container->SetFinished( this);
	}
}

/////////////////////////////////////////////////////////////

void yathCThread::Free()
{
	SoftTerminate();
	yathCGeneric::Free();
}

/////////////////////////////////////////////////////////////

BOOL yathCThread::IsDestroyLocked()
{
	m_CriticalSection.Enter();
	BOOL Result = (m_DestroyLock > 0);
	m_CriticalSection.Leave();
	return Result;
}

/////////////////////////////////////////////////////////////

void yathCThread::DestroyLock()
{
	m_CriticalSection.Enter();
	m_DestroyLock++;
	m_CriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

void yathCThread::DestroyUnlock()
{
	m_CriticalSection.Enter();
	m_DestroyLock--;
	m_CriticalSection.Leave();
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCUDPSender::yathCUDPSender()
{
	m_LastStepDone = 0;
}

/////////////////////////////////////////////////////////////

yathCUDPSender::~yathCUDPSender()
{
	if ( m_LastStepDone >= 1) closesocket( m_Socket);
}

/////////////////////////////////////////////////////////////

int yathCUDPSender::Send( char *Address, int Port, void *Buffer, int BufferSize, int &SentSize)
{
	if ( m_LastStepDone < 1){
		m_Socket = socket( AF_INET, SOCK_DGRAM, 0);

		if ( m_Socket == INVALID_SOCKET){
			m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG
			printf( "ERROR: socket() failed; %d\n", m_LastSockError);
#endif
			return 1;
		}

		m_LastStepDone = 1;
	}
	
	{
		SOCKADDR_IN Destanation;

		Destanation.sin_family = AF_INET;
		Destanation.sin_addr.s_addr = inet_addr( Address);
		Destanation.sin_port = htons( Port);
		SentSize = sendto( m_Socket, (char *)Buffer, BufferSize, 0, (sockaddr *)&Destanation, sizeof( Destanation));
		if ( SentSize == SOCKET_ERROR){
			m_LastSockError = WSAGetLastError();
#ifdef _YATH_DEBUG			
			printf( "ERROR: sendto() failed; %d\n", m_LastSockError);
#endif

			return 1;
		}
	}

	return 0;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCThreadsContainer::yathCThreadsContainer()
{
	m_FirstActive = NULL;
	m_LastActive = NULL;
	m_FirstIdle = NULL;
	m_LastIdle = NULL;
}

/////////////////////////////////////////////////////////////

yathCThreadsContainer::~yathCThreadsContainer()
{
	StopRequestAll();
	ClearIdleList();
}

/////////////////////////////////////////////////////////////

void yathCThreadsContainer::AddThread( yathCThread *Thread, BOOL AutoResume)
{
	if ( Thread != NULL) {
		yathTHREAD_LIST *NewItem = new yathTHREAD_LIST;
		
		NewItem->Thread = Thread;
		NewItem->Next = NULL;

		m_ListsCriticalSection.Enter();
		Thread->m_CriticalSection.Enter();

		if (( m_FirstActive != NULL) && ( m_LastActive != NULL)){
			NewItem->Prev = m_LastActive;
			m_LastActive->Next = NewItem;
		} else {
			NewItem->Prev = NULL;
			m_FirstActive = NewItem;
		}
		
		m_LastActive = NewItem;
		
		Thread->m_Container = this;
		Thread->m_InActiveList = TRUE;
		Thread->m_InFinishedList = FALSE;
		
		if ( AutoResume) Thread->Resume();

		Thread->m_CriticalSection.Leave();
		m_ListsCriticalSection.Leave();
	}
}

/////////////////////////////////////////////////////////////

void yathCThreadsContainer::SetFinished( yathCThread *Thread)
{
	// <<<>>> There is a problem in manipulating Finished & Active lists
	
	if ( Thread != NULL) {		
		BOOL IsInFinishedList = FALSE;
		
		m_ListsCriticalSection.Enter();
		Thread->m_CriticalSection.Enter();
		
		if ( Thread->m_Container == NULL) {
			Thread->m_Container = this;
			Thread->m_InActiveList = FALSE;
			Thread->m_InFinishedList = FALSE;
		}
	
		if ( Thread->m_Container == this) {
			if ( Thread->m_InActiveList){
				yathTHREAD_LIST *ScanItem = m_FirstActive;
				while ( ScanItem != NULL) {
					if ( ScanItem->Thread != Thread) 
						ScanItem = ScanItem->Next;
					else {
						if ( ScanItem->Prev != NULL) 
							ScanItem->Prev->Next = ScanItem->Next;
						else 
							m_FirstActive = ScanItem->Next;
						if ( ScanItem->Next != NULL) 
							ScanItem->Next->Prev = ScanItem->Prev;
						else 
							m_LastActive = ScanItem->Prev;

						Thread->m_InActiveList = FALSE;

						if ( Thread->m_InFinishedList == FALSE) {
							ScanItem->Next = NULL;
							if (( m_FirstIdle != NULL) && ( m_LastIdle != NULL)){
								ScanItem->Prev = m_LastIdle;
								m_LastIdle->Next = ScanItem;
							} else {
								ScanItem->Prev = NULL;
								m_FirstIdle = ScanItem;
							}
							m_LastIdle = ScanItem;
							Thread->m_InFinishedList = TRUE;
						} else delete ScanItem;

						IsInFinishedList = TRUE;

						break;
					}
				}
			}

			if ( IsInFinishedList == FALSE){
				yathTHREAD_LIST *ScanItem = new yathTHREAD_LIST;
				ScanItem->Thread = Thread;
				ScanItem->Next = NULL;
				if (( m_FirstIdle != NULL) && ( m_LastIdle != NULL)){
					ScanItem->Prev = m_LastIdle;
					m_LastIdle->Next = ScanItem;
				} else {
					ScanItem->Prev = NULL;
					m_FirstIdle = ScanItem;
				}
				m_LastActive = ScanItem;
				Thread->m_InFinishedList = TRUE;
			}
		}

		Thread->m_CriticalSection.Leave();
		m_ListsCriticalSection.Leave();
	}

	PeacefullyClearIdleList();
}

/////////////////////////////////////////////////////////////

void yathCThreadsContainer::StopRequestAll()
{
	m_ListsCriticalSection.Enter();
	for (;;){
		yathTHREAD_LIST *ScanItem = m_FirstActive;
		if ( ScanItem != NULL) {
			if ( ScanItem->Thread != NULL){
				m_ListsCriticalSection.Leave();
				ScanItem->Thread->StopRequest();
				m_ListsCriticalSection.Enter();
			} else {
				if ( ScanItem->Next != NULL) ScanItem->Next->Prev = NULL;
				m_FirstActive = ScanItem->Next;
				delete ScanItem;
			}
		} else break;
	}
	m_ListsCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

void yathCThreadsContainer::ClearIdleList()
{
	m_ListsCriticalSection.Enter();
	while ( m_FirstIdle != NULL) {
		yathTHREAD_LIST *ScanItem = m_FirstIdle;
		m_FirstIdle = ScanItem->Next;
		if ( ScanItem->Next != NULL) ScanItem->Next->Prev = NULL;
		else m_LastIdle = NULL;
		if ( ScanItem->Thread != NULL){
			ScanItem->Thread->Free();
			delete ScanItem;
		}
	}
	m_ListsCriticalSection.Leave();
}

/////////////////////////////////////////////////////////////

void yathCThreadsContainer::PeacefullyClearIdleList()
{
	m_ListsCriticalSection.Enter();
	yathTHREAD_LIST *ScanItem = m_FirstIdle;
	
	while ( ScanItem != NULL){
		BOOL RemoveScanItem;
		if ( ScanItem->Thread != NULL){
			if (( ScanItem->Thread->IsActive()) || ( ScanItem->Thread->IsDestroyLocked())) RemoveScanItem = FALSE;
			else {
				RemoveScanItem = TRUE;
				ScanItem->Thread->Free();
			}
		} else RemoveScanItem = TRUE;

		if ( RemoveScanItem){
			yathTHREAD_LIST *RemovedItem = ScanItem;
			ScanItem = ScanItem->Next;

			if ( RemovedItem->Prev != NULL) RemovedItem->Prev->Next = RemovedItem->Next;
			else m_FirstIdle = RemovedItem->Next;

			if ( RemovedItem->Next != NULL) RemovedItem->Next->Prev = RemovedItem->Prev;
			else m_LastIdle = RemovedItem->Prev;

			delete RemovedItem;

#ifdef _DEBUG
			::Beep( 700, 300);
#endif
		} else ScanItem = ScanItem->Next;
	}

	m_ListsCriticalSection.Leave();
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

int yathCalculateDataCRC( void *Data, int DataSize)
{
	unsigned char Extra = 0;
	int CRC = 0;
	while ( DataSize > 0){
		CRC += ((*(( unsigned char *)Data)) + Extra);
		DataSize --;
		Extra++;
	}
	return CRC;
}

/////////////////////////////////////////////////////////////

BOOL yathInitChunk( void *Buffer, int BufferSize, int ChunkStamp)
{
	BOOL Result = FALSE;
	
	if (( Buffer != NULL) && ( BufferSize >= sizeof( yathCHUNKHDR))){
		(( yathCHUNKHDR *)Buffer)->DataHeader.DataSize = ( sizeof( yathCHUNKHDR) - sizeof( yathDATAHDR));
		(( yathCHUNKHDR *)Buffer)->DataHeader.DataSizeCRC = 0;
		(( yathCHUNKHDR *)Buffer)->DataHeader.DataCRC = 0;
		(( yathCHUNKHDR *)Buffer)->ChunkStamp = ChunkStamp;
		(( yathCHUNKHDR *)Buffer)->ChunkFieldsCount = 0;
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathAppendFieldToChunk( void *Buffer, int BufferSize, const void *FieldData, int FieldDataSize)
{
	BOOL Result = FALSE;
	
	if (( Buffer != NULL) && ( BufferSize >= sizeof( yathCHUNKHDR))) {
		int UsedSize = (( yathCHUNKHDR *)Buffer)->DataHeader.DataSize;
		if ( BufferSize >= (int)( sizeof( yathDATAHDR) + UsedSize + sizeof( yathFIELDHDR) + FieldDataSize)){
			void *DataPtr = ((void *)(((unsigned char *)Buffer) + (int)sizeof( yathDATAHDR) + UsedSize));
			(( yathFIELDHDR *)DataPtr)->FieldDataSize = FieldDataSize;
			UsedSize += sizeof( yathFIELDHDR) + FieldDataSize;
			if ( FieldDataSize > 0){
				if ( FieldData != NULL){
					DataPtr = (void *)(((unsigned char *)DataPtr) + (int)sizeof( yathFIELDHDR));
					memcpy( DataPtr, FieldData, FieldDataSize);
					Result = TRUE;
				}
			} else Result = TRUE;

			if ( Result == TRUE){
				(( yathCHUNKHDR *)Buffer)->DataHeader.DataSize = UsedSize;
				(( yathCHUNKHDR *)Buffer)->ChunkFieldsCount += 1;
			}
		}
	}
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathAppendFieldsToChunk( void *Buffer, int BufferSize, const yathFIELDREC *Fields, int FieldsCount)
{
	BOOL Result = FALSE;
	
	if (( Buffer != NULL) && ( BufferSize >= sizeof( yathCHUNKHDR)) && ( Fields != NULL) && ( FieldsCount > 0)) {
		int UsedSize = (( yathCHUNKHDR *)Buffer)->DataHeader.DataSize;
		int I;
		int FieldsSize = FieldsCount * sizeof( yathFIELDHDR);

		for ( I = 0; I < FieldsCount; I++){
			int FieldDataSize = Fields[I].FieldDataSize;
			if ( FieldDataSize > 0) {
				if ( Fields[I].FieldData != NULL) FieldsSize += FieldDataSize;
				else {
					FieldsSize = 0;
					break;
				}
			}
		}

		if ( FieldsSize > 0){
			if ( BufferSize >= (int)( sizeof( yathDATAHDR) + UsedSize + FieldsSize)){
				void *DataPtr = ((void *)(((unsigned char *)Buffer) + ( sizeof( yathDATAHDR) + UsedSize)));
				for ( I = 0; I < FieldsCount; I++){
					int FieldDataSize = Fields[I].FieldDataSize;
					(( yathFIELDHDR *) DataPtr)->FieldDataSize = FieldDataSize;
					DataPtr = ( void *)((( unsigned char *)DataPtr) + sizeof( yathFIELDHDR));
					if ( FieldDataSize > 0){
						memcpy( DataPtr, Fields[I].FieldData, FieldDataSize);
						DataPtr = ( void *)((( unsigned char *) DataPtr) + FieldDataSize);
					}
				}

				UsedSize += FieldsSize;
				(( yathCHUNKHDR *) Buffer)->DataHeader.DataSize = UsedSize;
				(( yathCHUNKHDR *) Buffer)->ChunkFieldsCount += FieldsCount;
				Result = TRUE;
			}
		}
	}
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathFinalizeChunk( void *Buffer, int BufferSize)
{
	BOOL Result = FALSE;
	
	if (( Buffer != NULL) && ( BufferSize >= sizeof( yathCHUNKHDR))){
		int DataSize = (( yathDATAHDR *)Buffer)->DataSize;
		if ( BufferSize >= (int)( DataSize + sizeof( yathDATAHDR))){
			void *DataPtr = ((void *)((( unsigned char *)Buffer) + sizeof( yathDATAHDR)));
			int DataCRC = yathCalculateDataCRC( DataPtr, DataSize);
			int DataSizeCRC = yathCalculateDataCRC( &((( yathDATAHDR *)Buffer)->DataSize), sizeof ((( yathDATAHDR *)Buffer)->DataSize));
			(( yathDATAHDR *)Buffer)->DataCRC = DataCRC;
			(( yathDATAHDR *)Buffer)->DataSizeCRC = DataSizeCRC;
			Result = TRUE;
		}
	}
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathRetriveField( const void *Buffer, int BufferSize, int FieldNo, void **Field, int &FieldDataSize)
{
	BOOL Result = FALSE;

	if (( Buffer != NULL) && ( BufferSize >= sizeof( yathCHUNKHDR))) {
		int DataSize = (( yathDATAHDR *)Buffer)->DataSize;
		if ( BufferSize >= (int)( DataSize + sizeof( yathDATAHDR))){
			if ( DataSize >= (int)( sizeof( yathCHUNKHDR) - sizeof( yathDATAHDR))){
				void *DataPtr = ((void *)((( unsigned char *)Buffer) + sizeof( yathCHUNKHDR)));
				int Offset = sizeof( yathCHUNKHDR) - sizeof( yathDATAHDR);
				int FieldsCount = (( yathCHUNKHDR *)Buffer)->ChunkFieldsCount;
				if ( FieldsCount > FieldNo){
					while ( FieldNo > 0) {
						if ( DataSize >= (int)(Offset + sizeof( yathFIELDHDR))){
							int FieldSize = sizeof( yathFIELDHDR) + (( yathFIELDHDR *) DataPtr)->FieldDataSize;
							Offset += FieldSize;
							DataPtr = (void *)((( unsigned char *) DataPtr) + FieldSize);
							FieldNo--;
						} else break;
					}

					if ( FieldNo == 0){
						Offset += sizeof( yathFIELDHDR);
						if ( DataSize >= Offset){
							FieldDataSize =  (( yathFIELDHDR *) DataPtr)->FieldDataSize;
							*Field = (void *)((( unsigned char *) DataPtr) + sizeof( yathFIELDHDR));
							if ( DataSize >= (Offset + FieldDataSize)) Result = TRUE;
						}
					}
				}
			}
		}
	}
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathRetriveFields( const void *Buffer, int BufferSize, yathFIELDREC *Fields, int &FieldsCount)
{
	BOOL Result = FALSE;

	if (( Buffer != NULL) && ( BufferSize >= sizeof( yathCHUNKHDR))) {
		int DataSize = (( yathDATAHDR *)Buffer)->DataSize;
		if ( BufferSize >= (int)( DataSize + sizeof( yathDATAHDR))){
			if ( DataSize >= (int)( sizeof( yathCHUNKHDR) - sizeof( yathDATAHDR))){
				const void *DataPtr = ((void *)((( unsigned char *)Buffer) + sizeof( yathCHUNKHDR)));
				int Offset = sizeof( yathCHUNKHDR) - sizeof( yathDATAHDR);
				int ChunkFieldsCount = (( yathCHUNKHDR *)Buffer)->ChunkFieldsCount;
				int RequestedFieldsCount = FieldsCount;
				int Counter;
				BOOL Overflow = FALSE;
				FieldsCount = 0;
				for ( Counter = 0; (( Counter < RequestedFieldsCount) && ( Counter < ChunkFieldsCount)); Counter++){
					if ( DataSize >= (int)( Offset + sizeof( yathFIELDHDR))){
						int FieldSize = (( yathFIELDHDR *) DataPtr)->FieldDataSize;
						Fields[ Counter].FieldDataSize = FieldSize;
						if ( FieldSize > 0) Fields[ Counter].FieldData = ( void *)((( unsigned char *) DataPtr) + sizeof( yathFIELDHDR));
						else Fields[ Counter].FieldData = NULL;
						FieldSize += sizeof( yathFIELDHDR);
						Offset += FieldSize;
						DataPtr = (void *)((( unsigned char *) DataPtr) + FieldSize);
					} else {
						Overflow = TRUE; 
						break;
					}
				}

				if ( !Overflow){
					if ( Offset <= DataSize){
						FieldsCount = Counter;
						Result = TRUE;
					} else {
						FieldsCount = NULL;
					}
				}
			}
		}
	}
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathRetriveFieldsCount( const void *Buffer, int BufferSize, int &FieldsCount)
{
	BOOL Result = FALSE;

	if (( Buffer != NULL) && ( BufferSize >= sizeof( yathCHUNKHDR))){
		int DataSize = (( yathDATAHDR *)Buffer)->DataSize;
		if ( BufferSize >= (int)( DataSize + sizeof( yathDATAHDR))){
			if ( DataSize >= ( sizeof( yathCHUNKHDR) - sizeof( yathDATAHDR))){
				FieldsCount = (( yathCHUNKHDR *)Buffer)->ChunkFieldsCount;
				Result = TRUE;
			}
		}
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathRetriveChunkStamp( const void *Buffer, int BufferSize, int &Stamp)
{
	BOOL Result = FALSE;

	if (( Buffer != NULL) && ( BufferSize >= sizeof( yathCHUNKHDR))){
		int DataSize = (( yathDATAHDR *)Buffer)->DataSize;
		if ( BufferSize >= (int)( DataSize + sizeof( yathDATAHDR))){
			if ( DataSize >= (int)( sizeof( yathCHUNKHDR) - sizeof( yathDATAHDR))){
				Stamp = (( yathCHUNKHDR *)Buffer)->ChunkStamp;
				Result = TRUE;
			}
		}
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathRetriveChunkSize( const void *Buffer, int BufferSize, int &ChunkSize)
{
	BOOL Result = FALSE;

	if (( Buffer != NULL) && ( BufferSize >= sizeof( yathCHUNKHDR))){
		int DataSize = (( yathDATAHDR *)Buffer)->DataSize;
		ChunkSize = DataSize + sizeof( yathDATAHDR);
		if ( BufferSize >= ChunkSize) Result = TRUE;
	}

	return Result;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

const char *yathFindToken( const char *StartFrom, const char *&ParamStart, int &ParamLength, BOOL &IsOption)
{
	const char *Result;
	
	ParamLength = 0;

	if ( StartFrom != NULL){

		while ( *StartFrom == ' ') StartFrom++;

		if ( *StartFrom == '"'){
			
			IsOption = FALSE;
			StartFrom++;
			ParamStart = StartFrom;
		
			while ( *StartFrom != 0){
				
				if ( *StartFrom != '"'){
					StartFrom++;
					ParamLength++;
				} else {
					StartFrom++;
					break;
				}

			}

		} else if ( *StartFrom != 0){

			if ( *StartFrom == '-') {
				IsOption = TRUE;
				StartFrom++;
			} else IsOption = FALSE;
			
			ParamStart = StartFrom;
			
			while ( *StartFrom != 0) {

				if (( *StartFrom != ' ') && ( *StartFrom != '"')){
					StartFrom++;
					ParamLength++;
				} else break;

			} 

		} else ParamStart = NULL;

		if ( *StartFrom != 0) Result = StartFrom;
		else Result = NULL;

	} else {
		IsOption = FALSE;
		ParamStart = NULL;
		Result = NULL;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCmpBufWStr( const void *Buffer, int BufferLength, const char *Str)
{
	if (( Str != NULL) && ( Buffer != NULL) && ( BufferLength > 0)){
		if ( strlen( Str) == (unsigned int)BufferLength){
			return ( memcmp( Buffer, Str, BufferLength) == 0);
		} else return FALSE;
	} else return FALSE;
}

/////////////////////////////////////////////////////////////

const char *yathPathFindFileName( const char *Path)
{
	const char *Result;
	if ( Path != NULL){
		const char *ScanPtr = Path;
		Result = ScanPtr;
		while ( *ScanPtr != '\0'){
			if ( *ScanPtr == '\\') {
				ScanPtr++;
				Result = ScanPtr;
			} if ( *ScanPtr == ':'){
				ScanPtr++;
				if ( *ScanPtr != '\0') Result = ScanPtr;
			} else ScanPtr++;
		}
	} else Result = NULL;
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathPathAppend( char *Path, const char *Name, int MaxSize)
{
	BOOL Result = FALSE;

	if (( Path != NULL) && ( Name != NULL) && (MaxSize > 1)){
		int Remain = MaxSize - 1;
		char *ScanPtr = Path;
		BOOL NeedBkSlash = FALSE;
		
		while ( *ScanPtr != '\0'){
			if ( Remain > 0) {
				NeedBkSlash = (( *ScanPtr != '\\') && ( *ScanPtr != ':'));
				ScanPtr++;
				Remain--;
			} else goto NoRoom;
		}
		
		if ( NeedBkSlash) {
			if ( Remain > 0) {
				*ScanPtr = '\\';
				ScanPtr++;
				Remain--;
			} else goto NoRoom;
		}

		while ( *Name != '\0') {
			if ( Remain > 0) {
				*ScanPtr = *Name;
				ScanPtr++;
				Name++;
				Remain--;
			} else goto NoRoom;
		}

		Result = TRUE;

NoRoom:;
	   *ScanPtr = '\0';
	}

	return Result;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/
