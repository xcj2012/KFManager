#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "projectmanager.h"
#include<QDockWidget>
#include <QMap>
#include"global.h"
#include"new_project.h"
#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include "manage_project.h"
#include "libmanage.h"
#include <QDateTime>
#include <QTimer>
#include "packagetool.h"
class QWidget;
class QCloseEvent;
class DockWidget : public QDockWidget
{
    Q_OBJECT

public:
     explicit DockWidget(QString title,QWidget *parent = nullptr);

signals:
    void dockwidget_close();
protected:
     void closeEvent(QCloseEvent *event);
};
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SetupWidget();
    void Add_ProjectWidget(QString name,QString path);
    void hide_projectwidget();
    void show_projectwidget(QString name);
    void init_menu();
    void init_menu_folder();
    void init_menu_device();
    void init_connection();
    bool create_project(QString name,QString path);
    void update_regedit(QString name,QString path);
    bool open_project(QString name,QString path);
    pugi::xml_document doc;
    DockWidget *pProjectDock;
    QDockWidget *pLibrarytDock;
    QMap<QString,DockWidget*> Docks;
    QMap<QString,ProjectManager*> widgets;
    ProjectManager *pProjectWiget;
    QAction *action_window;
    QString widget_key;
private slots:
    void on_action_switchWindow();
    void on_action_New_clicked();
    void on_action_Open_clicked();
    void on_action_Del_clicked();
    void on_action_Manage_clicked();
    void on_action_Exit_clicked();
    void on_action_Archive_clicked();
    void on_action_Retrieve_clicked();
    void on_action_Project_close();
    void on_action_libManage_clicked();
    void on_action_cut_clicked();
    void on_action_copy_clicked();
    void on_action_paste_clicked();
    void on_action_del_clicked();
    void on_action_property_clicked();
    void on_action_connect_clicked();
    void on_action_import_clicked();
    void on_action_export_clicked();
    void on_action_update_clicked();
    void on_action_folder_clicked();
    void on_action_device_clicked();
    void on_action_Package();
    void on_action_close();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
