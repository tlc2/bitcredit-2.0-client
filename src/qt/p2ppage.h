#ifndef P2PPAGE_H
#define P2PPAGE_H

#include "clientmodel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QProcess>

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

    QProcess *proc1;
    QProcess *proc2;
    QProcess *proc3;

private Q_SLOTS:
    void SubmitLoanRequest();
    bool fileExists(QString path);
    QString pathAppend(const QString& path1, const QString& path2);

};

#endif // P2PPAGE_H
