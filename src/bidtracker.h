#ifndef BIDTRACKER_H
#define BIDTRACKER_H

#include "main.h"

#include <curl/curl.h>
#include <iostream>
#include <string>

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif

void getbids();

extern int totalbid;
extern std::map<std::string,double> getbidtracker();
extern size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
class Bidtracker
{
public:

      CURLcode res;
    CURL *curl;
	void btcgetunspent();
	//void btcgetunspentbackup();
	void btcsortunspent();
	//void btcsortunspentbackup();
    double getbalance(std::string url);
	double usdbtc();
	long double bcrbtc();
	double btcgetprice();
	double bcrgetprice();
	double credit();
	double newcredit;
	double totalcredit;
	void combine();
};


#endif // BIDTRACKER_H
