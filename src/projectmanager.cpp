#include "projectmanager.h"
#include "ui_projectmanager.h"

ProjectManager::ProjectManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectManager)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_DeleteOnClose );
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,9);
    ui->groupBox->setStyleSheet("border:none");
    ui->treeView->setStyleSheet("border:none");
    m_model= new QStandardItemModel;
    dev_model= new QStandardItemModel;
    ui->tableView->setModel(dev_model);
    ui->treeView->setModel(m_model);
    ui->treeView->header()->hide();
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    QStringList headers;
    headers<<"name";
    dev_model->setHorizontalHeaderLabels(headers);
    dev_model->setColumnCount(1);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setShowGrid(false);
    opera_type=No_NODE;
    mScdDelegat= new ScdDelegate();
    init_connect();
}

ProjectManager::~ProjectManager()
{
    delete  mScdDelegat;
    delete ui;
}
void ProjectManager::init_connect()
{
    connect(ui->treeView,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(treeview_customContextMenuRequested(const QPoint &)));
    connect(ui->tableView,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(tabview_customContextMenuRequested(const QPoint &)));
    connect(ui->treeView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(on_treeview_clicked(const QModelIndex &)));
    connect(ui->tableView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(on_tabview_doubleclicked(const QModelIndex &)));
    connect(ui->tableView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(on_tabview_clicked(const QModelIndex &)));


}

void ProjectManager::del_station_folder(QString name)
{
    g_delete_dir(file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+name);
    g_delete_dir(file_path+QDir::separator()+file_name+QDir::separator()+"IEC61850"+QDir::separator()+name);
}
void ProjectManager::del_station_device(QString station,QString dev)
{
    g_delete_dir(file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+station+QDir::separator()+dev);
}
void ProjectManager::add_SCD2Project(QString name)
{

}
void ProjectManager::add_Dev2Project(QString fromdir,QString lib,QString ver,QString name)
{
    QString xpath= new_index.data(Qt::UserRole).toString();
    QString station =new_index.data(Qt::UserRole+STATION_NAME).toString();
    QString dest =file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+station;
    QString source=fromdir;
    QString sID =mScdDelegat->get_validID(station);
    if(copydevicefromLib(source,sID,station))
    {
        mScdDelegat->AddPath(name,sID,lib,ver,station,xpath,DEVICE_NODE);
        mScdDelegat->AddMap(station);
    }else
    {
        g_delete_dir(file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+station+QDir::separator()+sID);
    }
    return;
}
QString ProjectManager::input_object_name(QString name)
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Plesase folder name"),
                                         tr("Folder name:"), QLineEdit::Normal,
                                         name, &ok);
    if(!text.isEmpty())
    {
        foreach (QChar var, text)
        {
            if(var.isLetterOrNumber()||var=='_')
            {
                continue;
            }else
            {
                QMessageBox::warning(this,tr("warning"),tr("the name of folder must be letter, number or '_'"));
                text=input_object_name(text);
                break;
            }
        }
    }
    return text;
}
void ProjectManager::add_station_folder(QString name)
{
    QDir dir(file_path);
    dir.mkpath(file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+name);
    dir.mkpath(file_path+QDir::separator()+file_name+QDir::separator()+"IEC61850"+QDir::separator()+name);
}

void ProjectManager::open_project()
{
    setWindowTitle(file_name+"--"+file_path);
    QString filename =file_path+QDir::separator()+file_name+".KFpro";
    mScdDelegat->loadPro(filename);
    mScdDelegat->list_substation(m_model);
    ui->treeView->expandToDepth(2);
    ui->treeView->setCurrentIndex(m_model->index(0,0));
    on_treeview_clicked(m_model->index(0,0));

}


void ProjectManager::renameProject(QString source,QString dest)
{

}

