#include "scddelegate.h"

ScdDelegate::ScdDelegate()
{
    moveToThread(this);
    m_doc = new pugi::xml_document();
    iconSet.addFile(QString::fromLocal8Bit(":/images/folder.png"));
    iconDev.addFile(QString::fromLocal8Bit(":/images/printer.png"));
    iconSCD.addFile(QString::fromLocal8Bit(":/images/order_159.png"));
    m_mutex = new QMutex();
    start();

}
ScdDelegate::~ScdDelegate()
{
    quit();
    m_mutex->lock();
    delete m_doc;
    m_mutex->unlock();

    delete m_mutex;
    wait();
}
void ScdDelegate::newPro(QString FileName)
{

}
bool ScdDelegate::loadPro(QString FileName)
{
    pugi::xml_parse_result result;
    result=m_doc->load_file(FileName.toStdString().data());
    if(result.status==pugi::status_ok)
        return  true;
    return false;
}
void ScdDelegate::savePro(QString FileName )
{
    m_doc->save_file(FileName.toStdString().data());
}
void ScdDelegate::list_substation(QStandardItemModel *tree)
{
    tree->setRowCount(0);
    QString xpath="./SCL/Project";
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    pugi::xml_node project_Node;
    if(xnode.node().empty())
        return;
    project_Node =xnode.node();
    QStandardItem* item =new QStandardItem;
    item->setText(project_Node.attribute("name").as_string());
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setData(xpath,Qt::UserRole);
    item->setData(ROOT_NODE,Qt::UserRole+NODE_TYPE);
    item->setIcon(QIcon(":/images/category.png"));
    tree->appendRow(item);
    xpath="./SCL/Project/Substation";
    pugi::xpath_node_set xnodes =m_doc->select_nodes(xpath.toStdString().data());
    pugi::xml_node station_node;
    for(pugi::xpath_node_set::const_iterator it = xnodes.begin();it != xnodes.end();it ++)
    {
        station_node=it->node();
        QStandardItem* item0 =new QStandardItem;
        item0->setText(station_node.attribute("name").as_string());
        item0->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        item0->setData(xpath+"[@name='"+station_node.attribute("name").as_string()+"']",Qt::UserRole);
        item0->setData(STATION_NODE,Qt::UserRole+NODE_TYPE);
        item0->setData(item0->text(),Qt::UserRole+STATION_NAME);
        add_folder(item0,xpath+"[@name='"+station_node.attribute("name").as_string()+"']",item0->text(),station_node);
        item0->setIcon(iconSet);
        item->appendRow(item0);
        QMap<QString,DevInfo_struct> device_infos;
        if(!device_map.keys().contains(station_node.attribute("name").as_string()))
        {
            device_map.insert(station_node.attribute("name").as_string(),device_infos);
        }
        QString str ="./Device";
        pugi::xml_node device=station_node.child("Device");
        if(!device.empty())
        {
            pugi::xml_node sub_item =device.child("item");
            while(!sub_item.empty())
            {
                DevInfo_struct m_info;
                qDebug()<<sub_item.attribute("name").as_string();
                m_info.name =sub_item.attribute("name").as_string();
                m_info.ID =sub_item.attribute("ID").as_string();
                m_info.type =sub_item.attribute("device").as_string();
                m_info.version =sub_item.attribute("ver").as_string();
                device_infos =device_map.value(station_node.attribute("name").as_string());
                device_infos.insert(m_info.ID,m_info);
                //device_map.remove(station_node.attribute("name").as_string());
                device_map.insert(station_node.attribute("name").as_string(),device_infos);
                sub_item=sub_item.next_sibling("item");
            }

        }

    }
}
void ScdDelegate::add_folder(QStandardItem* item,QString path,QString station,pugi::xml_node mnode)
{
    //QString xpath =path+"/item";
    pugi::xpath_node_set xnodes =mnode.select_nodes("./item");
    pugi::xml_node item_node;
    //qDebug()<<xpath;
    for(pugi::xpath_node_set::const_iterator it = xnodes.begin();it != xnodes.end();it ++)
    {
        item_node=it->node();
        QString type =QString(item_node.attribute("type").as_string());
        if(type.compare("folder",Qt::CaseInsensitive)!=0)
        {
            continue;
        }
        QStandardItem* sub_item =new QStandardItem;
        sub_item->setText(item_node.attribute("name").as_string());
        sub_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        sub_item->setData(path+"/item"+"[@name='"+item_node.attribute("name").as_string()+"' and @type='folder']",Qt::UserRole);
        sub_item->setData(FOLDER_NODE,Qt::UserRole+NODE_TYPE);
        sub_item->setData(station,Qt::UserRole+STATION_NAME);
        sub_item->setIcon(iconSet);
        item->appendRow(sub_item);
        add_folder(sub_item,path+"/item"+"[@name='"+item_node.attribute("name").as_string()+"' and @type='folder']",station,item_node);
    }
}
void ScdDelegate::deleteNode(QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    pugi::xml_node mparant =xnode.node().parent();
    mparant.remove_child(xnode.node());
}
void ScdDelegate::deleteDevUnderstation(QString ID,QString station)
{
    QString path=QString("./SCL/Project/Substation[@name='"+station+"']"+"/Device");
    pugi::xpath_node xnode =m_doc->select_node(path.toStdString().data());
    if(!xnode.node().empty())
    {
        pugi::xpath_node delnode =xnode.node().select_node(QString(".//item[@ID='"+ID+"']").toStdString().data());
        if(!delnode.node().empty())
            xnode.node().remove_child(delnode.node());
    }
    AddMap(station);
}
QStringList ScdDelegate::get_devUnderNode(QString xpath)
{
    QStringList keys;
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    pugi::xpath_node_set xset =xnode.node().select_nodes(".//item[@type='device']");
    for(pugi::xpath_node_set::const_iterator it = xset.begin();it != xset.end();it ++)
    {
        keys.append(it->node().attribute("ID").as_string());
    }
    return  keys;
}
void ScdDelegate::list_tablview(QString xpath,QStandardItemModel *model,QString station,int type)
{
    model->setRowCount(0);
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(xnode.node().empty())
        return;
    if((type==STATION_NODE)||(type==FOLDER_NODE))
    {
        pugi::xml_object_range<pugi::xml_named_node_iterator>node_set= xnode.node().children("item");
        for(pugi::xml_named_node_iterator it = node_set.begin();it != node_set.end();it ++)
        {
            QStandardItem* item0 =new QStandardItem;
            item0->setText(it->attribute("name").as_string());
            item0->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            item0->setData(xpath,Qt::UserRole);
            if(strcmp(it->attribute("type").as_string(),"folder")==0)
            {
                item0->setIcon(iconSet);
                item0->setData(xpath+"/item[@name='"+it->attribute("name").as_string()+"' and @type='folder']",Qt::UserRole);
                item0->setData(FOLDER_NODE,Qt::UserRole+NODE_TYPE);
                item0->setData(station,Qt::UserRole+STATION_NAME);
                model->appendRow(item0);
            }else if(strcmp(it->attribute("type").as_string(),"device")==0)
            {
                item0->setIcon(iconDev);
                item0->setData(xpath+"/item[@name='"+it->attribute("name").as_string()+"' and @type='device']",Qt::UserRole);
                item0->setData(station,Qt::UserRole+STATION_NAME);
                item0->setData(DEVICE_NODE,Qt::UserRole+NODE_TYPE);
                item0->setData(it->attribute("path").as_string(),Qt::UserRole+DEVICE_KEY);
                model->appendRow(item0);
            }
        }
        pugi::xml_node mnode=xnode.node().child("IEC61850");
        if(!mnode.empty())
        {
            QStandardItem* item0 =new QStandardItem;
            item0->setText(mnode.attribute("name").as_string());
            item0->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            item0->setData(xpath,Qt::UserRole);
            item0->setIcon(iconSCD);
            item0->setData(xpath+"/IEC61850[@name='"+mnode.attribute("name").as_string()+"']",Qt::UserRole);
            item0->setData(station,Qt::UserRole+STATION_NAME);
            item0->setData(SCD_NODE,Qt::UserRole+NODE_TYPE);
            model->appendRow(item0);
        }
    }else if(type==ROOT_NODE)
    {
        pugi::xml_object_range<pugi::xml_named_node_iterator>node_set= xnode.node().children("Substation");
        for(pugi::xml_named_node_iterator it = node_set.begin();it != node_set.end();it ++)
        {
            QStandardItem* item0 =new QStandardItem;
            item0->setText(it->attribute("name").as_string());
            item0->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            item0->setData(xpath,Qt::UserRole);

            item0->setIcon(iconSet);
            item0->setData(xpath+"/Substation[@name='"+it->attribute("name").as_string()+"']",Qt::UserRole);
            item0->setData(STATION_NODE,Qt::UserRole+NODE_TYPE);
            item0->setData(it->attribute("name").as_string(),Qt::UserRole+STATION_NAME);
            model->appendRow(item0);
        }
    }

}
QString ScdDelegate::get_validID(QString station)
{
    QMap<QString,DevInfo_struct> device_infos;
    if(!device_map.keys().contains(station))
    {
        device_map.insert(station,device_infos);
    }
    device_infos=device_map.value(station);
    QString sID;
    bool b_find=true;
    int i=1;
    while (b_find)
    {
        sID=tr("%1").arg(i, 5, 10, QLatin1Char('0'));
        if(!device_infos.keys().contains(sID))
            b_find=false;
        i++;
    }
    return sID;
}
void ScdDelegate::AddPath(QString name,QString ID,QString type,QString version,QString station,QString xpath,int node)
{
    switch (node) {
    case STATION_NODE:
        Add_Stationnode(name,xpath);
        break;
    case FOLDER_NODE:
        Add_Foldernode(name,station,xpath);
        break;
    case DEVICE_NODE:
        Add_Devicenode(name,ID,type,version,station,xpath);
        break;
    case SCD_NODE:
        Add_SCDnode(name,station,xpath);
        break;
    default:
        break;
    }


}
void ScdDelegate::Add_Devicenode(QString name,QString ID,QString type,QString version,QString station,QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(!xnode.node().empty())
    {
        pugi::xml_node item_node;
        if(xnode.node().child("Device").empty())
        {
            item_node=xnode.node().append_child("item");
        }else
        {
            item_node=xnode.node().insert_child_before("item",xnode.node().child("Device"));
        }

        item_node.append_attribute("name").set_value(name.toStdString().data());
        item_node.append_attribute("type").set_value("device");
        item_node.append_attribute("ID").set_value(ID.toStdString().data());
    }
    QString path=QString("./SCL/Project/Substation[@name='"+station+"']");
    xnode =m_doc->select_node(path.toStdString().data());
    if(!xnode.node().empty())
    {
        pugi::xml_node mnode;
        if(xnode.node().child("Device").empty())
        {
            xnode.node().append_child("Device");
        }
        mnode =xnode.node().child("Device").append_child("item");
        mnode.append_attribute("name").set_value(name.toStdString().data());
        mnode.append_attribute("ID").set_value(ID.toStdString().data());
        mnode.append_attribute("device").set_value(type.toStdString().data());
        mnode.append_attribute("ver").set_value(version.toStdString().data());
    }
}
void ScdDelegate::Add_SCDnode(QString name,QString station,QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(!xnode.node().empty())
    {
        xnode.node().append_child("IEC61850").append_attribute("name").set_value(name.toStdString().data());
    }
}
void ScdDelegate::Add_Foldernode(QString name,QString station,QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(!xnode.node().empty())
    {
        pugi::xml_node mnode= xnode.node().append_child("item");
        mnode.append_attribute("name").set_value(name.toStdString().data());
        mnode.append_attribute("type").set_value("folder");
    }
}
void ScdDelegate::Add_Stationnode(QString name,QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(!xnode.node().empty())
    {
        pugi::xml_node mnode= xnode.node().append_child("Substation");
        mnode.append_attribute("name").set_value(name.toStdString().data());
    }
}
void ScdDelegate::AddMap(QString station)
{
    QMap<QString,DevInfo_struct> device_infos;
    device_infos=device_map.value(station);
    QString path=QString("./SCL/Project/Substation[@name='"+station+"']"+"/Device");
    pugi::xpath_node xnode =m_doc->select_node(path.toStdString().data());
    if(!xnode.node().empty())
    {
        device_infos.clear();
        pugi::xpath_node_set xset =xnode.node().select_nodes("./item");
        for(pugi::xpath_node_set::const_iterator it = xset.begin();it != xset.end();it ++)
        {
            DevInfo_struct m_info;
            m_info.name =it->node().attribute("name").as_string();
            m_info.ID =it->node().attribute("ID").as_string();
            m_info.type =it->node().attribute("device").as_string();
            m_info.version =it->node().attribute("ver").as_string();
            device_infos.insert(m_info.ID,m_info);
        }
        device_map.insert(station,device_infos);
    }

}
bool ScdDelegate::isValidforder(QString name,QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());

    pugi::xpath_node mnode =xnode.node().select_node(QString("./item[@name='"+name+"']").toStdString().data());
    if(mnode.node().empty())
        return true;
    else
        return false;
}
bool ScdDelegate::isValidstation(QString name,QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());

    pugi::xpath_node mnode =xnode.node().select_node(QString(".//item[@Substation='"+name+"']").toStdString().data());
    if(mnode.node().empty())
        return true;
    else
        return false;
}

