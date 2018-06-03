////////////////////////////////////////////////////////////////////////////////
// Browse.cpp - Browsing data                                                 //
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

/*--------------- PACKAGE TEST SUBS -----------------*/
#ifdef __TEST_BROWSE__
#include "Browse.h"
#include "../Repository/Repository.h"
#include "../RepositoryCore/RepositoryCore.h"
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
	RepositoryCore file1 = RepositoryCore("DbCore", "DbCore.h");
	file1.clientDir("D:\\Spring2018\\Project\\OOD\\DbCore");
	try {
		DbElement<PayLoad>& elem = file1.checkIn<PayLoad>();
		// elem.addChildKey("elem1");
		elem.descrip("First checkin");
	}
	catch (std::exception& ex)
	{
		std::cout << "\n  Exception: " << ex.what() << "\n";
	}
	file1.closeCheckin();

	RepositoryCore file2 = RepositoryCore("DbCore", "DbCore.cpp");
	file2.clientDir("D:\\Spring2018\\Project\\OOD\\DbCore");
	try {
		DbElement<PayLoad>& elem = file2.checkIn<PayLoad>();
		// elem.addChildKey("elem1");
		elem.descrip("First checkin");
	}
	catch (std::exception& ex)
	{
		std::cout << "\n  Exception: " << ex.what() << "\n";
	}
	file2.closeCheckin();
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
	Browse().findFile("DbCore", "*").showResult();
	Browse().findFile("*", "*.cpp").showResult();
	Browse().findFile("*", "*.h").showResult();
	Browse().showFile("DbCore","DbCore.h");
	Browse().showFile("DbCore", "DbCore.cpp");
	if (!status)
		std::cout << "\n  ========Test failed=============";
	std::cout << "\n\n";
	getchar();
}
#endif