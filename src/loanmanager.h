#ifndef LOAN_MANAGER_H
#define LOAN_MANAGER_H 

#include <iostream>
#include <string>

#include "util.h"
#include "utilmoneystr.h"

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
	//bool senddata(string data);
	void getcreditratings();
    string loanreq(string address, CAmount amount, double premium, int expiry , int period, string message, string tx);

    string loan(string address, string receiver , string reqtx, CAmount amount,string reqid, string message, string tx);

    string reportdefault(string address, string defaulter , string reqtx, string loantx, CAmount amount,string reqid, string tx);

    string registerchainid(string address, string bitcointx , string tx);

    string newvote(string address, string topicstarter , string topic, string option1, string option2, string description, string tx);

    string vote(string address, string topic , int option, string tx);
	
};

#endif
