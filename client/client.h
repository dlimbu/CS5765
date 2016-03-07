/*
 * client.h
 *
 *  Created on: Mar 4, 2016
 *      Author: Dlimbu
 */
#include <string>
#include "../fs/FileSystem.h"
#include <errno.h>
#include <string>
#include <stdio.h>
#include <iostream>
//structured for Internet address
#include <netinet/in.h>
//socket structure
#include <sys/socket.h>
//data types in system call
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <cerrno>
#ifndef SRC_CLIENT_CLIENT_H_
#define SRC_CLIENT_CLIENT_H_

#define PORT 1490
#define MSS 100

using namespace std;

class Client {
private:
	FileSystem * _fs;

	int sockfd;
	int portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char cntrlBuff[MSS];
    char recvBuf[MSS];

public:
	Client();

	void openFs () {
		_fs = new FileSystem();
		string storage = "/clientStorage";
		try {
			_fs->openFs(storage.c_str());
		} catch (exception& e) {
			printf("Error %d\n", errno);
		}
	}

	bool handleCommand (std::string c);

	void start(string ip, int port);

	void stop();

	void ftpApp ();

	string read ();

	void sessionGet(string sfilePath);

	void write (string v);

	virtual ~Client();
};

#endif /* SRC_CLIENT_CLIENT_H_ */
