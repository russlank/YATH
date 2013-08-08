#ifndef _SERVERCLASSES_H_
#define _SERVERCLASSES_H_

#include "../YATH/classes.h"
#include "../YATH/common.h"

/////////////////////////////////////////////////////////////

class yathCServer;

/////////////////////////////////////////////////////////////

class yathCSessionThread: public yathCThread
{
private:
	yathCServer *m_Server;
	yathCTCPConnection *m_Connection;
	char m_Buffer[16 * 1024];
public:
	yathCSessionThread( yathCServer *Server, yathCTCPConnection *Connection);
	virtual ~yathCSessionThread();
	virtual DWORD Execute();
	virtual void Finish();
	void StopRequest();

	BOOL ResponseError();
	BOOL ResponseListFiles();
	BOOL ResponseChangeDir();
	BOOL ResponseGetFile();
	BOOL ResponsePutFile();
	BOOL ResponseCreateDir();
	BOOL ResponseRemoveDir();
	BOOL ResponseRemoveFile();
	BOOL ResponseMove();
};

/////////////////////////////////////////////////////////////

class yathCProxySessionThread: public yathCThread
{
private:
	yathCServer *m_Server;
	yathCTCPConnection *m_ClientConnection;
	yathCTCPConnection *m_ServerConnection;
public:
	yathCProxySessionThread( yathCServer *Server, yathCTCPConnection *ClientConnection);
	virtual ~yathCProxySessionThread();
	virtual DWORD Execute();
	void StopRequest();
};

/////////////////////////////////////////////////////////////

class yathCPortThread: public yathCThread
{
private:
	yathCServer *m_Server;
	yathCTCPListener *m_Listener;
public:
	yathCPortThread( yathCServer *Server);
	virtual ~yathCPortThread();
	virtual DWORD Execute();
	void StopRequest();
};

/////////////////////////////////////////////////////////////

class yathCProxyThread: public yathCThread
{
private:
	yathCServer *m_Server;
	yathCTCPListener *m_Listener;
public:
	yathCProxyThread( yathCServer *Server);
	virtual ~yathCProxyThread();
	virtual DWORD Execute();
	void StopRequest();
};

/////////////////////////////////////////////////////////////

class yathCServer: public yathCThreadsContainer
{
private:
	yathCThreadsContainer *m_Container;
	yathCPortThread *m_PortThread;
	yathCProxyThread *m_ProxyThread;
public:
	yathCServer();
	virtual ~yathCServer();
	void Run();
};

/////////////////////////////////////////////////////////////

#endif

