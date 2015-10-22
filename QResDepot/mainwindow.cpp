#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

#include <QDirIterator>
#include <QDir>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QSettings>
#include <QInputDialog>
#include <QStringList>

#include <QStringListModel>

//读json
//JSON
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonObject>
#include <QStringList>
#include <QMessageBox>

#include <QApplication>
//
#include <QAction>
#include <QFileSystemModel>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rootPath = ".";
    // 拖拽
    connect(ui->groupFileTableView, &GroupFileTableView::dropFinished, this, &MainWindow::onDropFinished);
    //保存数据
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveRes);
    //打开文件夹 项目资源
    connect(ui->actionOpenDir, &QAction::triggered, this, &MainWindow::openFile);

    //工具栏
    QToolBar *fileBar = this->addToolBar("open with file");
    fileBar->addAction(ui->actionOpenDir);
    fileBar->addAction(ui->actionSave);

    this->groupListModel = new QStringListModel(this);
    ui->groupListView->setModel(this->groupListModel);

    //init group files
    this->groupFilesModel = new QStandardItemModel(0, 2, this);
    this->groupFilesModel->setHeaderData(0, Qt::Horizontal, QObject::tr("key"));
    this->groupFilesModel->setHeaderData(1, Qt::Horizontal, QObject::tr("path"));

    this->groupFilesProxyModel = new QSortFilterProxyModel(this);
    this->groupFilesProxyModel->setSourceModel(this->groupFilesModel);
    ui->groupFileTableView->setModel(this->groupFilesProxyModel);
    ui->groupListView->setContextMenuPolicy(Qt::CustomContextMenu);

    initModelWithHashFile();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initModelWithHashFile() {
    this->resFilesModel = new QStandardItemModel(this->hashFile.size(), 2, this);
    this->resFilesModel->setHeaderData(0, Qt::Horizontal, QObject::tr("key"));
    this->resFilesModel->setHeaderData(1, Qt::Horizontal, QObject::tr("path"));

    this->resFilesProxyModel = new QSortFilterProxyModel(this);
    this->resFilesProxyModel->setSourceModel(this->resFilesModel);

    HashFile::Iterator it = this->hashFile.begin();
    int row = 0;
    while(it != this->hashFile.end()) {
        this->resFilesModel->setData(this->resFilesModel->index(row, 0), it.key());
        this->resFilesModel->setData(this->resFilesModel->index(row,1), it.value());
        row++;
        it++;
    }
    ui->resFileTableView->setModel(this->resFilesProxyModel);
}

void MainWindow::initHashFile(QString path)
{
    QDir dir(path);
    dir.setFilter(QDir::Files);
    QDirIterator it(dir, QDirIterator::Subdirectories);

    QString baseName;
    QString filePath;
    while(it.hasNext()) {
        it.next();
        QFileInfo fileInfo = it.fileInfo();
        filePath = fileInfo.canonicalFilePath();
        filePath = filePath.right(filePath.length() - path.length() - 1);

        baseName = fileInfo.fileName();
        int index = baseName.lastIndexOf(".");
        if (index != -1) {
            baseName = baseName.replace(index, 1, "_");
        }
        hashFile[baseName.toUpper()] = filePath;
        qDebug()<<"123456";
    }
    initModelWithHashFile();
}

bool MainWindow::pathIsValid(QString path){
    QFileSystemModel *fileSystemModel = new QFileSystemModel(this);
    QModelIndex rootIndex = fileSystemModel->index(QDir::cleanPath(path));
    if (!rootIndex.isValid() || path.isEmpty())
    {
        return false;
    }
    return true;
}

void MainWindow::messageBoxShow(QString msg)
{
    QMessageBox::warning(this,"提示",msg,QMessageBox::Yes);
}

bool MainWindow::openRectLyDirAndSave(){
    QSettings OSsetting("aoyou", "myWorkConfig");
    QString recentlyWorkPath = OSsetting.value("recentlyWorkPath").toString();

    QString openPath = recentlyWorkPath;
    if (!this->pathIsValid(recentlyWorkPath)) {
        openPath = ".";
    }

    QString lastOpenPath = QFileDialog::getExistingDirectory(this, tr("打开资源文件夹"), openPath, QFileDialog::ShowDirsOnly);
    if(!pathIsValid(lastOpenPath)){
        this->messageBoxShow("无效的路径");
        return false;
    }
    rootPath = lastOpenPath;
    if(rootPath[rootPath.length() - 1 ] != '/'){
        rootPath = rootPath + "\/";
    }
    OSsetting.setValue("recentlyWorkPath", rootPath);
    //清除所有数据
    this->clearAllData();
    //初始化所有的资源
    initHashFile(rootPath);
    qDebug()<< "当前工作路径 ＝＝"<<rootPath;
    return true;
}
//打开文件夹
void MainWindow::openFile() {
    if(openRectLyDirAndSave()){
        opFileAndShowList(rootPath);
    }
}

