#ifndef BLOCKEXPLORERPAGE_H
#define BLOCKEXPLORERPAGE_H

#include <QWidget>

#include "uint256.h"
#include "base58.h"

namespace Ui {
class BlockExplorerPage;
}

class CBlockIndex;
class CTransaction;

class BlockExplorerPage : public QWidget
{
    Q_OBJECT

public:
    explicit BlockExplorerPage(QWidget *parent = 0);
    ~BlockExplorerPage();

protected:
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent*);

private:
    Ui::BlockExplorerPage *ui;
    bool m_NeverShown;
    int m_HistoryIndex;
    QStringList m_History;

private Q_SLOTS:
    void onSearch();
    void goTo(const QString& query);
    void back();
    void forward();
    void setBlock(CBlockIndex* pBlock);
    bool switchTo(const QString& query);
    void setContent(const std::string &content);
    void updateNavButtons();

};

#endif // BLOCKEXPLORERPAGE_H
