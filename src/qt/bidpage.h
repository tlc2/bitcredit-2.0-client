#ifndef BIDPAGE_H
#define BIDPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>


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

    void setClientModel(ClientModel *model);

private:
    Ui::BidPage *ui;
    ClientModel *clientModel;

private Q_SLOTS:
    void SummonBTCWallet();   
    void SummonBTCExplorer(); 
    void GetBids();
    void setNumBlocks(int count);
    int getNumBlocks();
    void Estimate();
    void RPC();

    QString getDataDirectory();
    QString pathAppend(const QString& path1, const QString& path2);

};

#endif // BIDPAGE_H
