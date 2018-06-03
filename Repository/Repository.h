#pragma once
///////////////////////////////////////////////////////////////////////
// Repository.h - Handles files and package checkin into the repository //
// ver 1.0                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////
/*
*  Maintenance History:
*  --------------------
*  Ver 1.0 : 23 Feb 2018
*  - first release
*
*/

#include<string>
#include "../DbCore/Definitions.h"
#include "../FileSystem/FileSystem.h"

namespace NoSqlDb {
	class Repository {
	public:
		Repository() { createRepo(); }
		//path getPath() { return FileSystem::Directory::getCurrentDirectory()+_pathname; }
		path getPath() { return _pathname; }
		bool addFile(filepath, nameSpace, version);
		bool getFile(filename, nameSpace, filepath, version);
		bool getFiletoCurDir(filename, nameSpace, version);
	private:
		static path _pathname;
		void createRepo();
	};
	//path Repository::_pathname = "../REPOSITORY";
}