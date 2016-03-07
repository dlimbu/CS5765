/*
 * client.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: Dlimbu
 */

#include "client.h"

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
#include <errno.h>
#include "../fs/FileSystem.h"

using namespace std;

const string HELP = "help";
const string LPWD = "lpwd";
const string LCD  = "lcd";
const string LLS  = "lls";
const string GET = "get";
const string BYE = "bye";
const string BACK = "..";

Client::Client() { }

Client::~Client() {
}

void error(const char *msg)
{
	printf("Error: %s", msg);
    perror(msg);
    exit(0);
}

/**
 * Start socket
 */
void Client::start(string ip, int port) {

	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockfd < 0) {
		error("ERROR opening socket");
	}

	server = gethostbyname(ip.c_str());
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

	bzero((char *) &serv_addr, sizeof(this->serv_addr));
	this->serv_addr.sin_family = AF_INET;

	if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <=0) {
	   printf("\n inet_pton error occured\n");
	   exit(1);
	}

	this->serv_addr.sin_port = htons(port);

	if (connect(this->sockfd,(struct sockaddr *) &serv_addr,sizeof(this->serv_addr)) < 0) {
		error("ERROR connecting");
	}

	string s = "\t\tWelcome to SFTP 0.0.1 \n\tAccepted commands: ls, pwd, cd, get";
	printf("%s\n", s.c_str());

	ftpApp();

	printf("Socket closed \n");
	close(sockfd);
}

/**
 * Launch FTP client.
 */
void Client::ftpApp () {

    this->openFs();

	while(1) {
		printf("$: ");

		memset(cntrlBuff, 0, 100);
		fgets(cntrlBuff, 100, stdin);

		int numbytes = sizeof(cntrlBuff);
		cntrlBuff[numbytes]='\0'; //null terminate
		strtok(cntrlBuff, "\n");

		bool locallyHandled = this->handleCommand(cntrlBuff);
		if (locallyHandled) {
			if (numbytes == 0 || strncmp(cntrlBuff,"bye", 3) == 0) {
				break;
			}
			continue;
		}

		//send to server
		write(cntrlBuff);

		//read from server
		string res = read();

		//handle weirdness on empty message
		if (string(res) != "SIG_DONE") {
			printf("%s\n", res.c_str());
		}
	}
}

/**
 * download file
 */
void Client::sessionGet (string sfilePath) {
	FILE * _fi = this->_fs->write(sfilePath);
	if (_fi == NULL) {
		printf("Failed to open file\n");
		return;
	}

	int fByteSize = 0;
	//100byte MSS read
	while((fByteSize = recv(sockfd, recvBuf, MSS, 0)) > 0) {
		int write_sz = fwrite(recvBuf, sizeof(char), fByteSize, _fi);
		if(write_sz < fByteSize) {
			printf("Unexpected download Failure, Try again.\n");
			break;
		}
		bzero(recvBuf, MSS);
		if (fByteSize == 0 || fByteSize != MSS) {
			printf("Download Complete\n");
			break;
		}
	}

	fclose(_fi);
}

/**
 * Handle control commands.
 */
bool Client::handleCommand (string control) {
	if (control.length() <= 1) {
		printf(":command not supported\n");
		return true;
	}

	string toClient = "";
	string copyControl = control;
	commandStruct cs = this->_fs->parseControl(copyControl);
	string cntrl = cs.control;
	string path = cs.rPath;

	if (cntrl == LLS) {
		toClient = this->_fs->ls();
	} else if (cntrl == LPWD) {
		toClient = this->_fs->pwd();
	} else if (cntrl == LCD) {
		if (strcmp(path.c_str(), BACK.c_str()) == 0) {
			this->_fs->back();
		} else {
			this->_fs->cd(path);
		}
	} else if (cntrl == GET) {
	    string fileName = path;
		//send the control to server.
		write(control);
		read();

		if (string(recvBuf) == "FILE_OPEN_ERR") {
			printf("Error %s\n", recvBuf);
		} else {
			//blocking call to wait for download.
			sessionGet(fileName);
		}
	} else if (cntrl == BYE) {
		printf("bye\n");
		write(cntrlBuff);
	}
	else {
		return false;
	}

	if (toClient.size() > 0) {
		printf("%s\n", toClient.c_str());
	}

	return true;
}

/**
 * Read from socket.
 */
std::string Client::read () {

	memset(recvBuf, 0, 100);

	int string_size = 0;

	int nBytes = recv(sockfd, &string_size, sizeof(string_size), 0);
	nBytes = recv(sockfd, recvBuf, string_size, 0);
	recvBuf[string_size] = '\0';

	if (nBytes == 0) {
		printf("0 bytes received\n");
	}

	if (nBytes == -1) {
		error("ERROR reading from socket");
	}

	return recvBuf;
}

/**
 * Write to socket.
 */
void Client::write(string message) {
	int str_size = strlen(message.c_str());
	int n = send(sockfd, &str_size, sizeof(str_size), 0);
	n = send(sockfd, message.c_str(), str_size, 0);

	if (n == -1) {
		error("ERROR writing to socket");
	}
}