QString ScdDelegate::getitem(QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    QString name =xnode.node().name();
    return   name;
}
QString ScdDelegate::copysoure2target(QString source,QString target,QString name,int type)
{
    QString xpath;
    if(type!=STATION_NODE)
    {
        pugi::xpath_node source_node =m_doc->select_node(source.toStdString().data());
        pugi::xpath_node target_node =m_doc->select_node(target.toStdString().data());
        pugi::xml_node mnode=target_node.node().append_copy(source_node.node());
        mnode.attribute("name").set_value(name.toStdString().data());
        if(mnode.empty())
            return xpath;
        xpath=target+"/item[@name='"+mnode.attribute("name").as_string()+"']";
    }else
    {
        pugi::xpath_node source_node =m_doc->select_node(source.toStdString().data());
        pugi::xpath_node target_node =m_doc->select_node(target.toStdString().data());
        pugi::xml_node mnode;
        if(target_node.node().child("Device").empty())
            mnode=target_node.node().append_copy(source_node.node());
        else
            mnode=target_node.node().insert_copy_before(source_node.node(),target_node.node().child("Device"));
        mnode.attribute("name").set_value(name.toStdString().data());
        if(mnode.empty())
            return xpath;
        xpath=target+"/item[@name='"+mnode.attribute("name").as_string()+"']";    }

    return xpath;
}
QString ScdDelegate::movesoure2target(QString source,QString target,QString name,int type)
{
    QString xpath;
    pugi::xpath_node source_node =m_doc->select_node(source.toStdString().data());
    pugi::xpath_node target_node =m_doc->select_node(target.toStdString().data());
    pugi::xml_node mnode;
    if(target_node.node().child("Device").empty())
        mnode=target_node.node().append_move(source_node.node());
    else
        mnode=target_node.node().insert_move_before(source_node.node(),target_node.node().child("Device"));
    mnode.attribute("name").set_value(name.toStdString().data());
    if(mnode.empty())
        return xpath;
    xpath=target+"/item[@name='"+mnode.attribute("name").as_string()+"']";

    return xpath;
}

