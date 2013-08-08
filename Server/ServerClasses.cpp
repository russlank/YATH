#include "ServerClasses.h"

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCSessionThread::yathCSessionThread( yathCServer *Server, yathCTCPConnection *Connection)
:yathCThread( 1000)
{
	m_Server = Server;
	m_Connection = Connection;
}

yathCSessionThread::~yathCSessionThread()
{
	if ( m_Connection != NULL) m_Connection->Free();
}

DWORD yathCSessionThread::Execute()
{
	if (m_Connection == NULL) return -1;

#ifdef _DEBUG
	::Beep( 2000, 200);
#endif

	{
		int ReceivedDataSize;

		//while ( !m_Connection->ReadData( m_Buffer, sizeof( m_Buffer), ReceivedDataSize, FALSE)){
		while ( !m_Connection->ReadDataBlock( m_Buffer, sizeof( m_Buffer), ReceivedDataSize)){
			if ( ReceivedDataSize > 0){
				int Command;
				if ( ::yathRetriveChunkStamp( m_Buffer, ReceivedDataSize, Command)){
					BOOL ResponseResult = FALSE;
					switch ( Command) {
					case yathcmdLISTFILES:
						ResponseResult = ResponseListFiles();
						break;
					case yathcmdCHANGEDIR:
						ResponseResult = ResponseChangeDir();
						break;
					case yathcmdGETCURRENTRIR:
						ResponseResult = ResponseError();
						break;
					case yathcmdMAKEDIR:
						ResponseResult = ResponseCreateDir();
						break;
					case yathcmdREMOVEFILE:
						ResponseResult = ResponseRemoveFile();
						break;
					case yathcmdREMOVEDIR:
						ResponseResult = ResponseRemoveDir();
						break;
					case yathcmdGETFILE:
						ResponseResult = ResponseGetFile();
						break;
					case yathcmdPUTFILE:
						ResponseResult = ResponsePutFile();
						break;
					case yathcmdMOVE:
						ResponseResult = ResponseMove();
						break;
					default:;
						ResponseResult = ResponseError();
					}

					if ( !ResponseResult) goto StopSession;
				}
			} else goto StopSession;
		}
	}
	
StopSession:	
	m_Connection->Done();

#ifdef _DEBUG	
	::Beep( 1000, 200);
#endif

	return 0;
}

BOOL yathCSessionThread::ResponseError()
{
	BOOL Result = TRUE;
	::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
	::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
	if ( m_Connection->SendDataBlock( m_Buffer, sizeof( m_Buffer))) Result = FALSE;
	return Result;
}

BOOL yathCSessionThread::ResponseListFiles()
{
	BOOL Result = TRUE;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	int ChunkSize;
	int FieldsCount;


	::yathRetriveFieldsCount( m_Buffer, sizeof( m_Buffer), FieldsCount);
	{
		char *FileName;
		if ( FieldsCount > 0){
			int FileNameLength;
			::yathRetriveField( m_Buffer, sizeof( m_Buffer), 0, (void **)&FileName, FileNameLength);
			if ( FileNameLength == 0) FileName = NULL;
		} else FileName = NULL;
		
		if ( FileName == NULL) FileName = "*.*";

		hFind = FindFirstFile( FileName, &FindFileData);
	}

	if (hFind != INVALID_HANDLE_VALUE) {
		int RecsInChunk = 0;
		::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdLISTFILES);
		do {
			yathFILEINFO FileInfo;
			
			FileInfo.FileAttributes = FindFileData.dwFileAttributes;
			FileInfo.FileCreationTime = FindFileData.ftCreationTime;
			FileInfo.FileLastAcessTime = FindFileData.ftLastAccessTime;
			FileInfo.FileLastWriteTime = FindFileData.ftLastWriteTime;
			FileInfo.FileSizeLo = FindFileData.nFileSizeLow;
			FileInfo.FileSizeHi = FindFileData.nFileSizeHigh;
			strcpy( FileInfo.FileName, FindFileData.cFileName);
			
			if (!::yathAppendFieldToChunk( m_Buffer, sizeof( m_Buffer), &FileInfo, sizeof( FileInfo))){
				int ChunkSize;
				::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
				::yathRetriveChunkSize( m_Buffer, sizeof( m_Buffer), ChunkSize);
				if (!m_Connection->SendData( m_Buffer, ChunkSize)){
					::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdLISTFILES);
					::yathAppendFieldToChunk( m_Buffer, sizeof( m_Buffer), &FileInfo, sizeof( FileInfo));
					RecsInChunk = 1;
				} else {
					Result = FALSE;
					goto Skip;
				}
			} else RecsInChunk ++;
		} while ( ::FindNextFile( hFind, &FindFileData) != 0);

		if ( RecsInChunk > 0){
			::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
			::yathRetriveChunkSize( m_Buffer, sizeof( m_Buffer), ChunkSize);
			if (m_Connection->SendData( m_Buffer, ChunkSize)){
				Result = FALSE;
				goto Skip;
			}
		}

		::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdLISTFILES);
		::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
		::yathRetriveChunkSize( m_Buffer, sizeof( m_Buffer), ChunkSize);
		if (m_Connection->SendData( m_Buffer, ChunkSize)) Result = FALSE;
