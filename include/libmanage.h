#ifndef LIBMANAGE_H
#define LIBMANAGE_H

#include <QDialog>
#include "global.h"
#include "zip_process.h"
using namespace pugi;
#define LIB_PATH 0x01
#define LIB_DEPH 0x02

namespace Ui {
class LibManage;
}

class LibManage : public QDialog
{
    Q_OBJECT

public:
    explicit LibManage(QWidget *parent = nullptr);
    ~LibManage();
    void setupwidget(int type);
    void List_library();
    QFileInfoList get_Devices(QString name);
    QFileInfoList get_versions(QString name);
    bool Check_Lib_Hash(QString name,QString &ver);
    void init_library(QString name);
    void init_connect();
    QStandardItemModel *m_model;
    QString Lib_path;
    QString device;
    QString ver;
signals:
    void emit_help_message(QString);
private slots:
    void on_pushbutton_Delete_clicked();
    void on_pushbutton_Archive_clicked();
    void on_pushbutton_Retrieve_clicked();
    void on_pushbutton_Cancel_clicked();
    void on_pushbutton_Help_clicked();
    void on_pushbutton_Edit_clicked();
    void on_pushbutton_Ok_clicked();
private:
    Ui::LibManage *ui;
};

#endif // LIBMANAGE_H
