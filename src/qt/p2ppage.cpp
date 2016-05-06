#include "p2ppage.h"
#include "ui_p2ppage.h"


P2PPage::P2PPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::P2PPage)
{
    ui->setupUi(this);

}


P2PPage::~P2PPage()
{
    delete ui;
}
