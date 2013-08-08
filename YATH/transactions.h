#ifndef _TRANSACTIONS_H
#define _TRANSACTIONS_H

#include "classes.h"
#include "protocol.h"
#include "clientclasses.h"
#include "common.h"

/////////////////////////////////////////////////////////////

#define yathtaNONE		0

#define yathtaRCD		1
#define yathtaLCD		2
#define yathtaRLS		3
#define yathtaLLS		4
#define yathtaRMOVE		5
#define yathtaLMOVE		6
#define yathtaRMKDIR	7
#define yathtaLMKDIR	8
#define yathtaRRMDIR	9
#define yathtaLRMDIR	10
#define	yathtaRRMFILE	11
#define	yathtaLRMFILE	12
#define yathtaRTYPE		13

#define yathtaGET		14
#define yathtaPUT		15
#define yathtaMGET		16
#define yathtaMPUT		17

/////////////////////////////////////////////////////////////

typedef struct yathtagFILEINOFLIST {
	yathFILEINFO FileInfo;
	yathtagFILEINOFLIST *Next;
	yathtagFILEINOFLIST *Prev;
} yathFILEINOFLIST;

/////////////////////////////////////////////////////////////

#define yathFMPC	4

class yathCFileManagerTransaction: public yathCTransaction
{
private:
	yathFILEINOFLIST *m_FirstFileInfo;
	yathFILEINOFLIST *m_LastFileInfo;
	yathFILEINOFLIST *m_ScanFileInfo;

	DWORD m_Type;
	DWORD m_Flags;
	char *m_Strings[ yathFMPC];

protected:

	void SetString( int PathNo, const char *Path);

	BOOL AppendFileInfo( yathFILEINFO &FileInfo);
	BOOL GetFirstFileInfo( yathFILEINFO &FileInfo);
	BOOL GetLastFileInfo( yathFILEINFO &FileInfo);
	BOOL GetNextFileInfo( yathFILEINFO &FileInfo);
	BOOL GetPrevFileInfo( yathFILEINFO &FileInfo);
	BOOL GetCurrentFileInfo( yathFILEINFO &FileInfo);
	
	BOOL GetFile( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *RemoteFName, const char *LocalFName, BOOL &ConnectionFatalError);
	BOOL PutFile( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *LocalFName, const char *RemoteFName, BOOL &ConnectionFatalError);

	BOOL GetFilesList( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *FileName, BOOL &ConnectionFatalError);
	BOOL GetLocalFilesList( yathCConnectionManager *Manager, const char *FileName);
	
	BOOL ChangeCurrentDir( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *Path, char *NewPath, int &NewPathSize, BOOL &ConnectionFatalError);
	BOOL ChangeLocalCurrentDir( yathCConnectionManager *Manager, const char *DName, char *NewPath, int &NewPathSize);
	
	BOOL RemoveFile( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *FName, BOOL &ConnectionFatalError);
	BOOL RemoveLocalFile( yathCConnectionManager *Manager, const char *FName);

	BOOL RemoveDir( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *DName, BOOL &ConnectionFatalError);
	BOOL RemoveLocalDir( yathCConnectionManager *Manager, const char *DName);
	
	BOOL CreateDir( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *DName, BOOL &ConnectionFatalError);
	BOOL CreateLocalDir( yathCConnectionManager *Manager, const char *DName);

	BOOL MoveFile( yathCTCPConnection *Connection, yathCConnectionManager *Manager, const char *SrcFName, const char *DesFName, BOOL &ConnectionFatalError);
	BOOL MoveLocalFile( yathCConnectionManager *Manager, const char *SrcFName, const char *DesFName);

	BOOL Do_get( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	BOOL Do_mget( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	BOOL Do_put( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	BOOL Do_mput( yathCTCPConnection *Connection, yathCConnectionManager *Manager);

	BOOL Do_rcd( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	BOOL Do_lcd( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	
	BOOL Do_rls( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	BOOL Do_lls( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	
	BOOL Do_rrmdir( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	BOOL Do_lrmdir( yathCTCPConnection *Connection, yathCConnectionManager *Manager);

	BOOL Do_rrmfile( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	BOOL Do_lrmfile( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	
	BOOL Do_rmv( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	BOOL Do_lmv( yathCTCPConnection *Connection, yathCConnectionManager *Manager);

	BOOL Do_rmkdir( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	BOOL Do_lmkdir( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	
	BOOL Do_rtype( yathCTCPConnection *Connection, yathCConnectionManager *Manager);
	
public:
	yathCFileManagerTransaction();
	virtual ~yathCFileManagerTransaction();
	void ClearFileList();
	virtual void StopRequest();

	virtual BOOL Do( yathCTCPConnection *Connection, yathCConnectionManager *Manager);

	void SetType_rcd( const char *Path, DWORD Flags);
	void SetType_lcd( const char *Path, DWORD Flags);
	void SetType_rls( const char *Path, DWORD Flags);
	void SetType_lls( const char *Path, DWORD Flags);
	void SetType_rmove( const char *SrcFName, const char *DestFName, DWORD Flags);
	void SetType_lmove( const char *SrcFName, const char *DestFName, DWORD Flags);
	void SetType_rrmfile( const char *FName, DWORD Flags);
	void SetType_lrmfile( const char *FName, DWORD Flags);
	void SetType_rrmdir( const char *DName, DWORD Flags);
	void SetType_lrmdir( const char *DName, DWORD Flags);
	void SetType_rmkdir( const char *DName, DWORD Flags);
	void SetType_lmkdir( const char *DName, DWORD Flags);
	void SetType_rtype( const char *FName, DWORD Flags);

	void SetType_get( const char *SrcFName, const char *DestFName, DWORD Flags);
	void SetType_put( const char *SrcFName, const char *DestFName, DWORD Flags);
	void SetType_mget( const char *SrcFName, DWORD Flags);
	void SetType_mput( const char *SrcFName, DWORD Flags);
};

/////////////////////////////////////////////////////////////

#endif