void MainWindow::on_pushButton_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("In put a group name"),
                                         tr("Name"),
                                         QLineEdit::Normal,
                                         "test",
                                         &ok
                                         );
    if (!ok || text.isEmpty()) {
        this->messageBoxShow("无操作");
        return;
    }

    QStringList list = this->groupListModel->stringList();
    if (list.indexOf(text) != -1) {
        return;
    }

    this->groupListModel->insertRows(0,1);
    this->groupListModel->setData(this->groupListModel->index(0), text);
    groupHashFile[text] = QSetString();
    this->showGroupByCurrIndex();
    return ;
}

void MainWindow::showGroupByCurrIndex()
{
    QString groupName = ui->groupListView->currentIndex().data().toString();
    if(groupName.isEmpty()){
        qDebug()<<"当前没有选中任何一个, 清理数据";
        return;
    }
    QSetString& groupList = groupHashFile[groupName];
    this->groupFilesModel->removeRows(0, this->groupFilesModel->rowCount());

    foreach (QString key, groupList) {
        QString path = this->hashFile.value(key, "---");
        this->groupFilesModel->insertRow(0);
        this->groupFilesModel->setData(this->groupFilesModel->index(0, 0), key);
        this->groupFilesModel->setData(this->groupFilesModel->index(0, 1), path);
    }
    ui->groupFileTableView->setGroupName(groupName);
}

void MainWindow::on_groupListView_clicked(const QModelIndex &index)
{
    showGroupByCurrIndex();
}

void MainWindow::onDropFinished(QHash<QString , QString> &keyPaths )
{
    QString groupName = ui->groupListView->currentIndex().data().toString();
    QHash<QString , QString>::Iterator it = keyPaths.begin();
    while(it != keyPaths.end()){
        this->groupFilesModel->insertRow(0);
        this->groupFilesModel->setData(this->groupFilesModel->index(0, 0), it.key());
        this->groupFilesModel->setData(this->groupFilesModel->index(0, 1), it.value());
        //插入到本地文件中去
        groupHashFile[groupName] << it.key();
        it++;
    }
    //执行groupName touched
    qDebug() <<"-keys ="<<keyPaths.keys() << keyPaths.values();
    qDebug() <<"groupHashFile[groupName] ="<<groupHashFile[groupName].values();
    showGroupByCurrIndex();
    return;
}
void MainWindow::clearAllData(){
    qDebug()<< "清除所有的数据111";
    hashFile.clear();
    qDebug()<< "清除所有的数据222";
    groupHashFile.clear();
    qDebug()<< "清除所有的数据333";
    if(this->resFilesModel){
        qDebug()<< "清除所有的数据333....5555";
        resFilesModel->removeRows(0, resFilesModel->rowCount());
    }
    qDebug()<< "清除所有的数据444";
    if(this->groupListModel){
        groupListModel->removeRows(0, groupListModel->rowCount());

    }
    qDebug()<< "清除所有的数据555";
    if(this->groupFilesModel){
        groupFilesModel->removeRows(0, groupFilesModel->rowCount());
    }
    qDebug()<< "清除所有的数据666";
}
//读取json文件 加载数据
void MainWindow::analysisJsonObject(QJsonObject jobj)
{
    QJsonObject::iterator it = jobj.begin();
    while(it != jobj.end())
    {
        switch (it.value().type())
        {
        case QJsonValue::String:
            qDebug()<<"type is String!";
            qDebug()<<it.key()<<"="<<it.value().toString();
            break;
        case QJsonValue::Array:
        {
            // it's our fenzu
            qDebug()<<"type is array!";
            qDebug()<<it.key()<<"="<<it.value().toArray();
            QJsonArray subarray = it.value().toArray();
            QSetString listModel ;
            for(int i =0; i<subarray.size(); i++){
                listModel << subarray[i].toString();
            }
            groupHashFile[it.key()] = listModel;
            break;
        }
        case QJsonValue::Bool:
            qDebug()<<"type is bool!";
            qDebug()<<it.key()<<"="<<it.value().toBool();
            break;
        case QJsonValue::Double:
            qDebug()<<"type is Double!";
            qDebug()<<it.key()<<"="<<it.value().toDouble();
            break;
        case QJsonValue::Object:
            qDebug()<<"type is Object! 此处不需要添加文件中的数据路径";
            analysisJsonObject(it.value().toObject());
            //allResstr_str[it.value().toObject().keys().at(i)] = it.value().toObject()[it.value().toObject().keys().at(i)].toString();
            break;
        case QJsonValue::Null:
            qDebug()<<"type is null!";
            qDebug()<<it.key()<<"=null";
            break;
        case QJsonValue::Undefined:
            qDebug()<<"type is Undefined!";
            break;
        }
        it++;
    }
}

