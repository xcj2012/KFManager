#include "new_project.h"
#include "ui_new_project.h"

New_Project::New_Project(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::New_Project)
{
    ui->setupUi(this);
    setWindowTitle(tr("New Project"));
    m_model=new  QStandardItemModel();
    ui->tableView->setModel(m_model);
    setupwidget();
    connect(ui->pushButton_Ok,SIGNAL(clicked()),this,SLOT(on_pushbutton_Ok()));
    connect(ui->pushButton_Cancel,SIGNAL(clicked()),this,SLOT(on_pushbutton_Cancel()));
    connect(ui->pushButton_Help,SIGNAL(clicked()),this,SLOT(on_pushbutton_Help()));
    connect(ui->pushButton_Browse,SIGNAL(clicked()),this,SLOT(on_pushbutton_Browse()));

}

New_Project::~New_Project()
{
    delete ui;
}
void New_Project::setupwidget()
{
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    QStringList headers;
    headers<<tr("Name")<<tr("Path");
    m_model->setColumnCount(headers.size());
    m_model->setHorizontalHeaderLabels(headers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    QDir dir;
    dir.cdUp();
    dir.cd("project");
    //QString temp=dir.path()+QDir::separator()+"Project";
    ui->lineEdit_Path->setText(dir.path());
    //QDir dir(ui->lineEdit_Path->text());
    QFileInfoList infos;
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    foreach(QFileInfo var, file_list)
    {
        if(var.baseName()==".KFpro")
        {
            infos.append(var);
        }
    }
    QSettings m_setting;
    QStringList files =m_setting.value("KFproject/Files").toStringList();
    foreach (QFileInfo var, infos)
    {
        QList<QStandardItem*>row;
        QStandardItem* item0 =new QStandardItem;
        item0->setText(var.fileName());
        item0->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        row.append(item0);
        QStandardItem* item1 =new QStandardItem;
        item1->setText(var.path());
        item1->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        row.append(item1);
        m_model->appendRow(row);
        if(files.contains(var.absoluteFilePath()))
        {
            files.removeOne(var.absoluteFilePath());
        }
    }
    for(int i=0;i<files.size();i++)
    {
        QFileInfo info(files.at(i));
        if(!info.exists())
            continue;
        QList<QStandardItem*>row;
        QStandardItem* item0 =new QStandardItem;
        item0->setText(info.baseName());
        item0->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        row.append(item0);
        QStandardItem* item1 =new QStandardItem;
        item1->setText(info.path());
        item1->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        row.append(item1);
        m_model->appendRow(row);
    }
    ui->tableView->resizeColumnToContents(0);

}
void New_Project::on_pushbutton_Ok()
{
    name =ui->lineEdit_Name->text();
    path=ui->lineEdit_Path->text();
    if(ui->lineEdit_Name->text().isEmpty())
    {
        QMessageBox::warning(this,tr("warning"),tr("The name of project can't be empty"));
        return;
    }
    if(ui->lineEdit_Path->text().isEmpty())
    {
        QMessageBox::warning(this,tr("warning"),tr("The path of project can't be empty"));
        return;
    }
    for(int i=0;i<m_model->rowCount();i++)
    {
        if(path+QDir::separator()+name==m_model->item(i,1)->text()+QDir::separator()+m_model->item(i,0)->text())
        {
            QMessageBox::warning(this,tr("warning"),tr("The project is exist, reset the name or path"));
            return;
        }
    }

    accept();
}
void New_Project::on_pushbutton_Cancel()
{
    reject();
}
void New_Project::on_pushbutton_Help()
{
    emit emit_help_message("NewProject");
}
void New_Project::on_pushbutton_Browse()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    ui->lineEdit_Path->text(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
        ui->lineEdit_Path->setText(dir);
}
