#pragma once
///////////////////////////////////////////////////////////////////////
// CheckIn.h - Handles files and package checkin into the repository //
// ver 1.0                                                           //
// Shashank Singh, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  CheckIn files into NoSQl db and perform all query into it.

  Public Interface:
  =================
  CheckIn()                // CheckIn new file into the resppository

  Build Process:
  ==============
  Required files
    - CheckIn.h, CheckIn.cpp
  Build commands (either one)
    - devenv NoSqlDbProject4.sln
    - cl /EHsc /DTEST_TOKENIZER CheckIn.cpp
*
*  Maintenance History:
*  --------------------
*  Ver 1.0 : 30 Apr 2018
*  - first release
*
*/
#include "../DbCore/Definitions.h"
#include "../FileSystem/FileSystem.h"
#include "../Version/Version.h"
#include "../PayLoad/PayLoad.h"
#include "../Repository/Repository.h"

using namespace NoSqlDb;

class CheckIn{
public:
	CheckIn(nameSpace dir, filepath name) : _dir(dir), _filepath(name) { _version = 0; }
	bool checkInFile();
	static void identify(std::ostream& out = std::cout)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
private:
	nameSpace _dir;
	filepath _filepath;
	version _version;
};

//----< check in new file >------------------------------------------
bool CheckIn::checkInFile() {
	filename _name = FileSystem::Path::getName(_filepath);
	_version = Version(_dir, _name).getNxtVers(DbProvider::db());
	return Repository().addFile(_filepath, _dir, _version);
}