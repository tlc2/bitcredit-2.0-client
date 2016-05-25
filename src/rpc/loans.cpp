// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcredit Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "clientversion.h"
#include "init.h"
#include "loanmanager.h"
#include "main.h"
#include "net.h"
#include "netbase.h"
#include "rpc/server.h"
#include "timedata.h"
#include "util.h"
#include "utilstrencodings.h"
#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#include "wallet/walletdb.h"
#include "wallet/rpcwallet.h"
#endif

#define SERVER "B5C6Gci8ha6DcNizQvRsWj8oFMdgNKQVQx"

#include <stdint.h>

#include <boost/assign/list_of.hpp>

#include <univalue.h>

UniValue createloanrequest(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 6)
        throw runtime_error(
            "createloanrequest \"bitcreditaddress\" \"amount\" \"premium\" \"expiry\" \"period\" \n"
            "\nRequest a loan\n"
            "\nArguments:\n"
            "1. \"bitcreditaddress\"  (string, required) The ChainID to use for the request.\n"
            "2. \"amount\"       (integer, required) Amount you wish to request.\n"
            "3. \"premium\"         (integer, required) Percentage rate you will pay.\n"
            "4. \"expiry\"         (integer, required) Number of days before request expires (Max 30).\n"
            "5. \"period\"         (int, required) Number of days till repayment will be due (Max 90 ).\n"
            "6. \"message\"         (string, optional) Message you wish attachedto the request.\n"
            "\nResult:\n"
            "Done|Error   (boolean) If the request is valid or not.\n"
            "\nExamples:\n"
            "\nUnlock the wallet for 1 minute\n"
            + HelpExampleCli("walletpassphrase", "\"mypassphrase\" 30") +
            "\nCreate the request\n"
            + HelpExampleCli("createloanrequest", "\"1D1ZrZNe3JUo7ZycKEYQQiQAWd9y54F4XZ\" 10000 5 30 65 \"my message\"")
        );
	CLoanManager loanmgr;
	CWalletTx wtx;
    LOCK2(cs_main, pwalletMain->cs_wallet);

	string strAddress  = params[0].get_str();
    int64_t amount     = params[1].get_int64();
    int premium  = params[2].get_int();
    int expiry = params[3].get_int();
    int period  = params[4].get_int();
    string message  = params[0].get_str();

    CBitcreditAddress address(SERVER);

    // Fee Amount
    CAmount nAmount = AmountFromValue(1000);

    EnsureWalletIsUnlocked();

    SendMoney(address.Get(), nAmount, true, wtx);
    string tx= wtx.GetHash().GetHex();

    std::stringstream raw;

	raw<<"type="<<"loanrequest"<<'&'<<"address="<<strAddress<<'&'<<"amount="<<amount<<'&'<<"premium="<<premium<<'&'<<"expiry="<<expiry<<'&'<<"period="<<period<<'&'<<"message="<<message<<'&'<<"tx="<<tx<<' ';

	string request = raw.str();

    return loanmgr.senddata(request);

}

UniValue loanfunds(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() !=6)
        throw runtime_error(
            "loanfunds \"bitcreditaddress\" \"request-address\" \"request-tx\" \"amount\" \"requestID\" \"message\" \n"
            "\nLoan out funds\n"
            "\nArguments:\n"
            "1. \"bitcreditaddress\"  (string, required) The ChainID you wish to identify yourself with.\n"
            "2. \"request-address\"  (string, required) The ChainID the requster used to request a loan.\n"
            "3. \"request-tx\"  (string, required) The txID of the requster used identify and authenticate the request on the blockchain.\n"
            "4. \"amount\"       (integer, required) Amount you wish to request.\n"
            "5. \"requestID\"         (string, required) RequestID used for web based identification of the loan.\n"
            "6. \"message\"         (string, optional) Message you wish attached to the loan response.\n"
            "\nResult:\n"
            "Done|Error   (boolean) If the request is valid or not.\n"
            "\nExamples:\n"
            "\nUnlock the wallet for 1 minute\n"
            + HelpExampleCli("walletpassphrase", "\"mypassphrase\" 30") +
            "\nCreate the request\n"
            + HelpExampleCli("loanfunds", "\"1D1ZrZNe3JUo7ZycKEYQQiQAWd9y54F4XZ\"  \"5qoFUCqPUE4pyjus6U6jD6ba4oHR6NZ7c7\" \"3ab5a56201feb67bff71bbd1524bde874f92b94e64a85d217e66e47c7bab3b48\" 10000 \"14624162305qoFUC\" \"my message\"")
        );
	CLoanManager loanmgr;
	CWalletTx wtx;
    LOCK2(cs_main, pwalletMain->cs_wallet);

    CBitcreditAddress address(params[1].get_str());
    CAmount nAmount = AmountFromValue(params[2]);

    wtx.mapValue["comment"] = params[6].get_str();
    wtx.mapValue["to"]      = params[1].get_str();

    EnsureWalletIsUnlocked();

    SendMoney(address.Get(), nAmount, false, wtx);

    string tx= wtx.GetHash().GetHex();
	string strAddress  = params[0].get_str();
	string receiver  = params[1].get_str();
	string reqtx  = params[2].get_str();
    int64_t amount     = params[3].get_int64();
    string requestid  = params[4].get_str();
    string message  = params[5].get_str();

	std::stringstream raw;

	raw<<"type="<<"issueloan"<<'&'<<"address="<<strAddress<<'&'<<"receiver="<<receiver<<'&'<<"reqtx="<<reqtx<<'&'<<"amount="<<amount<<'&'<<"requestid="<<requestid<<'&'<<"message="<<message<<'&'<<"tx="<<tx<<' ';

	string request = raw.str();

    return loanmgr.senddata(request);

}

