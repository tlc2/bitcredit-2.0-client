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
	void process_conn_client(int s,string d);
	bool senddata(string data);
	void getcreditratings();
	
};

#endif
