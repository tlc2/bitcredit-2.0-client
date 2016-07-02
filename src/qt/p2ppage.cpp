#include "p2ppage.h"
#include "ui_p2ppage.h"

#include "util.h"
#include "guiutil.h"

#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>


P2PPage::P2PPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::P2PPage)
{
    ui->setupUi(this);
    connect(ui->bSubmit, SIGNAL(clicked()), this, SLOT(SubmitLoanRequest()));
}

void P2PPage::SubmitLoanRequest()
{
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
        return;
    }

    // unlock wallet
    QString pwd = ui->le_pwd->text();

    QString rpccall3 = "walletpassphrase " + pwd + " 60";
    QString callnix3 = cwd + "/bitcredit-cli --datadir=" + cwd + " " + rpccall3;
    QString callwin3 = cwd + "/bitcredit-cli.exe --datadir=" + cwd + " " + rpccall3;

    proc3 = new QProcess(this);
    #ifdef __linux
        proc3->start(callnix3);
        proc3->waitForFinished();
        QString output4(proc3->readAllStandardOutput()); 
    #elif _WIN32
        proc3->start(callwin3);
        proc3->waitForFinished();
        QString output4(proc3->readAllStandardOutput());      
    #endif 

    QString address = ui->le_address->text();
    address.replace(" ", "" ); // remove any whitespaces

    // check address is valid - ie one controlled by this wallet
    QString rpccall2 = "listaddressgroupings";
    QString callnix2 = cwd + "/bitcredit-cli --datadir=" + cwd + " " + rpccall2;
    QString callwin2 = cwd + "/bitcredit-cli.exe --datadir=" + cwd + " " + rpccall2;

    //QMessageBox::information(0, QString("Attention!"), callnix2, QMessageBox::Ok);

    proc2 = new QProcess(this);
    #ifdef __linux
        proc2->start(callnix2);
        proc2->waitForFinished();
        QString output3(proc2->readAllStandardOutput()); 
    #elif _WIN32
        proc2->start(callwin2);
        proc2->waitForFinished();
        QString output3(proc2->readAllStandardOutput());      
    #endif    

    if (!output3.contains(address)) 
    {
        QMessageBox::information(0, QString("Attention!"), QString("The address you have entered is unrecognised by this wallet!"), QMessageBox::Ok); 
        return;
    }

    QString amount = ui->le_amount->text();
    address.replace(" ", "" ); // remove any whitespaces
    QString premium = ui->cb_premium->currentText();
    QString expiry = ui->cb_expiry->currentText();
    QString period = ui->cb_period->currentText();
    QString message = ui->te_message->toPlainText();

    // convert premium / expiry / period to appropriate numbers

    QString premium2="";
    QString expiry2="";
    QString period2="";

    if (premium=="1%") premium2="0.01";
    else if (premium=="3%") premium2="0.03";
    else if (premium=="5%") premium2="0.05";
    else if (premium=="7%") premium2="0.07";
    else if (premium=="10%") premium2="0.10";
    else if (premium=="25%") premium2="0.25";
    else if (premium=="50%") premium2="0.5";
    else premium2="1";

    if (expiry=="1 day") expiry2="1";
    else if (expiry=="1 week") expiry2="7";
    else expiry2="30";

    if (period=="1 week") period2="7";
    else if (period=="2 weeks") period2="14";
    else if (period=="1 month") period2="30";
    else if (period=="3 months") period2="90";
    else if (period=="6 months") period2="180";
    else period2="365";

    // build RPC calls
    QString rpccallnix = ("createloanrequest " + address + " " + amount + " " + premium2 + " " + expiry2 + " " + period2 + " " + "\"" + message + "\"");
    QString rpccallwin = ("createloanrequest " + address + " " + amount + " " + premium2 + " " + expiry2 + " " + period2 + " " + "\"" + message + "\"");

    //QString summary = ("Loan address: " + address + "\nAmount: " + amount + "\nPremium: " + premium + "\nExpiry: " + expiry + "\nPeriod: " + period + "\nMessage: " + message + "\n\nRPC call:\n\n" + rpccallnix);
    //QMessageBox::information(0, QString("Test UI input parsing..."), summary, QMessageBox::Ok);

    // complete RPC call with cwd
    QString callnix = cwd + "/bitcredit-cli --datadir=" + cwd + " " + rpccallnix;
    QString callwin = cwd + "/bitcredit-cli.exe --datadir=" + cwd + " " + rpccallwin;

    //QMessageBox::information(0, QString("Info!"), callnix, QMessageBox::Ok);

    proc1 = new QProcess(this);
    #ifdef __linux
        proc1->start(callnix);
        proc1->waitForFinished();
        QString output(proc1->readAllStandardOutput()); 
        QString outputerror(proc1->readAllStandardError());
    #elif _WIN32
        proc1->start(callwin);
        proc1->waitForFinished();
        QString output(proc1->readAllStandardOutput());
        QString outputerror(proc1->readAllStandardError());      
    #endif

    QMessageBox::information(0, QString("Info!"), output, QMessageBox::Ok);

    // provide feedback on result
    if (output.contains("Sent")) 
    {
        QMessageBox::information(0, QString("Request submitted!"), QString("Thank you, your loan request has been submitted and the 100 BCR fee deducted from your balance."), QMessageBox::Ok); 
    }
    else
    {
        QMessageBox::information(0, QString("Attention!"), QString("There was a problem with your loan request. Please check your entered paramaters and password and retry.\n\n" + outputerror), QMessageBox::Ok); 
    }
}

bool P2PPage::fileExists(QString path) 
{
    QFileInfo check_file(path);
    return check_file.exists();
}

QString P2PPage::pathAppend(const QString& path1, const QString& path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

P2PPage::~P2PPage()
{
    delete ui;
}
