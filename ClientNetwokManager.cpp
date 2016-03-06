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
    return 0;
}

