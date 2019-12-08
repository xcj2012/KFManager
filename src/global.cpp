#include "global.h"
int  g_User_permissions=USER_CUSTOMER;
QDir g_dir;
QStringList g_logs;
QString g_Template_path;
bool g_delete_Project(QString name,QString path)
{

    QFile file(path+QDir::separator()+name+".KFpro");
    if(file.exists())
        file.remove();
    if(file.exists())
        return false;
    if(!g_delete_dir(path+QDir::separator()+name));
        return false;
    return true;
}
bool g_check_project(QString name,QString path)
{

    return true;
}

bool g_delete_dir(QString path)
{
    QDir dir(path);
    if(dir.exists(path))
    {
        dir.removeRecursively();
    }
    if(dir.exists(path))
        return false;
    return true;
}
QByteArray  g_get_MD5(QString path)
{
    QFile theFile(path);
    theFile.open(QIODevice::ReadOnly);
    QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
    theFile.close();
    qDebug() <<path<< ba.toHex().constData();
    return ba;
}
QByteArray  g_get_MD5(QByteArray data)
{
    QByteArray ba = QCryptographicHash::hash(data, QCryptographicHash::Md5);
    return ba.toHex();

}
QStringList g_getfiles(const QString &dir_path,int mode)
{
    QStringList get_files;
    QDir dir(dir_path);
    if(!dir.exists())
    {
        qDebug() << "it is not true dir_path";
    }
    /*设置过滤参数，QDir::NoDotAndDotDot表示不会去遍历上层目录*/
    if(mode==LIST_MODE_ALL)
    {
        QDirIterator dir_iterator(dir_path, QDir::Dirs | QDir::NoDotAndDotDot);
        while(dir_iterator.hasNext())
        {
            dir_iterator.next();
            QFileInfo file_info = dir_iterator.fileInfo();
            QString files = file_info.absoluteFilePath();
            get_files.append(files);
        }
    }else
    {
        if(mode==LIST_MODE_DIR_TOP)
        {
            QDirIterator dir_iterator(dir_path, QDir::Dirs | QDir::NoDotAndDotDot);
            while(dir_iterator.hasNext())
            {
                dir_iterator.next();
                QFileInfo file_info = dir_iterator.fileInfo();
                QString files = file_info.absoluteFilePath();
                get_files.append(files);
            }
        }else
        {
            QDirIterator dir_iterator(dir_path,QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,QDirIterator::Subdirectories);
            while(dir_iterator.hasNext())
            {
                dir_iterator.next();
                QFileInfo file_info = dir_iterator.fileInfo();
                if(mode==LIST_MODE_FILE)
                {
                    if(file_info.isDir())
                        continue;
                }
                if(mode==LIST_MODE_DIR)
                {
                    if(file_info.isFile())
                        continue;
                }
                QString files = file_info.absoluteFilePath();
                get_files.append(files);
            }
        }

    }

    return get_files;
}
extern void ErrorMessage(QString err)
{
    g_logs.append(err);
}
bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}
bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkpath(targetDir.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */
            if(!copyDirectoryFiles(fileInfo.filePath(),
                                   targetDir.filePath(fileInfo.fileName()),
                                   coverFileIfExist))
                return false;
        }
        else{            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            if(coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                targetDir.remove(fileInfo.fileName());
            }

            /// 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                            targetDir.filePath(fileInfo.fileName()))){
                return false;
            }
        }
    }
    return true;
}


