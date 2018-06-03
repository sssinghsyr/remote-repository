#pragma once
/////////////////////////////////////////////////////////////////////
// DbCore.h - Implements NoSql database prototype                  //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides two classes:
* - DbCore implements core NoSql database operations, but does not
*   provide editing, querying, or persisting.  Those are left as
*   exercises for students.
* - DbElement provides the value part of our key-value database.
*   It contains fields for name, description, date, child collection
*   and a payload field of the template type. 
* The package also provides functions for displaying:
* - set of all database keys
* - database elements
* - all records in the database

* Required Files:
* ---------------
* DbCore.h, DbCore.cpp
* DateTime.h, DateTime.cpp
* Utilities.h, Utilities.cpp
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Jan 2018
* ver 1.1 : 2 Feb 2018
*/
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "../DateTime/DateTime.h"

namespace NoSqlDb
{
  using Key = std::string;
  using Keys = std::vector<Key>;
  using Category = std::string;
  using Categories = std::vector<Category>;
  using FilePath = std::string;
  
#ifndef _PAYLOAD_
#define _PAYLOAD_
// Payload structure, NoSql database will store this prototype.
  struct PayLoad {
	  Categories& categories() { return categories_; }
	  Categories categories() const { return categories_; }
	  void categories(const Categories& categories) { categories_ = categories; }

	  FilePath& fpath() { return fpath_; }
	  FilePath fpath() const { return fpath_; }
	  void fpath(const FilePath& fpath) { fpath_ = fpath; }
	  bool empty() { return fpath_.empty(); }
	  void clear() { categories_.clear(); fpath_.clear(); }
	  void edit(PayLoad& in) {
		  Categories cat = in.categories();
		  if (!cat.empty()) {
			  for (auto& it : cat) {
				  Categories::iterator itr = find(categories_.begin(), categories_.end(), it);
				  if (itr != categories_.end())
					  *itr = it;
				  else
					  categories_.push_back(it);
			  }
		  }
		  if (!in.fpath().empty() && (in.fpath().compare(fpath_) != 0))
			  fpath_ = in.fpath();
	  }
	  friend std::ostream& operator<<(std::ostream&, const PayLoad&);
  private:
	  Categories categories_;
	  FilePath fpath_;
  };
 
  inline
  std::ostream& operator<<(std::ostream& out, const PayLoad& p) {
	  out << p.fpath_;
	  return out;
  }
#endif
  /////////////////////////////////////////////////////////////////////
  // DbElement class
  // - provides the value part of a NoSql key-value database

  template<typename T>
  class DbElement
  {
  public:
    using Children = std::vector<Key>;

    // methods to get and set DbElement fields

    std::string& name() { return name_; }
    std::string name() const { return name_; }
    void name(const std::string& name) { name_ = name; }

    std::string& descrip() { return descrip_; }
    std::string descrip() const { return descrip_; }
    void descrip(const std::string& name) { descrip_ = name; }
    
    DateTime& dateTime() { return dateTime_; }
    DateTime dateTime() const { return dateTime_; }
    void dateTime(const DateTime& dateTime) { dateTime_ = dateTime; }

	Children& children() {return children_; }
	Children children() const { return children_; }
	void children(const Children& children) { children_ = children; }

    T& payLoad() { return payLoad_; }
    T payLoad() const { return payLoad_; }
    void payLoad(const T& payLoad) { payLoad_ = payLoad; }

	void clear();

  private:
    std::string name_;
    std::string descrip_;
    DateTime dateTime_;
    Children children_;
    T payLoad_;
  };

  template<typename T>
  void DbElement<T>::clear() {
	  name_.clear();
	  descrip_.clear();
	  children_.clear();
	  payLoad_.clear();
  }


  /////////////////////////////////////////////////////////////////////
  // DbCore class
  // - provides core NoSql db operations
  // - does not provide editing, querying, or persistance operations

  template <typename T>
  class DbCore
  {
  public:
    using Key = std::string;
    using Keys = std::vector<Key>;
    using Children = Keys;
    using DbStore = std::unordered_map<Key,DbElement<T>>;
    using iterator = typename DbStore::iterator;

