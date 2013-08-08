#include "transactions.h"
#include <stdio.h>
#include <Shlwapi.h>


/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

yathCFileManagerTransaction::yathCFileManagerTransaction()
{
	m_FirstFileInfo = NULL;
	m_LastFileInfo = NULL;
	m_ScanFileInfo = NULL;

	m_Type = yathtaNONE;
	m_Flags = 0;
	
	{ 
		int I;
		for ( I = 0; I < yathFMPC; I++) m_Strings[ I] = NULL;
	}
}

/////////////////////////////////////////////////////////////

yathCFileManagerTransaction::~yathCFileManagerTransaction()
{
	ClearFileList();
	{ 
		int I;
		for ( I = 0; I < yathFMPC; I++) if ( m_Strings[ I] != NULL) delete m_Strings[ I];
	}
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::ClearFileList()
{
	while ( m_FirstFileInfo != NULL){
		m_ScanFileInfo = m_FirstFileInfo;
		m_FirstFileInfo = m_FirstFileInfo->Next;
		delete m_ScanFileInfo;
	}

	m_FirstFileInfo = NULL;
	m_LastFileInfo = NULL;
	m_ScanFileInfo = NULL;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::AppendFileInfo( yathFILEINFO &FileInfo)
{
	BOOL Result = FALSE;
	yathFILEINOFLIST *NewEntry = new yathFILEINOFLIST;

	if ( NewEntry != NULL){
		NewEntry->FileInfo = FileInfo;
		NewEntry->Next = NULL;
		if (( m_FirstFileInfo != NULL) && ( m_LastFileInfo != NULL)){
			NewEntry->Prev = m_LastFileInfo;
			m_LastFileInfo->Next = NewEntry;
		} else {
			NewEntry->Prev = NULL;
			m_FirstFileInfo = NewEntry;
		}
		m_LastFileInfo = NewEntry;

		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::GetFirstFileInfo( yathFILEINFO &FileInfo)
{
	BOOL Result = FALSE;
	
	m_ScanFileInfo = m_FirstFileInfo;
	
	if ( m_ScanFileInfo != NULL){
		FileInfo = m_ScanFileInfo->FileInfo;
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::GetLastFileInfo( yathFILEINFO &FileInfo)
{
	BOOL Result = FALSE;
	
	m_ScanFileInfo = m_LastFileInfo;
	
	if ( m_ScanFileInfo != NULL){
		FileInfo = m_ScanFileInfo->FileInfo;
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::GetNextFileInfo( yathFILEINFO &FileInfo)
{
	BOOL Result = FALSE;

	if ( m_ScanFileInfo != NULL){
		m_ScanFileInfo = m_ScanFileInfo->Next;
		if ( m_ScanFileInfo != NULL){
			FileInfo = m_ScanFileInfo->FileInfo;
			Result = TRUE;
		}
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::GetPrevFileInfo( yathFILEINFO &FileInfo)
{
	BOOL Result = FALSE;

	if ( m_ScanFileInfo != NULL){
		m_ScanFileInfo = m_ScanFileInfo->Prev;
		if ( m_ScanFileInfo != NULL){
			FileInfo = m_ScanFileInfo->FileInfo;
			Result = TRUE;
		}
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::GetCurrentFileInfo( yathFILEINFO &FileInfo)
{
	BOOL Result = FALSE;

	if ( m_ScanFileInfo != NULL){
		FileInfo = m_ScanFileInfo->FileInfo;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::GetFilesList( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *FileName, BOOL &ConnectionFatalError)
{
	BOOL Result = FALSE;

	ConnectionFatalError = TRUE;
	
	ClearFileList();

	if (( Connection != NULL) && ( Manager != NULL)){
		char Buffer[16 * 1024];
		// int ChunkSize;
		
		::yathInitChunk( Buffer, sizeof( Buffer), yathcmdLISTFILES);
		if ( FileName != NULL) ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), FileName, strlen( FileName) + 1);
		else ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), NULL, 0);
		
		::yathFinalizeChunk( Buffer, sizeof( Buffer));

		if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))){
			int ReceivedDataSize;

			if ( IsStopRequested()) goto Skip;

			while ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
				int FilesCount;
				int ChunkStamp;
				if ( IsStopRequested()) goto Skip;
				::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
				if ( ChunkStamp == yathcmdLISTFILES){
					::yathRetriveFieldsCount(  Buffer, ReceivedDataSize, FilesCount);
					if ( FilesCount > 0){
						yathFILEINFO *ScanFileInfo;
						int ScanFileInfoSize;
						int i;
						for ( i = 0; i < FilesCount; i++){
							if ( IsStopRequested()) goto Skip;
							::yathRetriveField( Buffer, ReceivedDataSize, i, (void **)&ScanFileInfo, ScanFileInfoSize);
							AppendFileInfo( *ScanFileInfo);
						}
						if ( IsStopRequested()) goto Skip;
						Manager->FlushOutputCatch();
					} else {
						Result = TRUE;
						break;
					}
				} else {
					Manager->WriteOutput( "ERROR: Server side error, unable to get files list\r\n");
					break;
				}
			}
		} else {
			Manager->WriteOutput( "ERROR: Unable to send the command to server\r\n");
			goto Skip;
		}
	}

	ConnectionFatalError = FALSE;

Skip:

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::GetLocalFilesList( yathCConnectionManager *Manager, const char *FileName)
{
	BOOL Result = FALSE;

	if ( Manager != NULL){

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;

		ClearFileList();

		if ( FileName == NULL) FileName = "*.*";

		if ( IsStopRequested()) goto Skip;
		
		hFind = FindFirstFile( FileName, &FindFileData);

		if (hFind != INVALID_HANDLE_VALUE) {	
			
			if ( !IsStopRequested()) {
			
				Result = TRUE;
				
				do {
					yathFILEINFO FileInfo;
					
					FileInfo.FileAttributes = FindFileData.dwFileAttributes;
					FileInfo.FileCreationTime = FindFileData.ftCreationTime;
					FileInfo.FileLastAcessTime = FindFileData.ftLastAccessTime;
					FileInfo.FileLastWriteTime = FindFileData.ftLastWriteTime;
					FileInfo.FileSizeLo = FindFileData.nFileSizeLow;
					FileInfo.FileSizeHi = FindFileData.nFileSizeHigh;
					strcpy( FileInfo.FileName, FindFileData.cFileName);

					if ( IsStopRequested()) {
						Result = FALSE;
						break;
					}

					AppendFileInfo( FileInfo);

				} while ( ::FindNextFile( hFind, &FindFileData) != 0);

			}

			FindClose(hFind);
		}
	}

Skip:

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::ChangeCurrentDir( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *Path, char *NewPath, int &NewPathSize, BOOL &ConnectionFatalError)
{
	BOOL Result = FALSE;

	ConnectionFatalError = TRUE;
	
	ClearFileList();

	if (( Connection != NULL) && ( Manager != NULL)){
		char Buffer[16 * 1024];
		
		::yathInitChunk( Buffer, sizeof( Buffer), yathcmdCHANGEDIR);
		if ( Path != NULL) ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), Path, strlen( Path) + 1);
		else ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), NULL, 0);
		
		::yathFinalizeChunk( Buffer, sizeof( Buffer));

		if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))){
			int ReceivedDataSize;

			if ( IsStopRequested()) goto Skip;

			if ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
				int FieldsCount;
				int ChunkStamp;
				if ( IsStopRequested()) goto Skip;
				::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
				if ( ChunkStamp == yathcmdCHANGEDIR){
					if (( NewPath != NULL) && ( NewPathSize > 0)){
						::yathRetriveFieldsCount(  Buffer, ReceivedDataSize, FieldsCount);
						if (( FieldsCount > 0)){
							char *ChangedToPath;
							int ChangedToPathSize;
							::yathRetriveField( Buffer, ReceivedDataSize, 0, (void **)&ChangedToPath, ChangedToPathSize);
							if ( ChangedToPathSize <= NewPathSize){
								memcpy( NewPath, ChangedToPath, ChangedToPathSize);
								NewPathSize = ChangedToPathSize;
								Result = TRUE;
							} else {
								if ( NewPathSize > 1) memcpy( NewPath, ChangedToPath, NewPathSize - 1);
								NewPath[ NewPathSize - 1] = 0;
							}
						} else {
							NewPath[ 0] = 0;
							NewPathSize = 0;
						}
					}
				} else Manager->WriteOutput( "ERROR: Server side error, unable to change current dir\r\n");					
			}
		} else {
			Manager->WriteOutput( "ERROR: Unable to send the command to server\r\n");
			goto Skip;
		}
	}

	ConnectionFatalError = FALSE;

Skip:

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::ChangeLocalCurrentDir( yathCConnectionManager *Manager, const char *DName, char *NewPath, int &NewPathSize)
{
	BOOL Result = FALSE;

	if (( Manager != NULL) && ( DName != NULL)){

		if ( DName != NULL){
			if ( !::SetCurrentDirectory( DName)){
				Manager->WriteOutputToCatch( "ERROR: Unable to change to local current directory \"");
				Manager->WriteOutputToCatch( DName);
				Manager->WriteOutputToCatch( "\"\r\n");
			}
		}

		if (( NewPath != NULL) && ( NewPathSize > 0)){
			DWORD CurrentDirLength = ::GetCurrentDirectory( NewPathSize, NewPath);
			if ( CurrentDirLength > 0) NewPathSize = CurrentDirLength + 1;
			else {
				NewPath[0] = '\0';
				NewPathSize = 0;
			}
		}

		Manager->FlushOutputCatch();
		
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::GetFile( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *RemoteFName, const char *LocalFName, BOOL &ConnectionFatalError)
{
	BOOL Result = FALSE;
	BOOL FileOpened = FALSE;
	HANDLE FileHandle;
	__int64 RecievedSize = 0;

	ConnectionFatalError = TRUE;
	
	if (( Connection != NULL) && ( Manager != NULL) && ( RemoteFName != NULL) && ( LocalFName != NULL)){
		char Buffer[16 * 1024];
		// int ChunkSize;
		
		FileHandle = ::CreateFile( LocalFName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if ( FileHandle != INVALID_HANDLE_VALUE){
			FileOpened = TRUE;
			
			Manager->WriteOutputToCatch( "GET \"");
			Manager->WriteOutputToCatch( RemoteFName);
			Manager->WriteOutputToCatch( "\" -> \"");
			Manager->WriteOutputToCatch( LocalFName);
			Manager->WriteOutputToCatch( "\"\r\n");
			Manager->FlushOutputCatch();

			::yathInitChunk( Buffer, sizeof( Buffer), yathcmdGETFILE);
			::yathAppendFieldToChunk( Buffer, sizeof( Buffer), RemoteFName, strlen( RemoteFName) + 1);
			::yathFinalizeChunk( Buffer, sizeof( Buffer));
			// ::yathRetriveChunkSize( Buffer, sizeof( Buffer), ChunkSize);
			if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))){
				BOOL PerformWrite = TRUE;
				int ReceivedDataSize;
				if ( IsStopRequested()) goto Skip;
				while ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
					// <<<>>> nust be changed from WHILE loop to something else
					int ChunkStamp;
					if ( IsStopRequested()) goto Skip;
					::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
					if ( ChunkStamp == yathcmdGETFILE){
						int FieldsCount;
						::yathRetriveFieldsCount(  Buffer, ReceivedDataSize, FieldsCount);
						if ( FieldsCount > 0){
							char *DataPtr;
							int DataSize;
							::yathRetriveField( Buffer, ReceivedDataSize, 0, (void **)&DataPtr, DataSize);
							if ( DataSize > 0){
								DWORD WrittenSize;
								if ( PerformWrite){
									::WriteFile( FileHandle, DataPtr, DataSize, &WrittenSize, NULL);
									RecievedSize += WrittenSize;
									if ( ((DWORD )DataSize) != WrittenSize) {
										Manager->WriteOutput( "ERROR: Unable to complete local file creation\r\n");
										PerformWrite = FALSE;
									}
								}
							} else {
								Result = !PerformWrite;
								break;
							}
						} else break;				
					} else {
						Manager->WriteOutput( "ERROR: Server side error, unable to get file data\r\n");
						break;
					}			
				}
				{
					char Message[30];
					sprintf( Message, "%d", RecievedSize);
					Manager->WriteOutputToCatch( Message);
				}
				Manager->WriteOutputToCatch( " Bytes transferred \r\n");
				Manager->FlushOutputCatch();
			} else {
				Manager->WriteOutput( "ERROR: Unable to send the command to server\r\n");
				goto Skip;
			}
		} else {
			Manager->WriteOutputToCatch( "ERROR: Unable to create local file \"");
			Manager->WriteOutputToCatch( LocalFName);
			Manager->WriteOutputToCatch( "\"\r\n");
			Manager->FlushOutputCatch();
		}
	}

	ConnectionFatalError = FALSE;
Skip:	
	if ( FileOpened) ::CloseHandle( FileHandle);
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::PutFile( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *LocalFName, const char *RemoteFName, BOOL &ConnectionFatalError)
{
	BOOL Result = FALSE;
	BOOL FileOpened = FALSE;
	HANDLE FileHandle;
	__int64 RecievedSize = 0;

	ConnectionFatalError = TRUE;
	
	if (( Connection != NULL) && ( Manager != NULL) && ( RemoteFName != NULL) && ( LocalFName != NULL)){
		char Buffer[16 * 1024];
		
		FileHandle = CreateFile( LocalFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if ( FileHandle != INVALID_HANDLE_VALUE){
			int ReceivedDataSize;
			
			FileOpened = TRUE;
			
			Manager->WriteOutputToCatch( "PUT \"");
			Manager->WriteOutputToCatch( LocalFName);
			Manager->WriteOutputToCatch( "\" -> \"");
			Manager->WriteOutputToCatch( RemoteFName);
			Manager->WriteOutputToCatch( "\"\r\n");
			Manager->FlushOutputCatch();

			::yathInitChunk( Buffer, sizeof( Buffer), yathcmdPUTFILE);
			::yathAppendFieldToChunk( Buffer, sizeof( Buffer), RemoteFName, strlen( RemoteFName) + 1);
			::yathFinalizeChunk( Buffer, sizeof( Buffer));
			if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))){
				if ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
					int ChunkStamp;
					::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
					if ( ChunkStamp == yathcmdPUTFILE){
						char ReadBuffer[ 1024 * 8];
						DWORD BytesRead;
						for (;;){
							if ( ReadFile( FileHandle, ReadBuffer, sizeof( ReadBuffer), &BytesRead, NULL)){
								::yathInitChunk( Buffer, sizeof( Buffer), yathcmdPUTFILE);
								::yathAppendFieldToChunk( Buffer, sizeof( Buffer), ReadBuffer, BytesRead);
								::yathFinalizeChunk( Buffer, sizeof( Buffer));
								if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))) {
									if ( BytesRead == 0) {
										if ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
											int ChunkStamp;
											::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
											if ( ChunkStamp == yathcmdPUTFILE){
												__int64 *WrittenSizeField;
												int WrittenSizeFieldSize;
												if ( ::yathRetriveField( Buffer, sizeof( Buffer), 0, (void **)&WrittenSizeField, WrittenSizeFieldSize)){
													if ( WrittenSizeFieldSize == sizeof( __int64)){
														char Message[30];
														sprintf( Message, "%d", *WrittenSizeField);
														Manager->WriteOutputToCatch( Message);
														Manager->WriteOutputToCatch( " Bytes transferred \r\n");
														Manager->FlushOutputCatch();
													}
												}
											}
										}
										Result = TRUE;
										break;
									}
								} else {
									Manager->WriteOutput( "ERROR: Unable to send data to the server\r\n");
									goto Skip;
								}
							} else {
								::yathInitChunk( Buffer, sizeof( Buffer), yathcmdERROR);
								::yathFinalizeChunk( Buffer, sizeof( Buffer));
								Manager->WriteOutput( "ERROR: Unable to read the data file\r\n");
								if ( Connection->SendDataBlock( Buffer, sizeof( Buffer))) {
									Manager->WriteOutput( "ERROR: Unable to send operation terminate command\r\n");
									goto Skip;
								}
								break;
							}
						}
					} else Manager->WriteOutput( "ERROR: Server side error, unable to put file\r\n");
				} else {
					Manager->WriteOutput( "ERROR: Unable to receive server's response\r\n");
					goto Skip;				
				}
			} else {
				Manager->WriteOutput( "ERROR: Unable to send a command to server\r\n");
				goto Skip;				
			}
		} else {
			Manager->WriteOutputToCatch( "ERROR: Unable to open local file \"");
			Manager->WriteOutputToCatch( LocalFName);
			Manager->WriteOutputToCatch( "\"\r\n");
			Manager->FlushOutputCatch();
		}
	}

	ConnectionFatalError = FALSE;