void ProjectManager::close_project()
{
    save_project();
    close();
}
void ProjectManager::save_project()
{
    QString filename =file_path+QDir::separator()+file_name+".KFpro";
    mScdDelegat->savePro(filename);
}
void ProjectManager::treeview_customContextMenuRequested(const QPoint &pos)
{
    if(new_index.isValid())
        last_index=new_index;
    new_index = ui->treeView->currentIndex(); //选中的行
    on_treeview_clicked(new_index);
    int itype =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
    popMenu = new QMenu(this);
    if(itype==ROOT_NODE)
    {
        QAction *folderAction = popMenu->addAction(tr("Folder"));
        QAction *propertyAction = popMenu->addAction(tr("Properties"));
        folderAction->setProperty("type",TREEVIEW_TYPE);
        propertyAction->setProperty("type",TREEVIEW_TYPE);
        connect(folderAction,SIGNAL(triggered()),this,SLOT(on_action_folder()));
        connect(propertyAction,SIGNAL(triggered()),this,SLOT(on_action_property()));

    }else if((itype==FOLDER_NODE)||(itype==STATION_NODE))
    {
        QAction *copyAction = popMenu->addAction(tr("Copy"));
        QAction *cutAction = popMenu->addAction(tr("Cut"));
        QAction *pasteAction = popMenu->addAction(tr("Paste"));
        if(itype==STATION_NODE)
            copyAction->setEnabled(false);
        if(itype==STATION_NODE)
            cutAction->setEnabled(false);
        if(opera_type==No_NODE)
            pasteAction->setEnabled(false);
        QAction *delAction = popMenu->addAction(tr("Delete"));
        popMenu->addSeparator();
        QMenu *insert_menu=popMenu->addMenu(tr("Insert Object"));
        QAction *insertAction_dev = insert_menu->addAction(tr("Insert device"));
        QAction *insertAction_scd = insert_menu->addAction(tr("Insert IEC61850"));
        if(itype==FOLDER_NODE)
            insertAction_scd->setEnabled(false);
        QAction *folderAction = popMenu->addAction(tr("Folder"));
        popMenu->addSeparator();
        QAction *propertyAction = popMenu->addAction(tr("Properties"));
        popMenu->addSeparator();
        //QAction *connectAction = popMenu->addAction(tr("Device Online"));
        copyAction->setProperty("type",TREEVIEW_TYPE);
        cutAction->setProperty("type",TREEVIEW_TYPE);
        pasteAction->setProperty("type",TREEVIEW_TYPE);
        delAction->setProperty("type",TREEVIEW_TYPE);
        insertAction_dev->setProperty("type",TREEVIEW_TYPE);
        insertAction_scd->setProperty("type",TREEVIEW_TYPE);
        folderAction->setProperty("type",TREEVIEW_TYPE);
        propertyAction->setProperty("type",TREEVIEW_TYPE);
        connect(copyAction,SIGNAL(triggered()),this,SLOT(on_action_copy()));
        connect(cutAction,SIGNAL(triggered()),this,SLOT(on_action_cut()));
        connect(pasteAction,SIGNAL(triggered()),this,SLOT(on_action_paste()));
        connect(delAction,SIGNAL(triggered()),this,SLOT(on_action_delete()));

        connect(insertAction_dev,SIGNAL(triggered()),this,SLOT(on_action_insert_device()));
        connect(insertAction_scd,SIGNAL(triggered()),this,SLOT(on_action_insert_SCD()));
        connect(folderAction,SIGNAL(triggered()),this,SLOT(on_action_folder()));
        connect(propertyAction,SIGNAL(triggered()),this,SLOT(on_action_property()));
    }
    popMenu->exec(QCursor::pos());
}


