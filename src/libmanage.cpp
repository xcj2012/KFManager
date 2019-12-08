#include "libmanage.h"
#include "ui_libmanage.h"
LibManage::LibManage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LibManage)
{
    ui->setupUi(this);
    m_model =new QStandardItemModel;
    ui->treeView->setModel(m_model);
    ui->treeView->header()->hide();
    init_connect();
    QDir dir;
    dir.cdUp();
    if(dir.cd("Templates")==false)
        dir.mkdir("Templates");
    dir.cd("Templates");
    Lib_path =dir.path();
    ui->pushButton_Update->hide();
    ui->pushButton_Edit->hide();
    if((g_User_permissions==USER_ENGINEER)||(g_User_permissions==USER_DEVELOPER))
    {
        ui->pushButton_Edit->show();
    }
}

LibManage::~LibManage()
{
    delete m_model;
    delete ui;
}


void LibManage::init_library(QString name)
{
}
void LibManage::init_connect()
{
    connect(ui->pushButton_Update,SIGNAL(clicked()),this,SLOT(on_pushbutton_Update_clicked()));
    connect(ui->pushButton_Delete,SIGNAL(clicked()),this,SLOT(on_pushbutton_Delete_clicked()));
    connect(ui->pushButton_Archive,SIGNAL(clicked()),this,SLOT(on_pushbutton_Archive_clicked()));
    connect(ui->pushButton_Retrieve,SIGNAL(clicked()),this,SLOT(on_pushbutton_Retrieve_clicked()));
    connect(ui->pushButton_Cancel,SIGNAL(clicked()),this,SLOT(on_pushbutton_Cancel_clicked()));
    connect(ui->pushButton_Help,SIGNAL(clicked()),this,SLOT(on_pushbutton_Help_clicked()));
    connect(ui->pushButton_Edit,SIGNAL(clicked()),this,SLOT(on_pushbutton_Edit_clicked()));
    connect(ui->pushButton_Ok,SIGNAL(clicked()),this,SLOT(on_pushbutton_Ok_clicked()));

}
void LibManage::setupwidget(int type)
{
    setWindowTitle(tr("library manage"));
    ui->pushButton_Help->setProperty("name","Library Manage");
    if(type==LIB_SHOW)
    {
        ui->pushButton_Delete->hide();
        ui->pushButton_Update->hide();
        ui->pushButton_Archive->hide();
        ui->pushButton_Retrieve->hide();
        ui->treeView->setDragEnabled(true);
        ui->treeView->setDragDropMode(QAbstractItemView::DragOnly);
    }else if(type==LIB_MANAGE)
    {
        ui->pushButton_Ok->hide();
    }
    List_library();
}
void LibManage::List_library()
{
    m_model->setRowCount(0);
    QFileInfoList devices =get_Devices(Lib_path);
    foreach(QFileInfo dev, devices)
    {
        QStandardItem *dev_item =new QStandardItem;
        dev_item->setText(dev.fileName());
        dev_item->setData(dev.absoluteFilePath(),Qt::UserRole+LIB_PATH);
        dev_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        dev_item->setData(LIB_DEV,Qt::UserRole);
        m_model->appendRow(dev_item);
        QFileInfoList versions =get_versions(Lib_path+QDir::separator()+dev.fileName());
        if(versions.size()==0)
            continue;
        foreach(QFileInfo ver, versions)
        {
            QString version;
            bool ret =Check_Lib_Hash(ver.absoluteFilePath(),version);
            if(ret==true)
            {
                QStandardItem *ver_item =new QStandardItem;
                ver_item->setText(version);
                ver_item->setData(ver.absoluteFilePath(),Qt::UserRole+LIB_PATH);
                ver_item->setData(LIB_VER,Qt::UserRole);
                ver_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                dev_item->appendRow(ver_item);
            }else
            {
                ui->label->setText(tr("The hash check of version %1 under device %1 is not correct").arg(ver.fileName(),dev.fileName()));
            }
        }
    }
    ui->treeView->setCurrentIndex(m_model->index(0,0));
}
QFileInfoList LibManage::get_Devices(QString name)
{
    QDir dir(name);
    dir.setFilter(QDir::Dirs | QDir::NoSymLinks| QDir::NoDotAndDotDot);
    QFileInfoList list=dir.entryInfoList();
    return list;
}
QFileInfoList LibManage::get_versions(QString name)
{
    QDir dir(name);
    dir.setFilter(QDir::Dirs | QDir::NoSymLinks| QDir::NoDotAndDotDot);
    QFileInfoList list=dir.entryInfoList();
    return list;
}
bool LibManage::Check_Lib_Hash(QString name,QString &ver)
{
    QString filename =name+QDir::separator()+"hash.xml";
    xml_document m_doc;
    if(m_doc.load_file(filename.toStdString().data()).status!=pugi::status_ok)
        return false;
    QString xpath ="/SCL/File/item";
    pugi::xpath_node_set xnodes=m_doc.select_nodes(xpath.toStdString().data());
    if(xnodes.size()==0)
        return false;
    pugi::xml_node item_node;
    for(pugi::xpath_node_set::const_iterator it = xnodes.begin();it != xnodes.end();it ++)
    {
        item_node=it->node();
        pugi::xml_node name_node =item_node.child("name");
        pugi::xml_node hash_node =item_node.child("MD5");
        QByteArray hash =g_get_MD5(name+QDir::separator()+name_node.child_value());
        if(QString::fromStdString(hash_node.child_value()).compare(tr(hash.toHex()),Qt::CaseInsensitive)!=0)
            return false;
    }
    xpath ="/SCL";
    pugi::xpath_node xnode=m_doc.select_node(xpath.toStdString().data());
    ver =QString::fromStdString(xnode.node().attribute("version").as_string())+"_"+QString::fromStdString(xnode.node().attribute("Database").as_string());
    return true;
}
void LibManage::on_pushbutton_Delete_clicked()
{
    ui->label->clear();
    QModelIndex index=ui->treeView->currentIndex();
    if(index.isValid()!=true)
        return;
    QString path =index.model()->data(index,Qt::UserRole+LIB_PATH).toString();
    g_delete_dir(path);
    List_library();
}
void LibManage::on_pushbutton_Archive_clicked()
{
    ui->label->clear();
    QModelIndex index=ui->treeView->currentIndex();
    if(index.isValid()!=true)
    {
        ui->label->setText(tr("Select one version under device or device at first"));
        return;
    }
    QString path =index.model()->data(index,Qt::UserRole+LIB_PATH).toString();
    int mode =index.model()->data(index,Qt::UserRole).toInt();
    if(mode!=LIB_VER)
    {
        ui->label->setText(tr("Select one version under device or device at first"));
        return;
    }
    Zip_process *m_dlg= new Zip_process(this);
    m_dlg->setsource(path);
    m_dlg->setmode(MODE_LIB);
    m_dlg->setmethod(LIB_MANAGE);
    m_dlg->settype(ZIP_ARCHIVE);
    m_dlg->exec();
    //List_library();
}
void LibManage::on_pushbutton_Retrieve_clicked()
{
    //    ui->label->clear();
    //    QModelIndex index=ui->treeView->currentIndex();
    //    if(index.isValid()!=true)
    //    {
    //        ui->label->setText(tr("Select one version under device or device at first"));
    //        return;
    //    }
    //    QString path;
    //    int type =index.model()->data(index,Qt::UserRole).toInt();
    //    if(type==LIB_DEV)
    //    {
    //        path =index.model()->data(index,Qt::UserRole+LIB_PATH).toString();

    //    }else
    //    {
    //        path =index.parent().model()->data(index.parent(),Qt::UserRole+LIB_PATH).toString();
    //    }
    Zip_process *m_dlg= new Zip_process(this);
    m_dlg->setdest(Lib_path);
    m_dlg->setmode( MODE_LIB);
    m_dlg->setmethod(LIB_MANAGE);
    m_dlg->settype(ZIP_RETRIEVE);
    m_dlg->exec();
    List_library();
}
void LibManage::on_pushbutton_Cancel_clicked()
{
    ui->label->clear();
    reject();
}
void LibManage::on_pushbutton_Ok_clicked()
{
    QModelIndex index=ui->treeView->currentIndex();
    QModelIndex child_index =index.child(0,0);
    if(child_index.isValid()==true)
    {
        ui->label->setText(tr("Select one version under device"));
        return;
    }
    device =index.parent().data().toString();
    ver =index.data().toString();
    Lib_path=index.model()->data(index,Qt::UserRole+LIB_PATH).toString();
    accept();
}
void LibManage::on_pushbutton_Edit_clicked()
{
    ui->label->clear();
    QModelIndex index=ui->treeView->currentIndex();
    int type =index.model()->data(index,Qt::UserRole).toInt();
    if(type==LIB_DEV)
        return;
    QString path =index.model()->data(index,Qt::UserRole+LIB_PATH).toString();
    QProcess *m_process=new QProcess(this);
    m_process->setProgram("./Kflogic.exe");
    QStringList args;
    args<<"-p "+path+QDir::separator()+"Logic"<<"-l"+path+QDir::separator()+"Datebase";
    m_process->setArguments(args);
    m_process->start();
    m_process->waitForStarted();
    m_process->waitForFinished();
    QString strTemp=QString::fromLocal8Bit(m_process->readAllStandardOutput());
    QString strerror=QString::fromLocal8Bit(m_process->readAllStandardError());
}

void LibManage::on_pushbutton_Help_clicked()
{
    ui->label->clear();
    emit emit_help_message(ui->pushButton_Help->property("name").toString());
}
