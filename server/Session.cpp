/*
 * session.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: Dlimbu
 */


#include "ServerSocket.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <fcntl.h>

#include "../fs/FileSystem.h"

using namespace std;

/**
 * Supported control
 */
const string PWD = "pwd";
const string CD  = "cd";
const string LS  = "ls";
const string GET = "get";
const string EXIT = "exit";
const string BACK = "..";

Session::Session(int sockFd, int pid) {
	this->_sockFd = sockFd;
	this->_pid = pid;
}

Session::~Session() {

}

void Session::dispose() {
	::close(this->_sockFd);
}

int Session::getSessionId() {
	return this->_pid;
}

int Session::getSockFd() {
	return this->_sockFd;
}

void Session::onError(const char *msg)
{
    perror(msg);
    exit(1);
}

/**
 * upload file to client.
 */
void Session::_sessionGet (std::string pathToFile) {
	FILE * _fi = this->_fs->read(pathToFile);
	if (_fi == NULL) {
		printf("Failed to open file\n");
		write("FILE_OPEN_ERR");
		return;
	}

	bzero(sendBuffer, MSS);
	int fByteSize = 0;
	while((fByteSize = fread(sendBuffer, sizeof(char), MSS, _fi)) > 0)
	{
		printf("Read byte size: %d\n", fByteSize);
		printf("Read byte data: %s\n", sendBuffer);

		if(send(this->_sockFd, sendBuffer, fByteSize, 0) < 0)
		{
			printf("unExpected error %d while sending byte to %s\n",
					errno,
					pathToFile.c_str()
					);

			write("");
			break;
		}

		printf("sent file byte size: %d\n", fByteSize);
		bzero(sendBuffer, MSS);
	}

	fclose(_fi);
}

/**
 * Handle client commands.
 */
void Session::handleCommand (std::string const& control) {

	string toClient = "";
	string copyControl = control;

	commandStruct cs = this->_fs->parseControl(copyControl);
	string cntrl = cs.control;
	string path = cs.rPath;

	if (cntrl == LS) {
		toClient = this->_fs->ls();
	} else if (cntrl == PWD) {
		toClient = this->_fs->pwd();
	} else if (cntrl == CD) {
		if (strcmp(path.c_str(), BACK.c_str()) == 0) {
			this->_fs->back();
		} else {
			this->_fs->cd(path);
		}
	} else if (cntrl == GET) {
		_sessionGet(path);
	} else {
		toClient = cntrl+":command not supported";
	}

	if (cntrl == GET) {
		return;
	}

	this->write(toClient);
}

/**
 * Start a session for client.
 */
void Session::startSession() {

	openFile();

	while (true) {
		int n;
		memset(cntrlBuffer, 0, 100);

		int string_size = 0;
		n = recv(this->_sockFd, &string_size, sizeof(string_size), 0);
		n = recv(this->_sockFd, cntrlBuffer, string_size, 0);
		cntrlBuffer[string_size] = '\0';

		if (n == -1) {
			onError("ERROR reading from socket");
		}

		bool exit = strncmp(this->cntrlBuffer, "exit", 4) == 0;
		bool quit = strncmp(this->cntrlBuffer, "bye", 3) == 0;

		if (quit || exit) {
			this->dispose();
			break;
		} else {
			handleCommand(cntrlBuffer);
		}
	}

	printf("Session ended %d\n", ::getpid());
}

/**
 * Read from socket.
 */
std::string Session::read () {
	int n = ::read(this->_sockFd, this->cntrlBuffer, 1000);

	if (n < 0) {
		onError("ERROR reading from socket");
	}

	printf("read %d  %s\n", n, this->cntrlBuffer);
	string _res = string(this->cntrlBuffer);
	return _res;
}

/**
 * Write to socket.
 */
void Session::write(string message) {

	if (message == "") {
		message = "SIG_DONE";
	}

	int str_size = strlen(message.c_str());
	int n = send(this->_sockFd, &str_size, sizeof(str_size), 0);
	n = send(this->_sockFd, message.c_str(), str_size, 0);

	if (n == -1) {
		onError("ERROR writing to socket");
	}

	printf("Echo %s\n", message.c_str());
}