QString ScdDelegate::updateID(QString oldID,QString newID,QString station,QString xpath,QString name)
{
    qDebug()<<xpath;
    pugi::xpath_node x_node =m_doc->select_node(xpath.toStdString().data());
    if(!x_node.node().empty())
    {
        pugi::xpath_node dev_node =x_node.node().select_node(QString(".//item[@ID='"+oldID+"']").toStdString().data());
        if(!dev_node.node().empty())
        {
            dev_node.node().attribute("ID").set_value(newID.toStdString().data());
            dev_node.node().attribute("name").set_value(name.toStdString().data());

            //            QString path =QString("./SCL/Project/Substation[@name='"+station+"']"+"/Device/item[@ID='"+oldID+"']");
            //            dev_node =m_doc->select_node(path.toStdString().data());
            //            if(!dev_node.node().empty())
            //            {
            //                dev_node.node().attribute("ID").set_value(newID.toStdString().data());
            //            }
        }
    }
    return xpath;
}
QString ScdDelegate::updateID(QString newID,QString station,QString xpath,QString name)
{
    pugi::xpath_node x_node =m_doc->select_node(xpath.toStdString().data());
    if(!x_node.node().empty())
    {
        x_node.node().attribute("ID").set_value(newID.toStdString().data());
        x_node.node().attribute("name").set_value(name.toStdString().data());
    }
    return xpath;
}
QString ScdDelegate::updatedevice(QString ID,QString station,DevInfo_struct info)
{
    QString xpath=QString("./SCL/Project/Substation[@name='"+station+"']"+"/Device");
    pugi::xpath_node dev_node =m_doc->select_node(xpath.toStdString().data());
    if(!dev_node.node().empty())
    {
        pugi::xpath_node item_node =dev_node.node().select_node(QString(".//item[@ID='"+ID+"']").toStdString().data());
        if(!info.name.isEmpty())
            item_node.node().attribute("name").set_value(info.name.toStdString().data());
        if(!info.ID.isEmpty())
            item_node.node().attribute("ID").set_value(info.ID.toStdString().data());
        if(!info.type.isEmpty())
            item_node.node().attribute("device").set_value(info.type.toStdString().data());
        if(!info.version.isEmpty())
            item_node.node().attribute("ver").set_value(info.version.toStdString().data());
    }
}