void MainWindow::opFileAndShowList(QString path){
    //    qDebug()<< "path.left(path.lastIndexOf('/') = " << path.left(path.lastIndexOf('/'));
    path = path.left(path.lastIndexOf('/'));
    path = path.left(path.lastIndexOf('/'));
    path = path + "\/Resources.json";
    qDebug()<<"try openFile = " <<path;
    QFileInfo fi(path);
    if(!fi.isFile()){
        this->messageBoxShow("当前不存在资源文件\n保存将创建新的Resources.json");
        return;
    }

    QFile *file = new QFile(path);
    QTextStream stream(file);
    QString line;

    if(!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        this->messageBoxShow("不能打开--文件损坏");
        return;
    }
    QString jsonStr = "";
    while (stream.readLineInto(&line)) {
        jsonStr = jsonStr + line.trimmed();
    }
    file->close();
    qDebug()<<"------------------test json!----------------";
    QJsonParseError jsonErr;
    QJsonDocument myjsonDoc1 = QJsonDocument::fromJson(jsonStr.toLatin1(),&jsonErr);
    if (jsonErr.error == QJsonParseError::NoError)
    {
        if (myjsonDoc1.isEmpty())
        {
            qDebug()<<"the json is empty!";
        }
        if (myjsonDoc1.isObject())
        {
            qDebug()<<"json is object";
            QJsonObject jobj = myjsonDoc1.object();
            analysisJsonObject(jobj);
        }
    }

    this->showGroupListView();
}

//显示GroupList中的数据
void MainWindow::showGroupListView(){
    //todo 清理数据
    GroupHashFile::Iterator it = groupHashFile.begin();
    while(it != groupHashFile.end()){
        this->groupListModel->insertRows(0,1);
        this->groupListModel->setData(this->groupListModel->index(0), it.key());
        it++;
    }
}
//筛选
void MainWindow::on_filterLineEdit_textChanged(const QString &arg1)
{

    resFilesProxyModel->setFilterKeyColumn(0);
    resFilesProxyModel->setFilterFixedString(ui->filterLineEdit->text().toUpper());
}
//save all data
void MainWindow::saveRes(){
    //    hashFile 数据存储
    HashFile::Iterator it = hashFile.begin();
    QString hashfilew = "\"hashFile\":{";
    int k=0;
    while(it != hashFile.end()){
        hashfilew = k>0 ? hashfilew + "," : hashfilew;
        hashfilew = hashfilew +  "\n    " + "\"" + it.key() + "\"" + ":" +"\"" + it.value() + "\"";
        it ++ ;
        k ++;
    }
    hashfilew = hashfilew +"\n }";
    qDebug()<<hashfilew;
    //所有的分组资源
    QString groupHashFilew = "\"groups\":{";
    GroupHashFile::Iterator iterator = groupHashFile.begin();
    int j=0;
    while (iterator != groupHashFile.end()) {
        groupHashFilew = j>0 ? groupHashFilew + ",\n" : groupHashFilew + "\n";
        groupHashFilew = groupHashFilew + "    \"" + iterator.key() + "\":[";
        QList<QString> list = groupHashFile[iterator.key()].values();
        for(int i=0; i<list.length(); i++){
            groupHashFilew = i>0 ? groupHashFilew + "," : groupHashFilew;
            groupHashFilew = groupHashFilew + "\n        \"" + list.at(i) + "\"";
            QString path = this->hashFile.value(list.at(i), "---");
            if(path == "---"){
                QMessageBox::warning(this,"错误","找不到组名：" + iterator.key() + "\n资源：" + list.at(i) + "的路径\n 请更正" ,QMessageBox::Yes);
                return;
            }
        }
        groupHashFilew = groupHashFilew + "\n     ]";
        iterator ++;
        j ++;
    }
    groupHashFilew = groupHashFilew + "\n   }\n";

    QSettings OSsetting("aoyou", "myWorkConfig");
    QString resOutPaht = OSsetting.value("resOutPaht").toString();

    if(resOutPaht.isEmpty()){
        resOutPaht = rootPath;
    }
    QString writeStr = "{\n" + hashfilew + ",\n\n" + groupHashFilew + "\n}";
    resOutPaht = QFileDialog::getExistingDirectory(this, tr("选择保存路径"), resOutPaht, QFileDialog::ShowDirsOnly);

    //当前目录一样的要保存

    QString root = rootPath.left(rootPath.lastIndexOf('/'));
    root = root.left(root.lastIndexOf('/'));
    root = root + "\/Resources.json";

    if(resOutPaht.isEmpty() || !pathIsValid(resOutPaht)){
        saveStringToFile(writeStr, root);
        this->messageBoxShow("无效的路径--已经保存到：\n" + root);
        return;
    }

    saveStringToFile(writeStr, root);

    OSsetting.setValue("resOutPaht", resOutPaht);
    QString outFile = resOutPaht + "\/Resources.json";
    if(outFile != root){
        saveStringToFile(writeStr, outFile);
    }
}