    // methods to access database elements

    Keys keys();
    bool contains(const Key& key);
    size_t size();
    DbElement<T>& operator[](const Key& key);
    DbElement<T> operator[](const Key& key) const;
    typename iterator begin() { return dbStore_.begin(); }
    typename iterator end() { return dbStore_.end(); }

    // methods to get and set the private database hash-map storage

    DbStore& dbStore() { return dbStore_; }
    DbStore dbStore() const { return dbStore_; }
    void dbStore(const DbStore& dbStore) { dbStore_ = dbStore; }
	bool dbAddElem(const Key&, DbElement<T>&);
	bool dbEditElem(const Key&, DbElement<T>&);
	bool dbDelElem(const Key&);
	bool dbAddChild(const Key&, const Key&);
	bool dbDelChild(const Key&, const Key&);
  
  private:
    DbStore dbStore_;
  };

  /////////////////////////////////////////////////////////////////////
  // DbCore<T> methods

  //----< does db contain this key? >----------------------------------

  template<typename T>
  bool DbCore<T>::contains(const Key& key)
  {
    iterator iter = dbStore_.find(key);
    return iter != dbStore_.end();
  }
  //----< returns current key set for db >-----------------------------

  template<typename T>
  typename DbCore<T>::Keys DbCore<T>::keys()
  {
    DbCore<T>::Keys dbKeys;
    DbStore& dbs = dbStore();
    size_t size = dbs.size();
    dbKeys.reserve(size);
    for (auto item : dbs)
    {
      dbKeys.push_back(item.first);
    }
    return dbKeys;
  }
  //----< return number of db elements >-------------------------------

  template<typename T>
  size_t DbCore<T>::size()
  {
    return dbStore_.size();
  }
  //----< extracts value from db with key >----------------------------
  /*
  *  - indexes non-const db objects
  */
  template<typename T>
  DbElement<T>& DbCore<T>::operator[](const Key& key)
  {
    if (!contains(key))
    {
      dbStore_[key] = DbElement<T>();
    }
    return dbStore_[key];
  }
  //----< extracts value from db with key >----------------------------
  /*
  *  - indexes const db objects
  */
  template<typename T>
  DbElement<T> DbCore<T>::operator[](const Key& key) const
  {
    if (!contains(key))
    {
      dbStore_[key] = DbElement<T>();
    }
    return dbStore_[key];
  }
  //----< Delete DbElement record from db >----------------------------
  template<typename T>
  bool DbCore<T>::dbDelElem(const Key& key) {
	  if (!contains(key))
		  return false;
	  dbStore_.erase(key);
	  //Deleting relationship allover
	  typename DbCore<T>::DbStore& dbs = dbStore_;
	  for (auto& item : dbs)
	  {
		  if (findVec<Key>(item.second.children(), key))
			  dbDelChild(item.first, key);
	  }
	  return true;
  }

  // return true if value is found in the vector
  template<typename T>
  bool findVec(std::vector<T>& vec, const T& value) {
	  typename std::vector<T>::iterator it = std::find(vec.begin(), vec.end(), value);
	  if (it != vec.end())
		  return true;
	  return false;
  }

  //----< Add Child relationship to a record >----------------------------
  template<typename T>
  bool DbCore<T>::dbAddChild(const Key& key, const Key& child) {
	  if (!contains(key) || !contains(child)) {
		  std::cout << "\n Error! No such key to add";
		  return false;
	  }
	  Children& childrn = dbStore_[key].children();
	  Children::iterator it = find(childrn.begin(), childrn.end(), child);
	  if (it == childrn.end())
		  childrn.push_back(child);
	  return true;
  }

  //----< Delete Child relationship to a record >----------------------------
  template<typename T>
  bool DbCore<T>::dbDelChild(const Key& key, const Key& child) {
	  if (!contains(key)) {
		  std::cout << "\n Error! No such key to delete";
		  return false;
	  }
	  Children& childrn = dbStore_[key].children();
	  Children::iterator it = find(childrn.begin(), childrn.end(), child);
	  if (it != childrn.end())
		  childrn.erase(it);
	  return true;
  }

