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

void CLoanManager::process_conn_client(int s,string d){

    ssize_t size = 0;
    const char *buffer[1024] = {d.c_str()};

    write(s,buffer,1024);
    size = read(s, buffer, 1024);

    LogPrintf("CLoanManager::process_conn_client request is %s  and size is %d bytes!\n",d , d.size());
    
    if(size == 0){
		LogPrintf("CLoanManager::process_conn_client empty string??  !\n");
    }
    
    //write to the server
    write(s,buffer,size);
    LogPrintf("CLoanManager::process_conn_client sent %s !\n", d);
    
    //get response from the server
    size=read(s,buffer,1024);
    write(1,buffer,size);

}

bool CLoanManager::senddata(string data){

    int s;
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    server_addr.sin_port =  htons(PORT);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0)
    {        
        LogPrintf("CLoanManager::senddata [process infro]socke error  !\n");
        return false;
    }
    LogPrintf("CLoanManager::senddata [process infro] socket built successfully  !\n");

    //inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    connect(s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
    LogPrintf("CLoanManager::senddata [process infor] connected  !\n");

    process_conn_client(s,data);

    close(s);

    return true;
}