void MainWindow::saveStringToFile(QString writeStr, QString writePath)
{
    qDebug()<< "输出配置路径" << writePath;
    // 判断文件是否存在 不存在则创建
    QFile outData(writePath);
    QFileInfo fi(writePath);
    if(!fi.isFile()){
        //创建该文件 开关一次
        outData.open(QIODevice::WriteOnly);
        outData.close();
    }

    if (outData.open(QFile::ReadWrite | QFile::Truncate)) {
        QTextStream out(&outData);
        out<< writeStr;
        outData.close();
        this->messageBoxShow("保存成功");
        return;
    }
    this->messageBoxShow("保存失败");
}
// saveBtn
void MainWindow::on_pushButton_2_clicked()
{
    saveRes();
}

void MainWindow::on_groupListView_doubleClicked(const QModelIndex &index)
{

    QMessageBox msgBox;
    msgBox.setText("删除提示");
    msgBox.setInformativeText("是否删除选中的分组");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        qDebug() << " 确定删除";
        break;
    default:
        qDebug() << " 不删除";
        return;
        break;
    }
    // 删除modellist中的数据 current 是当前选中的数据
    groupHashFile.remove(ui->groupListView->currentIndex().data().toString());
    qDebug()<< "groupHashFile = "<< groupHashFile;
    this->groupListModel->removeRow(ui->groupListView->currentIndex().row());
    qDebug()<< "this->groupListModel ="<< this->groupListModel;
}

//右键菜单
void MainWindow::on_groupListView_customContextMenuRequested(const QPoint &pos)
{
    qDebug() << "右键响应";
    cmenu = new QMenu(this);
    qDebug()<<"QCursor::pos()" << QCursor::pos();
    cmenu->exec(QCursor::pos());//QCursor::pos()
    QAction *ascendSortAction = cmenu->addAction("升序");
}

void MainWindow::on_groupFileTableView_doubleClicked(const QModelIndex &index)
{
    QMessageBox msgBox;
    msgBox.setText("删除提示");
    msgBox.setInformativeText("是否删除选中的分组");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        qDebug() << " 确定删除";
        break;
    default:
        qDebug() << " 不删除";
        return;
        break;
    }

    //删除
    int row = ui->groupFileTableView->currentIndex().row();
    QString needMove = this->groupFilesModel->index(row, 0).data().toString();
    groupHashFile[ui->groupListView->currentIndex().data().toString()].remove(needMove);
    this->groupFilesModel->removeRow(ui->groupFileTableView->currentIndex().row());

}
//改名 copy
void MainWindow::on_pushButton_3_clicked()
{
    if(ui->groupListView->currentIndex().data().toString().isEmpty()){
        qDebug()<<"选中行为空";
        return;
    }
    QString beforeStr = ui->groupListView->currentIndex().data().toString();
    //确定改名字
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("改名"),
                                         tr("改变之后的名字"),
                                         QLineEdit::Normal,
                                         beforeStr,
                                         &ok
                                         );
    if (!ok || text.isEmpty() || beforeStr == text) {
        qDebug()<<"不修改";
        return;
    }

    QStringList allGroupList = this->groupListModel->stringList();
    if (allGroupList.indexOf(text) != -1) {
        qDebug()<<"已经有这个名字了，要重新取";
        this->on_pushButton_3_clicked();
        return;
    }

    QList<QString> need = groupHashFile[beforeStr].values();
    QSetString needResAdd ;
    foreach (QString str, need) {
        needResAdd<<str;
    }
    //删除
    groupHashFile.remove(beforeStr);
    this->groupListModel->removeRow(ui->groupListView->currentIndex().row());
    // add
    this->groupListModel->insertRows(0,1);
    this->groupListModel->setData(this->groupListModel->index(0), text);
    groupHashFile[text] = needResAdd;
    showGroupByCurrIndex();
}
