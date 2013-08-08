#ifndef _CLASSES_H_
#define _CLASSES_H_

/////////////////////////////////////////////////////////////

#include <windows.h>
#include <winsock.h>

/////////////////////////////////////////////////////////////

typedef struct yathtagDATAHDR {
	int DataSize;
	int DataSizeCRC;
	int DataCRC;
} yathDATAHDR;

/////////////////////////////////////////////////////////////

typedef struct yathtagCHUNKHDR {
	yathDATAHDR DataHeader;
	int ChunkStamp;
	int ChunkFieldsCount;
} yathCHUNKHDR;

/////////////////////////////////////////////////////////////

typedef struct yathtagFIELDHDR {
	int FieldDataSize;
} yathFIELDHDR;

/////////////////////////////////////////////////////////////

typedef struct yathtagFIELDREC {
	void *FieldData;
	int FieldDataSize;
} yathFIELDREC;

/////////////////////////////////////////////////////////////

class yathCGeneric
{
public:
	yathCGeneric();
	virtual ~yathCGeneric();
	virtual void Free();
};

/////////////////////////////////////////////////////////////

class yathCCriticalSection: public yathCGeneric
{
private:
	CRITICAL_SECTION m_Section;
public:
	yathCCriticalSection();
	virtual ~yathCCriticalSection();
	void Enter();
	void Leave();

	// BOOL TryToEnter();
};

/////////////////////////////////////////////////////////////

class yathCUDPListener: public yathCGeneric
{
private:
	int m_Port;
	SOCKET m_Socket;
	void *m_Buffer;
	int m_BufferSize;
	SOCKADDR_IN m_Sender, m_Local;
	DWORD m_SenderSize;
	int m_RecvdSize;
	int m_LastStepDone;
	BOOL m_DataRecvd;
	int m_LastSockError;
	yathCCriticalSection m_CriticalSection;
public:
	yathCUDPListener( int Port, int BufferSize);
	~yathCUDPListener();
	int Initialize();	
	int Done();
	int Listen(void *Buffer, int BufferSize, int &DataSize);
	int GetLastError();
};

/////////////////////////////////////////////////////////////

class yathCUDPSender: public yathCGeneric
{
private:
	int m_LastStepDone;
	int m_LastSockError;
	SOCKET m_Socket;
	yathCCriticalSection m_CriticalSection;
public:
	yathCUDPSender();
	virtual ~yathCUDPSender();
	int Send( char *Address, int Port, void *Buffer, int BufferSize, int &SentSize);
};

/////////////////////////////////////////////////////////////

class yathCTCPConnection: public yathCGeneric
{
private:
	BOOL m_ServerSide;
	SOCKET m_Socket;
	SOCKADDR_IN m_SenderAddr;
	int m_Port;
	// char *m_IP;
	unsigned long m_Address;
	int m_SenderAddrLen;
	int m_LastSockError;
	int m_LastStepDone;
	yathCCriticalSection m_CriticalSection;
	yathCCriticalSection m_ReadCriticalSection;
	yathCCriticalSection m_SendCriticalSection;
public:
	yathCTCPConnection( SOCKET &Socket, SOCKADDR_IN &SenderAddr, int SenderAddrLen);
	yathCTCPConnection( int Port, char *Address);
	yathCTCPConnection( int Port, unsigned long Address);
	virtual ~yathCTCPConnection();
	int Initialize();
	int Done();
	int SendData( void *Buffer, int DataSize);
	int ReadData( void *Buffer, int ReadSize, int &DataSize, BOOL WaitToFill = FALSE);
	int SendDataBlock( void *Buffer, int BufferSize);
	int ReadDataBlock( void *Buffer, int BufferSize, int &DataSize);
	int GetLastError();
};

/////////////////////////////////////////////////////////////

class yathCTCPListener: public yathCGeneric
{
private:
	int m_Port;
	SOCKET m_Socket;
	SOCKADDR_IN m_Local;
	int m_LastStepDone;
	int m_LastSockError;
	yathCTCPConnection *m_Connection;
	yathCCriticalSection m_CriticalSection;
public:
	yathCTCPListener( int Port);
	virtual ~yathCTCPListener();
	int Initialize();	
	int Done();
	int Listen( int Pendings);
	int GetLastError();
	yathCTCPConnection *GetConnection();
};