Skip:	
	if ( FileOpened) ::CloseHandle( FileHandle);
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::MoveFile( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *SrcFName, const char *DesFName, BOOL &ConnectionFatalError)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL)){
		char Buffer[16 * 1024];

		Manager->WriteOutputToCatch( "MOVE \"");
		Manager->WriteOutputToCatch( SrcFName);
		Manager->WriteOutputToCatch( "\" -> \"");
		Manager->WriteOutputToCatch( DesFName);
		Manager->WriteOutputToCatch( "\"\r\n");
		Manager->FlushOutputCatch();

		::yathInitChunk( Buffer, sizeof( Buffer), yathcmdMOVE);

		if ( SrcFName != NULL) ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), SrcFName, strlen( SrcFName) + 1);
		else ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), NULL, 0);

		if ( DesFName != NULL) ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), DesFName, strlen( DesFName) + 1);
		else ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), NULL, 0);
		
		::yathFinalizeChunk( Buffer, sizeof( Buffer));

		if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))){
			int ReceivedDataSize;

			if ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
				int ChunkStamp;
				::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
				if ( ChunkStamp != yathcmdMOVE) Manager->WriteOutput( "ERROR: Server side error, unable to move file\r\n");					
			} else {
				Manager->WriteOutput( "ERROR: Unable to receive command response\r\n");
				goto Skip;
			}
		} else {
			Manager->WriteOutput( "ERROR: Unable to send the command to server\r\n");
			goto Skip;
		}
	}

	Result = TRUE;

