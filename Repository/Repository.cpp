#include "Repository.h"
#include "../FileSystem/FileSystem.h"
#include <iostream>

using namespace NoSqlDb;
using namespace FileSystem;

/*
* Check the availability of repo directory
* if not present, will create a new dir at the given path
*/
void Repository::createRepo() {
	if (!Directory::exists(Repository::_pathname)) {
		if (!Directory::create(Repository::_pathname)) {
			throw std::exception("Cannot create repository");
		}
	}
}

/*
* Add new file to the repository
* Input : filepath - file to be copied
*         nameSpace - dir inside repo root; where file will be copied
*         version - version number of latest checkin
*/
bool Repository::addFile(filepath file, nameSpace dirName, version vNum) {
	if (!File::exists(file)) {
		std::cout << "\n Error! File not present: "<<file;
		return false;
	}
	std::string path = getPath() + "\\" + dirName;
	if (!Directory::exists(path))
		Directory::create(path);
	return File::copy(file, path + "\\" + Path::getName(file) +"."+std::to_string(vNum));
}

/*
* get file from the repository
* Input : filename - file to get
*         nameSpace - dir inside repo root; where file can be found
*         version - version number of file to fetch
*/
bool Repository::getFile(filename name, nameSpace frmDir, filepath toDir, version vNum){
	path file = getPath() + "\\" + frmDir + "\\" + name + "."+std::to_string(vNum);
	if (!File::exists(file))
		return false;
	return File::copy(file, Directory::getCurrentDirectory() + "\\" + toDir + "\\" + name);
}

/*
* get file from the repository to current dir
* Input : filename - file to get
*         nameSpace - dir inside repo root; where file can be found
*         version - version number of file to fetch
*/
bool Repository::getFiletoCurDir(filename name, nameSpace frmDir, version vNum) {
	path file = getPath() + "\\" + frmDir + "\\" + name + "." + std::to_string(vNum);
	if (!File::exists(file))
		return false;
	return File::copy(file, Directory::getCurrentDirectory() + "\\" + name);
}