#ifndef P2PLPAGE_H
#define P2PLPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>

namespace Ui
{
    class P2PLPage;
}

class P2PLPage: public QWidget
{
    Q_OBJECT

public:
    P2PLPage(QWidget *parent = 0);
    ~P2PLPage();

    void setClientModel(ClientModel *model);

private:
    Ui::P2PLPage *ui;
    ClientModel *clientModel;

};

#endif // P2PLPAGE_H
