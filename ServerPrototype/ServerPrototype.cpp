/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0              functionalities                                               //
//                                                                                    //
//Language:      C++ 11                                                               //
// Platform    : HP Omen, Win Pro 10, Visual Studio 2017                              //
// Application : CSE-687 OOD Project 3                                                //
// Author      : Shashank Singh ,Syracuse University                                  //
// Source      : Dr. Jim Fawcett, EECS, SU                                            //
////////////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* The DateTime class represents clock time, and supports:
* - creating default instances and instances from specific time points
* - return times as formatted strings
* - building time points and durations from years, months, days, hours, ...
* - performing addition and subtraction of times
* - comparing times
* - extracting counts of years, months, days, hours, minutes, and seconds
*
* Required Files:
* ---------------
*   ServerPrototype.h
*
* Maintenance History:
* --------------------
* ver 1.1 : 10 Apr 2018
*  - Project 3 implementation
* ver 1.0 : 18 Feb 2018
* - first release
*
* Description
* - echo() : echo back message from client
* - getFiles() : reply files in given dir
* - getDirs() : reply dirs in given dir
* - connection() : make new connection with client
* - readFile() : send file content message to client
* - getFile() : send full file content to client
* - checkIn() : perform checkIn command on obtained file and namespace
* - checkOut() : perform checkOut command on obtained file and namespace
* - getMetaData() : perform getMetaData command on obtained file and namespace
*/
#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../DbCore/DbCore.h"
#include "../Query/Query.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../Process/Process.h"
#include "../Persist/Persist.h"
#include "../Edit/Edit.h"
#include "../Executive/Executive.h"
#include "../RepositoryCore/RepositoryCore.h"
#include "../Browse/Browse.h"
#include <chrono>
#include <utility>
#include <fstream>

namespace MsgPassComm = MsgPassingCommunication;
const bool test = true;

using namespace ServerRepository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

/*
- get files from the ServerRepository directory
*/
Files Server::getFiles(const ServerRepository::SearchPath& path)
{
  return Directory::getFiles(path);
}

/*
- get directories in the ServerRepository directory
*/
Dirs Server::getDirs(const ServerRepository::SearchPath& path)
{
  return Directory::getDirectories(path);
}

namespace MsgPassingCommunication
{
	// These paths, global to MsgPassingCommunication, are needed by 
	// several of the ServerProcs, below.
	// - should make them const and make copies for ServerProc usage

	std::string sendFilePath;
	std::string saveFilePath;
	/*
	- show template function based on types of message
	*/
	template<typename T>
	void show(const T& t, const std::string& msg)
	{
		std::cout << "\n  " << msg.c_str();
		for (auto item : t)
		{
			std::cout << "\n    " << item.c_str();
		}
	}

	/*
	- read file dispatcher function
	*/
	std::function<Msg(Msg)> readFile = [](Msg msg) {
		Msg reply = msg;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.remove("content");
		reply.attribute("content", "File sent from repository server| File name: " + msg.value("filename"));
		return reply;
	};

	/*
	- echo message dispatcher function
	*/
	std::function<Msg(Msg)> echo = [](Msg msg) {
		Msg reply = msg;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.remove("message");
		reply.attribute("message", "Echoed back from Server->" + msg.value("message"));
		return reply;
	};

	/*
	- connection command dispatcher function
	*/
	std::function<Msg(Msg)> connection = [](Msg msg) {
		Msg reply = msg;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.attribute("message", "Connected");
		if (test) {
			std::cout << "Received connection command\n";
			reply.attribute("testMes", "Connected successfully");
		}
		return reply;
	};

