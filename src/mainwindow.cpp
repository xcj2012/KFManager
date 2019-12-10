#include "mainwindow.h"
#include "ui_mainwindow.h"
DockWidget::DockWidget(QString title,QWidget *parent)
    : QDockWidget(title,parent)
{
    setWindowTitle(title);
}
void DockWidget::closeEvent(QCloseEvent *event)
{
    emit dockwidget_close();
    QWidget::closeEvent(event);
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->hide();
    widgets.clear();
    Docks.clear();
    pProjectWiget=NULL;
    pProjectDock=NULL;
    pLibrarytDock=NULL;
    init_menu();
    SetupWidget();
    init_connection();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow:: init_connection()
{
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(on_action_New_clicked()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(on_action_Open_clicked()));
    connect(ui->actionManage,SIGNAL(triggered()),this,SLOT(on_action_Manage_clicked()));
    connect(ui->actionLib_Manager,SIGNAL(triggered()),this,SLOT(on_action_libManage_clicked()));
    connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(on_action_cut_clicked()));
    connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(on_action_copy_clicked()));
    connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(on_action_paste_clicked()));
    connect(ui->actionDelete,SIGNAL(triggered()),this,SLOT(on_action_del_clicked()));
    connect(ui->actionProject_Properties,SIGNAL(triggered()),this,SLOT(on_action_property_clicked()));
    connect(ui->actionOpen_Object,SIGNAL(triggered()),this,SLOT(on_action_connect_clicked()));
    connect(ui->actionImport_Device,SIGNAL(triggered()),this,SLOT(on_action_import_clicked()));
    connect(ui->actionExport_Device,SIGNAL(triggered()),this,SLOT(on_action_export_clicked()));
    connect(ui->actionUpdate_Prameter,SIGNAL(triggered()),this,SLOT(on_action_update_clicked()));
    connect(ui->actionForder,SIGNAL(triggered()),this,SLOT(on_action_folder_clicked()));
    connect(ui->actionDevice,SIGNAL(triggered()),this,SLOT(on_action_device_clicked()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(on_action_close()));
    connect(ui->actionPackage_Tool,SIGNAL(triggered()),this,SLOT(on_action_Package()));


}
void MainWindow::init_menu()
{
    ui->menuEdit->setEnabled(false);
    ui->menuView->setEnabled(false);
    ui->menuInset->setEnabled(false);
    ui->menuDevice->setEnabled(false);
    ui->menuWindows->setEnabled(false);
}
void MainWindow::init_menu_folder()
{
    ui->menuEdit->setEnabled(true);
    ui->menuView->setEnabled(true);
    ui->menuInset->setEnabled(true);
    ui->menuWindows->setEnabled(true);
}
void MainWindow::init_menu_device()
{
    ui->menuDevice->setEnabled(true);

}
void MainWindow::SetupWidget()

{
    setWindowTitle(tr("Project manager"));
}
void MainWindow::Add_ProjectWidget(QString name,QString path)
{
    hide_projectwidget();
    pProjectDock = new DockWidget(name+"---"+path,this);
    pProjectDock->setProperty("name",path+QDir::separator()+name);
    pProjectDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    pProjectWiget = new ProjectManager(this);
    pProjectWiget->file_name =name;
    pProjectWiget->file_path =path;
    pProjectWiget->open_project();
    pProjectWiget->setProperty("name",path+QDir::separator()+name);
    pProjectDock->setWidget(pProjectWiget);
    this->addDockWidget(Qt::LeftDockWidgetArea, pProjectDock);
    pProjectDock->show();
    Docks.insert(path+QDir::separator()+name,pProjectDock);
    widgets.insert(path+QDir::separator()+name,pProjectWiget);
    widget_key =path+QDir::separator()+name;
    //    connect(pProjectDock,SIGNAL(close()),this,SLOT(on_action_Project_close()));
    connect(pProjectDock,SIGNAL(dockwidget_close()),this,SLOT(on_action_Project_close()));
    for(int i=0;i<ui->menuWindows->actions().size();i++)
    {
        QAction *action =ui->menuWindows->actions().at(i);
        action->setIcon(QIcon(""));
    }

    action_window =new QAction(name,this);
    action_window->setProperty("name",path+QDir::separator()+name);
    connect(action_window,SIGNAL(triggered()),this,SLOT(on_action_switchWindow()));
    action_window->setIcon(QIcon(":/images/pencil_edit.png"));
    ui->menuWindows->addAction(action_window);
    init_menu_folder();
}
void MainWindow::hide_projectwidget()
{
    for(int i=0;i<Docks.keys().size();i++)
    {
        Docks.value(Docks.keys().at(i))->hide();
    }
}
void MainWindow::show_projectwidget(QString name)
{

    if(Docks.keys().contains(name))
        Docks.value(name)->showMaximized();
}
void MainWindow::on_action_switchWindow()
{
    QString name =sender()->property("name").toString();
    hide_projectwidget();
    show_projectwidget(name);
    widget_key=name;
    for(int i=0;i<ui->menuWindows->actions().size();i++)
    {
        QAction *action =ui->menuWindows->actions().at(i);
        action->setIcon(QIcon(""));
        if(action->property("name").toString()==name)
        {
            action->setIcon(QIcon(":/images/pencil_edit.png"));
        }
    }
}
void MainWindow::on_action_New_clicked()
{
    New_Project m_dlg;
    if(m_dlg.exec()==QDialog::Accepted)
    {
        QString name =m_dlg.name;
        QString path =m_dlg.path;
        bool ret= create_project(name,path);
        if(ret==true)
        {
            update_regedit(name,path);
            open_project(name,path);
        }else
        {
            QMessageBox::warning(this,tr("warning"),tr("Create project:")+name+tr(" false"));
        }
    }
}
void MainWindow::on_action_Open_clicked()
{
    manage_project m_dlg;
    m_dlg.setupwidget_open();
    if(m_dlg.exec()==QDialog::Accepted)
    {
        QString name =m_dlg.name;
        QString path =m_dlg.path;
        open_project(name,path);
    }
}
void MainWindow::on_action_Del_clicked()
{

}
void MainWindow::on_action_Manage_clicked()
{
    manage_project m_dlg;
    m_dlg.setupwidget_manage();
    m_dlg.exec();
}
void MainWindow::on_action_Exit_clicked()
{
    close();
}
void MainWindow::on_action_Archive_clicked()
{

}
void MainWindow::on_action_Retrieve_clicked()
{

}
void MainWindow::on_action_libManage_clicked()
{
    LibManage m_dlg;
    m_dlg.setupwidget(LIB_MANAGE);
    m_dlg.exec();
}

