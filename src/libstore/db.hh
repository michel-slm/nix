#ifndef __DB_H
#define __DB_H

#include "types.hh"

#include <map>


/* Defined externally. */
class DbTxn;
class DbEnv;
class Db;


namespace nix {


class Database;


class Transaction
{
    friend class Database;

private:
    DbTxn * txn;
    
public:
    Transaction();
    Transaction(Database & _db);
    ~Transaction();

    void begin(Database & db);
    void abort();
    void commit();

    void moveTo(Transaction & t);
};


#define noTxn Transaction()


typedef unsigned int TableId; /* table handles */


class Database
{
    friend class Transaction;

private:
    DbEnv * env;

    TableId nextId;
    std::map<TableId, Db *> tables;

    void requireEnv();

    Db * getDb(TableId table);

    void open2(const string & path, bool removeOldEnv);
    
public:
    Database();
    ~Database();
    
    void open(const string & path);
    void close();

    TableId openTable(const string & table, bool sorted = false);
    void closeTable(TableId table);
    void deleteTable(const string & table);

    bool queryString(const Transaction & txn, TableId table, 
        const string & key, string & data);

    bool queryStrings(const Transaction & txn, TableId table, 
        const string & key, Strings & data);

    void setString(const Transaction & txn, TableId table,
        const string & key, const string & data);

    void setStrings(const Transaction & txn, TableId table,
        const string & key, const Strings & data,
        bool deleteEmpty = true);

    void delPair(const Transaction & txn, TableId table,
        const string & key);

    void enumTable(const Transaction & txn, TableId table,
        Strings & keys, const string & keyPrefix = "");
    
    /* TODO */
    Path makeStatePathRevision(const Path & statePath, const int revision);
    
    /* TODO */
    void splitStatePathRevision(const Path & revisionedStatePath, Path & statePath, int & revision);
    
    /* Set the stateReferences for a specific revision (and older until the next higher revision number in the table) */    
    void setStateReferences(const Transaction & txn, TableId table,
    	const Path & statePath, const int revision, const Strings & references);
    
    /* Returns the references for a specific revision (and older until the next higher revision number in the table) */
    bool queryStateReferences(const Transaction & txn, TableId table,
    	const Path & statePath, Strings & references, int revision = -1);
    
    /* Returns the referrers for a specific revision (and older until the next higher revision number in the table) */
    bool queryStateReferrers(const Transaction & txn, TableId table,
    	const Path & statePath, Strings & referrers, int revision = -1);
        
    /* Set the revision number of the statePath and the revision numbers of all state paths in the references closure */
    void setStateRevisions(const Transaction & txn, TableId table,
    	const Path & statePath, const int revision, const RevisionNumbers & revisions);
    
    /* Returns all the revision numbers of the state references closure of the given state path */
    bool queryStateRevisions(const Transaction & txn, TableId table,
    	const Path & statePath, RevisionNumbers & revisions, int revision = -1);
    
    /* Returns all available revision numbers of the given state path */
    bool queryAllStateRevisions(const Transaction & txn, TableId table,
    	const Path & statePath, RevisionNumbers & revisions);	
};


class DbNoPermission : public Error
{
public:
    DbNoPermission(const format & f) : Error(f) { };
};

 
}


#endif /* !__DB_H */
