#include "exchangebrowserpage.h"
#include "ui_exchangebrowserpage.h"


ExchangeBrowserPage::ExchangeBrowserPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::ExchangeBrowserPage)
{
    ui->setupUi(this);

}


ExchangeBrowserPage::~ExchangeBrowserPage()
{
    delete ui;
}
