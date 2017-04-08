/*
 * NetworkManager.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: Dlimbu
 */

#include "server/ServerSocket.h"
#include <string>
#include <stdio.h>
#include <iostream>
//structured for Internet address
#include <netinet/in.h>
//socket structure
#include <sys/socket.h>
//data types in system call
#include <sys/types.h>

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;
#include <signal.h>

int main(int argc, char * argv []) {

	if (argc < 3) {
		 fprintf(stderr,"usage %s hostname port\n", argv[0]);
		 exit(0);
	}

	cout << "\tNetworkManager \n";

	ServerSocket *s = new ServerSocket();
	string ipAddress = argv[1];
	int portno = atoi(argv[2]);
	s->start(ipAddress.c_str(), portno);
	return 0;
}
