/*
 * ServerSocket.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: Dlimbu
 */

#include "ServerSocket.h"
#include <string>

//#include <iostream>
//socket structure

//data types in system call
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
//structured for Internet address
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>

#include <netdb.h>
#include <arpa/inet.h>

#include "Session.h"
using namespace std;

void sigchld_handler(int s) {
	while(wait(NULL) > 0);
}

/**
 *
 */
ServerSocket::ServerSocket() {
	cout << "Server Socket Initialized\n";
}

/**
 *
 */
ServerSocket::~ServerSocket() {
	cout << "Server Socket disposed\n";
}

/**
 *
 */
std::string ServerSocket::readFrom(std::string input) {
  int n = read(this->newSockFd, this->cntrlBuffer, 100);

  if (n < 0) {
	  error("ERROR reading from socket");
	  exit(EXIT_FAILURE);
  }

  string _res = string(this->cntrlBuffer);
  return _res;
}

/**
 *
 */
void ServerSocket::writeTo(string control) {
	printf("Writing back to client message \n");
    int n = write(this->newSockFd, control.c_str(), 100);

    if (n < 0) {
	  error("ERROR writing to socket");
    }
}

/**
 *
 */
void error(const char *msg) {
    perror(msg);
    exit(1);
}

/**
 *
 */
void ServerSocket::start(const char * ip, int port) {

	if (port <= 0) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	this->_serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_serverSocketFD == -1) {
		error("ERROR opening socket");
	} else {
		fprintf(stdout,"Socket open successful!!\n");
	}

	int portno = port;
	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_addr.s_addr = inet_addr(ip);
	this->serverAddr.sin_port = htons(portno);
	memset(&(this->serverAddr.sin_zero), '\0', 8);

	char ipa [500];
	inet_ntop(AF_INET, &this->serverAddr.sin_addr, ipa, INET_ADDRSTRLEN);
	printf("Server starting at %s \n\n", ipa);

	if (bind(this->_serverSocketFD,
			(struct sockaddr *) &this->serverAddr,
			sizeof(this->serverAddr)) == -1) {
		error("ERROR on binding");
		exit(1);
	}

	int l = listen(this->_serverSocketFD, 5);
	if (l == -1) {
		perror("Listen error");
		exit(1);
	}

	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	onConnection();
}

/**
 *
 */
void ServerSocket::onConnection () {
	unsigned int sin_size;

	while (1) {
		sin_size = sizeof(struct sockaddr_in);

		if ((this->newSockFd = accept(this->_serverSocketFD,
				(struct sockaddr *) &this->clientAddr,
				&sin_size)) == -1) {
			perror("accept");
			continue;
		}

		printf("New client connected %s \n\n", inet_ntoa(this->clientAddr.sin_addr));

		char * recvBuf;
		int numBytes;
		int pid = fork();
		if (pid < 0) {
			error("ERROR on fork");
		}

		if (pid == 0) {
			close(this->_serverSocketFD);
			Session * s = new Session(this->newSockFd, ::getpid());
			s->startSession();

			printf("Session destroyed for process: %d\n", ::getpid());
			::kill(s->getSessionId(), SIGKILL);
			exit(0);
		}
		else {
			close(this->newSockFd);
		}
	}
}

/**
 *
 */
char * ServerSocket::readIp(char * writeTo, sockaddr_in desc) {
  struct sockaddr_in sa = desc;
  inet_ntop(AF_INET, &(sa.sin_addr), writeTo, INET_ADDRSTRLEN);
  return writeTo;
}

/**
 *
 */
void ServerSocket::error(const char *msg) {
    perror(msg);
    exit(1);
}