Skip:

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::MoveLocalFile( yathCConnectionManager *Manager, const char *SrcFName, const char *DesFName)
{
	BOOL Result = FALSE;

	if (( Manager != NULL) && ( SrcFName != NULL) && ( DesFName != NULL)){

		if ( ::MoveFile( SrcFName, DesFName)){
			Manager->WriteOutputToCatch( "The local file \"");
			Manager->WriteOutputToCatch( SrcFName);
			Manager->WriteOutputToCatch( "\" was moved to \"");
			Manager->WriteOutputToCatch( DesFName);
			Manager->WriteOutputToCatch( "\" successfully\r\n");
		} else {
			Manager->WriteOutputToCatch( "ERROR: Unable to move the local file \"");
			Manager->WriteOutputToCatch( SrcFName);
			Manager->WriteOutputToCatch( "\" to \"");
			Manager->WriteOutputToCatch( DesFName);
			Manager->WriteOutputToCatch( "\"\r\n");
		}

		Manager->FlushOutputCatch();
		
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::RemoveFile( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *FName, BOOL &ConnectionFatalError)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL)){
		char Buffer[16 * 1024];
		
		Manager->WriteOutputToCatch( "REMOVE FILE \"");
		Manager->WriteOutputToCatch( FName);
		Manager->WriteOutputToCatch( "\"\r\n");
		Manager->FlushOutputCatch();

		::yathInitChunk( Buffer, sizeof( Buffer), yathcmdREMOVEFILE);

		if ( FName != NULL) ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), FName, strlen( FName) + 1);
		else ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), NULL, 0);
	
		::yathFinalizeChunk( Buffer, sizeof( Buffer));

		if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))){
			int ReceivedDataSize;

			if ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
				int ChunkStamp;
				::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
				if ( ChunkStamp != yathcmdREMOVEFILE) Manager->WriteOutput( "ERROR: Server side error, unable to remove file\r\n");					
			} else {
				Manager->WriteOutput( "ERROR: Unable to receive command response\r\n");
				goto Skip;
			}
		} else {
			Manager->WriteOutput( "ERROR: Unable to send the command to server\r\n");
			goto Skip;
		}
	}

	Result = TRUE;

