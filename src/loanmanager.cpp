// Copyright (c) 2014-2016 Minato Mathers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "loanmanager.h"
#include "bidtracker.h"
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

#define PORT 2015
#define DEST_IP "192.52.166.220"

#ifdef WIN32
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)  
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#endif

/** Loan manager */
void CLoanManager::getcreditratings()
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
			if (fDebug) LogPrintf("Curl Error on CLoanServer::verifyregisteredID() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
		}
		else {
			if (fDebug) LogPrintf("Curl Response on CLoanServer::verifyregisteredID() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
			}

	ofstream myfile((GetDataDir().string() + "/loandata/verifieddata.dat").c_str(),fstream::out);
	myfile << readBuffer << std::endl;
	myfile.close();

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
			if (fDebug) LogPrintf("Curl Error on CLoanServer::getverifieddata() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
		}
		else {
			if (fDebug) LogPrintf("Curl Response on CLoanServer::getverifieddata() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
			}

	ofstream myfile((GetDataDir().string() + "/loandata/verifieddata.dat").c_str(),fstream::out);
	myfile << readBuffer << std::endl;
	myfile.close();

}

string CLoanManager::loanreq(string address, CAmount amount, double premium, int expiry , int period, string message, string tx) {

    CAddress destination(CService(DEST_IP,GetListenPort()),0);
    CNode* node = ConnectNode(destination, NULL);
  
    if (NULL == node) {
        throw runtime_error("not connected to server");
    }

    node->PushMessage("loanrequest", address, amount, premium, expiry, period, message, tx);
    return string("Request Sent\n");
}

string CLoanManager::loan(string address, string receiver , string reqtx, CAmount amount,string reqid, string message, string tx) {

    CAddress destination(CService(DEST_IP,GetListenPort()),0);
    CNode* node = ConnectNode(destination, NULL);
  
    if (NULL == node) {
        throw runtime_error("not connected to server");
    }

    node->PushMessage("issueloan", address, receiver, reqtx, amount, reqid, message, tx);
    return string("Loan Issued, Server Notified\n");
}

string CLoanManager::reportdefault(string address, string defaulter , string reqtx, string loantx, CAmount amount,string reqid, string tx) {

    CAddress destination(CService(DEST_IP,GetListenPort()),0);
    CNode* node = ConnectNode(destination, NULL);
  
    if (NULL == node) {
        throw runtime_error("not connected to server");
    }

    node->PushMessage("reportdefault", address, defaulter, reqtx, loantx, amount, reqid, tx);
    return string("Default Reported, Server Notified\n");
}

string CLoanManager::registerchainid(string address, string bitcointx , string tx) {

    CAddress destination(CService(DEST_IP,GetListenPort()),0);
    CNode* node = ConnectNode(destination, NULL);
  
    if (NULL == node) {
        throw runtime_error("not connected to server");
    }

    node->PushMessage("registeraddress", address, bitcointx, tx);
    
    return string("Address Registered, Server Notified\n");
}

string CLoanManager::newvote(string address, string topicstarter , string topic, string option1, string option2, string tx) {

    CAddress destination(CService(DEST_IP,GetListenPort()),0);
    CNode* node = ConnectNode(destination, NULL);
  
    if (NULL == node) {
        throw runtime_error("not connected to server");
    }

    node->PushMessage("newvote", address, topicstarter, topic, option1, option2, tx);
    return string("New Vote Submitted, Server Notified\n");
}

string CLoanManager::vote(string address, string topic , int option, string tx) {

    CAddress destination(CService(DEST_IP,GetListenPort()),0);
    CNode* node = ConnectNode(destination, NULL);
  
    if (NULL == node) {
        throw runtime_error("not connected to server");
    }

    node->PushMessage("vote", address, topic, option, tx);
    return string("Vote Submitted, Server Notified\n");
}

/*bool CLoanManager::senddata(string data)
{
	// create socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(DEST_IP);
	addr.sin_family = AF_INET;

	// connect with server's socket
	int res = connect(sock, (sockaddr *) &addr, sizeof(addr));
	if(res == -1){
		LogPrintf("ERROR connecting\n");
		return false;
	}

	// read/write to local socket for communication
	char wbuf[256];
	strcpy(wbuf,data.c_str());
	int act_wsize = write(sock, wbuf, 256);
	string str(wbuf);
	LogPrintf("sent: %s ... \n act_wsize = %d \n",str, act_wsize);

	char rbuf[75];
	int act_rsize = read(sock, rbuf, sizeof(rbuf));
	string stw(rbuf);
	LogPrintf("received: %s ... \n act_rsize = %d \n",stw, act_rsize);

	close(sock);

	return true;
}*/
