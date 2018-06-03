/////////////////////////////////////////////////////////////////////////////
// CheckOut.cpp - CheckOut individual file from the repo                       //
// ver 1.0                                                                 //
// ----------------------------------------------------------------------- //
// Author:      Shashank Singh, Syracuse University                        //
/////////////////////////////////////////////////////////////////////////////
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


/*--------------- PACKAGE TEST SUBS -----------------*/
#ifdef __TEST_CHECKOUT__
#include "../DbCore/DbCore.h"
#include "../PayLoad/PayLoad.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include "CheckOut.h"
#include "../CheckIn/CheckIn.h"
#include "../Repository/Repository.h"
using namespace NoSqlDb;

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
//----< demo function >------------------------------------------
bool testCheckInFile()
{
	DbProvider dbp;
	DbCore<PayLoad> &db = dbp.db();
	std::cout << "\n  Checkin new file";
	DbElement<PayLoad> demoElem;

	CheckIn("DbCore", "D:\\Spring2018\\Project\\OOD\\DbCore\\DbCore.h").checkInFile();
	demoElem.name("DbCore.h");
	demoElem.descrip("DbCore header");
	demoElem.dateTime(DateTime().now());
	demoElem.payLoad(PayLoad("//DbCore//DbCore.h.1"));
	db["DbCore::DbCore.h.1"] = demoElem;
	
	CheckIn("DbCore", "D:\\Spring2018\\Project\\OOD\\DbCore\\DbCore.cpp").checkInFile();
	demoElem.name("DbCore.cpp");
	demoElem.descrip("DbCore main code");
	demoElem.dateTime(DateTime().now());
	demoElem.payLoad(PayLoad("//DbCore//DbCore.cpp.1"));
	db["DbCore::DbCore.cpp.1"] = demoElem;
	
	CheckIn("DbCore", "D:\\Spring2018\\Project\\OOD\\DbCore\\DbCore.cpp").checkInFile();
	demoElem.name("DbCore.cpp");
	demoElem.descrip("DbCore main code");
	demoElem.dateTime(DateTime().now());
	demoElem.payLoad(PayLoad("//DbCore//DbCore.cpp.2"));
	db["DbCore::DbCore.cpp.2"] = demoElem;
	return true;
}

//----< demo requirement #2 >------------------------------------------
bool testCheckOutFile()
{
	DbProvider dbp;
	DbCore<PayLoad> &db = dbp.db();
	std::cout << "\n  Checkin new file";
	return CheckOut("DbCore", "DbCore.h").checkOutFile(".", 1);
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

	//status &= makeDb();
	status &= testCheckInFile();
	status &= testCheckOutFile();

	if (!status)
		std::cout << "\n  ========Test failed=============";
	std::cout << "\n\n";
	getchar();
}
#endif