Skip:

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::RemoveLocalFile( yathCConnectionManager *Manager, const char *FName)
{
	BOOL Result = FALSE;

	if (( Manager != NULL) && ( FName != NULL)){

		if ( ::DeleteFile( FName)){
			Manager->WriteOutputToCatch( "The local file \"");
			Manager->WriteOutputToCatch( FName);
			Manager->WriteOutputToCatch( "\" was removed successfully\r\n");
		} else {
			Manager->WriteOutputToCatch( "ERROR: Unable to remove the local file \"");
			Manager->WriteOutputToCatch( FName);
			Manager->WriteOutputToCatch( "\"\r\n");
		}

		Manager->FlushOutputCatch();
		
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::RemoveDir( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *DName, BOOL &ConnectionFatalError)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL)){
		char Buffer[16 * 1024];
		
		Manager->WriteOutputToCatch( "REMOVE DIR \"");
		Manager->WriteOutputToCatch( DName);
		Manager->WriteOutputToCatch( "\"\r\n");
		Manager->FlushOutputCatch();

		::yathInitChunk( Buffer, sizeof( Buffer), yathcmdREMOVEDIR);

		if ( DName != NULL) ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), DName, strlen( DName) + 1);
		else ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), NULL, 0);
	
		::yathFinalizeChunk( Buffer, sizeof( Buffer));

		if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))){
			int ReceivedDataSize;

			if ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
				int ChunkStamp;
				::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
				if ( ChunkStamp != yathcmdREMOVEDIR) Manager->WriteOutput( "ERROR: Server side error, unable to remove directory\r\n");					
			} else {
				Manager->WriteOutput( "ERROR: Unable to receive command response\r\n");
				goto Skip;
			}
		} else {
			Manager->WriteOutput( "ERROR: Unable to send the command to server\r\n");
			goto Skip;
		}
	}

	Result = TRUE;

