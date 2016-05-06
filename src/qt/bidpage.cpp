#include "bidpage.h"
#include "ui_bidpage.h"
#include "util.h"
#include "guiutil.h"
#include "clientmodel.h"
#include "chainparams.h"
#include "main.h"
#include "net.h"
#include "basenodeman.h"
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

BidPage::BidPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::BidPage)
{
    ui->setupUi(this);

    ui->lineEditBid->setEnabled(false);  //  cannot calc until update clicked and data fetched

    connect(ui->pushButtonBTCExplorer, SIGNAL(clicked()), this, SLOT(SummonBTCExplorer()));
    connect(ui->pushButtonBTC, SIGNAL(clicked()), this, SLOT(SummonBTCWallet()));
    connect(ui->pushButtonRefresh, SIGNAL(clicked()), this, SLOT(GetBids()));
    connect(ui->lineEditBid, SIGNAL(returnPressed()), this, SLOT(Estimate()));
    connect(ui->bImport, SIGNAL(clicked()), this, SLOT(RPC()));
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
    QString bidtotal = ui->labelTotal_2->text();
    float bidz = bidtotal.toFloat();
    float mybid = ui->lineEditBid->text().toFloat();
    float newtotal = bidz + mybid;
    float mybcr = (mybid / newtotal) * 18000;
    QString mybcrz = QString::number(mybcr);
    float cost = mybid / mybcr;
    QString coststr = QString::number(cost, 'f', 8);
    ui->labelBCR->setText("<b>" + mybcrz + "</b> BCR @ " + "<b>" + coststr + "</b>");
}

void BidPage::GetBids()
{
    // get current BTC assets
    Bidtracker r;
    double btcassets = r.getbalance("https://blockchain.info/q/addressbalance/16bi8R4FoDHfjNJ1RhpvcAEn4Cz78FbtZB");
    QString reserves = QString::number(btcassets/COIN, 'f', 8);
    ui->labelReserves->setText(reserves);

    // calc time until next 00:00 GMT
    long int startdate = 1450396800; // 18 December 2015 00:00
    long int current = GetTime();
    long int diff = current - startdate;

    int until = 86400 - (diff % 86400);
    ui->labelNumber->setText(GUIUtil::formatDurationStr(until));

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

    // btctot, ltctot and dashtot are in satoshis, so divide by 10000000 to get right units
    // to do - add radiobuttons or dropdown to select sats or not?
    double btctotU = btctot / 100000000;
    QString btctotal = QString::number(btctotU, 'f', 8);
    //ui->labelBTC_2->setText(btctotal);

    // add 'em up and display 'em
    double alltot = btctotU;
    QString alltotal = QString::number(alltot, 'f', 8);
    ui->labelTotal_2->setText(alltotal);

    // calc price per BCR based on total bids and display it
    double bcrprice = alltot / 18000;
    QString bcrPrice = QString::number(bcrprice, 'f', 8);
    ui->labelEstprice_2->setText(bcrPrice);

    ui->lineEditBid->setEnabled(true);
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

void BidPage::SummonBTCWallet()
{
    QProcess *proc = new QProcess(this);
    #ifdef linux
        proc->startDetached("bitcredit-qt");
    #elif _WIN32
        proc->startDetached("bitcredit-qt.exe");
    #endif
}

void BidPage::RPC()
{
    // check there's something to work with
    if (ui->lineEditPassphrase->text() == "") 
    {
        ui->lineEditPassphrase->setText("ENTER YOUR WALLET PASSPHRASE");
        return;
    }
    if (ui->lineEditPrivkey->text() == "") 
    {
        ui->lineEditPrivkey->setText("ENTER THE BITCOIN ADDRESS PRIVATE KEY");
        return;        
    }

    // get working data directory
    QString cwd = GUIUtil::boostPathToQString(GetDataDir());

    // get password
    QString pwd = ui->lineEditPassphrase->text();

    // build RPC call
    QString callnix = cwd + "/bitcredit-cli --datadir=" + cwd + " walletpassphrase " + pwd + " 60";
    QString callwin = cwd + "/bitcredit-cli.exe --datadir=" + cwd + " walletpassphrase " + pwd + " 60";

    // unlock wallet
    QProcess *proc2 = new QProcess(this);
    #ifdef linux
        proc2->start(callnix);
        proc2->waitForFinished();
        QString output(proc2->readAllStandardOutput()); // check for any output
        // reset pwd field
        ui->lineEditPassphrase->setText("");
    #elif _WIN32
        proc2->start(callwin);
        proc2->waitForFinished();
        QString output(proc2->readAllStandardOutput()); // check for any output
        // reset pwd field
        ui->lineEditPassphrase->setText("");       
    #endif

    // get privkey
    QString privkey = ui->lineEditPrivkey->text();

    // build RPC call
    QString callnix2 = cwd + "/bitcredit-cli --datadir=" + cwd + " importprivkey " + privkey;
    QString callwin2 = cwd + "/bitcredit-cli.exe --datadir=" + cwd + " importprivkey " + privkey;

    // import privkey
    QProcess *proc3 = new QProcess(this);
    #ifdef linux
        proc3->start(callnix2);
        proc3->waitForFinished();
        QString output2(proc3->readAllStandardOutput()); // check for any output
        // reset privkey field
        ui->lineEditPrivkey->setText("");
    #elif _WIN32
        proc3->start(callwin2);
        proc3->waitForFinished();
        QString output2(proc3->readAllStandardOutput()); // check for any output
        // reset privkey field
        ui->lineEditPrivkey->setText(output);
    #endif    

}

BidPage::~BidPage()
{
    delete ui;
}