Skip:
		FindClose(hFind);
	} else {
		::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
		::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
		::yathRetriveChunkSize( m_Buffer, sizeof( m_Buffer), ChunkSize);
		if (m_Connection->SendData( m_Buffer, ChunkSize)) Result = FALSE;
	}

	return Result;
}

BOOL yathCSessionThread::ResponseChangeDir()
{
	BOOL Result = TRUE;
	int FieldsCount;
	
	::yathRetriveFieldsCount( m_Buffer, sizeof( m_Buffer), FieldsCount);
	if ( FieldsCount > 0){
		int PathLength;
		char *NewDir;
		::yathRetriveField( m_Buffer, sizeof( m_Buffer), 0, (void **)&NewDir, PathLength);
		if ( PathLength > 0) SetCurrentDirectory( NewDir);
	}

	{
		char CurrentDir[1024];
		DWORD CurrentDirLength = GetCurrentDirectory( sizeof( CurrentDir), CurrentDir);
		if ( CurrentDirLength > 0){
			CurrentDirLength++;
			::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdCHANGEDIR);
			::yathAppendFieldToChunk( m_Buffer, sizeof( m_Buffer), CurrentDir, CurrentDirLength);
		} else {
			::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
		}
		::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
		{
			int ChunkSize;
			::yathRetriveChunkSize( m_Buffer, sizeof( m_Buffer), ChunkSize);
			if (m_Connection->SendData( m_Buffer, ChunkSize)) Result = FALSE;
		}
	}
	
	return Result;
}

BOOL yathCSessionThread::ResponseCreateDir()
{
	BOOL Result = TRUE;
	int FieldsCount;
	BOOL Succeed = FALSE;
	
	::yathRetriveFieldsCount( m_Buffer, sizeof( m_Buffer), FieldsCount);
	if ( FieldsCount > 0){
		int PathLength;
		char *NewDir;
		::yathRetriveField( m_Buffer, sizeof( m_Buffer), 0, (void **)&NewDir, PathLength);
		if ( PathLength > 0) if ( ::CreateDirectory( NewDir, NULL)) Succeed = TRUE;
	}

	if ( Succeed) ::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdMAKEDIR);
	else ::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
	::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
	if ( m_Connection->SendDataBlock( m_Buffer, sizeof( m_Buffer))) Result = FALSE;

	return Result;
}

BOOL yathCSessionThread::ResponseRemoveDir()
{
	BOOL Result = TRUE;
	int FieldsCount;
	BOOL Succeed = FALSE;
	
	::yathRetriveFieldsCount( m_Buffer, sizeof( m_Buffer), FieldsCount);
	if ( FieldsCount > 0){
		int PathLength;
		char *NewDir;
		::yathRetriveField( m_Buffer, sizeof( m_Buffer), 0, (void **)&NewDir, PathLength);
		if ( PathLength > 0) if ( ::RemoveDirectory( NewDir)) Succeed = TRUE;
	}

	if ( Succeed) ::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdREMOVEDIR);
	else ::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
	::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
	if ( m_Connection->SendDataBlock( m_Buffer, sizeof( m_Buffer))) Result = FALSE;

	return Result;
}