void ProjectManager::tabview_customContextMenuRequested(const QPoint &pos)
{
    TabMenu = new QMenu(this);
    if(ui->tableView->currentIndex().isValid())
    {

        on_tabview_clicked(ui->tableView->currentIndex());
        int itype =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
        if(itype==FOLDER_NODE||(itype==DEVICE_NODE)||(itype==STATION_NODE))
        {
            QAction *copyAction = TabMenu->addAction(tr("Copy"));
            copyAction->setProperty("type",TABVIEW_TYPE);
            QAction *cutAction = TabMenu->addAction(tr("Cut"));
            cutAction->setProperty("type",TABVIEW_TYPE);
            QAction *pasteAction = TabMenu->addAction(tr("Paste"));
            pasteAction->setProperty("type",TABVIEW_TYPE);
            if(opera_type==No_NODE)
                pasteAction->setEnabled(false);
            QAction *delAction = TabMenu->addAction(tr("Delete"));
            delAction->setProperty("type",TABVIEW_TYPE);

            TabMenu->addSeparator();
            QMenu *insert_menu=TabMenu->addMenu(tr("Insert Object"));
            QAction *insertAction_dev = insert_menu->addAction(tr("Insert device"));
            insertAction_dev->setProperty("type",TABVIEW_TYPE);
            QAction *insertAction_scd = insert_menu->addAction(tr("Insert IEC61850"));
            insertAction_scd->setProperty("type",TABVIEW_TYPE);

            if(itype==FOLDER_NODE||(itype==DEVICE_NODE))
                insertAction_scd->setEnabled(false);
            QAction *folderAction = TabMenu->addAction(tr("Folder"));
            folderAction->setProperty("type",TABVIEW_TYPE);

            TabMenu->addSeparator();
            QAction *propertyAction = TabMenu->addAction(tr("Properties"));
            propertyAction->setProperty("type",TABVIEW_TYPE);
            TabMenu->addSeparator();
            QAction *connectAction = TabMenu->addAction(tr("Device Online"));
            if(itype==FOLDER_NODE)
                connectAction->setEnabled(false);
            connect(copyAction,SIGNAL(triggered()),this,SLOT(on_action_copy()));
            connect(cutAction,SIGNAL(triggered()),this,SLOT(on_action_cut()));
            connect(pasteAction,SIGNAL(triggered()),this,SLOT(on_action_paste()));
            connect(delAction,SIGNAL(triggered()),this,SLOT(on_action_delete()));

            connect(insertAction_dev,SIGNAL(triggered()),this,SLOT(on_action_insert_device()));
            connect(insertAction_scd,SIGNAL(triggered()),this,SLOT(on_action_insert_SCD()));
            connect(folderAction,SIGNAL(triggered()),this,SLOT(on_action_folder()));
            connect(propertyAction,SIGNAL(triggered()),this,SLOT(on_action_property()));
            connect(connectAction,SIGNAL(triggered()),this,SLOT(on_action_connect()));
        }else if(itype==SCD_NODE)
        {
            QAction *copyAction = TabMenu->addAction(tr("Copy"));
            copyAction->setProperty("type",TABVIEW_TYPE);
            QAction *cutAction = TabMenu->addAction(tr("Cut"));
            cutAction->setProperty("type",TABVIEW_TYPE);
            QAction *pasteAction = TabMenu->addAction(tr("Paste"));
            pasteAction->setProperty("type",TABVIEW_TYPE);
            if(opera_type==No_NODE)
                pasteAction->setEnabled(false);
            QAction *delAction = TabMenu->addAction(tr("Delete"));
            delAction->setProperty("type",TABVIEW_TYPE);
            TabMenu->addSeparator();
            QAction *propertyAction = TabMenu->addAction(tr("Properties"));
            propertyAction->setProperty("type",TABVIEW_TYPE);
            connect(copyAction,SIGNAL(triggered()),this,SLOT(on_action_copy()));
            connect(cutAction,SIGNAL(triggered()),this,SLOT(on_action_cut()));
            connect(pasteAction,SIGNAL(triggered()),this,SLOT(on_action_paste()));
            connect(delAction,SIGNAL(triggered()),this,SLOT(on_action_delete()));
        }
    }else
    {
        int itype =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
        if(itype==DEVICE_NODE)
        {
            new_index=ui->treeView->currentIndex();
        }
          itype =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
        if(itype==ROOT_NODE)
        {
            QAction *folderAction = TabMenu->addAction(tr("Folder"));
            folderAction->setProperty("type",TREEVIEW_TYPE);

            QAction *propertyAction = TabMenu->addAction(tr("Properties"));
            propertyAction->setProperty("type",TREEVIEW_TYPE);

            connect(folderAction,SIGNAL(triggered()),this,SLOT(on_action_folder()));
            connect(propertyAction,SIGNAL(triggered()),this,SLOT(on_action_property()));

        }else if((itype==FOLDER_NODE)||(itype==STATION_NODE))
        {
            QAction *copyAction = TabMenu->addAction(tr("Copy"));
            QAction *cutAction = TabMenu->addAction(tr("Cut"));
            QAction *pasteAction = TabMenu->addAction(tr("Paste"));
            if(opera_type==No_NODE)
                pasteAction->setEnabled(false);
            QAction *delAction = TabMenu->addAction(tr("Delete"));
            TabMenu->addSeparator();
            QMenu *insert_menu=TabMenu->addMenu(tr("Insert Object"));
            QAction *insertAction_dev = insert_menu->addAction(tr("Insert device"));
            QAction *insertAction_scd = insert_menu->addAction(tr("Insert IEC61850"));
            if(itype==FOLDER_NODE)
                insertAction_scd->setEnabled(false);
            QAction *folderAction = TabMenu->addAction(tr("Folder"));
            TabMenu->addSeparator();
            QAction *propertyAction = TabMenu->addAction(tr("Properties"));
            TabMenu->addSeparator();
            copyAction->setProperty("type",TREEVIEW_TYPE);
            cutAction->setProperty("type",TREEVIEW_TYPE);
            pasteAction->setProperty("type",TREEVIEW_TYPE);
            delAction->setProperty("type",TREEVIEW_TYPE);
            insertAction_dev->setProperty("type",TREEVIEW_TYPE);
            insertAction_scd->setProperty("type",TREEVIEW_TYPE);
            folderAction->setProperty("type",TREEVIEW_TYPE);
            propertyAction->setProperty("type",TREEVIEW_TYPE);
            connect(copyAction,SIGNAL(triggered()),this,SLOT(on_action_copy()));
            connect(cutAction,SIGNAL(triggered()),this,SLOT(on_action_cut()));
            connect(pasteAction,SIGNAL(triggered()),this,SLOT(on_action_paste()));
            connect(delAction,SIGNAL(triggered()),this,SLOT(on_action_delete()));
            connect(insertAction_dev,SIGNAL(triggered()),this,SLOT(on_action_insert_device()));
            connect(insertAction_scd,SIGNAL(triggered()),this,SLOT(on_action_insert_SCD()));
            connect(folderAction,SIGNAL(triggered()),this,SLOT(on_action_folder()));
            connect(propertyAction,SIGNAL(triggered()),this,SLOT(on_action_property()));
        }
    }

    TabMenu->exec(QCursor::pos());
}
void ProjectManager::on_action_copy()
{
    source_xpath=new_index.data(Qt::UserRole).toString();
    source_station=new_index.data(Qt::UserRole+STATION_NAME).toString();
    opera_type =Copy_NODE;
}
void ProjectManager::on_action_cut()
{
    source_xpath=new_index.data(Qt::UserRole).toString();
    source_station=new_index.data(Qt::UserRole+STATION_NAME).toString();
    opera_type =Cut_NODE;
}
void ProjectManager::on_action_delete()
{
    int ret =QMessageBox::question(this,tr("Question"),tr("Do you want to delete the node:")+ QString(" ")+new_index.data().toString(),QMessageBox::Yes|QMessageBox::No,
                                   QMessageBox::No);
    if(ret==QMessageBox::No)
        return;
    QString xpath,station;
    xpath=new_index.data(Qt::UserRole).toString();
    station =new_index.data(Qt::UserRole+STATION_NAME).toString();
    QModelIndex m_parant= new_index.parent();
    QString path;
    if(new_index.parent().isValid())
        path=m_parant.data(Qt::UserRole).toString();
    else
        path =ui->treeView->currentIndex().data(Qt::UserRole).toString();
    QStringList keys= mScdDelegat->get_devUnderNode(xpath);
    foreach (QString key, keys)
    {
        mScdDelegat->deleteDevUnderstation(key,station);
        removeOnedevice(key,station);
    }
    mScdDelegat->deleteNode(xpath);
    mScdDelegat->list_substation(m_model);
    QString filename =file_path+QDir::separator()+file_name+".KFpro";
    mScdDelegat->savePro(filename);
    setTreeview_currindex(path);

}
void ProjectManager::on_action_paste()
{
    if(opera_type==No_NODE)
        return;
    QString target_path ;
    pugi::xpath_node xnode=doc.select_node(source_xpath.toStdString().data());
    QString item_name =QString(xnode.node().attribute("name").as_string());
    QString station =new_index.data(Qt::UserRole+STATION_NAME).toString();
    int val =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
    target_path =new_index.data(Qt::UserRole).toString();
    if(val==DEVICE_NODE||val==SCD_NODE)
    {
        target_path=ui->treeView->currentIndex().data(Qt::UserRole).toString();
    }
    if(source_xpath.contains(target_path))
    {
        QMessageBox::warning(this,tr("warning"),tr("The object ")+item_name+tr(" can't copy to itself."));
        return;
    }
    QString text =mScdDelegat->getitemName(source_xpath);
    while(!mScdDelegat->isValidforder(text,target_path))
    {
        bool ok;
        text = QInputDialog::getText(this, tr("Plesase item name"),
                                     tr("name:"), QLineEdit::Normal,
                                     text, &ok);
        if(text.isEmpty()||ok==false)
            return;
    }
    if(opera_type==Copy_NODE)
    {
        QString xpath=mScdDelegat->copysoure2target(source_xpath,target_path,text,val);
        int type =mScdDelegat->getitemType(xpath);
        if(type==FOLDER_NODE)
        {
            QStringList IDs =mScdDelegat->getitemIDs(xpath);
            foreach (QString ID, IDs)
            {
                QString newID=mScdDelegat->get_validID(station);
                mScdDelegat->updateID(ID,newID,station,xpath,text);
                mScdDelegat->copyDevice(ID,source_station,newID,station);
                QString sourcedir =file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+source_station+QDir::separator()+ID;
                QString targetdir =file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+station+QDir::separator()+newID;
                bool ret= copyDirectoryFiles(sourcedir,targetdir,true);
                qDebug()<<"copy folder error:"<< ret<< sourcedir;
            }

        }else if(type==DEVICE_NODE)
        {
            QString oldID =mScdDelegat->getitemID(xpath);
            QString newID=mScdDelegat->get_validID(station);
            mScdDelegat->updateID(newID,station,xpath,text);
            mScdDelegat->copyDevice(oldID,source_station,text,newID,station);
            QString sourcedir =file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+source_station+QDir::separator()+oldID;
            QString targetdir =file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+station+QDir::separator()+newID;
            bool ret= copyDirectoryFiles(sourcedir,targetdir,true);
            qDebug()<<"copy folder error:"<< ret<< sourcedir;

        }
        else if(type==SCD_NODE)
        {

        }
    }else
    {
        QString xpath=mScdDelegat->movesoure2target(source_xpath,target_path,text,val);
        int type =mScdDelegat->getitemType(xpath);
        if(station!=source_station)
        {
            if(type==FOLDER_NODE)
            {
                QStringList IDs =mScdDelegat->getitemIDs(xpath);
                foreach (QString ID, IDs)
                {
                    QString newID=mScdDelegat->get_validID(station);
                    mScdDelegat->updateID(ID,newID,station,xpath,text);
                    mScdDelegat->copyDevice(ID,source_station,newID,station);
                    mScdDelegat->removeDevice(ID,source_station);
                    QString sourcedir =file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+source_station+QDir::separator()+ID;
                    QString targetdir =file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+station+QDir::separator()+newID;
                    bool ret= copyDirectoryFiles(sourcedir,targetdir,true);
                    g_delete_dir(sourcedir);
                    qDebug()<<"copy folder error:"<< ret<< sourcedir;
                }
            }else if(type==DEVICE_NODE)
            {
                QString ID =mScdDelegat->getitemID(xpath);
                QString newID=mScdDelegat->get_validID(station);
                mScdDelegat->copyDevice(ID,source_station,text,newID,station);
                mScdDelegat->updateID(newID,station,xpath,text);
                mScdDelegat->removeDevice(ID,source_station);
                QString sourcedir =file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+source_station+QDir::separator()+ID;
                QString targetdir =file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+station+QDir::separator()+newID;
                bool ret= copyDirectoryFiles(sourcedir,targetdir,true);
                g_delete_dir(sourcedir);
                qDebug()<<"copy folder error:"<< ret<< sourcedir;

            }
        }
    }

    opera_type=No_NODE;
    save_project();
    mScdDelegat->list_substation(m_model);
    ui->treeView->expandToDepth(2);
    setTreeview_currindex(target_path);
    //    {
    //        pugi::xpath_node cur_node=doc.select_node(target_path.toStdString().data());
    //        bool ok;
    //        QString text = QInputDialog::getText(this, tr("Plesase folder name"),
    //                                             tr("Folder name:"), QLineEdit::Normal,
    //                                             item_name, &ok);
    //        if(!cur_node.node().select_node(QString("./item[@name='"+text+"']").toStdString().data()).node().empty())
    //        {
    //            QMessageBox::warning(this,tr("warning"),tr("The object ")+text+tr(" is alread exist."));
    //            return;
    //        }
    //        pugi::xml_node past_node;
    //        if(opera_type==Copy_NODE)
    //        {
    //            past_node=cur_node.node().append_copy(xnode.node());
    //        }
    //        if(opera_type==Cut_NODE)
    //        {
    //            past_node=cur_node.node().append_move(xnode.node());
    //        }
    //        past_node.set_name("item");
    //        past_node.attribute("name").set_value(text.toStdString().data());
    //        if(source_xpath.endsWith("@type='folder']"))
    //            past_node.attribute("type").set_value("folder");
    //        else if(source_xpath.endsWith("@type='device']"))
    //            past_node.attribute("type").set_value("device");
    //        else if(source_xpath.endsWith("@type='SCD']"))
    //            past_node.attribute("type").set_value("SCD");
    //        opera_type=No_NODE;
    //        mScdDelegat->list_substation(m_model);
    //        QList<QStandardItem*>items=m_model->findItems(item_name);
    //        foreach(QStandardItem* item,items)
    //        {
    //            if(item->data(Qt::UserRole).toString()==target_path)
    //            {
    //                ui->treeView->setExpanded(m_model->index(item->row(),item->column()),true);
    //                break;
    //            }
    //        }
    //        save_project();
    //    }

}

