/*
 * client.cpp
 *
 *  Created on: Feb 22, 2016
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

void Client::start(string ip, int port) {

	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);

	printf("Entry start \n");
	if (this->sockfd < 0) {
		error("ERROR opening socket");
	}
	printf("Client connecting to %s:%d\n", ip.c_str(), port);
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

	printf("Connecting.....\n");
	if (connect(this->sockfd,(struct sockaddr *) &serv_addr,sizeof(this->serv_addr)) < 0) {
		error("ERROR connecting");
	}

	printf("Client connect to %s:%d\n", ip.c_str(), port);

	string s = "\t\tWelcome to SFTP 0.0.1 \n\tAccepted commands: ls, pwd, cd, get";
	printf("%s\n", s.c_str());

	ftpApp();

	printf("Socket closed \n");
	close(sockfd);
}

void Client::ftpApp () {

    this->openFs();

	while(1) {
		printf("# ");

		memset(cntrlBuff, 0, 100);
		fgets(cntrlBuff, 100, stdin);

		int numbytes = sizeof(cntrlBuff);
		cntrlBuff[numbytes]='\0'; //null terminate
		strtok(cntrlBuff, "\n");

		bool locallyHandled = this->handleCommand(cntrlBuff);
		if (locallyHandled) {
//			printf("Locally handled \n");
			if (numbytes == 0 || strncmp(cntrlBuff,"bye", 3) == 0) {
				break;
			}
			continue;
		}

//		int str_size = strlen(sendBuf);
//		int n = send(sockfd, &str_size, sizeof(str_size), 0);
//		if (n < 0) {
//			error("First ERROR writing to socket");
//		}
//
//		n = send(sockfd, sendBuf, str_size, 0);
//		if (n < 0) {
//			error("Second ERROR writing to socket");
//		}

		//send to server
		write(cntrlBuff);

		//read from server
		string res = read();

		//handle weirdness on empty message
		if (string(res) != "SIG_DONE") {
			printf("%s\n", res.c_str());
		}

//		memset(recvBuf, 0, 100);
//		int string_size = 0;
//		int nBytes = recv(sockfd, &string_size, sizeof(string_size), 0);
//		printf("Recv received %d\n", nBytes);
//		nBytes = recv(sockfd, recvBuf, string_size, 0);
//		printf("Second Recv received %d\n", nBytes);
//		recvBuf[string_size] = '\0';
//		if (nBytes == 0) {
//			printf("0 bytes received\n");
//			continue;
//		}
//
//		if (nBytes == -1) {
//			error("ERROR reading from socket");
//		}
//
//		if (string(recvBuf) != "SIG_DONE") {
//			printf("%s\n", recvBuf);
//		}
	}
}

void Client::sessionGet (string sfilePath) {
	printf("sessionGet\n");

	FILE * _fi = this->_fs->write(sfilePath);
	if (_fi == NULL) {
		printf("Failed to open file\n");
		return;
	}

	int fr_block_sz = 0;

    //100byte read
	while((fr_block_sz = recv(sockfd, recvBuf, MAX_DATA_SIZE, 0)) > 0) {
			int write_sz = fwrite(recvBuf, sizeof(char), fr_block_sz, _fi);
			printf("writing byte to local file %d\n", write_sz);
			printf("writing data to local file %s\n", recvBuf);

			if(write_sz < fr_block_sz)
			{
				error("File write failed.\n");
			}
			bzero(recvBuf, MAX_DATA_SIZE);
			if (fr_block_sz == 0 || fr_block_sz != MAX_DATA_SIZE)
			{
				printf("Breaking out download %d\n", fr_block_sz);
				break;
			}
	}

	if(fr_block_sz < 0) {
		if (errno == EAGAIN) {
			printf("recv() timed out.\n");
		} else {
			fprintf(stderr, "recv() failed due to errno = %d\n", errno);
		}
	}

	printf("Ok received from server!\n");
	fclose(_fi);
}

bool Client::handleCommand (string control) {
	string toClient = "";
	string copyControl = control;
	commandStruct cs = this->_fs->parseControl(copyControl);

//	printf("Control structure %s %s\n", cs.control.c_str(), cs.rPath.c_str());
	string cntrl = cs.control;
	string path = cs.rPath;
//	printf("Control structure parsed: %s %s \n",cntrl.c_str(), path.c_str());

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
//		string fileToSave = this->_fs->pwd().append("/").append(path);

	    string fileName = path;
		printf("File will be save locally to %s\n", fileName.c_str());
		printf("File will be save locally to %s\n", control.c_str());

		//send the control to server.
		write(control);

		//blocking call to wait for download.
		sessionGet(fileName);

		printf("Done downloading file\n");
	} else if (cntrl == BYE) {
		printf("bye\n");
		write(cntrlBuff);
	}
	else {
		printf("Command locally not handled \n");
		return false;
	}

	if (toClient.size() > 0) {
		printf("%s\n", toClient.c_str());
	}

	return true;
}

std::string Client::read () {

	memset(recvBuf, 0, 100);

	int string_size = 0;

	int nBytes = recv(sockfd, &string_size, sizeof(string_size), 0);

	printf("Recv received %d\n", nBytes);
	nBytes = recv(sockfd, recvBuf, string_size, 0);

	printf("Second Recv received %d\n", nBytes);
	recvBuf[string_size] = '\0';

	if (nBytes == 0) {
		printf("0 bytes received\n");
	}

	if (nBytes == -1) {
		error("ERROR reading from socket");
	}

	return recvBuf;
}

void Client::write(string message) {

	if (message == "") {
		message = "SIG_DONE";
	}

	printf("Sending message back %s\n", message.c_str());

	int str_size = strlen(message.c_str());
	int n = send(sockfd, &str_size, sizeof(str_size), 0);
	n = send(sockfd, message.c_str(), str_size, 0);

	if (n == -1) {
		error("ERROR writing to socket");
	}
	printf("echo back: %s\n", message.c_str());
}

//int main(int argc, char *argv[])
//{
//    int sockfd, portno, n;
//    struct sockaddr_in serv_addr;
//    struct hostent *he;
//
//    char rcvBuffer[100];
//    char sendBuffer[100];
//
////    if (argc < 3) {
////       fprintf(stderr,"usage %s hostname port\n", argv[0]);
////       exit(0);
////    }
//    portno = atoi(argv[2]);
//
////	int ret = inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
////	if (ret == 0) {
////	  error("Invalid IP address");
////	  return 0;
////	}
//
////	char ipa [500];
////	inet_ntop(AF_INET, &serv_addr.sin_addr, ipa, INET_ADDRSTRLEN);
////	printf("Client will connect at %s \n\n", ipa);
//
//	he = gethostbyname(argv[1]);
//    if (he == NULL) {
//        fprintf(stderr,"ERROR, no such host\n");
//        exit(1);
//    }
//
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//    if (sockfd == -1) {
//        error("ERROR opening socket");
//        exit(1);
//    }
//
//
////    bzero((char *) &serv_addr, sizeof(serv_addr));
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_port = htons(portno);
//    bcopy((char *)he->h_addr,
//             (char *)&serv_addr.sin_addr.s_addr,
//			 he->h_length);
//    memset(&(serv_addr.sin_zero), '\0', 8);
////    struct
//
////    bcopy((char *)server->h_addr,
////         (char *)&serv_addr.sin_addr.s_addr,
////         server->h_length);
////    serv_addr.sin_port = htons(portno);
//
//    printf("Connecting...\n");
//    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) == -1) {
//        error("ERROR connecting");
//        exit(1);
//    }
//
//    printf("Client connected...\n");
//
//    //read the entry
////    bzero(rcvBuffer, 100);
////	read(sockfd, rcvBuffer,100);
////	printf("%s\n\n", rcvBuffer);
//
//	while(1) {
//	    bool quit = 0;
//
//		printf("# ");
//
//		bzero(sendBuffer, 100);
//		bzero(rcvBuffer, 100);
//
//		//Read from stdInput.
//		fgets(sendBuffer, 100, stdin);
//
////		char *pos;
////		if ((pos=strchr(Name, '\n')) != NULL)
////		    *pos = '\0';
//
//		int numbytes = sizeof(sendBuffer);
//		sendBuffer[numbytes]='\0'; //null terminate
////		strtok(sendBuffer, "\n");
//
//		if (numbytes == 0 || strncmp(sendBuffer,"bye", 3) == 0) {
//			printf("bye\n");
//			//close the socket
//			send(sockfd, sendBuffer, sizeof(sendBuffer), 0);
//			break;
//		}
//
//		printf("Sending to server: %s\n", sendBuffer);
//		n = send(sockfd, sendBuffer, sizeof(sendBuffer), 0);
//
////		printf("error %d", errno);
//
//		if (n < 0) {
//			error("ERROR writing to socket");
//		}
//
//		printf("Byte written to socket %s", sendBuffer);
//
//		n = recv(sockfd, rcvBuffer, 127, 0);
//		if (n < 0) {
//			error("ERROR reading from socket");
//			exit(1);
//		}
//
//		rcvBuffer[n] ='\0';
//
//		printf("%s\n", rcvBuffer);
//    }
//
//    printf("Outside of Loop closing!!!");
//    close(sockfd);
//    return 0;
//}
