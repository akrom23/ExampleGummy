/*
** gummy52
**	Interface for making a socket manager.
*/

#ifndef _SOCKETMGR_H
#define _SOCKETMGR_H

#include "stdafx.h"

class SocketMgr
{
	public:
		SocketMgr();
		virtual ~SocketMgr();

		void Uninitialise();

	protected:
		virtual void ListenThread() = 0;
		virtual void WorkerThread() = 0;
	
		// Must be called after child class construction.
		void startThreads()
		{
			m_threadWorker = std::thread(&SocketMgr::WorkerThread, this);
			m_threadListener = std::thread(&SocketMgr::ListenThread, this);
			m_bInit = true;
		}

		// Must be called
		void setListenSocket(SOCKET sSocket)
		{
			assert(m_sListenSocket == 0);
			m_sListenSocket = sSocket;
		}

		bool doCancelWorker() const { return m_bCancelWorker; }
		bool doCancelListener() const { return m_bCancelListener; }

	private:	
		bool m_bCancelWorker;
		bool m_bCancelListener;
		bool m_bInit;

		SOCKET m_sListenSocket;

		std::thread m_threadWorker;
		std::thread m_threadListener;

		WSAData m_wsaData;
};

#endif