QStandardItem* ProjectManager::getItem_xpath(QStandardItemModel *model, QString xpath)
{
    QStandardItem *getitem = NULL;
    if(!model->hasChildren())//判断是否有孩子,没有则返回0
        return NULL;
    QList<QStandardItem*> list =model->findItems(xpath);
    //qDebug() << tr("list is %1").arg(list.length());
    //qDebug() << tr("rowCount is %1").arg(model->rowCount());
    if(list.length() == 0)//如果链表长度为0，即没找到文本为s的条目
    {
        //将搜索子条目是否存在文本为s的条目
        for(int i = 0;i < model->rowCount()&& getitem == NULL;i++)//遍历model下的所有条目,如果getitem有获得对象，则退出循环
        {
            getitem = getItem_xpath(model->item(i),xpath);//寻找第i行条目下的子条目列中是否存在文本为s的条目。
        }
    }
    else
    {
        return list.at(0);
    }
    return getitem;

}
QStandardItem* ProjectManager::getItem_xpath(QStandardItem *item, QString xpath)
{
    if(item == NULL)
        return NULL;
    //qDebug() << tr("fine %1").arg(item->data(Qt::UserRole).toString());
    QStandardItem *getitem = NULL;
    if(item->data(Qt::UserRole).toString().compare(xpath) == 0)
        return item;
    if(!item->hasChildren())//判断是否有孩子,没有则返回0
        return NULL;
    for(int i = 0;i < item->rowCount() && getitem == NULL;i++)//遍历item下所有子条目，若果getitme有获得对象，则退出循环
    {
        QStandardItem * childitem = item->child(i);
        getitem = getItem_xpath(childitem,xpath);//寻找这个子条目的所有子条目是否存在文本为s的条目。
    }
    return getitem;


}
QStandardItem* ProjectManager::getItem(QStandardItem *item, QString s)
{
    if(item == NULL)
        return NULL;
    //qDebug() << tr("fined %1").arg(item->text());
    QStandardItem *getitem = NULL;
    //    if(item->text().compare(s) == 0)
    //        return item;
    if(!item->hasChildren())//判断是否有孩子,没有则返回0
        return NULL;
    for(int i = 0;i < item->rowCount() && getitem == NULL;i++)//遍历item下所有子条目，若果getitme有获得对象，则退出循环
    {
        QStandardItem * childitem = item->child(i);
        //qDebug() << tr("fined %1").arg(childitem->text());
        if(childitem->text().compare(s) == 0)
        {
            getitem =childitem;
            break;
        }

        //        getitem = getItem(childitem,s);//寻找这个子条目的所有子条目是否存在文本为s的条目。
    }
    return getitem;
}