BOOL yathCSessionThread::ResponseRemoveFile()
{
	BOOL Result = TRUE;
	int FieldsCount;
	BOOL Succeed = FALSE;
	
	::yathRetriveFieldsCount( m_Buffer, sizeof( m_Buffer), FieldsCount);
	if ( FieldsCount > 0){
		int PathLength;
		char *NewDir;
		::yathRetriveField( m_Buffer, sizeof( m_Buffer), 0, (void **)&NewDir, PathLength);
		if ( PathLength > 0) if ( ::DeleteFile( NewDir)) Succeed = TRUE;
	}

	if ( Succeed) ::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdREMOVEFILE);
	else ::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
	::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
	if ( m_Connection->SendDataBlock( m_Buffer, sizeof( m_Buffer))) Result = FALSE;

	return Result;
}

BOOL yathCSessionThread::ResponseMove()
{
	BOOL Result = TRUE;
	int FieldsCount;
	BOOL Succeed = FALSE;
	
	::yathRetriveFieldsCount( m_Buffer, sizeof( m_Buffer), FieldsCount);
	if ( FieldsCount > 1){
		char *Path1;
		char *Path2;
		int PathLength1;
		int PathLength2;
		::yathRetriveField( m_Buffer, sizeof( m_Buffer), 0, (void **)&Path1, PathLength1);
		::yathRetriveField( m_Buffer, sizeof( m_Buffer), 1, (void **)&Path2, PathLength2);
		if (( PathLength1 > 0) && ( PathLength2 > 0)) {
			if ( ::MoveFile( Path1, Path2)) Succeed = TRUE;
		}
	}

	if ( Succeed) ::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdMOVE);
	else ::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
	::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
	if ( m_Connection->SendDataBlock( m_Buffer, sizeof( m_Buffer))) Result = FALSE;

	return Result;
}

BOOL yathCSessionThread::ResponseGetFile()
{
	BOOL Result = TRUE;
	int FieldsCount;

	::yathRetriveFieldsCount( m_Buffer, sizeof( m_Buffer), FieldsCount);

	if ( FieldsCount > 0){
		int FileNameLength;
		char *FileName;
		::yathRetriveField( m_Buffer, sizeof( m_Buffer), 0, (void **)&FileName, FileNameLength);
		if ( FileNameLength > 0) {
			HANDLE FileHandle;
			FileHandle = CreateFile( FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if ( FileHandle != INVALID_HANDLE_VALUE){
				char ReadBuffer[ 1024 * 8];
				DWORD BytesRead;
				do {
					if ( ReadFile( FileHandle, ReadBuffer, sizeof( ReadBuffer), &BytesRead, NULL)){
						int ChunkSize;
						::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdGETFILE);
						::yathAppendFieldToChunk( m_Buffer, sizeof( m_Buffer), ReadBuffer, BytesRead);
						::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
						::yathRetriveChunkSize( m_Buffer, sizeof( m_Buffer), ChunkSize);
						if ( m_Connection->SendData( m_Buffer, ChunkSize)) {
							Result = FALSE;
							break;
						}
					} else {
						int ChunkSize;
						::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
						::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
						::yathRetriveChunkSize( m_Buffer, sizeof( m_Buffer), ChunkSize);
						if ( m_Connection->SendData( m_Buffer, ChunkSize)) Result = FALSE;
						break;
					}
				} while ( BytesRead > 0);
				CloseHandle( FileHandle);
			} else {
				int ChunkSize;
				::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
				::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
				::yathRetriveChunkSize( m_Buffer, sizeof( m_Buffer), ChunkSize);
				if ( m_Connection->SendData( m_Buffer, ChunkSize)) Result = FALSE;
			}
		} else {
			int ChunkSize;
			::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
			::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
			::yathRetriveChunkSize( m_Buffer, sizeof( m_Buffer), ChunkSize);
			if ( m_Connection->SendData( m_Buffer, ChunkSize)) Result = FALSE;
		}
	}

	return Result;
}