QString ScdDelegate::copyDevice(QString oldID,QString oldStation,QString newname,QString newID,QString newStation)
{
    QString xpath;
    if(device_map.keys().contains(oldStation))
    {
        QMap<QString,DevInfo_struct> dev_infos;
        dev_infos =device_map.value(oldStation);
        if(dev_infos.keys().contains(oldID))
        {
            DevInfo_struct m_info =dev_infos.value(oldID);
            m_info.ID=newID;
            m_info.name=newname;
            xpath=QString("./SCL/Project/Substation[@name='"+newStation+"']"+"/Device");
            pugi::xpath_node dev_node =m_doc->select_node(xpath.toStdString().data());
            if(!dev_node.node().empty())
            {
                pugi::xml_node mnode=dev_node.node().append_child("item");
                mnode.append_attribute("name").set_value(m_info.name.toStdString().data());
                mnode.append_attribute("ID").set_value(m_info.ID.toStdString().data());
                mnode.append_attribute("device").set_value(m_info.type.toStdString().data());
                mnode.append_attribute("ver").set_value(m_info.version.toStdString().data());
            }
        }
    }
    AddMap(newStation);
    return xpath+"/item[@ID='"+newID+"']";
}
QString ScdDelegate::copyDevice(QString oldID,QString oldStation,QString newID,QString newStation)
{
    QString xpath;
    if(device_map.keys().contains(oldStation))
    {
        QMap<QString,DevInfo_struct> dev_infos;
        dev_infos =device_map.value(oldStation);
        if(dev_infos.keys().contains(oldID))
        {
            DevInfo_struct m_info =dev_infos.value(oldID);
            m_info.ID=newID;
            xpath=QString("./SCL/Project/Substation[@name='"+newStation+"']"+"/Device");
            pugi::xpath_node dev_node =m_doc->select_node(xpath.toStdString().data());
            if(dev_node.node().empty())
            {
                pugi::xpath_node station_node =m_doc->select_node(QString("./SCL/Project/Substation[@name='"+newStation+"']").toStdString().data());
                station_node.node().append_child("Device");
                dev_node =m_doc->select_node(xpath.toStdString().data());
            }
            if(!dev_node.node().empty())
            {
                pugi::xml_node mnode=dev_node.node().append_child("item");
                mnode.append_attribute("name").set_value(m_info.name.toStdString().data());
                mnode.append_attribute("ID").set_value(m_info.ID.toStdString().data());
                mnode.append_attribute("device").set_value(m_info.type.toStdString().data());
                mnode.append_attribute("ver").set_value(m_info.version.toStdString().data());
            }
        }
    }
    AddMap(newStation);
    return xpath+"/item[@ID='"+newID+"']";
}
bool ScdDelegate::removeDevice(QString oldID,QString oldStation)
{
    QString xpath=QString("./SCL/Project/Substation[@name='"+oldStation+"']"+"/Device");
    pugi::xpath_node station_node =m_doc->select_node(xpath.toStdString().data());
    if(!station_node.node().empty())
    {
        pugi::xpath_node dev_node =station_node.node().select_node(QString(".//item[@ID='"+oldID+"']").toStdString().data());
        if(!dev_node.node().empty())
        {
            dev_node.parent().remove_child(dev_node.node());
            return true;
        }
    }
    return false;
}

