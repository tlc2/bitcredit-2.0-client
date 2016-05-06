#ifndef EXCHANGEBROWSERPAGE_H
#define EXCHANGEBROWSERPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>

namespace Ui
{
    class ExchangeBrowserPage;
}

class ExchangeBrowserPage: public QWidget
{
    Q_OBJECT

public:
    ExchangeBrowserPage(QWidget *parent = 0);
    ~ExchangeBrowserPage();

    void setClientModel(ClientModel *model);

private:
    Ui::ExchangeBrowserPage *ui;
    ClientModel *clientModel;


};

#endif // EXCHANGEBROWSERPAGE_H