BOOL yathCSessionThread::ResponsePutFile()
{
	BOOL Result = TRUE;
	int FieldsCount;
	__int64 RecievedSize = 0;
	BOOL FileOpened = FALSE;
	HANDLE FileHandle;

	::yathRetriveFieldsCount( m_Buffer, sizeof( m_Buffer), FieldsCount);

	if ( FieldsCount > 0){
		int FileNameLength;
		char *FileName;
		::yathRetriveField( m_Buffer, sizeof( m_Buffer), 0, (void **)&FileName, FileNameLength);
		if ( FileNameLength > 0) {
			FileHandle = ::CreateFile( FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if ( FileHandle != INVALID_HANDLE_VALUE){
				::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdPUTFILE);
				::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
				if ( !m_Connection->SendDataBlock( m_Buffer, sizeof( m_Buffer))){
					BOOL PerformWrite = TRUE;
					for (;;){
						int ReceivedDataSize;
						if ( !m_Connection->ReadDataBlock( m_Buffer, sizeof( m_Buffer), ReceivedDataSize)){
							int ChunkStamp;
							::yathRetriveChunkStamp( m_Buffer, ReceivedDataSize, ChunkStamp);
							if ( ChunkStamp == yathcmdPUTFILE){
								int FieldsCount;
								::yathRetriveFieldsCount( m_Buffer, ReceivedDataSize, FieldsCount);
								if ( FieldsCount > 0){
									void *DataPtr;
									int DataSize;
									::yathRetriveField( m_Buffer, ReceivedDataSize, 0, &DataPtr, DataSize);
									if ( DataSize > 0){
										if ( PerformWrite){
											DWORD WrittenSize;
											::WriteFile( FileHandle, DataPtr, DataSize, &WrittenSize, NULL);
											RecievedSize += WrittenSize;
											if ( ((DWORD )DataSize) != WrittenSize) PerformWrite = FALSE;
										}
									} else {
										::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdPUTFILE);
										::yathAppendFieldToChunk( m_Buffer, sizeof( m_Buffer), &RecievedSize, sizeof( RecievedSize));
										::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
										if ( m_Connection->SendDataBlock( m_Buffer, sizeof( m_Buffer))) Result = FALSE;
										break;
									}
								} else {
									Result = FALSE;
									break;
								}
							} else break;
						} else {
							Result = FALSE;
							break;
						}
					}

				} else Result = FALSE;
				::CloseHandle( FileHandle);
			} else {
				::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
				::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
				if ( m_Connection->SendDataBlock( m_Buffer, sizeof( m_Buffer))) Result = FALSE;
			}
		} else {
			::yathInitChunk( m_Buffer, sizeof( m_Buffer), yathcmdERROR);
			::yathFinalizeChunk( m_Buffer, sizeof( m_Buffer));
			if ( m_Connection->SendDataBlock( m_Buffer, sizeof( m_Buffer))) Result = FALSE;
		}
	}

	return Result;
}



void yathCSessionThread::Finish()
{
	yathCThread::Finish();
}

