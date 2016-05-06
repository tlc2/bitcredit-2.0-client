#ifndef P2PPAGE_H
#define P2PPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>

namespace Ui
{
    class P2PPage;
}

class P2PPage: public QWidget
{
    Q_OBJECT

public:
    P2PPage(QWidget *parent = 0);
    ~P2PPage();

    void setClientModel(ClientModel *model);

private:
    Ui::P2PPage *ui;
    ClientModel *clientModel;

};

#endif // P2PPAGE_H
