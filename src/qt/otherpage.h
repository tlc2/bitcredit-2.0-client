#ifndef OTHERPAGE_H
#define OTHERPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>

namespace Ui
{
    class OtherPage;
}

class OtherPage: public QWidget
{
    Q_OBJECT

public:
    OtherPage(QWidget *parent = 0);
    ~OtherPage();

    void setClientModel(ClientModel *model);

Q_SIGNALS:
    void bURIclicked(); 
    void bBackupclicked(); 
    void bSignmessageclicked();
    void bVerifymessageclicked(); 
    void bSendingaddressesclicked();
    void bReceivingaddressesclicked();
    void bEncclicked();
    void bChangeclicked(); 
    void bOptionsclicked();
    void bRPCclicked();
    void bHelpclicked();

private:
    Ui::OtherPage *ui;
    ClientModel *clientModel;

private Q_SLOTS:
    void emitbURIclicked();
    void emitbBackupclicked();
    void emitbSignmessageclicked();
    void emitbVerifymessageclicked();
    void emitbSendingaddressesclicked();
    void emitbReceivingaddressesclicked();
    void emitbEncclicked();
    void emitbChangeclicked();
    void emitbOptionsclicked();
    void emitbRPCclicked();
    void emitbHelpclicked();
};

#endif // OTHERPAGE_H
