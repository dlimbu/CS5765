/*
 * fileSystem.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: Dlimbu
 */

#include "FileSystem.h"
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
#include <cerrno>

FileSystem::FileSystem() {
	_dir = NULL;
}

FileSystem::~FileSystem() {

}