  //----< Add DbElement record into db >----------------------------
  template<typename T>
  bool DbCore<T>::dbAddElem(const Key& key, DbElement<T>& dbElem) {
	  if (contains(key))
		  return false;
	  //dbStore_.insert(key, dbElem);
	  dbStore_[key] = dbElem;
	  return true;
  }
  
  //----< Edit DbElement record into db >----------------------------
  template<typename T>
  bool DbCore<T>::dbEditElem(const Key& key, DbElement<T>& dbElem) {
	  if (!contains(key))
		  return false;
	  //Comparing name
	  if (!(dbElem.name().empty()) && (dbStore_[key].name().compare(dbElem.name()) != 0))
		  dbStore_[key].name() = dbElem.name();
	  //Comparing description
	  if (!(dbElem.descrip().empty()) && dbStore_[key].descrip().compare(dbElem.descrip()) != 0)
		  dbStore_[key].descrip() = dbElem.descrip();
	  //Comparing date time
	  if (!(dbStore_[key].dateTime() == dbElem.dateTime()))
		  dbStore_[key].dateTime() = dbElem.dateTime();
	  if (!dbElem.payLoad().empty()) {
		//Compare and edit payload
		  dbStore_[key].payLoad().edit(dbElem.payLoad());
	  }
	  return true;
  }

  /////////////////////////////////////////////////////////////////////
  // display functions

  //----< display database key set >-----------------------------------

  template<typename T>
  void showKeys(DbCore<T>& db, std::ostream& out = std::cout)
  {
    out << "\n  ";
    for (auto key : db.keys())
    {
      out << key << " ";
    }
  }
  //----< show record header items >-----------------------------------

  inline void showHeader(std::ostream& out = std::cout, bool flag = false)
  {
    out << "\n  ";
    out << std::setw(26) << std::left << "DateTime";
    out << std::setw(10) << std::left << "Name";
    out << std::setw(25) << std::left << "Description";
    out << std::setw(50) << std::left << "Payload";
	if(flag) out << std::setw(25) << std::left << "Categories";
    out << "\n  ";
    out << std::setw(26) << std::left << "------------------------";
    out << std::setw(10) << std::left << "--------";
    out << std::setw(25) << std::left << "-----------------------";
    out << std::setw(50) << std::left << "-----------------------";
	if (flag) out << std::setw(25) << std::left << "-----------------------";
  }
  //----< display DbElements >-----------------------------------------

  template<typename T>
  void showElem(const DbElement<T>& el, std::ostream& out = std::cout, bool flag = false)
  {
    out << "\n  ";
    out << std::setw(26) << std::left << std::string(el.dateTime());
    out << std::setw(10) << std::left << el.name();
    out << std::setw(25) << std::left << el.descrip();
	if (flag) {
		PayLoad py = el.payLoad();
		out << std::setw(50) << std::left << py;
		for (auto& it : py.categories()) {
			out << it << "|";
		}
	}else
		out << std::setw(25) << std::left << el.payLoad();
    typename DbElement<T>::Children children = el.children();
    if (children.size() > 0)
    {
      out << "\n    child keys: ";
      for (auto key : children)
      {
        out << " " << key;
      }
    }
  }
  //----< display all records in database >----------------------------

  template<typename T>
  void showDb(const DbCore<T>& db, std::ostream& out = std::cout, bool flag = false)
  {
    //out << "\n  ";
    //out << std::setw(26) << std::left << "DateTime";
    //out << std::setw(10) << std::left << "Name";
    //out << std::setw(25) << std::left << "Description";
    //out << std::setw(25) << std::left << "Payload";
    //out << "\n  ";
    //out << std::setw(26) << std::left << "------------------------";
    //out << std::setw(10) << std::left << "--------";
    //out << std::setw(25) << std::left << "-----------------------";
    //out << std::setw(25) << std::left << "-----------------------";
    showHeader(out, flag);
    typename DbCore<T>::DbStore dbs = db.dbStore();
    for (auto item : dbs)
    {
      showElem(item.second, out, flag);
    }
  }
}