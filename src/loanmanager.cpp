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
#define DEST_IP "84.200.32.78"

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

void CLoanManager::process_conn_client(int s,string d){

    ssize_t size = 0;
    char buffer[1024];

    //read from the file to be sent
    fstream outfile("programm.txt",ios::in|ios::out);

    if (outfile.fail()){
        printf("[process infro]cannot open the file to be sent\n");
        return ;
    }
    printf("[process infro]successfully open the file to be sent\n");

    while(!outfile.eof()){

        outfile.getline(buffer,1025,'\n');
        write(s,buffer,1024);
        size = read(s, buffer, 1024);
        if(size = 0)
        {
            return ;
        }
        //write to the server
        write(s,buffer,size);
        //get response from the server
        size=read(s,buffer,1024);
        write(1,buffer,size);

    }
    outfile.close();
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
        cout<<"[process infro]socke error"<<endl;
        return -1;
    }
    cout<<"[process infro] socket built successfully\n";

    //inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    connect(s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
    cout<<"[process infor] connected\n";
    process_conn_client(s,data);

    close(s);

    return true;
}

