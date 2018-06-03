/////////////////////////////////////////////////////////////////////////////
// Version.cpp - Version management of individual file in repo             //
// ver 1.0                                                                 //
// ----------------------------------------------------------------------- //
// Author:      Shashank Singh, Syracuse University                        //
/////////////////////////////////////////////////////////////////////////////

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
* main(): Main function
*
* Maintainance History:
* =====================
* ver 1.0
*
*/

/*--------------- PACKAGE TEST SUBS -----------------*/
#ifdef __TEST_VERSION__
#include "Version.h"
#include "../DbCore/DbCore.h"
#include "../PayLoad/PayLoad.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include <functional>

///////////////////////////////////////////////////////////////////////
// DbProvider class
// - provides mechanism to share a test database between test functions
//   without explicitly passing as a function argument.
//
//class DbProvider
//{
//public:
//	DbCore<PayLoad>& db() { return db_; }
//private:
//	static DbCore<PayLoad> db_;
//};
//DbCore<PayLoad> DbProvider::db_;

///////////////////////////////////////////////////////////////////////
// test functions
//----< reduce the number of characters to type >----------------------

auto putLine = [](size_t n = 1, std::ostream& out = std::cout)
{
	Utilities::putline(n, out);
};

//----< demo requirement #1 >------------------------------------------

bool makeDb()
{
  std::cout << "\n  Creating a database";
  DbProvider dbp;
  DbCore<PayLoad> &db = dbp.db();
  std::cout << "\n  Creating a db element with key \"DateTime::DateTime.cpp.1\":";
  // create some demo elements and insert into db
  DbElement<PayLoad> demoElem;

  demoElem.name("DateTime.cpp");
  demoElem.descrip("DateTime main code");
  demoElem.dateTime(DateTime().now());
  demoElem.payLoad(PayLoad("//DateTime//DateTime.cpp.1"));
  db["DateTime::DateTime.cpp.1"] = demoElem;

  demoElem.name("DateTime.h");
  demoElem.descrip("DateTime header file");
  demoElem.dateTime(DateTime().now());
  demoElem.payLoad(PayLoad("//DateTime//DateTime.h.1"));
  db["DateTime::DateTime.h.1"] = demoElem;

  demoElem.name("DateTime.h");
  demoElem.descrip("Updated DateTime header file");
  demoElem.dateTime(DateTime().now());
  demoElem.payLoad(PayLoad("//DateTime//DateTime.h.2"));
  db["DateTime::DateTime.h.2"] = demoElem;

  showDb(db);

  putLine();
  return true;
}

//----< demo requirement #2 >------------------------------------------
bool testGetVersion()
{
	DbProvider dbp;
	DbCore<PayLoad> &db = dbp.db();
	std::cout << "\n  Get next version number of file to be checkedIn";
	
	DbElement<PayLoad> demoElem;
	demoElem.name("DateTime.cpp");
	demoElem.descrip("Updated DateTime main code");
	demoElem.dateTime(DateTime().now());
	demoElem.payLoad(PayLoad("//DateTime//DateTime.cpp.1"));
	db["DateTime::DateTime.cpp.2"] = demoElem;

	return Version("DateTime", "DateTime.cpp").getNxtVers(db) == 3 ? true : false;
}

//----< demo requirement #3 >------------------------------------------
bool testCheckVersion()
{
	DbProvider dbp;
	DbCore<PayLoad> &db = dbp.db();
	std::cout << "\n  Check version number of file for checkout";

	return Version("DateTime", "DateTime.cpp").existVers(2, db);
}

int main()
{
	bool status = true;
	std::cout << "\n  Testing Version";
	std::cout << "\n =========================";
	
	DbCore<PayLoad> db;
	DbProvider dbp;
	dbp.db() = db;

	status &= makeDb();
	status &= testGetVersion();
	status &= testCheckVersion();

	if (!status)
		std::cout << "\n  ========Test failed=============";
	std::cout << "\n\n";
	getchar();
}
#endif