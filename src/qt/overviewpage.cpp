// Copyright (c) 2011-2015 The Bitcredit Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "overviewpage.h"
#include "ui_overviewpage.h"

#include "bitcreditunits.h"
#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "transactionfilterproxy.h"
#include "transactiontablemodel.h"
#include "walletmodel.h"

#include <QAbstractItemDelegate>
#include <QPainter>

#define DECORATION_SIZE 54
#define NUM_ITEMS 5

class TxViewDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    TxViewDelegate(const PlatformStyle *platformStyle):
        QAbstractItemDelegate(), unit(BitcreditUnits::BCR),
        platformStyle(platformStyle)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        painter->save();

        QIcon icon = qvariant_cast<QIcon>(index.data(TransactionTableModel::RawDecorationRole));
        QRect mainRect = option.rect;
        QRect decorationRect(mainRect.topLeft(), QSize(DECORATION_SIZE, DECORATION_SIZE));
        int xspace = DECORATION_SIZE + 8;
        int ypad = 6;
        int halfheight = (mainRect.height() - 2*ypad)/2;
        QRect amountRect(mainRect.left() + xspace, mainRect.top()+ypad, mainRect.width() - xspace, halfheight);
        QRect addressRect(mainRect.left() + xspace, mainRect.top()+ypad+halfheight, mainRect.width() - xspace, halfheight);
        icon = platformStyle->SingleColorIcon(icon);
        icon.paint(painter, decorationRect);

        QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
        QString address = index.data(Qt::DisplayRole).toString();
        qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
        bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();
        QVariant value = index.data(Qt::ForegroundRole);
        QColor foreground = option.palette.color(QPalette::Text);
        if(value.canConvert<QBrush>())
        {
            QBrush brush = qvariant_cast<QBrush>(value);
            foreground = brush.color();
        }

        painter->setPen(foreground);
        QRect boundingRect;
        painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, address, &boundingRect);

        if (index.data(TransactionTableModel::WatchonlyRole).toBool())
        {
            QIcon iconWatchonly = qvariant_cast<QIcon>(index.data(TransactionTableModel::WatchonlyDecorationRole));
            QRect watchonlyRect(boundingRect.right() + 5, mainRect.top()+ypad+halfheight, 16, halfheight);
            iconWatchonly.paint(painter, watchonlyRect);
        }

        if(amount < 0)
        {
            foreground = COLOR_NEGATIVE;
        }
        else if(!confirmed)
        {
            foreground = COLOR_UNCONFIRMED;
        }
        else
        {
            foreground = option.palette.color(QPalette::Text);
        }
        painter->setPen(foreground);
        QString amountText = BitcreditUnits::formatWithUnit(unit, amount, true, BitcreditUnits::separatorAlways);
        if(!confirmed)
        {
            amountText = QString("[") + amountText + QString("]");
        }
        painter->drawText(amountRect, Qt::AlignRight|Qt::AlignVCenter, amountText);

        painter->setPen(option.palette.color(QPalette::Text));
        painter->drawText(amountRect, Qt::AlignLeft|Qt::AlignVCenter, GUIUtil::dateTimeStr(date));

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(DECORATION_SIZE, DECORATION_SIZE);
    }

    int unit;
    const PlatformStyle *platformStyle;

};
#include "overviewpage.moc"

