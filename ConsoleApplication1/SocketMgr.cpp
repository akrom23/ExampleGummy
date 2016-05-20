/*
** gummy52
*/

#include "stdafx.h"

SocketMgr::SocketMgr() : 
	m_bCancelListener(false),
	m_bCancelWorker(false),
	m_bInit(true),
	m_sListenSocket(0)
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("WorldSocketMgr::WSAStartup - Failed with error %d, shutting down.", WSAGetLastError());
		Sleep(5000);
		exit(1);
	}	
}

SocketMgr::~SocketMgr()
{
	if (m_bInit)
		Uninitialise();
}

void SocketMgr::Uninitialise()
{
	if (!m_bInit)
		return;

	assert(m_sListenSocket);

	if (m_sListenSocket)
		closesocket(m_sListenSocket);

	// Wait for the thread to finish.
	m_bCancelListener = true;
	m_threadListener.join();

	// Wait for the thread to finish.
	m_bCancelWorker = true;
	m_threadWorker.join();

	m_bInit = false;
	assert(WSACleanup() == 0);
}