Skip:

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::RemoveLocalDir( yathCConnectionManager *Manager, const char *DName)
{
	BOOL Result = FALSE;

	if (( Manager != NULL) && ( DName != NULL)){

		if ( ::RemoveDirectory( DName)){
			Manager->WriteOutputToCatch( "The local drirectory \"");
			Manager->WriteOutputToCatch( DName);
			Manager->WriteOutputToCatch( "\" was removed successfully\r\n");
		} else {
			Manager->WriteOutputToCatch( "ERROR: Unable to remove the local directory \"");
			Manager->WriteOutputToCatch( DName);
			Manager->WriteOutputToCatch( "\"\r\n");
		}

		Manager->FlushOutputCatch();
		
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::CreateDir( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *DName, BOOL &ConnectionFatalError)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL)){
		char Buffer[16 * 1024];
		
		Manager->WriteOutputToCatch( "CREATE DIR \"");
		Manager->WriteOutputToCatch( DName);
		Manager->WriteOutputToCatch( "\"\r\n");
		Manager->FlushOutputCatch();

		::yathInitChunk( Buffer, sizeof( Buffer), yathcmdMAKEDIR);

		if ( DName != NULL) ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), DName, strlen( DName) + 1);
		else ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), NULL, 0);
	
		::yathFinalizeChunk( Buffer, sizeof( Buffer));

		if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))){
			int ReceivedDataSize;

			if ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
				int ChunkStamp;
				::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
				if ( ChunkStamp != yathcmdMAKEDIR) Manager->WriteOutput( "ERROR: Server side error, unable to create directory\r\n");					
			} else {
				Manager->WriteOutput( "ERROR: Unable to receive command response\r\n");
				goto Skip;
			}
		} else {
			Manager->WriteOutput( "ERROR: Unable to send the command to server\r\n");
			goto Skip;
		}
	}

	Result = TRUE;

