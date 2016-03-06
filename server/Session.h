/*
 * session.h
 *
 *  Created on: Mar 2, 2016
 *      Author: Dlimbu
 */

#include "../fs/FileSystem.h"
#include <cerrno>

#ifndef SRC_SERVER_SESSION_H_
#define SRC_SERVER_SESSION_H_

#define RECV_WDW_BYTE 100
#define MAX_DATA_SIZE 100
#define MAX_CLIENT_SUPPORT 100

class Session {
private:

	FileSystem * _fs;

	int _sockFd;
	int _pid;

	char cntrlBuffer[MAX_DATA_SIZE];
	char sendBuffer[MAX_DATA_SIZE];
	void _sessionGet(string pathToFile);

public:
	Session(int sockFd, int pid);

	void openFile () {

		printf("Initializing fileSystem \n");
		_fs = new FileSystem();

		string storage = "/serverStorage";
		printf("FileSystem opening storage %s\n", storage.c_str());
		try {
			_fs->openFs(storage.c_str());
		} catch (exception& e) {
			printf("Error %d\n", errno);
		}

		printf("Done initializing fs\n");

	}

	virtual ~Session();
	int getSessionId();
	int getSockFd();
	void startSession();
	void stopSession();
	void handleCommand (std::string const& control);
	void onError(const char *msg);
	std::string read ();
	void write (std::string v);
	void dispose();
};

#endif /* SRC_SERVER_SESSION_H_ */
