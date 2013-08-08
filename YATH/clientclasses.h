#ifndef _CLIENTCLASSES_H
#define _CLIENTCLASSES_H

/////////////////////////////////////////////////////////////

#include "classes.h"
#include "protocol.h"

/////////////////////////////////////////////////////////////

class yathCTransaction;
class yathCConnectionActor;
class yathCConnectionManager;
class yathCUserInterface;

/////////////////////////////////////////////////////////////

typedef struct yathtagTRANSACTION_LIST {
	yathCTransaction *Transaction;
	yathtagTRANSACTION_LIST *Next;
	yathtagTRANSACTION_LIST *Prev;
} yathTRANSACTION_LIST;

/////////////////////////////////////////////////////////////

class yathCUserInterface: public yathCGeneric
{
private:
public:
	yathCUserInterface();
	virtual ~yathCUserInterface();
	virtual void WriteOutput( const char *Str);
	virtual void WriteOutputToCatch( const char *Str);
	virtual void FlushOutputCatch();
	virtual void ClearOutput();
};

/////////////////////////////////////////////////////////////

class yathCTransaction: public yathCGeneric
{
private:
	yathCCriticalSection m_CriticalSection;
	BOOL m_StopRequested;
public:
	yathCTransaction();
	virtual ~yathCTransaction();
	virtual BOOL Do( yathCTCPConnection *Connection, yathCConnectionManager *Manager) = 0;
	virtual void StopRequest();
	virtual BOOL IsStopRequested();
};

/////////////////////////////////////////////////////////////

class yathCConnectionActor: public yathCThread
{
private:
	yathCConnectionManager * m_Manager;
	
	yathCTCPConnection *m_Connection;

	yathCTransaction *m_WorkingTransaction;

	yathCCriticalSection m_ListCriticalSection;
	yathTRANSACTION_LIST *m_FirstWaitingTransaction;
	yathTRANSACTION_LIST *m_LastWaitingTransaction;
	yathTRANSACTION_LIST *m_FirstDoneTransaction;
	yathTRANSACTION_LIST *m_LastDoneTransaction;

	void RemoveAllTransactions( BOOL EvenWorking = TRUE);
	BOOL ShiftToNextWaitingTransaction();
public:
	yathCConnectionActor( int Port, char *Address, yathCConnectionManager *Manager);
	yathCConnectionActor( int Port, unsigned long Address, yathCConnectionManager *Manager);
	virtual ~yathCConnectionActor();
	
	virtual void StopRequest();
	virtual DWORD Execute();
	void AppendTransaction( yathCTransaction *Transaction);
	yathCTransaction *GetDoneTransaction();
};

/////////////////////////////////////////////////////////////

class yathCConnectionManager: public yathCGeneric
{
private:
	yathCThreadsContainer m_ThreadsContainer;
	yathCConnectionActor *m_Actor;
	yathCCriticalSection m_OutputCriticalSection;
	yathCUserInterface *m_OutputView;
public:
	yathCConnectionManager();
	virtual ~yathCConnectionManager();
	BOOL Connect( char *Address, int Port);
	BOOL Connect( unsigned long Address, int Port);
	void Disconnect();
	void SetOutputView( yathCUserInterface *OutputView);
	void NewTransaction( yathCTransaction *Transaction);

	void WriteOutput( const char *Str);
	void WriteOutputToCatch( const char *Str);
	void FlushOutputCatch();
	void ClearOutput();
};

/////////////////////////////////////////////////////////////

#endif