void yathCSessionThread::StopRequest()
{
	yathCThread::StopRequest();
	if ( m_Connection != NULL){
		m_Connection->Done();
	}
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCProxySessionThread::yathCProxySessionThread( yathCServer *Server, yathCTCPConnection *ClientConnection)
{
	m_Server = Server;
	m_ClientConnection = ClientConnection;
	m_ServerConnection = new yathCTCPConnection( 3128, "192.168.2.9"); 
	if ( m_ServerConnection->Initialize()) {
		m_ServerConnection->Free();
		m_ServerConnection = NULL;
	}
}

yathCProxySessionThread::~yathCProxySessionThread()
{
	if ( m_ClientConnection != NULL) m_ClientConnection->Free();
	if ( m_ServerConnection != NULL) m_ServerConnection->Free();

}

DWORD yathCProxySessionThread::Execute()
{
	if ( m_ClientConnection == NULL) return -1;
	if ( m_ServerConnection == NULL) {
		m_ClientConnection->Done();
		return -1;
	}

	char Buffer[16384];
	int BufferUsed;

#ifdef _DEBUG	
	::Beep( 500, 100);
#endif

	if ( !m_ClientConnection->ReadData( Buffer, sizeof( Buffer), BufferUsed, FALSE)){
		if ( !m_ServerConnection->SendData( Buffer, BufferUsed)){
			while ( !m_ServerConnection->ReadData( Buffer, sizeof( Buffer), BufferUsed, FALSE)){
				if ( BufferUsed > 0) {
					if ( m_ClientConnection->SendData( Buffer, BufferUsed)) break;
				} else break;
			}
		}
	}

	m_ClientConnection->Done();
	m_ServerConnection->Done();

	return 0;
}

void yathCProxySessionThread::StopRequest()
{
	yathCThread::StopRequest();
	if ( m_ClientConnection != NULL) m_ClientConnection->Done();
	if ( m_ServerConnection != NULL) m_ServerConnection->Done();
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCPortThread::yathCPortThread( yathCServer *Server)
:yathCThread( 1000)
{
	m_Server = Server;
	m_Listener = new yathCTCPListener( 4000);
	m_Listener->Initialize();
}

yathCPortThread::~yathCPortThread()
{
	if ( m_Listener != NULL) delete m_Listener;
}

DWORD yathCPortThread::Execute()
{
	if ( m_Listener == NULL) return -1;

	while ( !m_Listener->Listen( 10)){
		yathCTCPConnection *NewConnection = m_Listener->GetConnection();
		if ( NewConnection != NULL){
			// MessageBox( NULL, "New Connection Acception", "Info", MB_OK);
			yathCSessionThread *NewSession = new yathCSessionThread( m_Server, NewConnection);
			AddPeerThread( NewSession, TRUE);
		}
	}
	return 0;
}

void yathCPortThread::StopRequest()
{
	yathCThread::StopRequest();
	if ( m_Listener != NULL) {
		m_Listener->Done();
		m_Listener->Initialize();
	}
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCProxyThread::yathCProxyThread( yathCServer *Server)
:yathCThread( 1000)
{
	m_Server = Server;
	m_Listener = new yathCTCPListener( 4001);
	m_Listener->Initialize();
}

yathCProxyThread::~yathCProxyThread()
{
	if ( m_Listener != NULL) delete m_Listener;
}

DWORD yathCProxyThread::Execute()
{
	if ( m_Listener == NULL) return -1;

	while ( !m_Listener->Listen( 10)){
		yathCTCPConnection *NewConnection = m_Listener->GetConnection();
		if ( NewConnection != NULL){
			yathCProxySessionThread *NewSession = new yathCProxySessionThread( m_Server, NewConnection);
			AddPeerThread( NewSession, TRUE);
		}
	}
	return 0;
}

void yathCProxyThread::StopRequest()
{
	yathCThread::StopRequest();
	if ( m_Listener != NULL) {
		m_Listener->Done();
		m_Listener->Initialize();
	}
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCServer::yathCServer()
{
	m_Container = new yathCThreadsContainer();
	m_PortThread = new yathCPortThread( this);
	m_ProxyThread = new yathCProxyThread( this);
	m_Container->AddThread( m_PortThread, FALSE);
	m_Container->AddThread( m_ProxyThread, FALSE);
}

yathCServer::~yathCServer()
{
	if ( m_Container != NULL) m_Container->Free();
}

void yathCServer::Run()
{
	if ( m_PortThread != NULL) m_PortThread->Resume();
	if ( m_ProxyThread != NULL) m_ProxyThread->Resume();
}

