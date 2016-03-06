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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//void ServerSocket::start(const char * ip, int port) {
//	 int sockfd, newsockfd, portno;
//	 socklen_t clilen;
//	 char buffer[256];
//	 struct sockaddr_in serv_addr, cli_addr;
//	 int n;
//
//	 sockfd = socket(AF_INET, SOCK_STREAM, 0);
//	 if (sockfd < 0)
//		error("ERROR opening socket");
//
//	 bzero((char *) &serv_addr, sizeof(serv_addr));
//	 portno = port;
//
//	 serv_addr.sin_family = AF_INET;
//	 serv_addr.sin_addr.s_addr = inet_addr(ip);
//	 serv_addr.sin_port = htons(portno);
//
//	 if (bind(sockfd, (struct sockaddr *) &serv_addr,
//			  sizeof(serv_addr)) < 0)
//			  error("ERROR on binding");
//	 listen(sockfd,5);
//
//	 clilen = sizeof(cli_addr);
//
//	 newsockfd = accept(sockfd,
//				 (struct sockaddr *) &cli_addr,
//				 &clilen);
//
//	 if (newsockfd < 0)
//		  error("ERROR on accept");
//
//	 bzero(buffer,256);
//	 n = read(newsockfd,buffer,255);
//	 if (n < 0) error("ERROR reading from socket");
//	 printf("Here is the message: %s\n",buffer);
//	 n = write(newsockfd,"I got your message",18);
//	 if (n < 0) error("ERROR writing to socket");
//
//	 close(newsockfd);
//	 close(sockfd);
//}
//Store IP in serverAddr
// store this IP address in sin_addr:
//	  int ret = inet_pton(AF_INET, ip.c_str(), &this->serverAddr.sin_addr);
//	  if (ret == 0) {
//		  error("Invalid IP address");
//		  return;
//	  }

// now get it back and print it
//	  char ipa [500];
//	  inet_ntop(AF_INET, &this->serverAddr.sin_addr, ipa, INET_ADDRSTRLEN);
//	  printf("Server starting at %s \n\n", ipa);


/**
 *
 */
void ServerSocket::start(const char * ip, int port) {

	 if (port <= 0) {
		 fprintf(stderr,"ERROR, no port provided\n");
		 exit(1);
	 }

	 //AF_INET: Internet domain
	 //SOCK_STREAM: streaming socket i.e. TCP
	 //0 TCP or UDP based on the SOCK_STREAM
	 //creates a socket and returns a file descriptor;
	 this->_serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);

	 if (this->_serverSocketFD == -1) {
		  error("ERROR opening socket");
	 } else {
		 fprintf(stdout,"Socket open successful!!\n");
	 }

	  int portno = port;
//	  this->serverAddr.sin_family = AF_INET; //Socket Internet domain
//	  this->serverAddr.sin_port = htons(portno); //save as network byte order;
//	  this->serverAddr.sin_addr.s_addr = INADDR_ANY; //server host 4 byte IP address

	  this->serverAddr.sin_family = AF_INET;
	  this->serverAddr.sin_addr.s_addr = inet_addr(ip);
	  this->serverAddr.sin_port = htons(portno);
	  // store this IP address in sa:
//	  inet_pton(AF_INET, ip.c_str(), &(this->serverAddr.sin_addr));

//	  this->serverAddr.sin_addr.s_addr = INADDR_ANY;
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

	  //big endian: MSB at the lower address, reads higher byte first.
	  //little endian: LSB at the lower address, reads lower byte first.
//	  printf("Server running in: %s\n", this->readIp(str, this->serverAddr));

	  //start listening for connection in the server socket.
	  //passive state
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

	  	unsigned int sin_size;

	  	while (1) {

	  		sin_size = sizeof(struct sockaddr_in);

	  		printf("\nENTRY IN LOOP\n");

	  		//Process will wait until the client is connected.
	  		//A new file descriptor will be created for the connection and
	  		//used for communicating to the client.
	  		if ((this->newSockFd = accept(this->_serverSocketFD, (struct sockaddr *) &this->clientAddr, &sin_size)) == -1) {
	  			perror("accept");
	  			continue;
	  		}

	  		printf("Client connected %s \n\n", inet_ntoa(this->clientAddr.sin_addr));

	  		char * recvBuf;
	  		int numBytes;
	  		int pid = fork();
			if (pid < 0){
				error("ERROR on fork");
			}

			printf("Parent process: %d\n", ::getppid());
			printf("Current process: %d\n", ::getpid());
			printf("New Process: %d\n", pid);

			if (pid == 0)  {
					close(this->_serverSocketFD);
					Session * s = new Session(this->newSockFd, ::getpid());
					s->startSession();

					printf("Right after startSession(), process: %d\n", ::getpid());
					::kill(s->getSessionId(), SIGKILL);
					exit(0);
			}
			else {
				close(this->newSockFd);
			    printf("ELSE Parent process pid: %d \n", ::getpid());
			}
			printf("Common code execution: %d \n", ::getpid());
//	  		if(!fork()) {
//	  			close(this->_serverSocketFD);
//	  			recvBuf = (char *) calloc(128, sizeof(char));
//
//	  			for (;;) {
//	  				numBytes = recv(this->newSockFd, recvBuf, 128, 0);
//	  				if (numBytes < 0) {
//	  					perror("recv");
//	  					close(this->newSockFd);
//	  					exit(1);
//	  				} else if(strncmp(recvBuf, "bye", 3) == 0) {
//	  					printf("bye recieved");
//	  					close(this->newSockFd);
//	  					exit(0);
//	  				}
//
//	  				printf("receive from client %s", recvBuf);
//	  				if (send(this->newSockFd,recvBuf, numBytes, 0) == -1) {
//	  					perror("send error");
//	  					close(this->newSockFd);
//	  					exit(1);
//	  				}
//	  			}
//
//	  			close(this->newSockFd);
//	  			exit(0);
//	  		}
//	  		printf("Parent done this\n");
//	  		close(this->newSockFd);
	  	}
}

void ServerSocket::spawnProcess () {

	int pid = fork();
	if (pid < 0){
		error("ERROR on fork");
	}

	printf("New Process id spawned %d", pid);

	if (pid == 0)  {
		close(this->_serverSocketFD);
		const char *k = "";
		this->readFrom(k);
		this->writeTo(k);
		printf("child killed %d", getpid());
		kill(getpid(), SIGKILL);
		exit(EXIT_SUCCESS);
	} else {
		close(this->newSockFd);
	}
}

void ServerSocket::onConnection () {
}



//		if (this->newSockFd < 0) {
//			error("ERROR on accept");
//			continue;
//		}
//
		//fork a process.
//		pid_t childP = fork();
//
//
//		if (childP < 0){
//			error("ERROR on fork");
//		}
//
//		printf("Parent process: %d\n", ::getppid());
//		printf("Current process: %d\n", ::getpid());
//		printf("New Process: %d\n", childP);
//
//		if (childP == 0)  {
//			printf("Initializing session..\n");
//			Session * s = new Session(this->newSockFd, ::getpid());
//			s->startSession();
//
//			printf("Right after startSession(), process: %d\n", ::getpid());
//			::kill(s->getSessionId(), SIGKILL);
//			exit(0);
//		} else {
//			close(this->newSockFd);
//			printf("ELSE Parent process pid: %d \n", ::getpid());
//		}
//
//		printf("Common code execution: %d \n", ::getpid());
//	}
//}

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
void ServerSocket::error(const char *msg)
{
    perror(msg);
    exit(1);
}

