/*
 * TestBed.cpp
 *
 *  Created on: Mar 5, 2016
 *      Author: Dlimbu
 */

#include <string>
#include <stdio.h>
#include <iostream>

using namespace std;

int main () {

	string path = "/hello/this/isTest";
	path.replace(path.find("/"), 1, "");
	printf("Replaced string %s \n", path.c_str());

	return 0;
}
