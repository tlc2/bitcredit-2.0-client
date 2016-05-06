#include "blockexplorerpage.h"
#include "ui_blockexplorerpage.h"


BlockExplorerPage::BlockExplorerPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::BlockExplorerPage)
{
    ui->setupUi(this);

}


BlockExplorerPage::~BlockExplorerPage()
{
    delete ui;
}
