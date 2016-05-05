#include "bidtracker.h"
#include "wallet/wallet.h"
#include "base58.h"

#include "util.h"
#include <iostream>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <sstream>

using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
//for (int i = 0; std::getline(f, line); ++i)
string remove(std::string input, char m)
{
  input.erase(std::remove(input.begin(),input.end(), m),input.end());

  return input;
}

std::string replacestring(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

double Bidtracker::getbalance(std::string url)
{
    const char * c = url.c_str();

      std::string readBuffer;
      CAmount balance;
      curl = curl_easy_init();
      if(curl) {
		curl_global_init(CURL_GLOBAL_ALL);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_URL, c);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bitcredit/0.30");
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
		}
		if(res != CURLE_OK) {
			if (fDebug) LogPrintf("Curl Error on Bidtracker::getbalance() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
		}
		else {
			if (fDebug) LogPrintf("Curl Response on Bidtracker::getbalance() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
		}
		std::string response = readBuffer;
		if ( ! (std::istringstream(response) >> balance) ) balance = 0;
    return balance;
}

void Bidtracker::btcsortunspent(){

	std::ifstream myfile ((GetDataDir()/ "bidtracker/btcunspentraw.dat").string().c_str());
	std::ofstream myfile2;
	myfile2.open((GetDataDir()/ "bidtracker/btcbids.dat").string().c_str(),std::fstream::out);

	std::string line, txid, url;
    try
    {

	if (myfile.is_open()){
		while ( myfile.good() ){
			getline (myfile,line);
			std::string temp = line;
			std::string search,search2;
			size_t pos;
			size_t f = line.find("tx_hash_big_endian:");
			size_t g = line.find("value:");

			search = "tx_hash_big_endian";
			pos = temp.find(search);
			if (pos != std::string::npos){
				std::string semp =line;
				semp = semp.replace(f, std::string("tx_hash_big_endian:").length(), "");
				semp = remove(semp, ',');
				txid = semp;
				url = "https://blockchain.info/rawtx/"+ txid ;
				const char * d = url.c_str();
				string readBuffer;
				curl = curl_easy_init();
				if(curl) {
					curl_global_init(CURL_GLOBAL_ALL);
					curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
					curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
					curl_easy_setopt(curl, CURLOPT_URL, d);
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
					curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bitcredit/0.30");
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
				}
				if(res != CURLE_OK) {
					if (fDebug) LogPrintf("Curl Error on Bidtracker::btcsortunspent() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
				}
				else {
					if (fDebug) LogPrintf("Curl Response on Bidtracker::btcsortunspent() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
					std::size_t pos1 = readBuffer.find("value");
					readBuffer = readBuffer.substr(0,pos1);
					readBuffer = remove(readBuffer, '"');
					readBuffer = remove(readBuffer, '{');
					readBuffer = remove(readBuffer,'}');
					readBuffer = remove(readBuffer, '[');
					readBuffer = remove(readBuffer, '\n');
					std::string uemp =readBuffer;
					std::size_t pos2 = uemp.find("addr:");
					uemp = uemp.substr(pos2);
					uemp = replacestring(uemp, "addr:", "");
					boost::algorithm::erase_all(uemp, " ");
					myfile2 << uemp;
				}
			}

			search2 = "value:";
			pos = temp.find(search2);
			if (pos != std::string::npos){
				std::string semp =line;
				semp = semp.replace(g, std::string("value:").length(), "");
				string value = semp;
				value = remove(value, ',');
				long double amount = atof(value.c_str());
				myfile2 << std::fixed <<  amount << std::endl;
			}
		}
		myfile.close();
		myfile2.close();
	}
	}

    catch (std::exception const &exc)
    {
		if (fDebug) LogPrintf("Bidtracker Error on Bidtracker::btcsortunspent() - %s .\n", exc.what());
    }
    catch (...)
    {
		if (fDebug) LogPrintf("Bidtracker Unknown Error on Bidtracker::btcsortunspent().\n");
    }
}

void Bidtracker::btcsortunspentbackup(){

	std::ifstream myfile ((GetDataDir()/ "bidtracker/btcunspentrawbackup.dat").string().c_str());
	std::ofstream myfile2;
	myfile2.open((GetDataDir()/ "bidtracker/btcbidsbackup.dat").string().c_str(),fstream::out);
    try
    {
	std::string line, txid, url;
    char * pEnd;
	if (myfile.is_open()){
		while (myfile.good()){
			getline(myfile,line);
			line = line.erase(line.find("txid:"), 5);
			line = line.erase(line.find("amount:"), 7);
			std::vector<std::string> strs;
			boost::split(strs, line, boost::is_any_of(","));
			long double amount = strtoll(strs[5].c_str(),&pEnd,10) *COIN;
			txid = strs[1].c_str();
			url = "https://blockchain.info/rawtx/"+ txid ;
			const char * d = url.c_str();
			string readBuffer;
			curl = curl_easy_init();
			if(curl) {
				curl_global_init(CURL_GLOBAL_ALL);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(curl, CURLOPT_URL, d);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
				curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bitcredit/0.30");
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
			}
			if(res != CURLE_OK) {
				if (fDebug) LogPrintf("Curl Error on Bidtracker::btcsortunspentbackup() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
			}
			else {
				if (fDebug) LogPrintf("Curl Response on Bidtracker::btcsortunspentbackup() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
				std::size_t pos1 = readBuffer.find("value");
				readBuffer = readBuffer.substr(0,pos1);
				readBuffer = remove(readBuffer, '"');
				readBuffer = remove(readBuffer, '{');
				readBuffer = remove(readBuffer,'}');
				readBuffer = remove(readBuffer, '[');
				readBuffer = remove(readBuffer, '\n');
				std::string uemp =readBuffer;
				std::size_t pos2 = uemp.find("addr:");
				uemp = uemp.substr(pos2);
				uemp = replacestring(uemp, "addr:", "");
				boost::algorithm::erase_all(uemp, " ");
				myfile2 << uemp <<amount<< endl;
			}
		}
	//myfile2 << strs[1].c_str() << "," << std::fixed << amount << std::endl;
	myfile.close();
	myfile2.close();
	}

	}
    catch (std::exception const &exc)
    {
		if (fDebug) LogPrintf("Bidtracker Error on Bidtracker::btcsortunspentbackup() - %s .\n", exc.what());
    }
    catch (...)
    {
		if (fDebug) LogPrintf("Bidtracker Unknown Error on Bidtracker::btcsortunspentbackup().\n");
    }
}

void Bidtracker::btcgetunspentbackup()
{
    std::string address = "1BCRbid2i3wbgqrKtgLGem6ZchcfYbnhNu";

    std::string url = "https://blockexplorer.com/api/addr/"+ address + "/utxo";
    try
    {
    const char * c = url.c_str() ;

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
				if (fDebug) LogPrintf("Curl Error on Bidtracker::btcgetunspentbackup() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
		}
		else {
			if (fDebug) LogPrintf("Curl Response on Bidtracker::btcgetunspentbackup() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
			readBuffer = remove(readBuffer, '[');
			readBuffer = remove(readBuffer, ']');
			readBuffer = replacestring(readBuffer, "},", "}\n");
			readBuffer = remove(readBuffer, '{');
			readBuffer = remove(readBuffer, '}');
			readBuffer = remove(readBuffer, '"');
			ofstream myfile((GetDataDir().string() + "/bidtracker/btcunspentrawbackup.dat").c_str(),fstream::out);
			myfile << readBuffer<< std::endl;
			myfile.close();
		}
	}

    catch (std::exception const &exc)
    {
		if (fDebug) LogPrintf("Bidtracker Error on Bidtracker::btcgetunspentbackup() - %s .\n", exc.what());
    }
    catch (...)
    {
		if (fDebug) LogPrintf("Bidtracker Unknown Error on Bidtracker::btcgetunspentbackup().\n");
    }
}

void Bidtracker::btcgetunspent()
{
    std::string address = "1BCRbid2i3wbgqrKtgLGem6ZchcfYbnhNu";

    std::string url;
    url = "https://blockchain.info/unspent?active=" + address;
    try
    {
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

    boost::filesystem::path biddir = GetDataDir() / "bidtracker";

    if(!(boost::filesystem::exists(biddir))){
        if(fDebug)LogPrintf("Biddir Doesn't Exists\n");

        if (boost::filesystem::create_directory(biddir))
            if(fDebug)LogPrintf("Biddir....Successfully Created !\n");
    }
	if(res != CURLE_OK) {
		if (fDebug) LogPrintf("Curl Error on Bidtracker::btcgetunspent() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
	}
	else {
			if (fDebug) LogPrintf("Curl Response on Bidtracker::btcgetunspent() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
			std::ofstream myfile((GetDataDir().string() + "/bidtracker/btcunspentraw.dat").c_str(),fstream::out);
			readBuffer = remove(readBuffer, ' ');
			readBuffer = remove(readBuffer, '"');
			myfile << readBuffer << std::endl;
			myfile.close();
	}
	}

    catch (std::exception const &exc)
    {
		if (fDebug) LogPrintf("Bidtracker Error on Bidtracker::btcgetunspent() - %s .\n", exc.what());
    }
    catch (...)
    {
		if (fDebug) LogPrintf("Bidtracker Unknown Error on Bidtracker::btcgetunspent().\n");
    }
}

double Bidtracker::btcgetprice()
{
	CAmount price = 0;
    std::string url;
    url = "https://blockchain.info/q/24hrprice";

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
			if (fDebug) LogPrintf("Curl Error on Bidtracker::btcgetprice() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
		}
		else {
			if (fDebug) LogPrintf("Curl Response on Bidtracker::btcgetprice() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
			price = atof(readBuffer.c_str());
		}

      return price;
}

double Bidtracker::bcrgetprice()
{
	double price;
    std::string url;
    url = "https://bittrex.com/api/v1.1/public/getticker?market=BTC-BCR";

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
			if (fDebug) LogPrintf("Curl Error on Bidtracker::bcrgetprice() - %s - on URL:%s.\n", curl_easy_strerror(res), url);
		}
		else {
			if (fDebug) LogPrintf("Curl Response on Bidtracker::bcrgetprice() - Lenght %lu - Buffer - %s .\n", (long)readBuffer.size(), readBuffer);
			std::size_t pos = readBuffer.find(",\"A");
			readBuffer = readBuffer.substr(0,pos);
			readBuffer = replacestring(readBuffer, ",", ",\n");
			readBuffer = remove(readBuffer, ',');
			readBuffer = remove(readBuffer, '"');
			readBuffer = remove(readBuffer, ':');
			readBuffer = remove(readBuffer, '{');
			readBuffer = replacestring(readBuffer, "successtrue", "");
			readBuffer = replacestring(readBuffer, "message", "");
			readBuffer = replacestring(readBuffer, "resultBid", "");
			readBuffer = remove(readBuffer, '\n');
			}
		
		if ( ! (istringstream(readBuffer) >> price) ) price = 0;

      return price;
}

double Bidtracker::usdbtc(){

return btcgetprice();

}

long double Bidtracker::bcrbtc(){

return bcrgetprice();

}

void Bidtracker::combine()
{
	std::ofstream myfile;
	myfile.open((GetDataDir() /"bidtracker/prefinal.dat").string().c_str(),fstream::out);
	std::ifstream myfile2((GetDataDir() /"bidtracker/btcbids.dat").string().c_str());
	std::ifstream myfile3((GetDataDir() /"bidtracker/btcbidsbackup.dat").string().c_str());


	if (myfile2.is_open()){
		std::string line;
		while ( myfile2.good() ){
			getline (myfile2,line);
	myfile<<line<<std::endl;
	}	}
	if (myfile3.is_open()){
		std::string line;
		while ( myfile3.good() ){
			getline (myfile3,line);
	myfile<<line<<std::endl;
	}	}

	myfile.close();
	myfile2.close();
	myfile3.close();
	remove((GetDataDir() /"bidtracker/btcbids.dat").string().c_str());
	remove((GetDataDir() /"bidtracker/btcbidsbackup.dat").string().c_str());
	remove((GetDataDir() /"bidtracker/btcunspentraw.dat").string().c_str());
	remove((GetDataDir() /"bidtracker/btcunspentrawbackup.dat").string().c_str());
}

int totalbid;
std::map<std::string,double>::iterator brit;
void sortbidtracker(){
	std::map<std::string,double> finalbids;
	std::fstream myfile2((GetDataDir() /"bidtracker/prefinal.dat").string().c_str());
	totalbid=0;
	char * pEnd;
	std::string line;
	while (getline(myfile2, line)){
		if (!line.empty()) {
			std::vector<std::string> strs;
			boost::split(strs, line, boost::is_any_of(","));
			totalbid+=strtoll(strs[1].c_str(),&pEnd,10);
			finalbids[strs[0]]+=strtoll(strs[1].c_str(),&pEnd,10);
		}
	}

	std::ofstream myfile;
	myfile.open((GetDataDir() /"bidtracker/final.dat").string().c_str(), std::ofstream::trunc);
	//myfile << std::fixed << setprecision(8);
	for(brit = finalbids.begin();brit != finalbids.end(); ++brit){
		if (!(brit->second ==0 || totalbid == 0) )
			myfile << brit->first << "," << (brit->second)/totalbid << std::endl;
	}

	myfile2.close();
	myfile.close();
}

std::map<std::string,double> getbidtracker(){

	std::map<std::string,double> finals;
	std::fstream myfile((GetDataDir() /"bidtracker/final.dat").string().c_str());
	char * pEnd;
	std::string line;
	while (getline(myfile, line)){
		if (!line.empty()) {
			std::vector<std::string> strs;
			boost::split(strs, line, boost::is_any_of(","));
			finals[strs[0]]=strtod(strs[1].c_str(),&pEnd);
		}
	}
	return finals;
}

void getbids(){

	int64_t nStart = GetTimeMillis();
	Bidtracker h;
	h.btcgetunspent();
	h.btcgetunspentbackup();
	h.btcsortunspent();
	h.btcsortunspentbackup();
	h.combine();
	sortbidtracker();
	if(fDebug)LogPrintf("Bids dump finished  %dms\n", GetTimeMillis() - nStart);

}