	/*
	- getFiles of given dir command dispatcher function
	*/
	std::function<Msg(Msg)> getFiles = [](Msg msg) {
		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.command("getFiles");
		std::string path = msg.value("path");
		if (path != "")
		{
			std::string searchPath = storageRoot;
			if (path != ".")
				searchPath = searchPath + "\\" + path;
			Files files = Server::getFiles(searchPath);
			size_t count = 0;
			for (auto item : files)
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("file" + countStr, item);
			}
		}
		else
		{
			std::cout << "\n  getFiles message did not define a path attribute";
		}
		return reply;
	};

	/*
	- getFile to read command dispatcher function
	*/
	std::function<Msg(Msg)> getFile = [](Msg msg) {
		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.command("sendFile");
		reply.attribute("sendingFile", msg.value("fileName"));
		reply.attribute("fileName", msg.value("fileName"));
		std::string path = msg.value("path");
		if (path != "")
		{
			std::string searchPath;
			if (path != ".")
				searchPath = path;
			std::string filePath = searchPath + "/" + msg.value("fileName");
			std::string fullSrcPath = FileSystem::Path::getFullFileSpec(filePath);
			std::string fullDstPath = sendFilePath;
			if (!FileSystem::Directory::exists(fullDstPath))
			{
				std::cout << "\n  file destination path does not exist";
				return reply;
			}
			fullDstPath += "/" + msg.value("fileName");
			FileSystem::File::copy(fullSrcPath, fullDstPath);
		}
		else
		{
			std::cout << "\n  getDirs message did not define a path attribute";
		}
		return reply;
	};
	/*
	- parse string
	*/
	std::vector<std::string> parseStr(std::string str, char delm) {

		size_t pos = 0;
		std::vector<std::string> tokens;
		while ((pos = str.find(delm)) != std::string::npos) {
			if(str.substr(0, pos).length() > 0)
				tokens.push_back(str.substr(0, pos));
			str.erase(0, pos + 1);
		}
		return std::move(tokens);
	}

	/*
	- checkIn local file command dispatcher function
	*/
	std::function<Msg(Msg)> checkIn = [](Msg msg) {
		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.command("checkIn");
		if (test) 
			std::cout << "\nReceived checkin command\n";
		DbElement<PayLoad>& elem = RepositoryCore(msg.value("namespace"), saveFilePath +"\\"+msg.value("fileName")).checkIn<PayLoad>();
		if (msg.containsKey("descrp")) elem.descrip(msg.value("descrp"));
		if (msg.value("cStatus") == "1") elem.payLoad().closeStatus() = PayLoad::close;
		if (msg.containsKey("childs")) {
			std::vector<std::string> vec = parseStr(msg.value("childs"), '|');
			for (std::string it : vec)
				elem.addChildKey(it);
		}
		if (msg.containsKey("catgs")) {
			std::vector<std::string> vec1 = parseStr(msg.value("catgs"), '|');
			for (std::string it : vec1) 
				elem.payLoad().categories().push_back(it);
		}
		reply.attribute("return", msg.value("fileName")+": CheckedIn successfully");
		return reply;
	};

	/*
	- checkOut file from server command dispatcher function
	*/
	std::function<Msg(Msg)> checkOut = [](Msg msg) {
		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.command("checkOut");
		if (test) {
			std::cout << "\nReceived checkout command\n";
			std::cout << "File :" << msg.value("filename") << std::endl;
			std::cout << "Namespace :" << msg.value("namespace") << std::endl;
			reply.attribute("testMes", "CheckOut file sent successfully " + msg.value("filename"));
		}
		return reply;
	};

	/*
	- Reply no parent keys to the server
	*/
	std::function<Msg(Msg)> noParent = [](Msg msg) {
		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.command("noParent");
		int cnt = 0;
		for (auto it : Browse().noParentKeys()) {
			reply.attribute("key"+std::to_string(cnt), it);
			cnt++;
		}
		return reply;
	};

	/*
	- checkOut file from server command dispatcher function
	*/
	std::function<Msg(Msg)> query = [](Msg msg) {
		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.command("query");
		Browse data = Browse();
		data.findFile(msg.containsKey("namespace") ? msg.value("namespace") : "*", msg.containsKey("filename") ? msg.value("filename") : "*");
		if (msg.containsKey("desrip")) { 
			auto hasValue = [&](DbElement<PayLoad>& elem) { return (elem.descrip().find(msg.value("desrip")) != std::string::npos); };
			data.runQuery(hasValue);
		}
	    	
		if (msg.containsKey("echoMessage"))
			reply.attribute("echoMessage", msg.value("echoMessage"));
		if (msg.containsKey("catgs")) {
			std::vector<std::string> vec1 = parseStr(msg.value("catgs"), '|');
			for (std::string it : vec1) {
				auto lambda = [&it](DbElement<PayLoad>& elem) { return (elem.payLoad().hasCategory(it));};
				data.runQuery(lambda);
			}
		}
		if (msg.containsKey("depns")) {
			std::vector<std::string> vec1 = parseStr(msg.value("depns"), '|');
			for (std::string it : vec1) {
				auto lambda = [&it](DbElement<PayLoad>& elem) { return elem.containsChildKey(it);};
				data.runQuery(lambda);
			}
		}
		if (msg.containsKey("cStatus")) {
			auto lambda = [&](DbElement<PayLoad>& elem) { return statusString[elem.payLoad().closeStatus()].compare(msg.value("cStatus")); };
			data.runQuery(lambda);
			
		}
		std::ofstream outfile;
		std::string qName = msg.value("queryName");
		outfile.open("../ServerPrototype/temp/"+ qName);
		data.showResult(outfile);
		reply.attribute("sendingFile", qName);
		reply.attribute("fileName", qName);
		outfile.close();
		FileSystem::File::copy("../ServerPrototype/temp/"+ qName, sendFilePath+"/"+ qName);
		return reply;
	};
	/*
	- get meta data of a file command dispatcher function
	*/
	std::function<Msg(Msg)> getMetaData = [&](Msg msg) {
		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.command("getMetaData");
		reply.attribute("filename",msg.value("filename"));
		std::vector<filename> br = Browse().findFile(msg.value("namespace"), msg.value("filename")).files();
		if (br.size() < 1) {
			reply.attribute("error", "No record found");
			return reply;
		}
		std::string search_ver;
		if (msg.value("version") != "0")
			search_ver = msg.value("version");
		else
			search_ver = std::to_string(Version(msg.value("namespace"), msg.value("filename")).getLatestVers(DbProvider::db()));
		Key key = msg.value("namespace") + "::" + msg.value("filename") + "." + search_ver;
		if (!DbProvider::db().contains(key)) {
			reply.attribute("error", "No record found");
			return reply;
		}
		DbElement<PayLoad> elem = DbProvider::db()[key];
		reply.attribute("descrip", elem.descrip());
		reply.attribute("dateTime", elem.dateTime());
		std::string depnds;
		for (auto it : elem.children())
			depnds += "|" + it;
		if (depnds.length() > 0) reply.attribute("depnds", depnds);
		reply.attribute("status", statusString[elem.payLoad().closeStatus()]);
		std::string categ;
		for (auto it : elem.payLoad().categories())
			categ += "|" + it;
		if(categ.length() > 0) reply.attribute("categ", categ);
		std::string ver;
		std::string idx; 
		int i = 0;
		for (std::string it : Version(msg.value("namespace"), msg.value("filename")).getAllVersions(DbProvider::db())) {
			ver += "|" + it;
			if (it == search_ver)
				idx = std::to_string(i);
			i++;
		}
		ver += "|" + idx;
		reply.attribute("versions", ver);
		return reply;
	};

	/*
	- get Directories at server end command dispatcher function
	*/
	std::function<Msg(Msg)> getDirs = [](Msg msg) {
		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.command("getDirs");
		std::string path = msg.value("path");
		if (path != "")
		{
			std::string searchPath = storageRoot;
			if (path != ".")
				searchPath = searchPath + "\\" + path;
			Files dirs = Server::getDirs(searchPath);
			size_t count = 0;
			for (auto item : dirs)
			{
				if (item != ".." && item != ".")
				{
					std::string countStr = Utilities::Converter<size_t>::toString(++count);
					reply.attribute("dir" + countStr, item);
				}
			}
		}
		else
		{
			std::cout << "\n  getDirs message did not define a path attribute";
		}
		return reply;
	};
	/*
	*  - Demonstrate checking full project files
	*/
	bool checkinRepo()
	{
		std::cout << "\n  Checkin all the current project codes into repository";
		Keys children;
		RepositoryCore("DbCore", "..\\Browse\\Browse.h").checkIn<PayLoad>();
		RepositoryCore("DbCore", "..\\Browse\\Browse.cpp").checkIn<PayLoad>();
		RepositoryCore("DbCore", "..\\DbCore\\DbCore.h").checkIn<PayLoad>();
		RepositoryCore("DbCore", "..\\DbCore\\DbCore.cpp").checkIn<PayLoad>();
		children.push_back("DbCore::DbCore.cpp.1");
		children.push_back("DateTime::DateTime.cpp.1");
		RepositoryCore("DateTime", "..\\DateTime\\DateTime.cpp").checkIn<PayLoad>();
		RepositoryCore("DateTime", "..\\DateTime\\DateTime.h").checkIn<PayLoad>();
		RepositoryCore("Query", "..\\Query\\Query.h").addDependency(children).checkIn<PayLoad>().payLoad().categories().push_back("Query header file");
		RepositoryCore("Query", "..\\Query\\Query.cpp").addDependency(children).checkIn<PayLoad>();
		RepositoryCore::closeCheckinAll();
		RepositoryCore("DbCore", "..\\CheckIn\\CheckIn.h").addDependency(children).checkIn<PayLoad>();
		RepositoryCore("DbCore", "..\\CheckIn\\CheckIn.cpp").addDependency(children).checkIn<PayLoad>();
		RepositoryCore("DbCore", "..\\CheckOut\\CheckOut.h").addDependency(children).checkIn<PayLoad>();
		RepositoryCore("DbCore", "..\\CheckOut\\CheckOut.cpp").addDependency(children).checkIn<PayLoad>();
		RepositoryCore("DbCore", "..\\Edit\\Edit.h").checkIn<PayLoad>();
		RepositoryCore("DbCore", "..\\Edit\\Edit.cpp").checkIn<PayLoad>();

		return true;
	}

}
path Repository::_pathname = "..\\Storage";