Skip:

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::CreateLocalDir( yathCConnectionManager *Manager, const char *DName)
{
	BOOL Result = FALSE;

	if (( Manager != NULL) && ( DName != NULL)){

		if ( ::CreateDirectory( DName, NULL)){
			Manager->WriteOutputToCatch( "The local drirectory \"");
			Manager->WriteOutputToCatch( DName);
			Manager->WriteOutputToCatch( "\" was created successfully\r\n");		
		} else {
			Manager->WriteOutputToCatch( "ERROR: Unable to create the local directory \"");
			Manager->WriteOutputToCatch( DName);
			Manager->WriteOutputToCatch( "\"\r\n");
		}

		Manager->FlushOutputCatch();
		
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::StopRequest()
{
	yathCTransaction::StopRequest();
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetString( int PathNo, const char *Path)
{
	if ( m_Strings[ PathNo] != NULL) delete m_Strings[ PathNo];

	if ( Path != NULL) m_Strings[ PathNo] = strdup( Path);
	else m_Strings[ PathNo] = NULL;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_rcd( const char *Path, DWORD Flags)
{
	SetString( 0, Path);
	m_Flags = Flags;
	m_Type = yathtaRCD;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_lcd( const char *Path, DWORD Flags)
{
	SetString( 0, Path);
	m_Flags = Flags;
	m_Type = yathtaLCD;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_rls( const char *Path, DWORD Flags)
{
	SetString( 0, Path);
	m_Flags = Flags;
	m_Type = yathtaRLS;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_lls( const char *Path, DWORD Flags)
{
	SetString( 0, Path);
	m_Flags = Flags;
	m_Type = yathtaLLS;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_rtype( const char *FName, DWORD Flags)
{
	SetString( 0, FName);
	m_Flags = Flags;
	m_Type = yathtaRTYPE;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_rmove( const char *SrcFName, const char *DestFName, DWORD Flags)
{
	SetString( 0, SrcFName);
	SetString( 1, DestFName);
	m_Flags = Flags;
	m_Type = yathtaRMOVE;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_lmove( const char *SrcFName, const char *DestFName, DWORD Flags)
{
	SetString( 0, SrcFName);
	SetString( 1, DestFName);
	m_Flags = Flags;
	m_Type = yathtaLMOVE;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_rmkdir( const char *DName, DWORD Flags)
{
	SetString( 0, DName);
	m_Flags = Flags;
	m_Type = yathtaRMKDIR;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_lmkdir( const char *DName, DWORD Flags)
{
	SetString( 0, DName);
	m_Flags = Flags;
	m_Type = yathtaLMKDIR;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_rrmfile( const char *FName, DWORD Flags)
{
	SetString( 0, FName);
	m_Flags = Flags;
	m_Type = yathtaRRMFILE;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_lrmfile( const char *FName, DWORD Flags)
{
	SetString( 0, FName);
	m_Flags = Flags;
	m_Type = yathtaLRMFILE;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_rrmdir( const char *DName, DWORD Flags)
{
	SetString( 0, DName);
	m_Flags = Flags;
	m_Type = yathtaRRMDIR;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_lrmdir( const char *DName, DWORD Flags)
{
	SetString( 0, DName);
	m_Flags = Flags;
	m_Type = yathtaLRMDIR;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_get( const char *SrcFName, const char *DestFName, DWORD Flags)
{
	SetString( 0, SrcFName);
	SetString( 1, DestFName);
	m_Flags = Flags;
	m_Type = yathtaGET;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_put( const char *SrcFName, const char *DestFName, DWORD Flags)
{
	SetString( 0, SrcFName);
	SetString( 1, DestFName);
	m_Flags = Flags;
	m_Type = yathtaPUT;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_mget( const char *SrcFName, DWORD Flags)
{
	SetString( 0, SrcFName);
	m_Flags = Flags;
	m_Type = yathtaMGET;
}

/////////////////////////////////////////////////////////////

void yathCFileManagerTransaction::SetType_mput( const char *SrcFName, DWORD Flags)
{
	SetString( 0, SrcFName);
	m_Flags = Flags;
	m_Type = yathtaMPUT;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_rcd( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL)){
		char NewPath[ MAX_PATH];
		int NewPathSize = sizeof( NewPath);
		BOOL ConnectionFatalError;
		if ( ChangeCurrentDir( Connection, Manager, m_Strings[ 0], NewPath, NewPathSize, ConnectionFatalError)){
			if ( NewPathSize > 0){
				Manager->WriteOutputToCatch( "Server's current directory : \"");
				Manager->WriteOutputToCatch( NewPath);
				Manager->WriteOutputToCatch( "\"\r\n\r\n");
				Manager->FlushOutputCatch();
			} else {
				Manager->WriteOutputToCatch( "Unable to retrive server's current directory\r\n");
				Manager->FlushOutputCatch();
			}
		} else {
			if ( ConnectionFatalError) goto Skip;
		}

		Result = TRUE;
	}
Skip:	
	return Result;	
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_lcd( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL)){
		char NewPath[ MAX_PATH];
		int NewPathSize = sizeof( NewPath);
		
		if ( ChangeLocalCurrentDir( Manager, m_Strings[ 0], NewPath, NewPathSize)){
			if ( NewPathSize > 0){
				Manager->WriteOutputToCatch( "Local current directory : \"");
				Manager->WriteOutputToCatch( NewPath);
				Manager->WriteOutputToCatch( "\"\r\n\r\n");
				Manager->FlushOutputCatch();
			} else {
				Manager->WriteOutputToCatch( "Unable to retrive local current directory\r\n");
				Manager->FlushOutputCatch();
			}
		}

		Result = TRUE;
	}

	
	return Result;	
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_rls( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;
	
	if (( Connection != NULL) && ( Manager != NULL)){
		BOOL ConnectionFatalError;
		char *Mask = ( char *)::yathPathFindFileName( m_Strings[0]);

		Manager->WriteOutput( "The file mask : \"");
		Manager->WriteOutputToCatch( Mask);
		Manager->WriteOutput( "\"\r\n");


		if ( GetFilesList( Connection, Manager, m_Strings[ 0], ConnectionFatalError)){
			if ( IsStopRequested()) goto Skip;
			yathFILEINFO ScanFileInfo;
			if ( GetFirstFileInfo( ScanFileInfo)){
				char OutputStr[1024];
				int CatchedLines = 0;
				int Count = 0;
				__int64 SizeSum = 0;
				do {
					if ( IsStopRequested()) goto Skip;
					sprintf( OutputStr, "--------- %14d \"%s\"\r\n", ScanFileInfo.FileSizeLo, ScanFileInfo.FileName);
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_DIRECTORY) OutputStr[0] = 'd';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_ARCHIVE) OutputStr[1] = 'a';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_COMPRESSED) OutputStr[2] = 'c';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_ENCRYPTED) OutputStr[3] = 'e';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_HIDDEN) OutputStr[4] = 'h';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_OFFLINE) OutputStr[5] = 'o';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_READONLY) OutputStr[6] = 'r';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_SYSTEM) OutputStr[7] = 's';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_TEMPORARY) OutputStr[8] = 't';
					if ( IsStopRequested()) goto Skip;
					Manager->WriteOutputToCatch( OutputStr);
					if ( IsStopRequested()) goto Skip;
					CatchedLines++;
					if ( CatchedLines >= 100) {
						Manager->FlushOutputCatch();
						CatchedLines = 0;
					}
					Count ++;
					SizeSum += ScanFileInfo.FileSizeLo;
					if ( IsStopRequested()) goto Skip;
				} while ( GetNextFileInfo( ScanFileInfo));
				if ( IsStopRequested()) goto Skip;
				Manager->FlushOutputCatch();
				sprintf( OutputStr, "\r\nFiles listed: %d\r\nSize listed: %d\r\n\r\n", Count, SizeSum);
				Manager->WriteOutput( OutputStr);
			}
		} else {
			if ( ConnectionFatalError) goto Skip;
		}

		Result = TRUE;
	}

Skip:	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_lls( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;
	
	if (( Connection != NULL) && ( Manager != NULL)){
		char *Mask = ( char *)::yathPathFindFileName( m_Strings[ 0]);

		Manager->WriteOutput( "The file mask : \"");
		Manager->WriteOutputToCatch( Mask);
		Manager->WriteOutput( "\"\r\n");


		if ( GetLocalFilesList( Manager, m_Strings[ 0])){
			if ( IsStopRequested()) goto Skip;
			yathFILEINFO ScanFileInfo;
			if ( GetFirstFileInfo( ScanFileInfo)){
				char OutputStr[1024];
				int CatchedLines = 0;
				int Count = 0;
				__int64 SizeSum = 0;
				do {
					if ( IsStopRequested()) goto Skip;
					sprintf( OutputStr, "--------- %14d \"%s\"\r\n", ScanFileInfo.FileSizeLo, ScanFileInfo.FileName);
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_DIRECTORY) OutputStr[0] = 'd';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_ARCHIVE) OutputStr[1] = 'a';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_COMPRESSED) OutputStr[2] = 'c';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_ENCRYPTED) OutputStr[3] = 'e';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_HIDDEN) OutputStr[4] = 'h';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_OFFLINE) OutputStr[5] = 'o';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_READONLY) OutputStr[6] = 'r';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_SYSTEM) OutputStr[7] = 's';
					if ( ScanFileInfo.FileAttributes & FILE_ATTRIBUTE_TEMPORARY) OutputStr[8] = 't';
					if ( IsStopRequested()) goto Skip;
					Manager->WriteOutputToCatch( OutputStr);
					if ( IsStopRequested()) goto Skip;
					CatchedLines++;
					if ( CatchedLines >= 100) {
						Manager->FlushOutputCatch();
						CatchedLines = 0;
					}
					Count ++;
					SizeSum += ScanFileInfo.FileSizeLo;
					if ( IsStopRequested()) goto Skip;
				} while ( GetNextFileInfo( ScanFileInfo));
				if ( IsStopRequested()) goto Skip;
				Manager->FlushOutputCatch();
				sprintf( OutputStr, "\r\nFiles listed: %d\r\nSize listed: %d\r\n\r\n", Count, SizeSum);
				Manager->WriteOutput( OutputStr);
			}
		} else {
			if ( IsStopRequested()) goto Skip;
			Manager->WriteOutputToCatch( "ERROR: Bad file name/mask :\"");
			Manager->WriteOutputToCatch( m_Strings[0]);
			Manager->WriteOutputToCatch( "\"\r\n");
			Manager->FlushOutputCatch();
		}

		Result = TRUE;
	}

Skip:	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_rtype( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;
	
	if (( Connection != NULL) && ( Manager != NULL)){
		char Buffer[16 * 1024];

		::yathInitChunk( Buffer, sizeof( Buffer), yathcmdGETFILE);
		if ( m_Strings[0] != NULL) ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), m_Strings[0], strlen( m_Strings[0]) + 1);
		else ::yathAppendFieldToChunk( Buffer, sizeof( Buffer), NULL, 0);
		::yathFinalizeChunk( Buffer, sizeof( Buffer));
		if ( !Connection->SendDataBlock( Buffer, sizeof( Buffer))){
			int ReceivedDataSize;

			if ( IsStopRequested()) goto Skip;

			Manager->WriteOutputToCatch( "\r\n\r\n<START OF \"");
			Manager->WriteOutputToCatch( m_Strings[0]);
			Manager->WriteOutputToCatch( "\">\r\n");
			Manager->FlushOutputCatch();

			while ( !Connection->ReadDataBlock( Buffer, sizeof( Buffer), ReceivedDataSize)){
				int ChunkStamp;
				if ( IsStopRequested()) goto Skip;
				::yathRetriveChunkStamp( Buffer, sizeof( Buffer), ChunkStamp);
				if ( ChunkStamp == yathcmdGETFILE){
					int FieldsCount;
					::yathRetriveFieldsCount(  Buffer, ReceivedDataSize, FieldsCount);
					if ( FieldsCount > 0){
						char *DataPtr;
						int DataSize;
						::yathRetriveField( Buffer, ReceivedDataSize, 0, (void **)&DataPtr, DataSize);
						if ( DataSize > 0){
							// <<<>>> Must be changed
							DataPtr[ DataSize] = 0;
							Manager->WriteOutput( DataPtr);
						} else {
							Manager->WriteOutputToCatch( "\r\n<END OF \"");
							Manager->WriteOutputToCatch( m_Strings[0]);
							Manager->WriteOutputToCatch( "\">\r\n\r\n");
							Manager->FlushOutputCatch();
							break;
						}
					} else break;				
				} else {
					Manager->WriteOutput( "ERROR: Server side error, unable to get file data\r\n");
					break;
				}
			}
		} else {
			Manager->WriteOutput( "ERROR: Unable to send the command to server\r\n");
			goto Skip;
		}

		Result = TRUE;
	}

Skip:	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_get( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL) && ( m_Strings[1] != NULL)){
		BOOL ConnectionFatalError;
		
		GetFile( Connection, Manager, m_Strings[0], m_Strings[1], ConnectionFatalError);
		Manager->WriteOutput( "\r\n");
		if ( ConnectionFatalError) goto Skip;

		Result = TRUE;
	}

Skip:	
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_put( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL) && ( m_Strings[1] != NULL)){
		BOOL ConnectionFatalError;

		PutFile( Connection, Manager, m_Strings[0], m_Strings[1], ConnectionFatalError);
		Manager->WriteOutput( "\r\n");
		if ( ConnectionFatalError) goto Skip;

		Result = TRUE;
	}

