#ifndef SCDDELEGATE_H
#define SCDDELEGATE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSet>
#include <QMutex>
#include <QThread>

#include "pugixml.hpp"
#include "global.h"
class ScdDelegate : public QThread
{
    Q_OBJECT
public:
    ScdDelegate();
    virtual ~ScdDelegate();
    void                newPro(QString FileName);
    bool                loadPro(QString FileName);
    void                savePro(QString FileName = QString());
    void                list_substation(QStandardItemModel *tree);
    void                add_folder(QStandardItem* item,QString xpath,QString station,pugi::xml_node mnode);
    void                deleteNode(QString xpath);
    void                deleteDevUnderstation(QString ID,QString station);
    QStringList         get_devUnderNode(QString xpath);
    void                list_tablview(QString xpath,QStandardItemModel *model,QString station,int type);
    QString             get_validID(QString station);
    void                AddPath(QString name,QString ID,QString type,QString version,QString station,QString xpath,int node);
    void                Add_Devicenode(QString name,QString ID,QString type,QString version,QString station,QString xpath);
    void                Add_SCDnode(QString name,QString station,QString xpath);
    void                Add_Foldernode(QString name,QString station,QString xpath);
    void                Add_Stationnode(QString name,QString xpath);
    void                AddMap(QString station);
    bool                isValidforder(QString name,QString xpath);
    bool                isValidstation(QString name,QString xpath);
    QString             getitem(QString xpath);
    QString             copysoure2target(QString source,QString target,QString name,int type);
    QString             movesoure2target(QString source,QString target,QString name,int type);
    QString             updateID(QString oldID,QString newID,QString station,QString xpath,QString name);
    QString             updateID(QString newID,QString station,QString xpath,QString name);
    QString             updatedevice(QString newID,QString station,DevInfo_struct info);
    QString             copyDevice(QString oldID,QString oldStation,QString newname,QString newID,QString newStation);
    QString             copyDevice(QString oldID,QString oldStation,QString newID,QString newStation);
    bool                removeDevice(QString oldID,QString oldStation);
    QString             getitemID(QString xpath);
    QStringList         getitemIDs(QString xpath);
    QString             getitemName(QString xpath);
    int                 getitemType(QString xpath);
    bool                isExistSCD(QString station);
    bool                getitemInfo(Item_struct &item,QString xpath);
    bool                setitemInfo(Item_struct &item,QString xpath);
    QString                getcreatetime();
    QString                getmodifytime();
    void                   setmodifytime(QString);
    void                   getDevByIDfromStation(DevInfo_struct&,QString ID,QString station);
private:
    bool m_cancel;
    bool m_changed;
    QMutex* m_mutex;
    pugi::xml_document* m_doc;
    QIcon iconSet;         //
    QIcon iconDev;         //
    QIcon iconSCD;         //
    QMap<QString,QMap<QString,DevInfo_struct>> device_map;

};

#endif // SCDDELEGATE_H
