/*
 * ServerSocket.h
 *
 *  Created on: Mar 4, 2016
 *      Author: Dlimbu
 */

#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include "Session.h"

#ifndef SRC_SERVER_SERVERSOCKET_H_
#define SRC_SERVER_SERVERSOCKET_H_

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

	char cntrlBuffer[MSS];
	char sendBuffer[MSS];

private:
	void onConnection ();
public:

	ServerSocket();

	virtual ~ServerSocket();

	/**
	 *
	 */
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
	 *Read from input stream
	 */
	string readFrom(const std::string control);

	/**
	 *Write to output stream
	 */
	void writeTo(std::string message);
};

#endif /* SRC_SERVER_SERVERSOCKET_H_ */
