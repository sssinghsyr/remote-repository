/////////////////////////////////////////////////////////////////////////////
// RepositoryCore.cpp - Implementation of core functionality of repository //
// ver 1.0                                                                 //
// ----------------------------------------------------------------------- //
// Author:      Shashank Singh, Syracuse University                        //
/////////////////////////////////////////////////////////////////////////////
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

/*--------------- PACKAGE TEST SUBS -----------------*/
#ifdef __TEST_REPO_CORE__
#include "RepositoryCore.h"
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
bool testCheckInFile()
{
	DbProvider dbp;
	DbCore<PayLoad> &db = dbp.db();
	std::cout << "\n  Checkin new file";
	RepositoryCore file = RepositoryCore("DbCore", "DbCore.h");
	file.clientDir("D:\\Spring2018\\Project\\OOD\\DbCore");
	try {
		DbElement<PayLoad>& elem = file.checkIn<PayLoad>();
		// elem.addChildKey("elem1");
		elem.descrip("First checkin");
	}
	catch (std::exception& ex)
	{
		std::cout << "\n  Exception: " << ex.what() << "\n";
	}
	file.closeCheckin();
	showDb(db);
	PayLoad::showDb(db);
	return true;
}

//----< demo requirement #1 >------------------------------------------
bool testCheckInFileAgain()
{
	DbProvider dbp;
	DbCore<PayLoad> &db = dbp.db();
	std::cout << "\n  Checkin same file again";
	RepositoryCore file = RepositoryCore("DbCore", "DbCore.h");
	file.clientDir("D:\\Spring2018\\Project\\OOD\\DbCore");
	try {
		DbElement<PayLoad>& elem = file.checkIn<PayLoad>();
	}
	catch (std::exception& ex)
	{
		std::cout << "\n  Exception: " << ex.what() << "\n";
	}
	file.usePrevCheckinData();
	//file.closeCheckin();
	showDb(db);
	PayLoad::showDb(db);
	return true;
}

//----< demo requirement #1 >------------------------------------------
bool testCheckOutFile()
{
	DbProvider dbp;
	DbCore<PayLoad> &db = dbp.db();
	std::cout << "\n  Checkout file";
	RepositoryCore("DbCore", "DbCore.h").checkOut();
	showDb(db);
	PayLoad::showDb(db);
	return true;
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

	status &= testCheckInFile();
	status &= testCheckInFileAgain();
	status &= testCheckOutFile();
	if (!status)
		std::cout << "\n  ========Test failed=============";
	std::cout << "\n\n";
	getchar();
}
#endif