UniValue reportloandefault(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 6)
        throw runtime_error(
            "reportloandefault \"bitcreditaddress\" \"request-address\" \"request-tx\" \"loan-tx\" \"amount\" \"requestID\" \n"
            "\nReport loan default\n"
            "\nArguments:\n"
            "1. \"bitcreditaddress\"  (string, required) The ChainID to use for the request.\n"
            "2. \"request-address\"  (string, required) The ChainID that is in default.\n"
            "3. \"request-tx\"  (string, required) The txid of the loan request.\n"
            "4. \"loan-tx\"  (string, required) The txid of the actual loan.\n"
            "5. \"amount\"       (integer, required) Amount in deafault.\n"
            "6. \"requestID\"         (string, required) Request ID of the loan (web view useage).\n"
            "\nResult:\n"
            "Done|Error   (boolean) If the request is valid or not.\n"
            "\nExamples:\n"
            "\nUnlock the wallet for 1 minute\n"
            + HelpExampleCli("walletpassphrase", "\"mypassphrase\" 30") +
            "\nCreate the request\n"
            + HelpExampleCli("reportloandefault", "\"1D1ZrZNe3JUo7ZycKEYQQiQAWd9y54F4XZ\" \"1D1ZrZNe3JUo7ZycKEYQQiQAWd9y54F4XZ\" \"3ab5a56201feb67bff71bbd1524bde874f92b94e64a85d217e66e47c7bab3b48\" \"3ab5a56201feb67bff71bbd1524bde874f92b94e64a85d217e66e47c7bab3b48\" 10000 \"14624162305qoFUC\"")
        );
	CLoanManager loanmgr;
	CWalletTx wtx;
    LOCK2(cs_main, pwalletMain->cs_wallet);

    CBitcreditAddress address(SERVER);

    // Fee Amount
    CAmount nAmount = AmountFromValue(1000);

    EnsureWalletIsUnlocked();

    SendMoney(address.Get(), nAmount, true, wtx);

    string tx= wtx.GetHash().GetHex();

	string strAddress  = params[0].get_str();
	string defaulter  = params[1].get_str();
	string reqtx  = params[2].get_str();
	string loantx  = params[3].get_str();
    int64_t amount     = params[4].get_int64();
    string requestid  = params[5].get_str();

	std::stringstream raw;

	raw<<"type="<<"reportdefault"<<'&'<<"address="<<strAddress<<'&'<<"defaulter="<<defaulter<<'&'<<"reqtx="<<reqtx<<'&'<<"loantx="<<loantx<<'&'<<"amount="<<amount<<'&'<<"requestid="<<requestid<<'&'<<"tx="<<tx<<' ';

	string request = raw.str();
    return loanmgr.senddata(request);

}