QString ScdDelegate::getitemID(QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    QString ID =xnode.node().attribute("ID").as_string();
    return   ID;
}
QStringList ScdDelegate::getitemIDs(QString xpath)
{
    QStringList IDs;
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(!xnode.node().empty())
    {
        pugi::xpath_node_set xset =xnode.node().select_nodes(".//item[@type='device']");
        for(pugi::xpath_node_set::const_iterator it = xset.begin();it != xset.end();it ++)
        {
            IDs.append(it->node().attribute("ID").as_string());
        }
    }
    return IDs;
}
QString ScdDelegate::getitemName(QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    QString name =xnode.node().attribute("name").as_string();
    return   name;
}

int ScdDelegate::getitemType(QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(strcmp(xnode.node().name(),"Substation")==0)
        return STATION_NODE;
    if(strcmp(xnode.node().name(),"IEC61850")==0)
        return SCD_NODE;
    if(strcmp(xnode.node().attribute("type").as_string(),"device")==0)
        return DEVICE_NODE;
    if(strcmp(xnode.node().attribute("type").as_string(),"folder")==0)
        return FOLDER_NODE;
}
bool ScdDelegate::isExistSCD(QString station)
{
    pugi::xpath_node station_node =m_doc->select_node(QString("./SCL/Project/Substation[@name='"+station+"']").toStdString().data());
    if(station_node.node().child("IEC61850").empty())
        return false;

    return true;
}
bool ScdDelegate::getitemInfo(Item_struct &item,QString xpath)
{
    pugi::xpath_node item_node =m_doc->select_node(xpath.toStdString().data());
    if(!item_node.node().empty())
    {
        item.name=item_node.node().attribute("name").as_string();
        item.ID=item_node.node().attribute("ID").as_string();
        item.type=item_node.node().attribute("type").as_string();
        item.author=item_node.node().attribute("author").as_string();
        item.commit=item_node.node().attribute("commit").as_string();
        item.IP=item_node.node().attribute("IP").as_string();
        item.Address=item_node.node().attribute("address").as_string();

        return true;
    }
    return false;

}
bool ScdDelegate::setitemInfo(Item_struct &item,QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(!xnode.node().empty())
    {
        xnode.node().attribute("name").set_value(item.name.toStdString().data());
        if(xnode.node().attribute("author").empty())
            xnode.node().append_attribute("author");
        xnode.node().attribute("author").set_value(item.author.toStdString().data());
        if(xnode.node().attribute("commit").empty())
            xnode.node().append_attribute("commit");
        xnode.node().attribute("commit").set_value(item.commit.toStdString().data());
        if(!item.IP.isEmpty())
        {
            if(xnode.node().attribute("IP").empty())
                xnode.node().append_attribute("IP");
            xnode.node().attribute("IP").set_value(item.IP.toStdString().data());
        }
        if(!item.Address.isEmpty())
        {
            if(xnode.node().attribute("address").empty())
                xnode.node().append_attribute("address");
            xnode.node().attribute("address").set_value(item.Address.toStdString().data());
        }
        return true;
    }
    return  false;
}
bool ScdDelegate::isExistItem(QString xpath)
{
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(xnode.node().empty())
        return false;
    return true;
}

