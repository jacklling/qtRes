#ifndef OPENFILE_H
#define OPENFILE_H

#include <QLineEdit>
#include <QDragEnterEvent>
#include <QFileInfo>
#include <QMimeData>


class openFile: public QLineEdit
{
public:
    openFile(QWidget *parent =0);
protected:
    virtual void dragEnterEvent ( QDragEnterEvent * event ) override
    {
        if (event->mimeData()->hasUrls())
            event->acceptProposedAction();
    }

    virtual void dragMoveEvent(QDragMoveEvent * event) override
    {
        if (event->mimeData()->hasUrls())
            event->acceptProposedAction();
    }

    virtual void dropEvent(QDropEvent * event) override
    {
        if (event->mimeData()->hasUrls())
        {
            QList<QUrl> list = event->mimeData()->urls();
            QFileInfo * fileInfo = new QFileInfo(list.at(0).toLocalFile());
            setText(fileInfo->absoluteFilePath());
            event->acceptProposedAction();
        }
    }
};

#endif // OPENFILE_H



