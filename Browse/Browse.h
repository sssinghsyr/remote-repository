#pragma once
////////////////////////////////////////////////////////////////////////////////
// Browse.h - provides the capability to locate files and view their contents //
// ver 1.0                                                                    //
// Shashank Singh, CSE687 - Object Oriented Design, Spring 2018               //
////////////////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines a tokenizer class.  Its instances read words from 
  an attached file or string.

  Public Interface:
  =================
  setApp()                        // set App for project
  identify()               // identify content
  runQuery()      // run Query
  showFile()      // show file content
  showResult()       // show result
  showDescrip()                  // show description of data

  Build Process:
  ==============
  Required files
    - Browse.h, Browse.cpp
  Build commands (either one)
    - devenv NoSqlDbProject4.sln
    - cl /EHsc /DTEST_TOKENIZER Browse.cpp
*
*  Maintenance History:
*  --------------------
*  Ver 1.0 : 30 Apr 2018
*  - first release
*
*/
#include "../DbCore/Definitions.h"
#include "../DbCore/DbCore.h"
#include "../PayLoad/PayLoad.h"
#include "../Version/Version.h"
#include "../Process/Process.h"
#include <regex>
#include <set>

using namespace NoSqlDb;

class Browse {
public:
	using position = enum { NAMESPACE, FILENAME };
	Browse(): db_(DbProvider::db()) {}
	void showFile(nameSpace, filename);
	template<typename CallObj>
	Browse& runQuery(CallObj);
	Browse& findFile(nameSpace, filename);
	int showResult(std::ostream&);
	void showDescrip(std::ostream&);
	std::vector<filename> noParentKeys();
	Browse& setApp(filepath path) { _appPath = path; return *this; }
	static void identify(std::ostream& out = std::cout){
		out << "\n  \"" << __FILE__ << "\"";
	}
	std::vector<filename>& files() { return fileLocation; }
private:
	//bool existFile();
	std::string getName(filename, position);
	DbCore<PayLoad>& db_;
	std::vector<filename> fileLocation; // Entry will be in the format of namespace::filename, no version number
	//std::set<filename> fileLocation; // Entry will be in the format of namespace::filename, no version number
	filepath _appPath;
};

/*----< supports application defined queries for Repository core >---------*/
/*
*  - CallObj is defined by the application to return results from
*    application's payload.  See test stub for example uses.
*/
template<typename CallObj>
Browse& Browse::runQuery(CallObj callObj) {
	Keys newKeys;
	for (auto item : db_){
		if (callObj(item.second))
			newKeys.push_back(item.first);
	}
	fileLocation = newKeys;
	return *this;
}

/*
*  - Reply list of keys having no Parents
*/
std::vector<filename> Browse::noParentKeys() {
	Keys keys;
	for (auto item : db_.keys())
		keys.push_back(item);
	for (auto item : db_) {
		for (auto it : item.second.children()) {
			Keys::iterator position = std::find(keys.begin(), keys.end(), it);
			if (position != keys.end())
				keys.erase(position);
		}
	}
	return std::move(keys);
}

/*
* Show content of file
*/
void Browse::showFile(nameSpace dir, filename name) {
	if (_appPath.empty())
		_appPath = "c:/windows/system32/notepad.exe";
	Process p;
	p.application(_appPath);
	Key key = dir + "::" + name + "."+std::to_string(Version(dir, name).getLatestClosedVers(db_));
	filepath path = db_[key].payLoad().value();

	std::string cmdLine = "/A " + path;
	p.commandLine(cmdLine);
	std::cout << "\n  starting process: \"" << _appPath << "\"";
	std::cout << "\n  with this cmdlne: \"" << cmdLine << "\"";
	p.create();
	CBP callback = []() { std::cout << "\n  --- child process exited with this message ---"; };
	p.setCallBackProcessing(callback);
	p.registerCallback();

	std::cout << "\n  after OnExit";
	std::cout.flush();
	char ch;
	std::cin.read(&ch, 1);
}

/*
* return namespace string or filename based upon input flag
*/
std::string Browse::getName(filename name, position flag) {
	size_t pos = name.find("::");
	if(flag == NAMESPACE)
		return name.substr(0, pos);
	else
		return name.substr(pos+2, name.length());
}

/*
* Show result of file browsing
* Display namespace and files only.
*/
int Browse::showResult(std::ostream& out = std::cout) {
	int counter = 0;
	showHeader(true, out);
	for (auto item : fileLocation)
	{
		counter++;
		showRecord(item, db_[item], out);
	}
	return counter;
}

/*
* Show result description of file after browsing
* Display all content from NoSqlDb
*/
void Browse::showDescrip(std::ostream& out = std::cout) {
	PayLoad::showPayLoadHeaders(out);
	for (auto it : fileLocation)
		PayLoad::showElementPayLoad(db_[it], out);
}

/*
* find file from the NoSqlDb based on given query
* Return all the list of files which matches the input query
*/
Browse& Browse::findFile(nameSpace dir, filename name) {
	std::string rKey;
	dir == "*" ? rKey += "(.*)" : rKey += dir;
	rKey += "(::)";
	name == "*" ? rKey += "(.*)" : rKey += name;
	std::regex rex(rKey);
	for (auto it : db_.keys()) {
		if (std::regex_search(it, rex))
			fileLocation.push_back(it);
			//fileLocation.insert(Version::removeVer(it));
	}
	return *this;
}

