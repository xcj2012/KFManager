#include "objectproperty.h"
#include "ui_objectproperty.h"

ObjectProperty::ObjectProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectProperty)
{
    ui->setupUi(this);
    ui->lineEdit_path->setEnabled(false);
    connect(ui->pushButton_Ok,SIGNAL(clicked()),this,SLOT(on_pushbutton_Ok()));
    connect(ui->pushButton_Cancel,SIGNAL(clicked()),this,SLOT(on_pushbutton_Cancel()));
    connect(ui->pushButton_Help,SIGNAL(clicked()),this,SLOT(on_pushbutton_Help()));

}

ObjectProperty::~ObjectProperty()
{
    delete ui;
}
void ObjectProperty::setupWidget(int type)
{
    ui->lineEdi_name->setText(name);
    ui->lineEdit_path->setText(path);
    ui->lineEdit_author->setText(author);
    ui->lineEdit_create->setText(createtime);
    ui->lineEdit_modify->setText(modifytime);
    ui->textBrowser->setText(commit);
    if((type==ROOT_NODE)||(type==STATION_NODE)||(type==FOLDER_NODE))
    {
        for(int i=1;i<ui->tabWidget->tabBar()->children().size();i++)
        {
            ui->tabWidget->removeTab(i);
        }
        if(type==ROOT_NODE)
            ui->lineEdi_name->setEnabled(false);
        ui->pushButton_Help->setProperty("name","Folder Property");
    }
    if(type==DEVICE_NODE)
    {
        ui->lineEdit_Ver->setText(version);
        ui->lineEdit_Type->setText(device);
        ui->lineEdit_ID->setText(address);
        ui->lineEdit_IP->setText(ipaddress);
        ui->pushButton_Help->setProperty("name","Device Property");
    }
}
void ObjectProperty::on_pushbutton_Ok()
{

    name =ui->lineEdi_name->text();
    author=ui->lineEdit_author->text();
    commit=ui->textBrowser->document()->toPlainText();
    ipaddress =ui->lineEdit_IP->text();
    address =ui->lineEdit_ID->text();
    accept();
}
void ObjectProperty::on_pushbutton_Cancel()
{
    reject();
}
void ObjectProperty::on_pushbutton_Help()
{
    emit emit_help_message(ui->pushButton_Help->property("name").toString());
}
