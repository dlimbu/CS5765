/*
 * fileSystem.h
 *
 *  Created on: Mar 5, 2016
 *      Author: Dlimbu
 */

#ifndef SRC_FILESYSTEM_H_
#define SRC_FILESYSTEM_H_

#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <fcntl.h>
#include <sstream>

using namespace std;

//FILE info structure
struct fileInfo {
	int fd;
	int size;
};

struct commandStruct {
	string control;
	string rPath;
};

/**
 *File system error type
 */
enum FS_ERROR {
	//File errors.
	INVALID_FILE,
	ERROR_OPENING_FILE,

	//Directory errors
	ERROR_OPENING_DIR,
	ERROR_LISTING_DIR
};

class FileSystem {

private:

	fileInfo fInfo;
	DIR * _dir;
	struct dirent *ent;
	string _currPath;
	std::map<string, int> aCode;

	/**
	 *
	 */
	bool _equals(char * c1, char * c2) {
		return strcmp(c1, c2) == 0;
	}

	/**
	 *
	 */
	void _onError (int errorCode) { }

public:
	FileSystem();

	/**
	 *
	 */
	void openFs(string path) {
		string root = '.' + path;

		DIR * _t = opendir (root.c_str());

		if (_t == NULL) {
			printf("Error opening %s\n", root.c_str());
			return;
		}

		close();

		_dir = _t;
		_currPath = path;

		ls();
	}

	/**
	 * is File
	 */
	bool isFile (const char code) {
		return code == 0x8;
	};

	/**
	 * is directory
	 */
	bool isDir (const char code) {
		return code == 0x4;
	}

	/**
	 *
	 */
	string ls () {
		string _res;
		try {
			aCode.clear();
			while ((ent = readdir(_dir)) != NULL) {

				if (strcmp(ent->d_name, ".") == 0 ||
						strcmp(ent->d_name, "..") == 0) {
					continue;
				}

				if(isDir(ent->d_type)) {
					_res.append(ent->d_name);
					_res.append(" ");
					aCode[ent->d_name] = 0x4;
				} else if (isFile(ent->d_type)) {
					_res.append(ent->d_name);
					_res.append(" ");
					aCode[ent->d_name] = 0x8;
				}
			}
			rewinddir(_dir);
		} catch (exception& e) {
			printf("Exception in LS %s", e.what());
		}
		return _res;
	}

	/**
	 * Print working directory.
	 */
	string pwd () {
		return _currPath;
	}

	/**
	 *
	 */
	void back() {
		string path = _currPath;
		int i = 0;
		for (i = path.length() - 1; i >= 0; i--) {
			char v = path[i];
			if (v == '/') {
				break;
			}
		}

		if (i == 0) {
			return;
		}

		//remove the last path
		path = path.substr(0, i);
		openFs(path);
	}

	/**
	 *
	 */
	void cd (string dir) {
		if (dir.compare("") == 0 ||
				dir.compare(_currPath) == 0) {
			printf("Invalid CD\n");
			return;
		}

		int code = _getCode(dir);
		if (isFile(code)) {
			printf("Not a directory\n");
			return;
		}

		string cwdPath = _currPath;
		cwdPath.append("/").append(dir);
		openFs(cwdPath);
	}

	/**
	 * Code associated with a file.
	 */
	int _getCode (string file) {
		int code = -1;
		try {
			code = aCode.at(file);
		} catch (exception& e){
			printf("Invalid file name %s\n", file.c_str());
		}

		return code;
	}

	/**
	 *Write download data to file.
	 */
	FILE * write (string file) {
		string copy = _currPath;
		string fPath = "."+ copy.append("/").append(file);
		FILE * f = fopen(fPath.c_str(), "w");
		return f;
	}

	/**
	 * Return a fileDescriptor for the file stream.
	 * @return int
	 */
	FILE * read (string file) {

		printf("path and file name: %s\n", file.c_str());
		int code = _getCode(file);
		if (code < 0) {
			printf("Invalid file name\n");
			return NULL;
		}

		if (isDir(code)) {
			printf("Not a file\n");
			return NULL;
		}

		//find the first /
		string fp = _currPath;
		//replace with empty string
		fp.replace(fp.find("/"), 1, "");
		file = fp.append("/").append(file);

		FILE * f = fopen(file.c_str(), "r");
		return f;
	}

	commandStruct parseControl (string control) {
		string com [2];
		size_t pos = 0;
		char delim = ' ';
		string token;
		istringstream f(control);
		string s;

		int i = 0;
		while (std::getline(f, s, ' ')) {
			if (s == "") {
				continue;
			}
			com[i++] = s;
		}

		commandStruct cs;
		cs.control = com[0];
		cs.rPath = com[1];
		return cs;
	}

	/**
	 *
	 */
	void close () {
		if (aCode.size() >= 1) {
			aCode.clear();
		}

		if (_dir != NULL) {
			closedir(_dir);
		}
	}

	/**
	 *
	 */
	virtual ~FileSystem();
};


#endif /* SRC_FILESYSTEM_H_ */
