#ifndef BIDPAGE_H
#define BIDPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QLabel>
#include <QProcess>


namespace Ui
{
    class BidPage;
}

class BidPage: public QWidget
{
    Q_OBJECT

public:
    BidPage(QWidget *parent = 0);
    ~BidPage();

    QString str;
    QString btctotal;
    double btctot;
    int until;
    float podl;

    void setClientModel(ClientModel *model);

private:
    Ui::BidPage *ui;
    ClientModel *clientModel;
    
    QProcess *proc;
    QProcess *proc2;
    QProcess *proc3;
    
    QProcess *procsched;
    QProcess *procsched2;    
    
    QTimer *electrumtimer;

private Q_SLOTS:
    void SummonElectrum();   
    void SummonBTCExplorer(); 
    void GetBids();
    void setNumBlocks(int count);
    int getNumBlocks();
    void Estimate();
    void RPC();
    void scheduleBid();
    void callElectrumDaemon();
    
    bool fileExists(QString path);

    QString getDataDirectory();
    QString pathAppend(const QString& path1, const QString& path2);

};

#endif // BIDPAGE_H
