#include "Query.h"
#include "Conditions.h"
#include<regex>
using namespace NoSqlDb;

template <typename T>
Keys& Query<T>::keys() {
	return keys_;
}

template <typename T>
Query<T>::Query(DbCore<T>& db) {
	db_ = db;
}

template <typename T>
Query<T>& Query<T>::select(const Conditions& c, bool flagAndOr) {
	//Check whether keys has been initialized
	if (keys_.size() == 0) {
		if (db_.size()==0)
			return *this;
		for (const auto& it : db_) {
			if (regexSearch(it.second.name(), c.regName) &&
				regexSearch(it.second.descrip(), c.regDescip) &&
				regexSearch(it.second.children(), c.regChildrn) && 
				regexSearch(it.second.dateTime(), c.regdateTime)
				)
				keys_.push_back(it.first);
		}
	}
	return *this;
}

bool regexSearch(std::string buffer, std::regex exp) {
	return std::regex_search(buffer, exp);
}

bool regexSearch(std::vector<std::string> vec, std::regex exp) {
	for (const auto& it : vec) {
		if (std::regex_search(it, exp))
			return true;
	}
	return false;
}

bool regexSearch(DateTime date, dateTimeCond exp) {
	return ((date > exp.from) && (date < exp.to));
}

template <typename T>
Query<T>& Query<T>::from(Keys& keys) {
	keys_ = keys;
}

template <typename T>
void Query<T>::show(void) {
	if (keys_.size() == 0)
		return;
	NoSqlDb::showHeader();
	for (const auto& it : keys_) {
		NoSqlDb::showElem(db_[it]);
	}
}

//----< test stub >--------------------------------------------------

#ifdef TEST_QUERY

#include <iostream>
#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../Utilities/TestUtilities/TestUtilities.h"
#include "../DbCore/DbCore.h"

class DbProvider
{
public:
	DbCore<std::string>& db() { return db_; }
private:
	static DbCore<std::string> db_;
};

DbCore<std::string> DbProvider::db_;

int main()
{
	Utilities::Title("Testing Query class");

	DbProvider dbp;
	DbCore<std::string> db = dbp.db();

	DbElement<std::string> demoElem = db["OOD"];

	demoElem.name("Shashank");
	demoElem.descrip("CIS687");
	demoElem.payLoad("Taught by Dr Fawcett");
	demoElem.dateTime(DateTime().now());
	db["OOD"] = demoElem;
	//if (!db.contains("ML"))
	//	return false;
	DbElement<std::string> demoElem1 = db["ML"];
	demoElem1.name("Singh Shashank");
	demoElem1.descrip("CIS700");
	demoElem1.payLoad("Taught by Dr Phoha");
	db["ML"] = demoElem1;

	db["ML"].children().push_back("Week1");
	db["ML"].children().push_back("Week2");
	db["ML"].children().push_back("Week3");
	db["ML"].children().push_back("Month1");

	Query<std::string> qy = Query<std::string>(db);
	Conditions cond;
	// "\\b(File)([^ ]*)"
	cond.name("\\b(Singh)([^ ]*)").descrip("\\b(CIS)([^ ]*)").children("\\b(Week)([^ ]*)").dateTime("Mon Jan  1 20:20:21 2018");
	qy.select(cond, false).show();
	std::cout << "\n\n";
}
#endif