OverviewPage::OverviewPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OverviewPage),
    clientModel(0),
    walletModel(0),
    currentBalance(-1),
    currentUnconfirmedBalance(-1),
    currentImmatureBalance(-1),
    currentWatchOnlyBalance(-1),
    currentWatchUnconfBalance(-1),
    currentWatchImmatureBalance(-1),
    txdelegate(new TxViewDelegate(platformStyle)),
    filter(0)
{
    ui->setupUi(this);
    
    // help stylesheet find things
    ui->btx->setObjectName("btx");
    ui->bsend->setObjectName("bsend");
    ui->bgetbcr->setObjectName("bgetbcr");
    ui->bp2p->setObjectName("bp2p");
    ui->bassets->setObjectName("bassets");
    ui->butilities->setObjectName("butilities");

    // use a SingleColorIcon for the "out of sync warning" icon
    QIcon icon = platformStyle->SingleColorIcon(":/icons/warning");
    icon.addPixmap(icon.pixmap(QSize(64,64), QIcon::Normal), QIcon::Disabled); // also set the disabled icon because we are using a disabled QPushButton to work around missing HiDPI support of QLabel (https://bugreports.qt.io/browse/QTBUG-42503)
    //ui->labelTransactionsStatus->setIcon(icon);
    //ui->labelWalletStatus->setIcon(icon);

    // Recent transactions
    //ui->listTransactions->setItemDelegate(txdelegate);
    //ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    //ui->listTransactions->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    //ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);

    //connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));

    // start with displaying the "out of sync" warnings
    showOutOfSyncWarning(true);
    
    // emit signals when menu buttons clicked that walletview can pick up to switch pages
    connect(ui->btx, SIGNAL(clicked()), this, SLOT(emitbtxclicked()));
    connect(ui->bsend, SIGNAL(clicked()), this, SLOT(emitbsendclicked()));
    connect(ui->bgetbcr, SIGNAL(clicked()), this, SLOT(emitbgetbcrclicked()));
    connect(ui->bp2p, SIGNAL(clicked()), this, SLOT(emitbp2pclicked()));
    connect(ui->bassets, SIGNAL(clicked()), this, SLOT(emitbassetsclicked()));
    connect(ui->butilities, SIGNAL(clicked()), this, SLOT(emitbutilitiesclicked()));
    
}

void OverviewPage::emitbtxclicked()
{
    Q_EMIT btxclicked();
}

void OverviewPage::emitbsendclicked()
{
    Q_EMIT bsendclicked();
}

void OverviewPage::emitbrecclicked()
{
    Q_EMIT brecclicked();
}

void OverviewPage::emitbgetbcrclicked()
{
    Q_EMIT bgetbcrclicked();
}

void OverviewPage::emitbp2pclicked()
{
    Q_EMIT bp2pclicked();
}

void OverviewPage::emitbassetsclicked()
{
    Q_EMIT bassetsclicked();
}

void OverviewPage::emitbutilitiesclicked()
{
    Q_EMIT butilitiesclicked();
}

void OverviewPage::handleTransactionClicked(const QModelIndex &index)
{
    if(filter)
        Q_EMIT transactionClicked(filter->mapToSource(index));
}

OverviewPage::~OverviewPage()
{
    delete ui;
}

void OverviewPage::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance)
{
    int unit = walletModel->getOptionsModel()->getDisplayUnit();
    currentBalance = balance;
    currentUnconfirmedBalance = unconfirmedBalance;
    currentImmatureBalance = immatureBalance;
    currentWatchOnlyBalance = watchOnlyBalance;
    currentWatchUnconfBalance = watchUnconfBalance;
    currentWatchImmatureBalance = watchImmatureBalance;
    ui->labelBalance->setText(BitcreditUnits::formatWithUnit(unit, balance, false, BitcreditUnits::separatorAlways));

    QString bal = ui->labelBalance->text();
    someFunctionThatChangesText(bal);
 
     //ui->labelUnconfirmed->setText(BitcreditUnits::formatWithUnit(unit, unconfirmedBalance, false, BitcreditUnits::separatorAlways));
    //ui->labelImmature->setText(BitcreditUnits::formatWithUnit(unit, immatureBalance, false, BitcreditUnits::separatorAlways));
    //ui->labelTotal->setText(BitcreditUnits::formatWithUnit(unit, balance + unconfirmedBalance + immatureBalance, false, BitcreditUnits::separatorAlways));
    //ui->labelWatchAvailable->setText(BitcreditUnits::formatWithUnit(unit, watchOnlyBalance, false, BitcreditUnits::separatorAlways));
    //ui->labelWatchPending->setText(BitcreditUnits::formatWithUnit(unit, watchUnconfBalance, false, BitcreditUnits::separatorAlways));
   //ui->labelWatchImmature->setText(BitcreditUnits::formatWithUnit(unit, watchImmatureBalance, false, BitcreditUnits::separatorAlways));
    //ui->labelWatchTotal->setText(BitcreditUnits::formatWithUnit(unit, watchOnlyBalance + watchUnconfBalance + watchImmatureBalance, false, BitcreditUnits::separatorAlways));

    // only show immature (newly mined) balance if it's non-zero, so as not to complicate things
    // for the non-mining users
    //bool showImmature = immatureBalance != 0;
    //bool showWatchOnlyImmature = watchImmatureBalance != 0;

    // for symmetry reasons also show immature label when the watch-only one is shown
    //ui->labelImmature->setVisible(showImmature || showWatchOnlyImmature);
    //ui->labelImmatureText->setVisible(showImmature || showWatchOnlyImmature);
    //ui->labelWatchImmature->setVisible(showWatchOnlyImmature); // show watch-only immature balance
}

