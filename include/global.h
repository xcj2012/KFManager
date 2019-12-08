#ifndef GLOBAL_H
#define GLOBAL_H
#include <QStandardItemModel>
#include <QDir>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include "pugixml/src/pugiconfig.hpp"
#include "pugixml/src/pugixml.hpp"
#include <QCryptographicHash>
#include <QtDebug>
#include <QByteArray>
#include <QDirIterator>
#include <QProcess>
#include <QInputDialog>
#include <QDateTime>
#include <QDockWidget>
#include <QTableView>
#include <QMouseEvent>
#define LIB_SHOW    0x01
#define LIB_MANAGE  0x02
#define DEV_MANAGE  0x02
#define PRO_MANAGE  0x08
#define LIB_DEV 0x03
#define LIB_VER 0x04

#define MODE_LIB 0x01
#define MODE_DEV (MODE_LIB+1)
#define MODE_PRO (MODE_LIB+2)
#define MODE_DEV2LIB (MODE_LIB+3)
#define PRO_DEV 0x05
#define PRO_PRO 0x06
#define PRO_DEV2LIB 0x07

#define ZIP_UPDATE   0x01
#define ZIP_ARCHIVE   0x02
#define ZIP_RETRIEVE   0x03
#define USER_CUSTOMER 0x01
#define USER_ENGINEER 0x02
#define USER_DEVELOPER 0x04
#define  Copy_NODE 0x01
#define  Cut_NODE 0x02
#define  No_NODE 0x00
#define  OBJECT_PROJECT 0x01
#define  OBJECT_STATION 0x02
#define  OBJECT_DEVICE 0x03

#define HASH_FILE_NAME  "hash.xml"
#define DEV_FILE_NAME  "Device.xml"
#define LIB_FILE_NAME  "Library.xml"
#define PRO_FILE_NAME  "Project.xml"
#define LIBRARY_PATH  "Templates"

#define  ROOT_NODE 0x01
#define  STATION_NODE 0x02
#define  FOLDER_NODE 0x04
#define  DEVICE_NODE 0x08
#define  SCD_NODE 0x10
#define  STATION_NAME   (SCD_NODE+1)
#define  DEVICE_KEY   (SCD_NODE+2)
#define  NODE_TYPE 0x01
#define  TREEVIEW_TYPE 0x01
#define  TABVIEW_TYPE 0x02
#define  LIST_MODE_ALL  0x03
#define  LIST_MODE_DIR  0x01
#define  LIST_MODE_FILE  0x00
#define  LIST_MODE_DIR_TOP  0x05
extern QStringList g_logs;
extern int  g_User_permissions;
extern bool g_delete_Project(QString name,QString path);
extern bool g_check_project(QString name,QString path);
extern bool g_delete_dir(QString path);
extern QByteArray  g_get_MD5(QString path);
extern QByteArray  g_get_MD5(QByteArray data);
extern QStringList g_getfiles(const QString &dir_path,int mode=LIST_MODE_FILE);
extern void ErrorMessage(QString);
struct DevInfo_struct
{
    QString name;
    QString ID;
    QString type;
    QString version;
    DevInfo_struct()
    {
        name.clear();
        ID.clear();
        type.clear();
        version.clear();

    }
};
struct Item_struct
{
    QString name;
    QString ID;
    QString type;
    QString IP;
    QString Address;
    QString author;
    QString commit;
};
bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);
bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist);
extern QDir g_dir;
extern QString g_Template_path;

#endif // GLOBAL_H