/////////////////////////////////////////////////////////////

class yathCThreadsContainer;

/////////////////////////////////////////////////////////////

class yathCThread: public yathCGeneric
{
private:
	HANDLE m_ThreadHandle;
	DWORD m_ThreadId;
	BOOL m_StopRequest;
	DWORD m_LastThreadError;
	BOOL m_FirstTimeResume;
	DWORD m_TimeBeforeTerminate;
	
	yathCThreadsContainer *m_Container;
	BOOL m_InActiveList;
	BOOL m_InFinishedList;
	int m_DestroyLock;
	
	static DWORD WINAPI _Runner( LPVOID lpvParam);
	yathCCriticalSection m_CriticalSection;
	friend yathCThreadsContainer;
protected:
	BOOL IsStopRequested();
public:
	yathCThread( DWORD TimeBeforeTerminate = 0);
	virtual ~yathCThread();
	virtual DWORD Resume();
	virtual DWORD Suspend();
	virtual void StopRequest();
	virtual BOOL BeforeExecute();
	virtual BOOL AfterTermination();
	virtual DWORD Execute() = 0;
	virtual BOOL SoftTerminate();
	void AddPeerThread( yathCThread *Thread, BOOL AutoStart = FALSE);
	virtual void Finish();
	BOOL IsActive();
	BOOL IsDestroyLocked();
	void DestroyLock();
	void DestroyUnlock();
	virtual void Free();
};

/////////////////////////////////////////////////////////////

typedef struct tagyathTHREAD_LIST{
	yathCThread *Thread;
	tagyathTHREAD_LIST *Next;
	tagyathTHREAD_LIST *Prev;
} yathTHREAD_LIST;

/////////////////////////////////////////////////////////////

class yathCThreadsContainer: public yathCGeneric
{
private:
	yathCCriticalSection m_ListsCriticalSection;
	yathTHREAD_LIST *m_FirstActive;
	yathTHREAD_LIST *m_LastActive;
	yathTHREAD_LIST *m_FirstIdle;
	yathTHREAD_LIST *m_LastIdle;
public:
	yathCThreadsContainer();
	virtual ~yathCThreadsContainer();
	void AddThread( yathCThread *Thread, BOOL AutoResume = TRUE);
	void SetFinished( yathCThread *Thread);
	void StopRequestAll();
	void ClearIdleList();
	void PeacefullyClearIdleList();
};

/////////////////////////////////////////////////////////////

int yathCalculateDataCRC( void *Data, int DataSize);

BOOL yathInitChunk( void *Buffer, int BufferSize, int ChunkStamp);
BOOL yathFinalizeChunk( void *Buffer, int BufferSize);
//BOOL yathCheckChunk( void *Buffer, int BufferSize);

BOOL yathAppendFieldToChunk( void *Buffer, int BufferSize, const void *FieldData, int FieldDataSize);
BOOL yathAppendFieldsToChunk( void *Buffer, int BufferSize, const yathFIELDREC *Fields, int FieldsCount);

BOOL yathRetriveFieldsCount( const void *Buffer, int BufferSize, int &FieldsCount);
BOOL yathRetriveField( const void *Buffer, int BufferSize, int FieldNo, void **Field, int &FieldDataSize);
BOOL yathRetriveFields( const void *Buffer, int BufferSize, yathFIELDREC *Fields, int &FieldsCount);

BOOL yathRetriveChunkStamp( const void *Buffer, int BufferSize, int &Stamp);
BOOL yathRetriveChunkSize( const void *Buffer, int BufferSize, int &ChunkSize);

/////////////////////////////////////////////////////////////

const char *yathFindToken( const char *StartFrom, const char *&ParamStart, int &ParamLength, BOOL &IsOption);
BOOL yathCmpBufWStr( const void *Buffer, int BufferLength, const char *Str);
const char *yathPathFindFileName( const char *Path);
BOOL yathPathAppend( char *Path, const char *Name, int MaxSize);

/////////////////////////////////////////////////////////////

#endif
