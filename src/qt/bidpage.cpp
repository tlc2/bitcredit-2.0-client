#include "bidpage.h"
#include "ui_bidpage.h"
#include "util.h"
#include "guiutil.h"
#include "clientmodel.h"
#include "chainparams.h"
#include "main.h"
#include "net.h"

#include "bidtracker.h"

#include <cstdlib>

#include <fstream>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QTimer>

BidPage::BidPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::BidPage)
{
    ui->setupUi(this);

    ui->lineEditBid->setEnabled(false);  //  cannot calc until update clicked and data fetched
    
    ui->electrumpwd->setEnabled(false);   //  disable scheduled bid elements until getbids method executed, we need some stuff it calcs
    ui->bElectrum_2->setEnabled(false);

    connect(ui->pushButtonBTCExplorer, SIGNAL(clicked()), this, SLOT(SummonBTCExplorer()));
    connect(ui->bElectrum, SIGNAL(clicked()), this, SLOT(SummonElectrum()));
    connect(ui->pushButtonRefresh, SIGNAL(clicked()), this, SLOT(GetBids()));
    connect(ui->lineEditBid, SIGNAL(returnPressed()), this, SLOT(Estimate()));
    connect(ui->bImport, SIGNAL(clicked()), this, SLOT(RPC()));
    connect(ui->bElectrum_2, SIGNAL(clicked()), this, SLOT(scheduleBid()));
}

void BidPage::setClientModel(ClientModel *model)
{
    clientModel = model;
    if(model)
    {
        setNumBlocks(model->getNumBlocks());
        //connect(model, SIGNAL(numBlocksChanged(int)), this, SLOT(setNumBlocks(int)));
    }
}

int BidPage::getNumBlocks()
{
    LOCK(cs_main);
    return chainActive.Height();
}

void BidPage::setNumBlocks(int count)
{
    ui->labelNumber->setText(QString::number(count));
}

void BidPage::Estimate()
{
    QString bidtotal = ui->labelBids->text();
    float bidz = bidtotal.toFloat();
    float mybid = ui->lineEditBid->text().toFloat();
    float newtotal = bidz + mybid;
    float mybcr = (mybid / newtotal) * 18000 * podl; 
    QString mybcrz = QString::number(mybcr);
    float cost = mybid / mybcr;
    QString coststr = QString::number(cost, 'f', 8);
    ui->labelBCR->setText("<b>" + mybcrz + "</b> BCR @ " + "<b>" + coststr + "</b>");
}

void BidPage::GetBids()
{
    Bidtracker r;
    
    // get current BTC resrve assets
    double btcassets = r.getbalance("https://blockchain.info/q/addressbalance/16bi8R4FoDHfjNJ1RhpvcAEn4Cz78FbtZB");
    QString reserves = QString::number(btcassets/COIN, 'f', 8);
    ui->labelReserves->setText(reserves);
    
    // get current bids
    double bids = r.getbalance("https://blockchain.info/q/addressbalance/1BCRbid2i3wbgqrKtgLGem6ZchcfYbnhNu");
    QString bidz = QString::number(bids/COIN, 'f', 8);
    ui->labelBids->setText(bidz);    
    
    // calc time until next 00:00 GMT
    long int startdate = 1450396800; // 18 December 2015 00:00
    long int current = GetTime();
    long int diff = current - startdate;

    int until = 86400 - (diff % 86400);
    ui->labelNumber->setText(GUIUtil::formatDurationStr(until));

    // percentage of day left for use in calculator estimate
    podl = (float)until/86400;

/*  //  no files being geberated yet in bidtracker dir so we'll just calc based on what the bids are we just pulled from blockchain.info
    // get default datadir, tack on bidtracker
    QString dataDir = getDataDirectory();
    QString bidDir = "bidtracker";
    QString datPath = pathAppend(dataDir, bidDir);

    // get bids from /bidtracker/final.dat
    QString bidspath = QDir(datPath).filePath("prefinal.dat");
    double btctot = 0;
    // for each line in file, get the float after the comma
    QFile bidsFile(bidspath);
    if (bidsFile.open(QIODevice::ReadOnly))
    {
       QTextStream btcin(&bidsFile);
       while (!btcin.atEnd())
       {
           QString line = btcin.readLine();
           if (line.isEmpty()){ continue; }
           else if (line.startsWith("1"))  //  BTC
           {
               QString btcamount = line.remove(0, 35);
               btctot = btctot + btcamount.toDouble();
           }
       else //  we should never get here
           {
               QMessageBox::information(0, QString("Oops!"), QString("There is a problem with the file, please try again later!"), QMessageBox::Ok);
           }
       }
       bidsFile.close();
    }

    // btctot in satoshis, so divide by 10000000 to get right units
    double btctotU = btctot / 100000000;
    QString btctotal = QString::number(btctotU, 'f', 8);
    //ui->labelTotal_2->setText(btctotal);

    // add 'em up and display 'em
    double alltot = btctotU;
    QString alltotal = QString::number(alltot, 'f', 8);
    ui->labelTotal_2->setText(alltotal);
*/

    // calc price per BCR based on total bids and display it
    //double bcrprice = btctotU / 18000;
    double bcrprice = (bids / 18000) / 100000000;
    QString bcrPrice = QString::number(bcrprice, 'f', 8);
    ui->labelEstprice_2->setText(bcrPrice);

    ui->lineEditBid->setEnabled(true);
    ui->electrumpwd->setEnabled(true);
    ui->bElectrum_2->setEnabled(true);
}

