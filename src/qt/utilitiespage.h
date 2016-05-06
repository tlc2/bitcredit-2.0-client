#ifndef UTILITIESPAGE_H
#define UTILITIESPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>

namespace Ui
{
    class UtilitiesPage;
}

class UtilitiesPage: public QWidget
{
    Q_OBJECT

public:
    UtilitiesPage(QWidget *parent = 0);
    ~UtilitiesPage();

    void setClientModel(ClientModel *model);

private:
    Ui::UtilitiesPage *ui;
    ClientModel *clientModel;


};

#endif // UTILITIESPAGE_H
