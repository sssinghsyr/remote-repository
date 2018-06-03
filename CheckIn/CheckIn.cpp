/////////////////////////////////////////////////////////////////////////////
// CheckIn.cpp - CheckIn individual file in the repo                       //
// ver 1.0                                                                 //
// ----------------------------------------------------------------------- //
// Author:      Shashank Singh, Syracuse University                        //
/////////////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  CheckIn files into NoSQl db and perform all query into it.

  Public Interface:
  =================
  makeDb()                // create db

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


/*--------------- PACKAGE TEST SUBS -----------------*/
#ifdef __TEST_CHECKIN__
#include "CheckIn.h"
#include "../Repository/Repository.h"
#include "../DbCore/DbCore.h"
#include "../PayLoad/PayLoad.h"
#include "../Utilities/StringUtilities/StringUtilities.h"

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

	showDb(db);

	putLine();
	return true;
}

//----< demo requirement #2 >------------------------------------------
bool testCheckInFile()
{
	DbProvider dbp;
	DbCore<PayLoad> &db = dbp.db();
	std::cout << "\n  Checkin new file";
	CheckIn("DbCore", "D:\\Spring2018\\Project\\OOD\\DbCore\\DbCore.h").checkInFile();
	return CheckIn("DbCore", "D:\\Spring2018\\Project\\OOD\\DbCore\\DbCore.cpp").checkInFile();
}

path Repository::_pathname = "REPOSITORY_DIR";
int main()
{
	bool status = true;
	std::cout << "\n  Testing Version";
	std::cout << "\n =========================";

	DbCore<PayLoad> db;
	DbProvider dbp;
	dbp.db() = db;

	status &= makeDb();
	status &= testCheckInFile();

	if (!status)
		std::cout << "\n  ========Test failed=============";
	std::cout << "\n\n";
	getchar();
}
#endif