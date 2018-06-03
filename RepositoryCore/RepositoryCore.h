#pragma once
#pragma once
///////////////////////////////////////////////////////////////////////
// RepositoryCore.h - Core functionality of Repository over NoSqlDb     //
// ver 1.0                                                           //
// Shashank Singh, CSE687 - Object Oriented Design, Spring 2018      //
///////////////////////////////////////////////////////////////////////
/*
* Module Operations:
*===================
* RepositoryCore.h - Core functionality of Repository over NoSqlDb
*
* Build Process:
* --------------
*   devenv NoSqlDbProject4.sln /debug rebuild
*
* public Interfaces:
* =================
* RepositoryCore(): Core functionality
*
* Maintainance History:
* =====================
* ver 1.0
*
*/
#include "../DbCore/Definitions.h"
#include "../FileSystem/FileSystem.h"
#include "../Version/Version.h"
#include "../PayLoad/PayLoad.h"
#include "../Repository/Repository.h"
#include "../CheckOut/CheckOut.h"
#include "../CheckIn/CheckIn.h"

using namespace NoSqlDb;

class RepositoryCore {
public:
	/*RepositoryCore(nameSpace dir, filename file) : _dir(dir), _filename(file) { _version = 0; _clientDir = "."; }*/
	RepositoryCore(nameSpace dir, filepath);
	RepositoryCore& ver(version ver) { _version = ver;  return *this; }
	RepositoryCore& clientDir(filepath );
	RepositoryCore& addDependency(Keys);
	version ver() { return _version; }
	bool checkOut();
	template<typename P> DbElement<P>& checkIn();
	template<typename P> DbElement<P>& getMetaHandle(version);
	bool closeCheckin();
	static bool closeCheckinAll();
	bool usePrevCheckinData();
	static void identify(std::ostream& out = std::cout)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}
private:
	template<typename P> DbElement<P>& addCheckinFileInDb();
	void getVersions();
	bool checkChildDepStatus();
	Key getKey(version);

	nameSpace _dir;                // namespace
	filename _filename;            // Just file name. No path
	version _version;              // Current version to be checkedin or checkout
	version _last_ver;             // Last active version; to pull meta data
	filepath _clientDir;           // Current working dir of client
	Keys _keys;
};

/*
* Add dependency for the newly added checkin file
*/
RepositoryCore& RepositoryCore::addDependency(Keys keys) {
	_keys = keys;
	return *this;
}

/*
* Get metadata handle of any given key to edit content in the repo.
*/
template<typename P> 
DbElement<P>& RepositoryCore::getMetaHandle(version ver) {
	_version = ver;
	DbCore<PayLoad> &db = DbProvider::db();
	if(db[getKey(ver)].payLoad().isClosed())
		throw std::exception("Version is closed.");
	return db[getKey(ver)];
}

/*
* Constructor to take full path as input
*/
RepositoryCore::RepositoryCore(nameSpace dir, filepath file) {
	if (!FileSystem::File::exists(file)) {
		_filename = file;
		_clientDir = ".";
	}
	else {
		_clientDir = FileSystem::Path::getPath(file);
		_filename = FileSystem::Path::getName(file);
	}
	_dir = dir;
	_version = 0;
}

/*
* Set client's local path to push or pull files
*/
RepositoryCore& RepositoryCore::clientDir(filepath dirpath) {
	if (!FileSystem::Directory::exists(dirpath)) {
		std::cout << "\n  Error! : Client dir path is not correct - [" << dirpath << "]";
	}
	_clientDir = dirpath;
	return *this;
}

/*
* close checkin
* check for status of all dependent files
*/
bool RepositoryCore::closeCheckin() {
	DbCore<PayLoad> &db = DbProvider::db();
	if (checkChildDepStatus() == false) {
		db[getKey(_version)].payLoad().closeStatus() = PayLoad::closing;
		std::cout << "\n  Cannot checkin given file, changing status to CLOSING. Close above stated OPEN checkins first";
		return false;
	}
	db[getKey(_version)].payLoad().closeStatus() = PayLoad::close;
	return true;
}

/*
* close checkin all
*/
bool RepositoryCore::closeCheckinAll() {
	DbCore<PayLoad> &db = DbProvider::db();
	for(auto it: db.keys())
		db[it].payLoad().closeStatus() = PayLoad::close;
	return true;
}

/*
* Check for child depencies, return false is one of it not closed
*/
bool RepositoryCore::checkChildDepStatus() {
	DbCore<PayLoad> &db = DbProvider::db();
	std::vector<Key> dependent;
	for (auto it : db[getKey(_version)].children()) {
		if (!db[it].payLoad().isClosed())
			dependent.push_back(it);
	}
	if(dependent.empty())
		return true;
	std::cout << "\n  CheckedIn file has dependent files OPEN: ";
	for (auto it : dependent)
		std::cout << it << ", ";
	return false;
}

/*
* Code checkout from the repository
*/
bool RepositoryCore::checkOut() {
	return CheckOut(_dir, _filename).checkOutFile(_clientDir, _version);
}
/*
* Code checkin into the repository
*/
template<typename P>
DbElement<P>& RepositoryCore::checkIn() {
	getVersions();
	if (!CheckIn(_dir, _clientDir+"\\"+_filename).checkInFile())
		throw std::runtime_error("Checkin Failed");
	return addCheckinFileInDb<P>();
}

/*
* Code checkin file to NoSqlDB
*/
template<typename P>
DbElement<P>& RepositoryCore::addCheckinFileInDb() {
	DbCore<PayLoad> &db = DbProvider::db();
	DbElement<PayLoad> elem = makeElement<PayLoad>(_filename, "Insertion required");
	PayLoad pl;
	Key key = getKey(_version);
	pl.value() = Repository().getPath() + "//" + _dir + "//" + _filename+"."+std::to_string(_version);
	pl.closeStatus() = PayLoad::open;
	elem.payLoad(pl);
	db.addRecord(key, elem);
	db[key].children(_keys);
	return db[key];
}

/*
* Will update checkedIn file's meta data from previous version's
* Following fields will be updated:
* - Description
* - Dependencies
* - Payload catagories
*/
bool RepositoryCore::usePrevCheckinData() {
	if (_last_ver == 0 ) {
		std::cout << "\n Error!: This is first checkin, or no previous checkins are closed. You need to insert metadata, Thank You!";
		return false;
	}
	DbCore<PayLoad> &db = DbProvider::db();
	db[getKey(_version)].descrip() = db[getKey(_last_ver)].descrip();
	db[getKey(_version)].children() = db[getKey(_last_ver)].children();
	db[getKey(_version)].payLoad().categories() = db[getKey(_last_ver)].payLoad().categories();
	return true;
}

/*
* Get latest versions from database.
*/
void RepositoryCore::getVersions() {
	_version = Version(_dir, _filename).getNxtVers(DbProvider::db());
	_last_ver = Version(_dir, _filename).getLatestClosedVers(DbProvider::db());
}

/*
* Create key string out of namespace::filename.version
*/
Key RepositoryCore::getKey(version ver) {
	return _dir + "::" + _filename + "." + std::to_string(ver);
}