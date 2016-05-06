#ifndef ASSETSPAGE_H
#define ASSETSPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>

namespace Ui
{
    class AssetsPage;
}

class AssetsPage: public QWidget
{
    Q_OBJECT

public:
    AssetsPage(QWidget *parent = 0);
    ~AssetsPage();

    void setClientModel(ClientModel *model);

private:
    Ui::AssetsPage *ui;
    ClientModel *clientModel;


};

#endif // ASSETSPAGE_H
