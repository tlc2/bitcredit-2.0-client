#ifndef BLOCKEXPLORERPAGE_H
#define BLOCKEXPLORERPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>

namespace Ui
{
    class BlockExplorerPage;
}

class BlockExplorerPage: public QWidget
{
    Q_OBJECT

public:
    BlockExplorerPage(QWidget *parent = 0);
    ~BlockExplorerPage();

    void setClientModel(ClientModel *model);

private:
    Ui::BlockExplorerPage *ui;
    ClientModel *clientModel;


};

#endif // BLOCKEXPLORERPAGE_H