Skip:	
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_mget( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL) /*&& ( m_Strings[1] != NULL)*/){
		BOOL ConnectionFatalError;

		if ( GetFilesList( Connection, Manager, m_Strings[ 0], ConnectionFatalError)){
			char FullFileName[ MAX_PATH];
			::strcpy(  FullFileName, m_Strings[ 0]);
			char *FileNameStart = (char *)::yathPathFindFileName( FullFileName);

			if ( FileNameStart != NULL){
				if ( IsStopRequested()) goto Skip;
				yathFILEINFO ScanFileInfo;
				
				if ( GetFirstFileInfo( ScanFileInfo)){
					do {
						if ( IsStopRequested()) goto Skip;
						*FileNameStart = '\0';
						if ( ::yathPathAppend( FullFileName, ScanFileInfo.FileName, sizeof( FullFileName))){
							if ( !GetFile( Connection, Manager, FullFileName, ScanFileInfo.FileName, ConnectionFatalError)){
								if ( ConnectionFatalError) goto Skip;
							}
						} else {
							Manager->WriteOutputToCatch( "ERROR: Unable to form full file name for :\"");
							Manager->WriteOutputToCatch( ScanFileInfo.FileName);
							Manager->WriteOutputToCatch( "\"\r\n");
							Manager->FlushOutputCatch();
						}
					} while ( GetNextFileInfo( ScanFileInfo));
				}
			} else {
				Manager->WriteOutputToCatch( "ERROR: Bad file name/mask :\"");
				Manager->WriteOutputToCatch( FullFileName);
				Manager->WriteOutputToCatch( "\"\r\n");
				Manager->FlushOutputCatch();
			}
		} else {
			if ( ConnectionFatalError) goto Skip;
		}
		
		Manager->WriteOutput( "\r\n");
		if ( ConnectionFatalError) goto Skip;

		Result = TRUE;
	}

