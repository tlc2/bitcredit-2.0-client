#include "trust.h"

#include "bidtracker.h"
#include "util.h"
#include "utilmoneystr.h"
#include "base58.h"
#include "primitives/transaction.h"
#include "primitives/block.h"

using namespace std;

int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void TrustEngine::createdb()
{
   sqlite3 *rawdb;
   char *zErrMsg = 0;
   int rc;

	vector<const char*> sql;

   rc = sqlite3_open((GetDataDir() /"ratings/rawdata.db").string().c_str(), &rawdb);
   if( rc ){
      if(fDebug)LogPrintf("Can't open database: %s\n", sqlite3_errmsg(rawdb));
      exit(0);
   }else{
      if(fDebug)LogPrintf("Opened database successfully\n");
   }

   /* Create SQL statements */
   sql.push_back("CREATE TABLE RAWDATA("  \
         "ADDRESS TEXT PRIMARY KEY      NOT NULL," \
         "BALANCE           INTEGER     DEFAULT 0," \
         "FIRSTSEEN         INTEGER     DEFAULT 0," \
         "TXINCOUNT         INTEGER     DEFAULT 0," \
         "TXOUTCOUNT        INTEGER     DEFAULT 0," \
         "TOTALIN           INTEGER     DEFAULT 0," \
         "TOTALOUT          INTEGER     DEFAULT 0);");

  sql.push_back("CREATE TABLE BLOCKS(" \
            "    ID INTEGER PRIMARY KEY AUTOINCREMENT," \
            "    HASH TEXT," \
            "    TIME INTEGER," \
            "    MINER TEXT);");

   sql.push_back("CREATE TABLE RATINGS("  \
         "ADDRESS TEXT PRIMARY KEY      NOT NULL," \
         "TRUST         REAL     DEFAULT 0," \
         "CREDITRATING        REAL     DEFAULT 0," \
         "REGISTERED           TEXT     DEFAULT 0," \
         "BAN        TEXT     DEFAULT 0," \
         "LIMIT          INTEGER     DEFAULT 0);");

   /* Execute SQL statements */
	for (unsigned int i =0;i < sql.size();i++){
		rc = sqlite3_exec(rawdb, sql[i], callback, 0, &zErrMsg);
		if( rc != SQLITE_OK ){
			if (fDebug)LogPrintf("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		else{
			if (fDebug)LogPrintf( "Tables created successfully\n");
		}
	}

	if(sqlite3_close(rawdb) != SQLITE_OK ){
		if (fDebug)LogPrintf("SQL unable to close database %s\n", sqlite3_errmsg(rawdb));
		sqlite3_free(zErrMsg);
	}else{
		if (fDebug)LogPrintf( "database closed successfully\n");
	}
}

std::map<std::string,int64_t> getbalances(){
std::map<std::string,int64_t> addressvalue;
	fstream myfile ((GetDataDir()/ "ratings/balances.dat").string().c_str());
	char * pEnd;
	std::string line;
	if (myfile.is_open()){
		while ( myfile.good() ){
			getline (myfile,line);
			if (line.empty()) continue;
			std::vector<std::string> strs;
			boost::split(strs, line, boost::is_any_of(","));
			addressvalue[strs[0]]=strtoll(strs[1].c_str(),&pEnd,10);
		}
		myfile.close();
	}
	return addressvalue;
}