UniValue registeraddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "registeraddress \"bitcreditaddress\" \"bitcoin-txid\" \n"
            "\nRequest a loan\n"
            "\nArguments:\n"
            "1. \"bitcreditaddress\"  (string, required) The ChainID to use for the request.\n"
            "2. \"bitcoin-txid\"  (string, required) The bitcoin transaction-id for registration of an address (wait for at least two confirms).\n"
            "\nResult:\n"
            "Done|Error   (boolean) If the request is valid or not.\n"
            "\nExamples:\n"
            "\nUnlock the wallet for 1 minute\n"
            + HelpExampleCli("walletpassphrase", "\"mypassphrase\" 30") +
            "\nCreate the request\n"
            + HelpExampleCli("registeraddress", "\"1D1ZrZNe3JUo7ZycKEYQQiQAWd9y54F4XZ\" \"3ab5a56201feb67bff71bbd1524bde874f92b94e64a85d217e66e47c7bab3b48\"")
        );
	CLoanManager loanmgr;
	CWalletTx wtx;
    LOCK2(cs_main, pwalletMain->cs_wallet);

    CBitcreditAddress address(SERVER);

    CAmount nAmount = AmountFromValue(5000);
    EnsureWalletIsUnlocked();
    SendMoney(address.Get(), nAmount, true, wtx);

    string tx= wtx.GetHash().GetHex();
	string strAddress  = params[0].get_str();
	string bitcointx  = params[1].get_str();
	std::stringstream raw;

	raw<<"type="<<"registeraddress"<<'&'<<"address="<<strAddress<<'&'<<"bitcointx="<<bitcointx<<'&'<<"tx="<<tx<<' ';

	string request = raw.str();
    return loanmgr.senddata(request);

}

UniValue createnewvote(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 5)
        throw runtime_error(
            "createnewvote \"bitcreditaddress\" \"topic-starter\" \"topic\" \"option 1\" \"option 2\" \n"
            "\nRequest a loan\n"
            "\nArguments:\n"
            "1. \"bitcreditaddress\"  (string, required) The ChainID to use for the request.\n"
            "2. \"topic-starter\"  (string, required) Name of the topic starter of the vote (ChainID can be used as well).\n"
            "3. \"topic\"  (string, required) The Topic of the vote.\n"
            "4. \"option 1\"  (string, required) First Option.\n"
            "5. \"option 2\"  (string, required) Second Option (in future releases we will enable more than two choices).\n"
            "\nResult:\n"
            "Done|Error   (boolean) If the request is valid or not.\n"
            "\nExamples:\n"
            "\nUnlock the wallet for 1 minute\n"
            + HelpExampleCli("walletpassphrase", "\"mypassphrase\" 30") +
            "\nCreate the request\n"
            + HelpExampleCli("createnewvote", "\"1D1ZrZNe3JUo7ZycKEYQQiQAWd9y54F4XZ\" \"topic-starter\" \"topic\" \"option 1\" \"option 2\"")
        );
	CLoanManager loanmgr;
	CWalletTx wtx;
    LOCK2(cs_main, pwalletMain->cs_wallet);

    CBitcreditAddress address(SERVER);

    CAmount nAmount = AmountFromValue(1000);
    EnsureWalletIsUnlocked();
    SendMoney(address.Get(), nAmount, true, wtx);

    string tx= wtx.GetHash().GetHex();
	string strAddress  = params[0].get_str();
	string topicstarter  = params[1].get_str();
	string topic  = params[2].get_str();
	string option1  = params[3].get_str();
	string option2  = params[4].get_str();
	std::stringstream raw;

	raw<<"type="<<"createnewvote"<<'&'<<"address="<<strAddress<<'&'<<"topicstarter="<<topicstarter<<'&'<<"topic="<<topic<<'&'<<"option1="<<option1<<'&'<<"option2="<<option2<<'&'<<"tx="<<tx<<' ';

	string request = raw.str();
    return loanmgr.senddata(request);

}

static const CRPCCommand commands[] =
{ //  category              name                      actor (function)         okSafeMode
  //  --------------------- ------------------------  -----------------------  ----------
    { "loan",               "createloanrequest",      &createloanrequest,      true  },
    { "loan",               "loanfunds",              &loanfunds,              true  },
    { "loan",               "registeraddress",        &registeraddress,        true  },
    { "loan",               "reportloandefault",      &reportloandefault,      true  },
    
    //{ "vote",               "createnewvote",          &createnewvote,          true  },
    //{ "vote",               "vote",                   &vote,                   true  },

};

void RegisterLoanRPCCommands(CRPCTable &tableRPC)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        tableRPC.appendCommand(commands[vcidx].name, &commands[vcidx]);
}
