/*
 * session.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: Dlimbu
 */


#include "ServerSocket.h"
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
 *
 * Supported control
 */
const string PWD = "pwd";
const string CD  = "cd";
const string LS  = "ls";
const string GET = "get";
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

bool equals (std::string const& control, std::string c) {
	bool res = strncmp(control.c_str(), c.c_str(), c.size()) == 0;
	printf("Equals: %d\n", res);
	return res;
}

void Session::_sessionGet (std::string pathToFile) {
	FILE * _fi = this->_fs->read(pathToFile);
	if (_fi == NULL) {
		printf("Failed to open file\n");
		write("");
		return;
	}

    bzero(sendBuffer, MAX_DATA_SIZE);
	int fByteSize = 0;
	while((fByteSize = fread(sendBuffer, sizeof(char), MAX_DATA_SIZE, _fi)) > 0)
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
		bzero(sendBuffer, MAX_DATA_SIZE);
	}

	fclose(_fi);
	printf("Done sending data...\n");
}

void Session::handleCommand (std::string const& control) {

	string toClient = "";
	string copyControl = control;

	commandStruct cs = this->_fs->parseControl(copyControl);

	printf("Control structure %s %s\n", cs.control.c_str(), cs.rPath.c_str());

	string cntrl = cs.control;
	string path = cs.rPath;

	printf("Control structure parsed: %s %s \n",cntrl.c_str(), path.c_str());

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

	printf("Sending non empty data %s\n", toClient.c_str());
	this->write(toClient);
}


void Session::startSession() {

	openFile();
	printf("File opened at %s\n", this->_fs->pwd().c_str());

	while (true) {

		printf("Inside child\n");

		int n;
//		bzero(this->cntrlBuffer, sizeof(this->cntrlBuffer));

		memset(cntrlBuffer, 0, 100);

		int string_size = 0;
		n = recv(this->_sockFd, &string_size, sizeof(string_size), 0);
		printf("First recv error received %d ", errno);
		printf("Received count string size %d", string_size);

		n = recv(this->_sockFd, cntrlBuffer, string_size, 0);

		printf("second recv error received %d \n", errno);
		cntrlBuffer[string_size] = '\0';
		printf("message %s", cntrlBuffer);

		if (n == -1) {
			onError("ERROR reading from socket");
		}

		printf("Received %s\n", cntrlBuffer);

		bool quit = strncmp(this->cntrlBuffer, "bye", 3) == 0;
		printf("Quit is: %d\n", quit);

		if (quit) {
			this->dispose();
			break;
		} else {
//			char sendBuffer[1024];
//			strncpy(sendBuffer, cntrlBuffer, sizeof(sendBuffer));
//			sendBuffer[sizeof(sendBuffer) - 1] = '\0';
//			this->write(sendBuffer);
			handleCommand(cntrlBuffer);
		}

//		printf("Inside Session WAITING process: %d\n", ::getpid());
//		string k = "";
//
//		bzero(this->cntrlBuffer, 1000);
//		std::string fromClient = this->read();
//
//		printf("Socket data from client %s\n", fromClient.c_str());
//
//		bool quit = strncmp(fromClient.c_str(), "bye", 3) == 0;
//
//		printf("Quit is: %d\n", quit);
//
//		if (quit) {
//			this->dispose();
//			break;
//		} else {
//			this->handleCommand(fromClient);
//		}
	}

	printf("Session ended %d\n", ::getpid());
}

/**
 * Read from socket.
 */
std::string Session::read () {
	printf("Session Going to Read from socket\n");
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

	printf("Sending message back %s\n", message.c_str());

	int str_size = strlen(message.c_str());
	int n = send(this->_sockFd, &str_size, sizeof(str_size), 0);
	n = send(this->_sockFd, message.c_str(), str_size, 0);

	if (n == -1) {
		onError("ERROR writing to socket");
	}

	printf("echo back: %s\n", message.c_str());
}







