#include "utilitiespage.h"
#include "ui_utilitiespage.h"


UtilitiesPage::UtilitiesPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::UtilitiesPage)
{
    ui->setupUi(this);

}


UtilitiesPage::~UtilitiesPage()
{
    delete ui;
}
