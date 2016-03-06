/*
 * ServerSocket.h
 *
 *  Created on: Jan 28, 2016
 *      Author: Dlimbu
 */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include "Session.h"
#include <vector>

#ifndef SRC_SERVER_SERVERSOCKET_H_
#define SRC_SERVER_SERVERSOCKET_H_

#define RECV_WDW_BYTE 100
#define MAX_CLIENT_SUPPORT 100

class ServerSocket {

	/**
	*struct sockaddr_in
	*{
	*  short   sin_family; //must be AF_INET
	*  u_short sin_port;
	*  struct  in_addr sin_addr; //variable unsigned long called s_addr
	*  char    sin_zero[8]; // Not used, must be zero
	*};
	*/
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;

	int _serverSocketFD;
	int newSockFd;

	std::vector<Session *> sessions;
	char cntrlBuffer[RECV_WDW_BYTE];
	char sendBuffer[RECV_WDW_BYTE];

private:

	int _i;

	int clientSockFd [MAX_CLIENT_SUPPORT];

	void onConnection ();
	void addSocket (int clientSocktId) {
		if (_i < MAX_CLIENT_SUPPORT) {
			this->clientSockFd[_i++] = clientSocktId;
		}
	}

public:

	enum ERROR_TYPE {
		INVALID_TUPLE,
		MAX_SOCKET_RCHD
	};

	ServerSocket();

	virtual ~ServerSocket();

	void error (const char *msg);

	/**
	 * readIp address from socket description.
	 */
	char * readIp(char * writeTo, sockaddr_in desc);

	/**
	 * Start at ip and port tuple.
	 */
	void start(const char * ip, int port);

	/**
	 * Stop the server
	 */
	void stop();

	/**
	 *Spawn a process to handle new clients.
	 */
	void spawnProcess ();

	/**
	 * Get live client based on desc.
	 */
	int getLiveClient (int clientDesc);

	/**
	 *Kill a client
	 */
	void killClient(int clientDesc);

	/**
	 *Kill All running clients.
	 */
	void killAll ();

	/**
	 *Read from input stream
	 */
	std::string readFrom(const std::string control);

	/**
	 *Write to output stream
	 */
	void writeTo(std::string message);
};

#endif /* SRC_SERVER_SERVERSOCKET_H_ */
