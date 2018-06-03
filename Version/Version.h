#pragma once
///////////////////////////////////////////////////////////////////////
// Version.h - define aliases used throughout NoSqlDb namespace      //
// ver 1.0                                                           //
// Shashank Singh, CSE687 - Object Oriented Design, Spring 2018      //
///////////////////////////////////////////////////////////////////////
/*
* Module Operations:
*===================
*Version controlling
*
* Build Process:
* --------------
*   devenv NoSqlDbProject4.sln /debug rebuild
*
* public Interfaces:
* =================
* Version(): Return version of a file present in the repository
*
* Maintainance History:
* =====================
* ver 1.0
*
*/

#include "../DbCore/Definitions.h"
#include "../DbCore/DbCore.h"
#include <regex>
using namespace NoSqlDb;

class Version {
public:
	template<typename T> int getNxtVers(DbCore<T>&);
	template<typename T> int getLatestVers(DbCore<T>&);
	template<typename T> int getLatestClosedVers(DbCore<T>&);
	template<typename T> std::vector<std::string> getAllVersions(DbCore<T>&);
	template<typename T> bool existVers(int, DbCore<T>&);
	Version(nameSpace dirName, filename name) :_dir(dirName) , _name(name) {}
	static filename removeVer(Key);
	static void identify(std::ostream& out = std::cout)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
private:
	template<typename T> int getMaxVers(bool condition, DbCore<T>&);
	nameSpace _dir;
	filename _name;
};

/*
* Get the last position of deliminator
* return zero, if not found
*/
int getLastIdxDlm(std::string str, char del) {
	for (int i = str.length(); i >= 0; i--) {
		if (str[i] == del)
			return i + 1;
	}
	return 0;
}

/*
* Remove version number for given key
*/
filename Version::removeVer(Key key) {
	return key.substr(0, getLastIdxDlm(key, '.')-1);
}

/*
* Get the Latest version number for checkin
* condition : Condition to value closeStatus
* Error: If file is not present, will return '0' as version
*/
template<typename T>
int Version::getMaxVers(bool condition, DbCore<T>& db) {
	int max = 0;
	std::regex re(_dir + "::" + _name);
	for (auto it : db.keys()) {
		if (condition && !db[it].payLoad().isClosed())
			continue;
		if (std::regex_search(it, re)) {
			int imax = stoi(it.substr(getLastIdxDlm(it, '.'), it.length()));
			if (imax > max)
				max = imax;
		}
	}
	return max;
}

/*
* Get the all versions
* condition : Condition to value closeStatus
* Error: If file is not present, will return '0' as version
*/
template<typename T>
std::vector<std::string> Version::getAllVersions(DbCore<T>& db) {
	std::vector<std::string> retVal;
	std::regex re(_dir + "::" + _name);
	for (auto it : db.keys()) {
		if (std::regex_search(it, re)) {
			std::string ver = it.substr(getLastIdxDlm(it, '.'), it.length());
			retVal.push_back(ver);
		}
	}
	return std::move(retVal);
}
/*
* Get the next version number for checkin
* Error: If file is not present, will return '1' as version
*/
template<typename T>
int Version::getNxtVers(DbCore<T>& db) {
	return getLatestVers(db) + 1;
}

/*
* Get the latest version from the database
* Error: If file is not present, will return '1' as version
*/
template<typename T>
int Version::getLatestVers(DbCore<T>& db) {
	return getMaxVers(false, db);
}

/*
* Get the latest version from the database, will check for closed checkin files only
* Error: If file is not present, will return '1' as version
*/
template<typename T>
int Version::getLatestClosedVers(DbCore<T>& db) {
	return getMaxVers(true, db);
}

/*
* Check wether input version exist
* Used while checking out file explicitly demanded by user
* If the past checkin is not closed, it will return false
*/
template<typename T>
bool Version::existVers(int version, DbCore<T>& db) {
	Key _key = _dir + "::" + _name +"."+std::to_string(version);
	return db.contains(_key) && db[_key].payLoad().isClosed();
}