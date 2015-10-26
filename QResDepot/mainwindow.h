#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QHash>
#include <QSet>
typedef QSet<QString> QSetString;
typedef QHash<QString, QString> HashFile;
typedef QHash<QString, QSetString> GroupHashFile;

class QStandardItemModel;
class QSortFilterProxyModel;
class QStringListModel;
class QComboBox;
#include<QModelIndexList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initHashFile(QString path);
    void showGroupListView();
    //    void sortAscend();
    void showGroupByCurrIndex();
    void saveRes();
    void analysisJsonObject(QJsonObject jobj);
    void opFileAndShowList(QString path);
    bool pathIsValid(QString path);
    void messageBoxShow(QString msg);
    bool openRectLyDirAndSave();
    bool saveStringToFile(QString writeStr, QString writePath);
    void clearResData();
    void clearGroupData();
private:
    void initModelWithHashFile();

private slots:
    void openFile();

    void on_pushButton_clicked();

    void on_groupListView_clicked(const QModelIndex &index);

    void onDropFinished(QHash<QString , QString> &keyPaths);

    void on_filterLineEdit_textChanged(const QString &arg1);


    void on_pushButton_2_clicked();

    void on_groupListView_doubleClicked(const QModelIndex &index);

    //    void on_groupListView_customContextMenuRequested(const QPoint &pos);

    void on_groupFileTableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_3_clicked();

    void on_saveAction_triggered();

    void on_openActionMenu_triggered();

private:
    Ui::MainWindow *ui;
    HashFile hashFile;
    QStandardItemModel* resFilesModel;
    QSortFilterProxyModel* resFilesProxyModel;

    QStandardItemModel* groupFilesModel;
    QSortFilterProxyModel* groupFilesProxyModel;

    QStringListModel* groupListModel;
    GroupHashFile     groupHashFile;
    QString rootPath;
    //文件过滤
    QComboBox *filterSyntaxComboBox;
};

#endif // MAINWINDOW_H
