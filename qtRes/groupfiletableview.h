#ifndef GROUPFILETABLEVIEW_H
#define GROUPFILETABLEVIEW_H

#include <QTableView>

class GroupFileTableView : public QTableView
{
    Q_OBJECT
public:
    explicit GroupFileTableView(QWidget *parent = 0);


    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

    void setGroupName(QString value) {
        groupName = value;
    }

signals:
    void dropFinished(QHash<QString , QString> &keyPaths);

public slots:

private:
    QString groupName;
};

#endif // GROUPFILETABLEVIEW_H