void MainWindow::on_action_Project_close()
{
    QString name =sender()->property("name").toString();
    widgets.value(name)->close_project();
    widgets.remove(name);
    //delete Docks.value(name);
    Docks.remove(name);
    for(int i=0;i<ui->menuWindows->actions().size();i++)
    {
        QAction *action =ui->menuWindows->actions().at(i);
        if(action->property("name").toString()==name)
        {
            ui->menuWindows->removeAction(action);
            break;
        }
    }
    if(Docks.keys().size()!=0)
        Docks.value(Docks.keys().first())->show();
}
bool MainWindow::create_project(QString name,QString path)
{
    QDir dir(path+QDir::separator()+name);
    if(!dir.exists(path+QDir::separator()+name))
    {
        if(dir.mkpath(path+QDir::separator()+name)==false)
            return false;
    }
    pugi::xml_document doc;
    pugi::xml_node xdec = doc.prepend_child(pugi::node_declaration);
    xdec.append_attribute("version").set_value("1.0");
    xdec.append_attribute("encoding").set_value("utf-8");
    pugi::xml_node xnode = doc.prepend_child(pugi::node_element);
    xnode.set_name("SCL");
    pugi::xml_node xPro = xnode.prepend_child(pugi::node_element);
    xPro.set_name("Project");
    xPro.append_attribute("name").set_value(name.toStdString().data());
    xPro.append_attribute("create").set_value(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss").toStdString().data());
    QString filename =path+QDir::separator()+name+".KFpro";
    if(doc.save_file(filename.toStdString().data())==false)
        return false;
    return true;
}
void MainWindow::update_regedit(QString name,QString path)
{
    QDir dir;
    dir.cdUp();
    QString temp=dir.path()+QDir::separator()+"Project";
    if(path==temp)
        return;
    QSettings m_setting;
    QStringList files =m_setting.value("KFproject/Files").toStringList();
    files.append(path+QDir::separator()+name+".KFpro");
    m_setting.setValue("KFproject/Files",files);
}

bool MainWindow::open_project(QString name,QString path)
{
    QString filename =path+QDir::separator()+name+".KFpro";
    if(doc.load_file(filename.toStdString().data())==false)
    {
        QMessageBox::warning(this,tr("warning"),tr("The project is illegal, reselect project"));
        return false;
    }
    if(widgets.keys().contains(path+QDir::separator()+name))
    {
        widgets.value(path+QDir::separator()+name)->show();
        for(int i=0;i<ui->menuWindows->actions().size();i++)
        {
            QAction *action =ui->menuWindows->actions().at(i);
            action->setIcon(QIcon(""));
            if(action->property("name").toString()==path+QDir::separator()+name)
            {
                action->setIcon(QIcon(":/images/pencil_edit.png"));
                break;
            }
        }
    }
    else
        Add_ProjectWidget(name,path);
    return true;
}
void MainWindow::on_action_cut_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_cut()));
}
void MainWindow::on_action_copy_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_cut()));

}
void MainWindow::on_action_paste_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_paste()));

}
void MainWindow::on_action_del_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_delete()));

}
void MainWindow::on_action_property_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_property()));

}
void MainWindow::on_action_connect_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_connect()));

}
void MainWindow::on_action_import_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_importdev()));

}
void MainWindow::on_action_export_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_exportdev()));

}
void MainWindow::on_action_update_clicked()
{

}
void MainWindow::on_action_folder_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_folder()));

}
void MainWindow::on_action_device_clicked()
{
    QTimer::singleShot(0, widgets.value(widget_key), SLOT(on_action_insert_device()));

}
void MainWindow::on_action_Package()
{
    QString program = "./elfeditor.exe";
    QStringList arguments;
    arguments << "-style" << "fusion";
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}
void MainWindow::on_action_close()
{
    for (int i=0;i<widgets.keys().size();i++)
    {
        widgets.value(widgets.keys().at(i))->close_project();
    }
    exit(0);
}
