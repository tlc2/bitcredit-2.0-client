// Copyright (c) 2014-2016 Minato Mathers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "loanmanager.h"
#include "util.h"
#include "addrman.h"
#include "base58.h"
#include "trust.h"
#include "main.h"

#include <fstream>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <sstream>
#include <algorithm>
#include <exception>
#include <set>
#include <stdint.h>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>

/** Loan manager */

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void CLoanManager::getverifieddata()
{
    std::string url = "";

    const char * c = url.c_str();

      std::string readBuffer;

      curl = curl_easy_init();
      if(curl) {
		curl_global_init(CURL_GLOBAL_ALL);
        curl_easy_setopt(curl, CURLOPT_URL, c);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bitcredit/0.30");
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        }
		if(res != CURLE_OK) {
			if (fDebug) LogPrintf("Curl Error on CLoanManager::getverifieddata() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
		}
		else {
			if (fDebug) LogPrintf("Curl Response on CLoanManager::getverifieddata() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
			}

    boost::filesystem::path loandir = GetDataDir() / "loandata";

    if(!(boost::filesystem::exists(loandir))){
        if(fDebug)LogPrintf("Loandir Doesn't Exists\n");

        if (boost::filesystem::create_directory(loandir))
            if(fDebug)LogPrintf("Loandir....Successfully Created !\n");
    }

	ofstream myfile((GetDataDir().string() + "/loandata/verifieddata.dat").c_str(),fstream::out);
	myfile << readBuffer << std::endl;
	myfile.close();

}

bool CLoanManager::sendloanrequest(string address, int64_t amount, int premium, int expiry, int period, string message, string tx){
 
#ifdef WIN32
    curl_global_init(CURL_GLOBAL_ALL);
#endif  

	std::stringstream raw;

	raw<<"address="<<address<<'&'<<"amount="<<amount<<'&'<<"premium="<<premium<<'&'<<"expiry="<<expiry<<'&'<<"period="<<period<<'&'<<"message="<<message<<'&'<<"tx="<<tx<<' ';
	
	string request = raw.str();

	curl = curl_easy_init();
	if(curl) {
			 
		curl_easy_setopt(curl, CURLOPT_URL, "http://bcr-e.com/loanbook/loanrequest/");
    
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.c_str());
 
		res = curl_easy_perform(curl);
 
		if(res != CURLE_OK){
			//fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			return false;
		}
		curl_easy_cleanup(curl);
  }
	return true;
}

bool CLoanManager::sendloan(string address,string receiver,string  reqtx ,int64_t amount,string requestid,string message,string tx){
 
#ifdef WIN32
    curl_global_init(CURL_GLOBAL_ALL);
#endif  

	std::stringstream raw;

	raw<<"address="<<address<<'&'<<"receiver="<<receiver<<'&'<<"reqtx="<<reqtx<<'&'<<"amount="<<amount<<'&'<<"requestid="<<requestid<<'&'<<"message="<<message<<'&'<<"tx="<<tx<<' ';
	
	string request = raw.str();

	curl = curl_easy_init();
	if(curl) {
			 
		curl_easy_setopt(curl, CURLOPT_URL, "http://bcr-e.com/loanbook/newloan/");
    
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.c_str());
 
		res = curl_easy_perform(curl);
 
		if(res != CURLE_OK){
			//fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			return false;
		}
		curl_easy_cleanup(curl);
  }
	return true;
}

bool CLoanManager::registeraddress(string address, string bitcointx,string tx){
	
#ifdef WIN32
    curl_global_init(CURL_GLOBAL_ALL);
#endif  

	std::stringstream raw;

	raw<<"address="<<address<<'&'<<"bitcointx="<<bitcointx<<'&'<<"tx="<<tx<<' ';
	
	string request = raw.str();

	curl = curl_easy_init();
	if(curl) {
			 
		curl_easy_setopt(curl, CURLOPT_URL, "http://bcr-e.com/loanbook/register/");
    
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.c_str());
 
		res = curl_easy_perform(curl);
 
		if(res != CURLE_OK){
			//fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			return false;
		}
		curl_easy_cleanup(curl);
  }
	return true;	
	
}

bool CLoanManager::reportloandefault(string address, string defaulter, string reqtx, string loantx, int64_t amount, string requestid,string tx){

#ifdef WIN32
    curl_global_init(CURL_GLOBAL_ALL);
#endif  

	std::stringstream raw;

	raw<<"address="<<address<<'&'<<"defaulter="<<defaulter<<'&'<<"reqtx="<<reqtx<<'&'<<"loantx="<<loantx<<'&'<<"amount="<<amount<<'&'<<"requestid="<<requestid<<'&'<<"tx="<<tx<<' ';
	
	string request = raw.str();

	curl = curl_easy_init();
	if(curl) {
			 
		curl_easy_setopt(curl, CURLOPT_URL, "http://bcr-e.com/loanbook/newloan/");
    
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.c_str());
 
		res = curl_easy_perform(curl);
 
		if(res != CURLE_OK){
			//fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			return false;
		}
		curl_easy_cleanup(curl);
  }
	return true;	
	
}
