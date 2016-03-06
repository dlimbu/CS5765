/*
 * ClientNetwokManager.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: Dlimbu
 */

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
#include "fs/FileSystem.h"
#include "client/client.h"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }

    printf("Entry Client \n");
    int portnumber = atoi(argv[2]);
    string ipAddress = argv[1];
    int portno = atoi(argv[2]);
    Client * c = new Client();
    c->start(ipAddress, portno);

//	printf("##### Initializing fileSystem ##### \n");
//	fs = new FileSystem();
//
//	string storage = "/storage";
//	printf("FileSystem opening storage %s\n", storage.c_str());
//	try {
//		fs->openFs(storage.c_str());
//	} catch (exception& e) {
//		printf("Error %d\n", errno);
//	}
//
//	printf("##### Done initializing FileSystem ##### \n");
//
//    int sockfd, portno, n;
//    struct sockaddr_in serv_addr;
//    struct hostent *server;
//
//    char sendBuf[100];
//    char recvBuf[100];
//
//    if (argc < 3) {
//       fprintf(stderr,"usage %s hostname port\n", argv[0]);
//       exit(0);
//    }
//    portno = atoi(argv[2]);
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//
//    if (sockfd < 0)
//        error("ERROR opening socket");
//    server = gethostbyname(argv[1]);
//    if (server == NULL) {
//        fprintf(stderr,"ERROR, no such host\n");
//        exit(0);
//    }
//    bzero((char *) &serv_addr, sizeof(serv_addr));
//    serv_addr.sin_family = AF_INET;
//
//    //////////////////
//    int portnumber = atoi(argv[2]);
//    string ipAddress = argv[1];
//    Client * c = new Client();
//    c->start(ipAddress, portno);
//    //////////////////
//
//    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <=0) {
//           printf("\n inet_pton error occured\n");
//           return 1;
//    }
//
//    serv_addr.sin_port = htons(portno);
//    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
//        error("ERROR connecting");
//
//	string s = "\t\tWelcome to SFTP 0.0.1 \n\tAccepted commands: ls, pwd, cd, get";
//	printf("%s\n", s.c_str());
//    while(1) {
//    	printf("# ");
//
//    	memset(this.sendBuf, 0, 100);
//		fgets(sendBuf, 100, stdin);
//
//		int numbytes = sizeof(sendBuf);
//		sendBuf[numbytes]='\0'; //null terminate
//		strtok(sendBuf, "\n");
//
//		if (numbytes == 0 || strncmp(sendBuf,"bye", 3) == 0) {
//			printf("bye\n");
//			//close the socket
//			int str_size = strlen(sendBuf);
//			send(sockfd, &str_size, sizeof(str_size), 0);
//			send(sockfd, sendBuf, str_size, 0);
//			break;
//		}
//
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
//
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
//    }
//    printf("Socket closed \n");
//    close(sockfd);
    return 0;
}

