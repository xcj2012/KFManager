#ifndef MANAGE_PROJECT_H
#define MANAGE_PROJECT_H

#include <QDialog>
#include "global.h"
#include "zip_process.h"
struct PRo_struct
{
    QString name;
    QString path;
};
namespace Ui {
class manage_project;
}

class manage_project : public QDialog
{
    Q_OBJECT

public:
    explicit manage_project(QWidget *parent = nullptr);
    ~manage_project();
    void setupwidget_open();
    void setupwidget_manage();
    void setupwidget();
    void list_projects();
    void delete_project();
    void update_list();
    void add_oneProject(QString filename);
    QList<PRo_struct> project_list;
    QDir dir;
    QString name;
    QString path;
    QStandardItemModel *m_model;
signals:
    void emit_help_message(QString);
private slots:
    void on_pushbutton_Open_clicked();
    void on_pushbutton_Delete_clicked();
    void on_pushbutton_Browse_clicked();
    void on_pushbutton_Help_clicked();
    void on_pushbutton_Cancel_clicked();
    void on_pushbutton_Archive_clicked();
    void on_pushbutton_Retrieve_clicked();
private:
    Ui::manage_project *ui;
};

#endif // MANAGE_PROJECT_H
