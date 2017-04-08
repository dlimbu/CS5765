/*
 * session.h
 *
 *  Created on: Mar 3, 2016
 *      Author: Dlimbu
 */

#include "../fs/FileSystem.h"
#include <cerrno>

#ifndef SRC_SERVER_SESSION_H_
#define SRC_SERVER_SESSION_H_

#define MSS 100

class Session {
private:

	FileSystem * _fs;
	int _sockFd;
	int _pid;

	char cntrlBuffer[MSS];
	char sendBuffer[MSS];
	void _sessionGet(string pathToFile);

public:
	Session(int sockFd, int pid);

	/**
	 *Open a file
	 */
	void openFile () {
		_fs = new FileSystem();
		string storage = "/serverStorage";
		try {
			_fs->openFs(storage.c_str());
		} catch (exception& e) {
			printf("Error %d \n", errno);
		}
	}

	virtual ~Session();
	int getSessionId();
	int getSockFd();
	void startSession();
	void stopSession();
	void handleCommand (std::string const& control);
	void onError(const char *msg);
	string read ();
	void write (std::string v);
	void dispose();
};

#endif /* SRC_SERVER_SESSION_H_ */