QString BidPage::pathAppend(const QString& path1, const QString& path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

QString BidPage::getDataDirectory()
{
    return GUIUtil::boostPathToQString(GetDataDir());
}

void BidPage::SummonBTCExplorer()
{
    QDesktopServices::openUrl(QUrl("https://btc.blockr.io/address/info/1BCRbid2i3wbgqrKtgLGem6ZchcfYbnhNu", QUrl::TolerantMode));
}

void BidPage::SummonElectrum()
{
    proc = new QProcess(this);
    #ifdef __linux
        QString elect = "electrum \"bitcoin:1BCRbid2i3wbgqrKtgLGem6ZchcfYbnhNu?amount=0.005\"";
        proc->startDetached(elect);
    #elif _WIN32
        QString elect = "electrum.exe \"bitcoin:1BCRbid2i3wbgqrKtgLGem6ZchcfYbnhNu?amount=0.005\"";
        proc->startDetached(elect);
    #endif
}

bool BidPage::fileExists(QString path) 
{
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists();
}

void BidPage::RPC()
{
    // check there's something to work with
    if (ui->lineEditPassphrase->text() == "" || ui->lineEditPrivkey->text() == "") 
    {
        QMessageBox::information(0, QString("Attention!"), QString("Please enter your wallet passphrase and the private key of the Bitcoin address you sent your bid from."), QMessageBox::Ok);
        return;
    }
  
    // get working data directory
    QString cwd = GUIUtil::boostPathToQString(GetDataDir());

    // check bitcredit-cli and bitcredit-conf exist in cwd
    QString cli = "bitcredit-cli";
    QString clipath = pathAppend(cwd, cli);
    QString conf = "bitcredit.conf";
    QString confpath = pathAppend(cwd, conf);

   if (!fileExists(clipath) || !fileExists(confpath))
    {
        QMessageBox::information(0, QString("Attention!"), QString("Please make sure that bitcredit-cli(.exe) exists in the same directory as the currently loaded wallet.\n\nYou must also have a bitcredit.conf file present, containing the following:\n\nrpcuser=blah\nrpcpassword=blahblah\nrpcallowip=127.0.0.1\nserver=1\n\nOnce these are in place, please restart bitcredit-qt to proceed."), QMessageBox::Ok); 
    }

    // get password
    QString pwd = ui->lineEditPassphrase->text();

    // build RPC call
    QString callnix = cwd + "/bitcredit-cli --datadir=" + cwd + " walletpassphrase " + pwd + " 60";
    QString callwin = cwd + "/bitcredit-cli.exe --datadir=" + cwd + " walletpassphrase " + pwd + " 60";

    // unlock wallet
    proc2 = new QProcess(this);
    #ifdef __linux
        proc2->start(callnix);
        proc2->waitForFinished();
        //QString output(proc2->readAllStandardOutput()); // check for any output
    #elif _WIN32
        proc2->start(callwin);
        proc2->waitForFinished();
        //QString output(proc2->readAllStandardOutput()); // check for any output       
    #endif

    // reset pwd field
    ui->lineEditPassphrase->setText("");

    // get privkey
    QString privkey = ui->lineEditPrivkey->text();

    // build RPC call
    QString callnix2 = cwd + "/bitcredit-cli --datadir=" + cwd + " importprivkey " + privkey;
    QString callwin2 = cwd + "/bitcredit-cli.exe --datadir=" + cwd + " importprivkey " + privkey;

    // import privkey
    proc3 = new QProcess(this);
    #ifdef __linux
        proc3->start(callnix2);
        proc3->waitForFinished();
        QString output2(proc3->readAllStandardOutput()); // check for any output
    #elif _WIN32
        proc3->start(callwin2);
        proc3->waitForFinished();
        QString output2(proc3->readAllStandardOutput()); // check for any output
    #endif 
    
    // reset privkey field
    ui->lineEditPrivkey->setText("");   
}

void BidPage::scheduleBid()
{
    // check passphrase has been entered and bid amount >= 0.005
    QString electrumpwd = ui->electrumpwd->text();
    double bidamount = ui->lineEditBid_2->text().toDouble();
    if ((electrumpwd == "") || (bidamount < 0.0005))             // ************ change back to 0.005 after testing ************** 
    {
        QMessageBox::information(0, QString("Attention!"), QString("You must enter the passphrase for your Bitcoin Electrum wallet\nand the bid amount must be at least 0.005 BTC."), QMessageBox::Ok);
        return;
    }
    
    // get secs until next bid period starts
    double untilsecs = (podl * 86400) + 120; // percentage of day left calced in getbids() * secs in day, then add 120 secs leeway so bid is made about 2min after midnight GMT
    QString untilsecz = QString::number(untilsecs);
    QMessageBox::information(0, QString("Bid scheduled!"), QString(untilsecz + " seconds until midnight GMT!\nYour bid: " + ui->lineEditBid_2->text() + " BTC"), QMessageBox::Ok);
    // set timer to call electrum daemon
    electrumtimer = new QTimer(this);
    electrumtimer->setSingleShot(true);
    connect(electrumtimer, SIGNAL(timeout()), this, SLOT(callElectrumDaemon()));
    electrumtimer->start(untilsecs * 1000); // millisecs
    //electrumtimer->start(3000);
    
    ui->bElectrum_2->setStyleSheet("color: #ff1a00;");
    ui->bElectrum_2->setText("Bid Scheduled");
}

void BidPage::callElectrumDaemon()
{
    // for now we'll leave it to the user to make sure the electrum daemon is running
       
    QString cmdnix = "electrum payto 1BCRbid2i3wbgqrKtgLGem6ZchcfYbnhNu " + ui->lineEditBid_2->text();
    QString cmdwin = "electrum.exe payto 1BCRbid2i3wbgqrKtgLGem6ZchcfYbnhNu " + ui->lineEditBid_2->text();
        
    procsched = new QProcess(this);
        
    // create tx
    #ifdef __linux
        procsched->start(cmdnix);
        procsched->waitForFinished();
        QString outputsched(procsched->readAllStandardOutput()); // check for any output
        //QMessageBox::information(0, QString("Attention!"), QString(outputsched), QMessageBox::Ok);
    #elif _WIN32
        procsched->start(cmdwin);
        procsched->waitForFinished();
        QString outputsched(procsched->readAllStandardOutput()); // check for any output
    #endif 

    // deal with password request
    

    // reset privkey field
    ui->electrumpwd->setText("");       

    // extract hex
    QStringList chunks = outputsched.split(":");

    QString hexwithquotes = chunks.value(2);
    //QMessageBox::information(0, QString("Attention!"), hexwithquotes, QMessageBox::Ok);
    
    QString hex = hexwithquotes.replace("\"", "");
    hex = hex.replace("\n", "");
    hex = hex.replace("\r", "");
    hex = hex.replace(" ", "");
    hex = hex.replace("}", "");
    //QMessageBox::information(0, QString("Attention!"), hex, QMessageBox::Ok);
   
    // broadcast tx
    QString cmdnix2 = "electrum broadcast " + hex;
    QString cmdwin2 = "electrum.exe broadcast " + hex;
    
    //QMessageBox::information(0, QString("Attention!"), "Running:\n" + cmdnix2, QMessageBox::Ok);

    procsched2 = new QProcess(this);

    #ifdef __linux
        procsched2->start(cmdnix2);
        procsched2->waitForFinished();
        QString outputsched2(procsched2->readAllStandardOutput()); // check for any output
    #elif _WIN32
        procsched2->start(cmdwin2);
        procsched2->waitForFinished();
        QString outputsched2(procsched2->readAllStandardOutput()); // check for any output
    #endif 
        
    QMessageBox::information(0, QString("Bid made!"), "txid: " + outputsched2, QMessageBox::Ok); // txid of bid  if all went well
    ui->bElectrum_2->setStyleSheet("color: white;");
    ui->bElectrum_2->setText("Schedule Bid");
}

BidPage::~BidPage()
{
    delete ui;
}
