#include "assetspage.h"
#include "ui_assetspage.h"


AssetsPage::AssetsPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::AssetsPage)
{
    ui->setupUi(this);

}


AssetsPage::~AssetsPage()
{
    delete ui;
}