// show/hide watch-only labels
void OverviewPage::updateWatchOnlyLabels(bool showWatchOnly)
{
    //ui->labelSpendable->setVisible(showWatchOnly);      // show spendable label (only when watch-only is active)
    //ui->labelWatchonly->setVisible(showWatchOnly);      // show watch-only label
    //ui->lineWatchBalance->setVisible(showWatchOnly);    // show watch-only balance separator line
    //ui->labelWatchAvailable->setVisible(showWatchOnly); // show watch-only available balance
    //ui->labelWatchPending->setVisible(showWatchOnly);   // show watch-only pending balance
    //ui->labelWatchTotal->setVisible(showWatchOnly);     // show watch-only total balance

    //if (!showWatchOnly)
        //ui->labelWatchImmature->hide();
}

void OverviewPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
        // Show warning if this is a prerelease version
        connect(model, SIGNAL(alertsChanged(QString)), this, SLOT(updateAlerts(QString)));
        updateAlerts(model->getStatusBarWarnings());
    }
}

void OverviewPage::someFunctionThatChangesText(const QString& newtext)
{
    Q_EMIT textChanged(newtext);
}

void OverviewPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
        // Set up transaction list
        filter = new TransactionFilterProxy();
        filter->setSourceModel(model->getTransactionTableModel());
        filter->setLimit(NUM_ITEMS);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->setShowInactive(false);
        filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);

        //ui->listTransactions->setModel(filter);
        //ui->listTransactions->setModelColumn(TransactionTableModel::ToAddress);

        // Keep up to date with wallet
        setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance(),
                   model->getWatchBalance(), model->getWatchUnconfirmedBalance(), model->getWatchImmatureBalance());
        connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)), this, SLOT(setBalance(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)));
        
        connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)), this, SLOT(emitbalancechanged()));

        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

        updateWatchOnlyLabels(model->haveWatchOnly());
        connect(model, SIGNAL(notifyWatchonlyChanged(bool)), this, SLOT(updateWatchOnlyLabels(bool)));
    }

    // update the display unit, to not use the default ("BCR")
    updateDisplayUnit();
}

void OverviewPage::updateDisplayUnit()
{
    if(walletModel && walletModel->getOptionsModel())
    {
        if(currentBalance != -1)
            setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance,
                       currentWatchOnlyBalance, currentWatchUnconfBalance, currentWatchImmatureBalance);

        // Update txdelegate->unit with the current unit
        txdelegate->unit = walletModel->getOptionsModel()->getDisplayUnit();

        //ui->listTransactions->update();
    }
}

void OverviewPage::updateAlerts(const QString &warnings)
{
    //this->ui->labelAlerts->setVisible(!warnings.isEmpty());
    //this->ui->labelAlerts->setText(warnings);
}

void OverviewPage::showOutOfSyncWarning(bool fShow)
{
    //ui->labelWalletStatus->setVisible(fShow);
    //ui->labelTransactionsStatus->setVisible(fShow);
}
