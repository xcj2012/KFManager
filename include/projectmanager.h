#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QWidget>
#include "global.h"
#include "objectproperty.h"
#include "libmanage.h"
#include "scddelegate.h"
#include "zip_process.h"
namespace Ui {
class ProjectManager;
}

class ProjectManager : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectManager(QWidget *parent = nullptr);
    ~ProjectManager();
    QString             file_name;
    QString             file_path;
    void                open_project();
    void                save_project();
    void                close_project();
    void                renameProject(QString source,QString dest);
    void                init_connect();
    void                add_station_folder(QString name);
    void                del_station_folder(QString name);
    void                del_station_device(QString station,QString dev);
    void                add_Dev2Project(QString fromdir,QString lib,QString ver,QString name);
    void                add_SCD2Project(QString name);
    void                connect2Devic(const QModelIndex &index);
    QString             input_object_name(QString);
    pugi::xml_document  doc;
    QStandardItemModel  *m_model;
    QStandardItemModel  *dev_model;
    QModelIndex         cur_index;
    QString             source_xpath;
    QString             source_station;
    QMenu               *popMenu;//定义一个右键弹出菜单
    QMenu               *TabMenu;//定义一个右键弹出菜单
    int                 opera_type;

    QStandardItem*      getItem(QStandardItem *item, QString s);
    QStandardItem*      getItem_xpath(QStandardItemModel *model, QString xpath);
    QStandardItem*      getItem_xpath(QStandardItem *item, QString s);
    void                setTreeview_currindex(QString);
    void                openSCDbyProcess(QString);
    QMap<QString,QMap<QString,DevInfo_struct>> device_map;
    QModelIndex         last_index;
    QModelIndex         new_index;
    ScdDelegate         *mScdDelegat;


    bool                    removeOnedevice(QString ID,QString station);
    bool                    copydevicefromLib(QString source,QString ID,QString station);
signals:
    void                emit_close_signal();
private slots:
    void                treeview_customContextMenuRequested(const QPoint &pos);
    void                tabview_customContextMenuRequested(const QPoint &pos);
    void                on_action_copy();
    void                on_action_cut();
    void                on_action_paste();
    void                on_action_delete();
    void                on_action_folder();
    void                on_action_insert_device();
    void                on_action_insert_SCD();
    void                on_action_connect();
    void                on_action_property();
    void                on_treeview_clicked(const QModelIndex &index);
    void                on_tabview_clicked(const QModelIndex &index);
    void                on_tabview_doubleclicked(const QModelIndex &index);
    void                on_action_importdev();
    void                on_action_exportdev();

private:
    Ui::ProjectManager *ui;
};

#endif // PROJECTMANAGER_H
