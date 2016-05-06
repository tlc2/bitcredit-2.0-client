#include "p2plpage.h"
#include "ui_p2plpage.h"


P2PLPage::P2PLPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::P2PLPage)
{
    ui->setupUi(this);

}


P2PLPage::~P2PLPage()
{
    delete ui;
}
