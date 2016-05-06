#include "otherpage.h"
#include "ui_otherpage.h"


OtherPage::OtherPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::OtherPage)
{
    ui->setupUi(this);

    connect(ui->bURI, SIGNAL(clicked()), this, SLOT(emitbURIclicked()));
    connect(ui->bBackup, SIGNAL(clicked()), this, SLOT(emitbBackupclicked()));
    connect(ui->bSignmessage, SIGNAL(clicked()), this, SLOT(emitbSignmessageclicked()));
    connect(ui->bVerifymessage, SIGNAL(clicked()), this, SLOT(emitbVerifymessageclicked()));
    connect(ui->bSendingaddresses, SIGNAL(clicked()), this, SLOT(emitbSendingaddressesclicked()));
    connect(ui->bReceivingaddresses, SIGNAL(clicked()), this, SLOT(emitbReceivingaddressesclicked()));
    connect(ui->bEnc, SIGNAL(clicked()), this, SLOT(emitbEncclicked()));
    connect(ui->bChange, SIGNAL(clicked()), this, SLOT(emitbChangeclicked()));
    connect(ui->bOptions, SIGNAL(clicked()), this, SLOT(emitbOptionsclicked()));
    connect(ui->bRPC, SIGNAL(clicked()), this, SLOT(emitbRPCclicked()));
    connect(ui->bHelp, SIGNAL(clicked()), this, SLOT(emitbHelpclicked()));
}


void OtherPage::emitbURIclicked()
{
    Q_EMIT bURIclicked();
}

void OtherPage::emitbBackupclicked()
{
    Q_EMIT bBackupclicked();
}

void OtherPage::emitbSignmessageclicked()
{
    Q_EMIT bSignmessageclicked();
}

void OtherPage::emitbVerifymessageclicked()
{
    Q_EMIT bVerifymessageclicked();
}

void OtherPage::emitbSendingaddressesclicked()
{
    Q_EMIT bSendingaddressesclicked();
}

void OtherPage::emitbReceivingaddressesclicked()
{
    Q_EMIT bReceivingaddressesclicked();
}

void OtherPage::emitbEncclicked()
{
    Q_EMIT bEncclicked();
}

void OtherPage::emitbChangeclicked()
{
    Q_EMIT bChangeclicked();
}

void OtherPage::emitbOptionsclicked()
{
    Q_EMIT bOptionsclicked();
}

void OtherPage::emitbRPCclicked()
{
    Q_EMIT bRPCclicked();
}

void OtherPage::emitbHelpclicked()
{
    Q_EMIT bHelpclicked();
}

OtherPage::~OtherPage()
{
    delete ui;
}
