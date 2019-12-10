#include "packagetool.h"
#include "ui_packagetool.h"

PackageTool::PackageTool(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PackageTool)
{
    ui->setupUi(this);
}

PackageTool::~PackageTool()
{
    delete ui;
}
void PackageTool::disable_function()
{
    ui->pushButton_HMI->setDisabled(true);
    ui->pushButton_IEC->setDisabled(true);
    ui->pushButton_MPU->setDisabled(true);
    ui->pushButton_Zip->setDisabled(true);
    ui->pushButton_Logic->setDisabled(true);
    ui->pushButton_IECDetail->setDisabled(true);
}
void PackageTool::init_devList()
{
    QString path;
    path=g_dir.path()+QDir::separator()+"config"+QDir::separator()+"Library.xml";
    xml_document m_doc;
    if(m_doc.load_file(path.toStdWString().data()).status!=pugi::status_ok)
    {
        QMessageBox::warning(this,tr("warning"),tr("The Library.xml file "));
        return;
    }
    QString xpath = "./SCL/Library";
    xpath_node_set xset =m_doc.select_nodes(xpath.toStdString().data());
}
