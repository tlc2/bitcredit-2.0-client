#ifndef LOAN_MANAGER_H
#define LOAN_MANAGER_H 

#include <iostream>
#include <string>

#include <sqlite3.h>
#include <curl/curl.h>

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
using namespace std;
class CLoanManager
{
public:

    CURLcode res;
    CURL *curl;
	void getverifieddata();
	bool sendloanrequest(string address, int64_t amount, int premium, int expiry, int period, string message, string tx);
	bool sendloan(string address,string receiver,string  reqtx ,int64_t amount,string requestid,string message,string tx);
	bool registeraddress(string address, string bitcointx,string tx);
	bool reportloandefault(string address, string defaulter, string reqtx, string loantx, int64_t amount, string requestid,string tx);

};

#endif