QString ScdDelegate::getcreatetime()
{
    QString value;
    QString xpath ="./SCL/Project";
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(!xnode.node().empty())
        value=xnode.node().attribute("create").as_string();
    return value;
}
QString ScdDelegate::getmodifytime()
{
    QString value;
    QString xpath ="./SCL/Project";
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(!xnode.node().empty())
        value=xnode.node().attribute("modify").as_string();
    return value;
}
void  ScdDelegate::setmodifytime(QString value)
{
    QString xpath ="./SCL/Project";
    pugi::xpath_node xnode =m_doc->select_node(xpath.toStdString().data());
    if(!xnode.node().empty())
    {
        if(xnode.node().attribute("modify").empty())
            xnode.node().append_attribute("modify").set_value(value.toStdString().data());
         else
            xnode.node().attribute("modify").set_value(value.toStdString().data());

    }
}
void  ScdDelegate::getDevByIDfromStation(DevInfo_struct& info,QString ID,QString station)
{
    if(device_map.keys().contains(station))
    {
        QMap<QString,DevInfo_struct> infos=device_map.value(station);
        if(infos.keys().contains(ID))
        {
            info.ID=infos.value(ID).ID;
            info.name=infos.value(ID).name;
            info.type=infos.value(ID).type;
            info.version=infos.value(ID).version;

        }
    }
}