void ProjectManager::setTreeview_currindex(QString xpath)
{
    QStandardItem* childitem=getItem_xpath(m_model,xpath);
    qDebug()<<xpath;
    ui->treeView->setCurrentIndex(m_model->indexFromItem(childitem));
    ui->treeView->selectionModel()->select(m_model->indexFromItem(childitem),QItemSelectionModel::Select | QItemSelectionModel::Rows);
    on_treeview_clicked(ui->treeView->currentIndex());
}


void ProjectManager::openSCDbyProcess(QString path)
{

}
bool ProjectManager::removeOnedevice(QString ID,QString station)
{

}
bool ProjectManager::copydevicefromLib(QString source,QString ID,QString station)
{
    QString target=file_path+QDir::separator()+file_name+QDir::separator()+"Device"+QDir::separator()+station+QDir::separator()+ID;
    bool ret=copyDirectoryFiles(source,target,true);
    return ret;
}

void ProjectManager::on_action_folder()
{
    QString xpath=new_index.data(Qt::UserRole).toString();
    QString name=new_index.data().toString();
    QString station=new_index.data(Qt::UserRole+STATION_NAME).toString();
    bool ok;
    QString text = QInputDialog::getText(this, tr("Plesase folder name"),
                                         tr("Folder name:"), QLineEdit::Normal,
                                         name+"1", &ok);
    if(!text.isEmpty())
    {
        foreach (QChar var, text)
        {
            if(var.isLetterOrNumber()||var=='_')
            {
                continue;
            }else
            {
                QMessageBox::warning(this,tr("warning"),tr("The name of folder must be letter, number or '_'"));
                return;
            }
        }
        int type =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
        if(type==ROOT_NODE)
        {
            mScdDelegat->AddPath(text,"","","",station,xpath,STATION_NODE);

        }else
        {
            if(!mScdDelegat->isValidforder(text,xpath))
            {
                QMessageBox::warning(this,tr("warning"),tr("The name:%1 is already exist.").arg(text));
                return;
            }
            mScdDelegat->AddPath(text,"","","",station,xpath,FOLDER_NODE);
        }

        mScdDelegat->list_substation(m_model);
        setTreeview_currindex(xpath);

        on_treeview_clicked(ui->treeView->currentIndex());
    }
    save_project();

}
void ProjectManager::on_action_insert_device()
{
    LibManage m_dlg;
    m_dlg.setupwidget(LIB_SHOW);

    if(m_dlg.exec()==QDialog::Accepted)
    {
        bool b_vaild =false;
        QString text;
        while (b_vaild==false)
        {
            text= input_object_name(m_dlg.device);
            QString xpath= new_index.data(Qt::UserRole).toString()+"/item[@name='"+text+"' and @type ='device']";
            pugi::xpath_node xnode=doc.select_node(xpath.toStdString().data());
            if(xnode.node().empty())
                b_vaild=true;
            else
            {
                QMessageBox::warning(this,tr("warning"),tr("The object")+QString(" ")+text+QString(" ")+tr("is already exist"));
            }
        }
        add_Dev2Project(m_dlg.Lib_path,m_dlg.device,m_dlg.ver,text);
        on_treeview_clicked(ui->treeView->currentIndex());
        save_project();
    }
}
void ProjectManager::on_action_insert_SCD()
{
    QString station=new_index.data(Qt::UserRole+STATION_NAME).toString();
    QString xpath=new_index.data(Qt::UserRole).toString();
    if(mScdDelegat->isExistSCD(station))
    {
        QMessageBox::warning(this,tr("warning"),tr("The IEC61850 project is already exist!"));
        return;
    }
    bool ok;
    QString text = QInputDialog::getText(this, tr("Plesase name"),
                                         tr("Project name:"), QLineEdit::Normal,
                                         "IEC61850", &ok);
    if(!text.isEmpty())
    {
        foreach (QChar var, text)
        {
            if(var.isLetterOrNumber()||var=='_')
            {
                continue;
            }else
            {
                QMessageBox::warning(this,tr("warning"),tr("The name of folder must be letter, number or '_'"));
                return;
            }
        }
        mScdDelegat->AddPath(text,"","","",station,xpath,SCD_NODE);
    }
    setTreeview_currindex(xpath);
    save_project();
}

