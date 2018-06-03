#pragma once
//////////////////////////////////////////////////////////////////////////
// CheckOut.h - Handles files and package checkout from the repository //
// ver 1.0                                                            //
// Shashank Singh, CSE687 - Object Oriented Design, Spring 2018       //
///////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  CheckOut files into NoSQl db and perform all query into it.

  Public Interface:
  =================
  checkOutFile()                // create db
  identify()                    // identify content
  
  Build Process:
  ==============
  Required files
    - CheckOut.h, CheckOut.cpp
  Build commands (either one)
    - devenv NoSqlDbProject4.sln
    - cl /EHsc /DTEST_TOKENIZER CheckOut.cpp
*
*  Maintenance History:
*  --------------------
*  Ver 1.0 : 30 Apr 2018
*  - first release
*
*/
#include "../DbCore/Definitions.h"
#include "../Version/Version.h"
#include "../Repository/Repository.h"

using namespace NoSqlDb;

class CheckOut {
public:
	CheckOut(nameSpace dir, filename name) : _dir(dir), _name(name) { _version = 0; }
	bool checkOutFile(filepath toDir, version ver);
	static void identify(std::ostream& out = std::cout)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
private:
	nameSpace _dir;
	filename _name;
	version _version;
};

/*
* Checkout file from the repository
* Check for the version first
* If given version is not closed, will decrement version to checkout only if asked for latest version
* Remove suffix of version number and copy demanded file in the current diretory
*/
bool CheckOut::checkOutFile(filepath toDir, version ver = 0) {

	ver == 0 ? _version = Version(_dir, _name).getLatestClosedVers(DbProvider::db()) : _version = ver;
	if (!Version(_dir, _name).existVers(_version, DbProvider::db())) {
		std::cout << "\n Error! : File with this given version not present";
		return false;
	}

	if(toDir == ".")
		return Repository().getFiletoCurDir(_name, _dir, _version);
	else
		return Repository().getFile(_name, _dir, toDir, _version);
}