#include "groupfiletableview.h"
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QMouseEvent>
#include <QDebug>

GroupFileTableView::GroupFileTableView(QWidget *parent) :
    QTableView(parent)
{

}

void GroupFileTableView::dragEnterEvent(QDragEnterEvent *event)
{
    QObject* source = event->source();
    if (source->objectName() == "resFileTableView" && !groupName.isEmpty()) {
        event->accept();
    }
}

void GroupFileTableView::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void GroupFileTableView::dropEvent(QDropEvent *event)
{
    QTableView* tableView = qobject_cast<QTableView*>(event->source());
    QAbstractItemModel* model = tableView->model();
    QItemSelectionModel* selectModel = tableView->selectionModel();
    QModelIndexList keyList = selectModel->selectedRows(0);
    QModelIndexList pathList = selectModel->selectedRows(1);
    QHash<QString , QString> key_paths;

//    QAbstractItemModel* currModel = this->model();
    for(int i = 0; i < keyList.length(); i++) {
        QString key = keyList[i].data().toString();
        QString path = pathList[i].data().toString();
        key_paths[key] = path;

//        currModel->insertRow(0);
//        currModel->setData(currModel->index(0, 0), key);
//        currModel->setData(currModel->index(0, 1), path);
    }

    event->accept();
    emit this->dropFinished(key_paths);
}

void GroupFileTableView::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug() << "sfdsf";
}
