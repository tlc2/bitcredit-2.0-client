// Copyright (c) 2011-2015 The Bitcredit Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCREDIT_QT_OVERVIEWPAGE_H
#define BITCREDIT_QT_OVERVIEWPAGE_H

#include "amount.h"

#include <QWidget>

class ClientModel;
class TransactionFilterProxy;
class TxViewDelegate;
class PlatformStyle;
class WalletModel;

namespace Ui {
    class OverviewPage;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Overview ("home") page widget */
class OverviewPage : public QWidget
{
    Q_OBJECT

public:
    explicit OverviewPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~OverviewPage();

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);
    void showOutOfSyncWarning(bool fShow);

public Q_SLOTS:
    void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
                    const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);

Q_SIGNALS:
    void transactionClicked(const QModelIndex &index);
    void btxclicked();
    void bsendclicked();
    void brecclicked();
    void bgetbcrclicked();
    void bp2pclicked();
    void bassetsclicked();
    void butilitiesclicked();
    void textChanged(QString);

private:
    Ui::OverviewPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    CAmount currentBalance;
    CAmount currentUnconfirmedBalance;
    CAmount currentImmatureBalance;
    CAmount currentWatchOnlyBalance;
    CAmount currentWatchUnconfBalance;
    CAmount currentWatchImmatureBalance;

    TxViewDelegate *txdelegate;
    TransactionFilterProxy *filter;

private Q_SLOTS:
    void updateDisplayUnit();
    void handleTransactionClicked(const QModelIndex &index);
    void updateAlerts(const QString &warnings);
    void updateWatchOnlyLabels(bool showWatchOnly);
    
    void emitbtxclicked();
    void emitbsendclicked();
    void emitbrecclicked();
    void emitbgetbcrclicked();
    void emitbp2pclicked();
    void emitbassetsclicked();
    void emitbutilitiesclicked();
    
    void someFunctionThatChangesText(const QString& newtext);
};

#endif // BITCREDIT_QT_OVERVIEWPAGE_H