void ProjectManager::on_action_connect()
{

}
void ProjectManager::connect2Devic(const QModelIndex &index)
{

}
void ProjectManager::on_tabview_clicked(const QModelIndex &index)
{
    if(new_index.isValid())
        last_index=new_index;
    new_index =index;

}
void ProjectManager::on_tabview_doubleclicked(const QModelIndex &index)
{
    if(new_index.isValid())
        last_index=new_index;
    new_index =index;
    int itype =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
    QString xpath =new_index.data(Qt::UserRole).toString();
    QString station =new_index.data(Qt::UserRole+STATION_NAME).toString();

    if(itype==DEVICE_NODE)
    {
        connect2Devic(new_index);
        return;
    }else if(itype==FOLDER_NODE||itype==STATION_NODE)
    {
        setTreeview_currindex(xpath);

    }else if(itype==SCD_NODE)
    {
        QString path =file_path+QDir::separator()+file_name+QDir::separator()+"IEC61850"+QDir::separator()+station+QDir::separator()+new_index.data().toString();
        openSCDbyProcess(path);

    }
}
void ProjectManager::on_treeview_clicked(const QModelIndex &index)
{
    if(new_index.isValid())
        last_index=new_index;
    new_index =index;
    //cur_index = ui->treeView->currentIndex(); //选中的行
    int itype =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
    QString station =new_index.data(Qt::UserRole+STATION_NAME).toString();
    QString xpath=new_index.data(Qt::UserRole).toString();
    mScdDelegat->list_tablview(xpath,dev_model,station,itype);
    ui->tableView->resizeColumnsToContents();
}
void ProjectManager::on_action_property()
{
    int itype;
    QString xpath;
    QString name;
    QString station;
    itype =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
    xpath=new_index.data(Qt::UserRole).toString();
    station =new_index.data(Qt::UserRole+STATION_NAME).toString();
    name=new_index.data().toString();
    ObjectProperty m_dlg;
    if(itype==ROOT_NODE)
        m_dlg.name=file_name;
    else
        m_dlg.name =name;
    m_dlg.path=file_path;
    Item_struct m_info;
    mScdDelegat->getitemInfo(m_info,xpath);
    m_dlg.createtime =mScdDelegat->getcreatetime();
    m_dlg.modifytime =mScdDelegat->getmodifytime();
    DevInfo_struct info;
    if(itype==DEVICE_NODE)
    {
        mScdDelegat->getDevByIDfromStation(info,m_info.ID,station);
        m_dlg.ipaddress =m_info.IP;
        m_dlg.address =m_info.Address;
        m_dlg.device =info.type;
        m_dlg.version =info.version;
    }
    m_dlg.setupWidget(itype);
    if(m_dlg.exec()==QDialog::Accepted)
    {
        Item_struct new_info;
        new_info.ID =m_info.ID;
        new_info.type =m_info.type;
        new_info.IP=m_dlg.ipaddress;
        new_info.name=m_dlg.name;
        new_info.author=m_dlg.author;
        new_info.commit=m_dlg.commit;
        new_info.Address=m_dlg.address;
        if(new_info.IP!=m_info.IP||new_info.name!=m_info.name||new_info.author!=m_info.author
                ||new_info.commit!=m_info.commit||new_info.Address!=m_info.Address)
        {
            mScdDelegat->setitemInfo(new_info,xpath);
            if(itype==DEVICE_NODE)
            {
                info.name=new_info.name;
                mScdDelegat->updatedevice(new_info.ID,station,info);
                mScdDelegat->list_tablview(ui->treeView->currentIndex().data(Qt::UserRole).toString(),dev_model,station,ui->treeView->currentIndex().data(Qt::UserRole+NODE_TYPE).toInt());
                ui->tableView->resizeColumnsToContents();
            }else
            {
                QString tmp =new_index.parent().data(Qt::UserRole).toString();
                mScdDelegat->list_substation(m_model);
                setTreeview_currindex(tmp);
            }
            save_project();

        }
    }
}
void ProjectManager::on_action_importdev()
{
    int itype =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
    if(itype==ROOT_NODE)
    {
        QMessageBox::warning(this,tr("warnning"),tr("Please select the folder at first."));
        return;
    }

}
void ProjectManager::on_action_exportdev()
{
    int itype =new_index.data(Qt::UserRole+NODE_TYPE).toInt();
    QString name =new_index.data().toString();
    if(itype!=DEVICE_NODE)
    {
        QMessageBox::warning(this,tr("warnning"),tr("Please select the device at first."));
        return;
    }
    QString dir = QFileDialog::getExistingDirectory(this, tr("Output path"),
                                                    g_dir.currentPath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
    {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Output file name"),
                                             tr("name:"), QLineEdit::Normal,
                                             name, &ok);
        if (ok && !text.isEmpty())
        {

        }
    }
}
