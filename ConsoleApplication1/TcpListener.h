#ifndef _TCPLISTENER_H
#define _TCPLISTENER_H

#include "Util.h"

class TcpListener
{
	public:
		TcpListener(uint32 port) :
			m_sListenSocket(INVALID_SOCKET)
		{
			struct sockaddr_in local;

			// Create our listening socket
			m_sListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

			if (m_sListenSocket == INVALID_SOCKET)
			{
				printf("TcpListener socket() failed: %d\n", WSAGetLastError());
				return;
			}

			local.sin_addr.s_addr = htonl(INADDR_ANY);
			local.sin_family = AF_INET;
			local.sin_port = htons(port);

			// Bind
			if (::bind(m_sListenSocket, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
			{
				printf("TcpListener bind() failed: %d\n", WSAGetLastError());
				m_sListenSocket = INVALID_SOCKET;
				return;
			}

			if (::listen(m_sListenSocket, SOMAXCONN) == SOCKET_ERROR)
			{
				printf("TcpListener listen() failed: %d\n", WSAGetLastError());
				m_sListenSocket = INVALID_SOCKET;
				return;
			}
		}

		~TcpListener()
		{
			closesocket(m_sListenSocket);
			m_sListenSocket = INVALID_SOCKET;
		}
			
		bool okay() const 
		{ 
			return m_sListenSocket > 0;
		}
		
		// @bNoDelay Will setsockopt TCP_NODELAY, disabling Nagle's algorithm (increased speed, more traffic)
		// @uiTimeout  If set a value it's the number of milliseconds to add onto 500 minimum for Windows to SO_RCVTIMEO. 
		SOCKET waitAndAccept(std::string& addr, bool bNoDelay = false, const uint32 uiTimeout = 0)
		{
			if (!okay())
			{
				printf("TcpListener::waitAndAccept you can not do this on an invalid socket.\n");
				return INVALID_SOCKET;
			}

			sockaddr_in client;
			int32 addrsize = sizeof(client);

			// WinSOck ::accept
			SOCKET csock = ::accept(m_sListenSocket, (struct sockaddr*)&client, &addrsize);
			
			if (csock == SOCKET_ERROR || csock == INVALID_SOCKET)
			{			
				cleanup();
				return SOCKET_ERROR;
			}

			// TCP_NODELAY, optional
			if (bNoDelay)
			{
				int32 i = 1;
				setsockopt(csock, IPPROTO_TCP, TCP_NODELAY, (char*)&i, sizeof(i));
			}

			if (uiTimeout)
			{			
				int32 iTimeout = uiTimeout;
				setsockopt(csock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&iTimeout, sizeof(iTimeout));
			}

			addr = inet_ntoa(client.sin_addr);
			return csock;
		}

		SOCKET mySocket() const { return m_sListenSocket; }

	private:
		void cleanup()
		{				
			closesocket(m_sListenSocket);
			m_sListenSocket = INVALID_SOCKET;
		}

		SOCKET m_sListenSocket;
};

#endif