using namespace MsgPassingCommunication;

int main(){
  std::cout << "\n  Testing Server Prototype"; std::cout << "\n ==========================\n";
  sendFilePath = FileSystem::Directory::createOnPath("../ServerPrototype/SendFiles");
  saveFilePath = FileSystem::Directory::createOnPath("../ServerPrototype/SaveFiles");
  NoSqlDb::DbCore<PayLoad> db;
  NoSqlDb::DbProvider dbp;
  dbp.db() = db;
  checkinRepo();
  Server server(serverEndPoint, "ServerPrototype", dbp.db());
  server.start();
  std::cout << "\n  testing getFiles and getDirs methods"; std::cout << "\n --------------------------------------";
  Files files = server.getFiles();
  show(files, "Files:");
  Dirs dirs = server.getDirs();
  show(dirs, "Dirs:");
  std::cout << "\n\n  testing message processing"; std::cout << "\n ----------------------------";
  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", echo);
  server.addMsgProc("connection", connection);
  server.addMsgProc("readFile", readFile);
  server.addMsgProc("getFile", getFile);
  server.addMsgProc("checkIn", checkIn);
  server.addMsgProc("checkOut", checkOut);
  server.addMsgProc("getMetaData", getMetaData);
  server.addMsgProc("query", query);
  server.addMsgProc("noParent", noParent);
  server.processMessages();
  Msg msg(serverEndPoint, serverEndPoint);  // send to self
  msg.name("msgToSelf");
  msg.command("echo");
  msg.attribute("verbose", "show me");
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  msg.command("getFiles");
  msg.remove("verbose");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  msg.command("getDirs");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "\n  press enter to exit";
  std::cin.get(); std::cout << "\n";
  msg.command("serverQuit");
  server.postMessage(msg); server.stop();
  return 0;
}