Skip:	
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_mput( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL)){
		
		if ( GetLocalFilesList( Manager, m_Strings[0])){
			char FullFileName[ MAX_PATH];
			::strcpy(  FullFileName, m_Strings[ 0]);
			char *FileNameStart = (char *)::yathPathFindFileName( FullFileName);
			yathFILEINFO ScanFileInfo;
		
			if ( IsStopRequested()) goto Skip;

			if ( GetFirstFileInfo( ScanFileInfo)){
				BOOL ConnectionFatalError;
				do {
					if ( IsStopRequested()) goto Skip;
					*FileNameStart = '\0';
					if ( ::yathPathAppend( FullFileName, ScanFileInfo.FileName, sizeof( FullFileName))){
						if ( !PutFile( Connection, Manager, FullFileName, ScanFileInfo.FileName, ConnectionFatalError)){
							if ( ConnectionFatalError) goto Skip;
						}
					} else {
						Manager->WriteOutputToCatch( "ERROR: Unable to form full file name for :\"");
						Manager->WriteOutputToCatch( ScanFileInfo.FileName);
						Manager->WriteOutputToCatch( "\"\r\n");
						Manager->FlushOutputCatch();
					}
				} while ( GetNextFileInfo( ScanFileInfo));
			}
		} else {
			if ( IsStopRequested()) goto Skip;
			Manager->WriteOutputToCatch( "ERROR: Bad file name/mask :\"");
			Manager->WriteOutputToCatch( m_Strings[0]);
			Manager->WriteOutputToCatch( "\"\r\n");
			Manager->FlushOutputCatch();
		}

		Manager->WriteOutput( "\r\n");

		Result = TRUE;
	}

Skip:	
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_rmv( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL) && ( m_Strings[1] != NULL)){
		BOOL ConnectionFatalError;

		MoveFile( Connection, Manager, m_Strings[0], m_Strings[1], ConnectionFatalError);
		Manager->WriteOutput( "\r\n");
		if ( ConnectionFatalError) goto Skip;

		Result = TRUE;
	}

Skip:	
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_lmv( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;
	
	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL) && ( m_Strings[1] != NULL)){

		MoveLocalFile( Manager, m_Strings[0], m_Strings[1]);
		Manager->WriteOutput( "\r\n");
		
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_rrmfile( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL)){
		BOOL ConnectionFatalError;

		RemoveFile( Connection, Manager, m_Strings[0], ConnectionFatalError);
		Manager->WriteOutput( "\r\n");
		if ( ConnectionFatalError) goto Skip;

		Result = TRUE;
	}

Skip:	
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_lrmfile( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;
	
	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL)){

		RemoveLocalFile( Manager, m_Strings[0]);
		Manager->WriteOutput( "\r\n");
		
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_rrmdir( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL)){
		BOOL ConnectionFatalError;

		RemoveDir( Connection, Manager, m_Strings[0], ConnectionFatalError);
		Manager->WriteOutput( "\r\n");
		
		if ( ConnectionFatalError) goto Skip;

		Result = TRUE;
	}

Skip:	
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_lrmdir( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;
	
	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL)){

		RemoveLocalDir( Manager, m_Strings[0]);
		Manager->WriteOutput( "\r\n");
		
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_rmkdir( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL)){
		BOOL ConnectionFatalError;

		CreateDir( Connection, Manager, m_Strings[0], ConnectionFatalError);
		Manager->WriteOutput( "\r\n");
		
		if ( ConnectionFatalError) goto Skip;

		Result = TRUE;
	}

Skip:	
	
	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do_lmkdir( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;
	
	if (( Connection != NULL) && ( Manager != NULL) && ( m_Strings[0] != NULL)){

		CreateLocalDir( Manager, m_Strings[0]);
		Manager->WriteOutput( "\r\n");
		
		Result = TRUE;
	}

	return Result;
}

/////////////////////////////////////////////////////////////

BOOL yathCFileManagerTransaction::Do( yathCTCPConnection *Connection, yathCConnectionManager *Manager)
{
	BOOL Result = FALSE;

	if (( Connection != NULL) && ( Manager != NULL)){
		
		switch ( m_Type){
		case yathtaRCD:
			Result = Do_rcd( Connection, Manager);
			break;
		case yathtaLCD:
			Result = Do_lcd( Connection, Manager);
			break;
		case yathtaRLS:
			Result = Do_rls( Connection, Manager);
			break;
		case yathtaLLS:
			Result = Do_lls( Connection, Manager);
			break;
		case yathtaRMOVE:
			Result = Do_rmv( Connection, Manager);
			break;
		case yathtaLMOVE:
			Result = Do_lmv( Connection, Manager);
			break;
		case yathtaRRMFILE:
			Result = Do_rrmfile( Connection, Manager);
			break;
		case yathtaLRMFILE:
			Result = Do_lrmfile( Connection, Manager);
			break;
		case yathtaRRMDIR:
			Result = Do_rrmdir( Connection, Manager);
			break;
		case yathtaLRMDIR:
			Result = Do_lrmdir( Connection, Manager);
			break;
		case yathtaRMKDIR:
			Result = Do_rmkdir( Connection, Manager);
			break;
		case yathtaLMKDIR:
			Result = Do_lmkdir( Connection, Manager);
			break;
		case yathtaRTYPE:
			Result = Do_rtype( Connection, Manager);
			break;
		case yathtaGET:
			Result = Do_get( Connection, Manager);
			break;
		case yathtaPUT:
			Result = Do_put( Connection, Manager);
			break;
		case yathtaMGET:
			Result = Do_mget( Connection, Manager);
			break;
		case yathtaMPUT:
			Result = Do_mput( Connection, Manager);
			break;
		}
	}

	return Result;
}

/************************************************************
*                                                           *
*                                                           *
*                                                           *
************************************************************/

