#include "manage_project.h"
#include "ui_manage_project.h"

manage_project::manage_project(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::manage_project)
{
    ui->setupUi(this);
    m_model= new QStandardItemModel;
    ui->tableView->setModel(m_model);
    dir.setPath(g_dir.path()+QDir::separator()+"Project");
    //dir.cdUp();
    //dir.cd("project");
    setupwidget();
    connect(ui->pushButton_Open,SIGNAL(clicked()),this,SLOT(on_pushbutton_Open_clicked()));
    connect(ui->pushButton_Delete,SIGNAL(clicked()),this,SLOT(on_pushbutton_Delete_clicked()));
    connect(ui->pushButton_Browse,SIGNAL(clicked()),this,SLOT(on_pushbutton_Browse_clicked()));
    connect(ui->pushButton_Cancel,SIGNAL(clicked()),this,SLOT(on_pushbutton_Cancel_clicked()));
    connect(ui->pushButton_Archive,SIGNAL(clicked()),this,SLOT(on_pushbutton_Archive_clicked()));
    connect(ui->pushButton_Retrieve,SIGNAL(clicked()),this,SLOT(on_pushbutton_Retrieve_clicked()));
    connect(ui->pushButton_Help,SIGNAL(clicked()),this,SLOT(on_pushbutton_Help_clicked()));

}
void manage_project::setupwidget()
{
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    QStringList headers;
    headers<<tr("Name")<<tr("Path");
    m_model->setColumnCount(headers.size());
    m_model->setHorizontalHeaderLabels(headers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    update_list();
    list_projects();

}
void manage_project::list_projects()
{
    m_model->setRowCount(0);
    for(int i=0;i<project_list.size();i++)
    {
        QList<QStandardItem*>row;
        QStandardItem* item0 =new QStandardItem;
        item0->setText(project_list.at(i).name);
        item0->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        row.append(item0);
        QStandardItem* item1 =new QStandardItem;
        item1->setText(project_list.at(i).path);
        item1->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        row.append(item1);
        m_model->appendRow(row);
    }
//    QDir dir;
//    dir.cdUp();
//    dir.cd("project");
//    //    QString temp=dir.path()+QDir::separator()+"Project";
//    //    QDir dir(temp);
//    QFileInfoList infos;
//    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
//    foreach(QFileInfo var, file_list)
//    {
//        if(var.baseName()==".KFpro")
//        {
//            infos.append(var);
//        }
//    }
//    QSettings m_setting;
//    QStringList files =m_setting.value("KFproject/Files").toStringList();
//    foreach (QFileInfo var, infos)
//    {
//        QList<QStandardItem*>row;
//        QStandardItem* item0 =new QStandardItem;
//        item0->setText(var.fileName());
//        item0->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//        row.append(item0);
//        QStandardItem* item1 =new QStandardItem;
//        item1->setText(var.path());
//        item1->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//        row.append(item1);
//        m_model->appendRow(row);
//        if(files.contains(var.absoluteFilePath()))
//        {
//            files.removeOne(var.absoluteFilePath());
//        }
//    }
//    for(int i=0;i<files.size();i++)
//    {
//        QFileInfo info(files.at(i));
//        if(!info.exists())
//            continue;
//        QList<QStandardItem*>row;
//        QStandardItem* item0 =new QStandardItem;
//        item0->setText(info.baseName());
//        item0->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//        row.append(item0);
//        QStandardItem* item1 =new QStandardItem;
//        item1->setText(info.path());
//        item1->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//        row.append(item1);
//        m_model->appendRow(row);
//    }
    ui->tableView->resizeColumnToContents(0);
    ui->tableView->setCurrentIndex(m_model->index(0,0));
}
void manage_project::setupwidget_open()
{
    ui->pushButton_Browse->hide();
    ui->pushButton_Delete->hide();
    ui->pushButton_Archive->hide();
    ui->pushButton_Retrieve->hide();
    ui->pushButton_Help->setProperty("name","Open Project");
    setWindowTitle(tr("Open Project"));

}
void manage_project::setupwidget_manage()
{
    ui->pushButton_Open->hide();
    ui->pushButton_Help->setProperty("name","Manage Project");
    setWindowTitle(tr("Manage Project"));

}
manage_project::~manage_project()
{
    delete ui;
}
void manage_project::on_pushbutton_Open_clicked()
{
//    QModelIndexList list= ui->tableView->selectionModel()->selectedRows();
//    if(list.size()==0)
//        return;
    QModelIndex index =ui->tableView->currentIndex();
    if(!index.isValid())
        return;
    name = m_model->item(index.row(),0)->text();
    path = m_model->item(index.row(),1)->text();
    if(!g_check_project(name,path))
    {
        int ret =QMessageBox::information(this,tr("warning"),tr("The project:")+name+tr("is illegal project\n")+tr("Do you want to remove this information")
                                          ,QMessageBox::Yes|QMessageBox::No);
        if(ret==QMessageBox::Yes)
            g_delete_Project(name,path);
        return;
    }
    accept();
}
void manage_project::add_oneProject(QString filename)
{
    QFileInfo info(filename);
    for (int i=0;i<m_model->rowCount();i++)
    {
        QString tmp =m_model->item(i,1)->text()+QDir::separator()+m_model->item(i,0)->text()+".KFpro";
        if(tmp==filename)
        {
            QMessageBox::warning(this,tr("Notice"),tr("The project is already exist"));
            return;
        }
    }
    QSettings m_setting;
    QStringList files =m_setting.value("KFproject/Files").toStringList();
    if(!files.contains(filename))
    {
        if(!g_check_project(info.baseName(),info.filePath()))
        {
            QMessageBox::warning(this,tr("warning"),tr("The project:")+info.baseName()+tr("is illegal project"));
            return;
        }
        files.append(filename);
        m_setting.setValue("KFproject/Files",files);
    }
}
void manage_project::update_list()
{
    project_list.clear();
    QSettings m_setting;
    QStringList files =m_setting.value("KFproject/Files").toStringList();
    QFileInfoList infos;
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    foreach (QFileInfo var, infos)
    {
        PRo_struct m_dat;
        m_dat.name =var.baseName();
        m_dat.path =var.absolutePath();
        project_list.append(m_dat);
        if(files.contains(var.path()))
        {
            files.removeOne(var.path());
        }
    }
    for(int i=0;i<files.size();i++)
    {
        QFileInfo info(files.at(i));
        if(!info.exists())
            continue;
        PRo_struct m_dat;
        m_dat.name =info.baseName();
        m_dat.path =info.absolutePath();
        project_list.append(m_dat);
    }
}
void manage_project::delete_project()
{
    QModelIndex index =ui->tableView->currentIndex();
    name = m_model->item(index.row(),0)->text();
    path = m_model->item(index.row(),1)->text();
    g_delete_Project(name,path);
    QSettings m_setting;
    QStringList files =m_setting.value("KFproject/Files").toStringList();
    files.removeOne(path+QDir::separator()+name+".KFpro");
    m_setting.setValue("KFproject/Files",files);
}
void manage_project::on_pushbutton_Delete_clicked()
{
    QModelIndex index =ui->tableView->currentIndex();
    if(!index.isValid())
        return;
    name = m_model->item(index.row(),0)->text();
    path = m_model->item(index.row(),1)->text();
    int ret =QMessageBox::information(this,tr("Question"),tr("Do you want to delete the project from list and disk?")+"\n"+tr("Project:")+name,QMessageBox::Yes|QMessageBox::No);
    if(ret==QMessageBox::Yes)
        delete_project();

    update_list();
    list_projects();
}
void manage_project::on_pushbutton_Browse_clicked()
{
//    QDir dir;
//    dir.cdUp();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    dir.path(),
                                                    tr("project (*.KFpro)"));
    if(!fileName.isEmpty())
    {
        add_oneProject(fileName);

        update_list();
        list_projects();
    }

}
void manage_project::on_pushbutton_Help_clicked()
{
    emit emit_help_message(ui->pushButton_Help->property("name").toString());
}
void manage_project::on_pushbutton_Cancel_clicked()
{
    reject();
}
void manage_project::on_pushbutton_Archive_clicked()
{
    QModelIndex index =ui->tableView->currentIndex();
    if(!index.isValid())
        return;
    path = m_model->item(index.row(),1)->text();
    name = m_model->item(index.row(),0)->text();
    Zip_process *m_dlg= new Zip_process(this);
    m_dlg->setsource(path+QDir::separator()+name);
    m_dlg->setmode(PRO_PRO);
    m_dlg->setmethod(PRO_MANAGE);
    m_dlg->settype(ZIP_ARCHIVE);
    m_dlg->exec();
}
void manage_project::on_pushbutton_Retrieve_clicked()
{
    Zip_process *m_dlg= new Zip_process(this);
    m_dlg->setdest(dir.path());
    m_dlg->setmode(PRO_PRO);
    m_dlg->setmethod(PRO_MANAGE);
    m_dlg->settype(ZIP_RETRIEVE);
    m_dlg->exec();
}
