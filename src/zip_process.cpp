#include "zip_process.h"
#include "ui_zip_process.h"


Zip_process::Zip_process(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Zip_process)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    init_connect();
    ui->pushButton_Ok->setEnabled(false);
}

Zip_process::~Zip_process()
{
    delete ui;
}
void Zip_process::init_connect()
{
    connect(ui->pushButton_Ok,SIGNAL(clicked()),this,SLOT(on_pushbutton_Ok()));
    connect(ui->pushButton_Cancel,SIGNAL(clicked()),this,SLOT(on_pushbutton_Cancel()));
    connect(ui->pushButton_Help,SIGNAL(clicked()),this,SLOT(on_pushbutton_Help()));
    connect(ui->pushButton_Browse,SIGNAL(clicked()),this,SLOT(on_pushbutton_Browse()));
    connect(this,SIGNAL(emit_process_index(int)),this,SLOT(on_process_index(int)));
}
void Zip_process::archive_Pro2Pro(QString source,QString dest)
{
    QFileInfo info(source);
    QString basename =info.baseName();
    QString path =info.path();
    QFile file(source+".KFpro");
    if(!file.exists())
        return;
    QStringList list =g_getfiles(source,1);
    //list.append(source+".KFpro");
    ui->progressBar->setRange(0,list.size()+1);
    try
    {
        zipFile zf = NULL;
#ifdef WINDOWS_32
        path =path.replace("/","\\");
#endif
        zf = zipOpen(dest.toLocal8Bit().data(),APPEND_STATUS_CREATE);
        if(zf==NULL)
        {
            ui->label_Note->setText(tr("Archive false"));
            return ;
        }
        zip_fileinfo FileInfo;
        memset(&FileInfo,0,sizeof(zip_fileinfo));
        int ret;
        for (int i = 0; i < list.size(); i ++)
        {
            QString key =list.at(i);
            QFile m_file(key);
            info.setFile(key);
            if(info.isFile())
            {
                if(!m_file.open(QIODevice::ReadOnly))
                {
                    ui->label_Note->setText(tr("Archive false"));
                    return ;
                }
            }

            QByteArray arry =m_file.readAll();
            QString temp =key.remove(path+QDir::separator());
            if(!info.isFile())
                temp+=QDir::separator();
            //temp=info.fileName()+QDir::separator()+temp;
            zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
            if(info.isFile())
            {
                uLong readlen =arry.size();
                if(readlen<=0)
                {
                    ui->label_Note->setText(tr("Archive false"));
                    return ;
                }
                ret = zipWriteInFileInZip(zf,arry.data(),readlen);
                if(ret != ZIP_OK )
                {

                    zipCloseFileInZip(zf);
                    zipClose(zf,NULL);
                    {
                        ui->label_Note->setText(tr("Archive false"));
                        return ;
                    }
                }
            }

            zipCloseFileInZip(zf);
            emit emit_process_index(i+1);

        }
        QFile m_file(source+".KFpro");
        info.setFile(source+".KFpro");
        QString temp =PRO_FILE_NAME;
        zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
        m_file.open(QIODevice::ReadOnly);
        QByteArray arry =m_file.readAll();
        uLong readlen =arry.size();
        if(readlen<=0)
        {
            ui->label_Note->setText(tr("Archive false"));
            return ;
        }
        ret = zipWriteInFileInZip(zf,arry.data(),readlen);
        if(ret != ZIP_OK )
        {

            zipCloseFileInZip(zf);
            zipClose(zf,NULL);
            {
                ui->label_Note->setText(tr("Archive false"));
                return ;
            }
        }
        zipCloseFileInZip(zf);
        emit emit_process_index(list.size()+1);
        zipClose(zf,NULL);
    }
    catch(...)
    {
        {
            ui->label_Note->setText(tr("Archive false"));
            return ;
        }
    }
    ui->label_Note->setText(tr("Archive success"));
}
void Zip_process::archive_Device2Lib(QString source,QString dest)
{

}
void Zip_process::archive_Lib2Lib(QString source,QString dest)
{
    QFileInfo info(source);
    QStringList list =g_getfiles(source);
    QByteArray md5_arry;
    ui->progressBar->setRange(0,list.size());
    try
    {
        zipFile zf = NULL;
#ifdef WINDOWS_32
        path =path.replace("/","\\");
#endif
        zf = zipOpen(dest.toLocal8Bit().data(),APPEND_STATUS_CREATE);
        if(zf==NULL)
        {
            ui->label_Note->setText(tr("Archive false"));
            return ;
        }
        zip_fileinfo FileInfo;
        memset(&FileInfo,0,sizeof(zip_fileinfo));
        int ret;
        for (int i = 0; i < list.size(); i ++)
        {
            QString key =list.at(i);
            QFile m_file(key);
            if(!m_file.open(QIODevice::ReadOnly))
            {
                ui->label_Note->setText(tr("Archive false"));
                return ;
            }
            QByteArray arry =m_file.readAll();
            QString temp =key.remove(source+QDir::separator());
            QFileInfo m_info(key);
            if(m_info.isDir())
                temp+=QDir::separator();
            //temp=info.fileName()+QDir::separator()+temp;
            zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
            uLong readlen =arry.size();
            if(readlen<=0)
            {
                ui->label_Note->setText(tr("Archive false"));
                return ;
            }
            ret = zipWriteInFileInZip(zf,arry.data(),readlen);
            if(ret != ZIP_OK )
            {

                zipCloseFileInZip(zf);
                zipClose(zf,NULL);
                {
                    ui->label_Note->setText(tr("Archive false"));
                    return ;
                }            }
            zipCloseFileInZip(zf);
            emit emit_process_index(i+1);

        }
        zipClose(zf,NULL);
    }
    catch(...)
    {
        {
            ui->label_Note->setText(tr("Archive false"));
            return ;
        }
    }
}
void Zip_process::archive_Device2Device_PRO(QString source,QString dest)
{

}
void Zip_process::archive_Device2Device_Lib(QString source,QString dest)
{
    QFileInfo info(source);
    QStringList list =g_getfiles(source);
    QByteArray md5_arry;
    ui->progressBar->setRange(0,list.size()+1);

    pugi::xml_document doc;
    pugi::xml_node xdec = doc.prepend_child(pugi::node_declaration);
    xdec.append_attribute("version").set_value("1.0");
    xdec.append_attribute("encoding").set_value("utf-8");
    pugi::xml_node xnode = doc.prepend_child(pugi::node_element);
    xnode.set_name("SCL");
    xnode.append_attribute("Device").set_value(info.fileName().toStdString().data());
    pugi::xml_node file_node=xnode.append_child("File");
    for (int i = 0; i < list.size(); i ++)
    {
        pugi::xml_node item_node=file_node.append_child("item");
        QString filename =list.at(i);
        QByteArray ba=g_get_MD5(filename);
        QString temp =filename.remove(source+QDir::separator());
        //temp=info.fileName()+QDir::separator()+temp;
        item_node.append_child("name").text().set(temp.toStdString().data());
        item_node.append_child("MD5").text().set(QString::fromLocal8Bit(ba.toHex()).toStdString().data());
    }
    xml_string_writer m_data1;
    doc.print(m_data1);
    md5_arry.append(QString::fromStdString(m_data1.xml).toLocal8Bit());
    //zip device to zip file add one xml to include the all sub file;

    try
    {
        zipFile zf = NULL;
#ifdef WINDOWS_32
        path =path.replace("/","\\");
#endif
        zf = zipOpen(dest.toLocal8Bit().data(),APPEND_STATUS_CREATE);
        if(zf==NULL)
        {
            ui->label_Note->setText(tr("Archive false"));
            return ;
        }
        zip_fileinfo FileInfo;
        memset(&FileInfo,0,sizeof(zip_fileinfo));
        int ret;
        for (int i = 0; i < list.size(); i ++)
        {
            QString key =list.at(i);
            QFile m_file(key);
            if(!m_file.open(QIODevice::ReadOnly))
            {
                ui->label_Note->setText(tr("Archive false"));
                return ;
            }
            QByteArray arry =m_file.readAll();
            QString temp =key.remove(source+QDir::separator());
            QFileInfo m_info(key);
            if(m_info.isDir())
                temp+=QDir::separator();
            //temp=info.fileName()+QDir::separator()+temp;
            zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
            uLong readlen =arry.size();
            if(readlen<=0)
            {
                ui->label_Note->setText(tr("Archive false"));
                return ;
            }
            ret = zipWriteInFileInZip(zf,arry.data(),readlen);
            if(ret != ZIP_OK )
            {

                zipCloseFileInZip(zf);
                zipClose(zf,NULL);
                {
                    ui->label_Note->setText(tr("Archive false"));
                    return ;
                }            }
            zipCloseFileInZip(zf);
            emit emit_process_index(i+1);

        }
        QString temp=LIB_FILE_NAME;
        zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
        uLong readlen =md5_arry.size();
        if(readlen<=0)
        {
            ui->label_Note->setText(tr("Archive false"));
            return ;
        }
        ret = zipWriteInFileInZip(zf,md5_arry.data(),readlen);
        if(ret != ZIP_OK )
        {

            zipCloseFileInZip(zf);
            zipClose(zf,NULL);
            {
                ui->label_Note->setText(tr("Archive false"));
                return ;
            }            }
        zipCloseFileInZip(zf);
        emit emit_process_index(list.size()+1);

        zipClose(zf,NULL);
    }
    catch(...)
    {
        {
            ui->label_Note->setText(tr("Archive false"));
            return ;
        }
    }
    ui->label_Note->setText(tr("Archive success"));
}
void Zip_process::archive_source2dest(QString source,QString dest,int mode)
{
    if(mehtod==LIB_MANAGE)
    {
        if(mode==LIB_DEV)
        {
            archive_Device2Device_Lib(source,dest);
        }
        if(mode==LIB_VER)
        {
            archive_Lib2Lib(source,dest);
        }
    }
    if(mehtod==PRO_MANAGE)
    {
        if(mode==PRO_DEV2LIB)
        {
            archive_Device2Lib(source,dest);
        }
        if(mode==PRO_DEV)
        {
            archive_Device2Device_PRO(source,dest);
        }
        if(mode==PRO_PRO)
        {
            archive_Pro2Pro(source,dest);
        }
    }

    return ;
}


void Zip_process::retrieve_Pro2Pro(QString source,QString dest)
{
    unzFile unzfile = unzOpen(source.toLocal8Bit().data());
    QMap<QString,QByteArray> data_map;
    if(unzfile == NULL)
    {
        ui->label_Note->setText(tr("open the zip file false!"));
        return;
    }
    //read info of
    unz_global_info* pGlobalInfo = new unz_global_info;
    int nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
    if(nReturnValue != UNZ_OK)
    {
        ui->label_Note->setText(tr("open the zip file false!"));
        return;
    }
    //read data from main.os
    unz_file_info* pFileInfo = new unz_file_info;
    char szZipFName[MAX_PATH];
    ui->progressBar->setRange(0,pGlobalInfo->number_entry);
    for(int i=0; i<pGlobalInfo->number_entry; i++)
    {
        nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH,
                                             NULL, 0, NULL, 0);          //analyse the infomation
        if(nReturnValue != UNZ_OK)
        {
            ui->label_Note->setText(tr("open the zip file false!"));
            return;
        }
        switch((pFileInfo->external_fa&FILE_ATTRIBUTE_DIR))//is dir or file
        {
        case FILE_ATTRIBUTE_DIR:                    //
        {
            QByteArray m_arry;
            data_map.insert(szZipFName,m_arry);
            break;
        }
        default:                                        //文件
        {

            nReturnValue = unzOpenCurrentFile(unzfile); //open file
            if(nReturnValue != UNZ_OK)
            {
                ui->label_Note->setText(tr("open the zip file false!"));
                return;
            }
            //read file
            QByteArray m_arry;
            const int BUFFER_SIZE = 4096;
            char szReadBuffer[BUFFER_SIZE];
            while(true)
            {
                memset(szReadBuffer, 0, BUFFER_SIZE);
                int nReadFileSize = unzReadCurrentFile(unzfile, szReadBuffer, BUFFER_SIZE);
                if(nReadFileSize < 0)                //
                {
                    ui->label_Note->setText(tr("open the zip file false!"));
                    int val =unzCloseCurrentFile(unzfile);
                    return;
                }
                else if(nReadFileSize == 0)            //
                {
                    int val =unzCloseCurrentFile(unzfile);
                    data_map.insert(szZipFName,m_arry);
                    emit emit_process_index(i+1);
                    break;
                }
                else                                //
                {
                    m_arry.append(szReadBuffer,nReadFileSize);
                }
            }
        }
            break;
        }
        unzGoToNextFile(unzfile);
    }
    //Close
    if(unzfile)
        unzClose(unzfile);
    pugi::xml_document m_doc;
    pugi::xml_parse_result ret =m_doc.load_string((data_map.value(PRO_FILE_NAME)).data());
    if(ret.status!=pugi::status_ok)
    {
        ui->label_Note->setText(tr("Read file information for zip file false"));
        return;
    }
    QString xpath="/SCL/Project";
    pugi::xpath_node xnode =m_doc.select_node(xpath.toStdString().data());
    if(xnode.node().empty())
    {
        ui->label_Note->setText(tr("Read file information for zip file false"));
        return;
    }
    QString name =QString(xnode.node().attribute("name").as_string());
    QDir dir(dest+QDir::separator()+name);
    if(dir.exists())
    {
        int ret=QMessageBox::information(this,tr("Information"),tr("The project :")+name+tr("is exist\nDo you want to rewrite it?")
                                         ,QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        if(ret==QMessageBox::No)
            return;
        g_delete_Project(name,dest);
    }
    dir.mkpath(dest+QDir::separator()+name);
    xpath="/SCL/Project/File/item";
    pugi::xpath_node_set xnodes =m_doc.select_nodes(xpath.toStdString().data());
    pugi::xml_node item_node;
    for(pugi::xpath_node_set::const_iterator it = xnodes.begin();it != xnodes.end();it ++)
    {

    }
}
void Zip_process::retrieve_source2dest(QString source,QString dest)
{

}
void Zip_process::setupwidget()
{
    switch (type) {
    case ZIP_UPDATE:
        setWindowTitle(tr("Update library of device"));
        ui->pushButton_Help->setProperty("name","Update library");
        break;
    case ZIP_ARCHIVE:
        setWindowTitle(tr("Archive object"));
        ui->pushButton_Help->setProperty("name","Archive object");
        break;
    case ZIP_RETRIEVE:
        setWindowTitle(tr("Retrieve object"));
        ui->pushButton_Help->setProperty("name","Retrieve object");
        break;
    default:
        break;
    }
}
void Zip_process::setsource(QString value)
{
    source=value;
}
void Zip_process::setdest(QString value)
{
    dest =value;
}
void Zip_process::setlibrary(QString value)
{
    library=value;
}
void Zip_process::setdevice(QString value)
{
    device =value;
}
void Zip_process::setversion(QString value)
{
    version =value;
}
void Zip_process::setlibpath(QString value)
{
    libpath =value;
}
void Zip_process::setdatabse(QString value)
{
    database =value;
}
void Zip_process::settype(int value)
{
    type=value;
}
void Zip_process::setmode(int value)
{
    mode=value;
}
void Zip_process::setmethod(int value)
{
    mehtod =value;
}
QString Zip_process::getlibrary()
{
    return  library;
}
QString Zip_process::getversion()
{
    return version;
}
QString Zip_process::getdatabse()
{
    return database;
}
int Zip_process::unzip2map(QString source,QMap<QString,QByteArray>&map)
{
    unzFile unzfile = unzOpen(source.toLocal8Bit().data());
    unz_global_info* pGlobalInfo = new unz_global_info;
    unz_file_info* pFileInfo = new unz_file_info;
    char szZipFName[MAX_PATH];
    unzGetGlobalInfo(unzfile, pGlobalInfo);
    for(int i=0; i<pGlobalInfo->number_entry; i++)
    {
        int nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH,
                                                 NULL, 0, NULL, 0);
        if(nReturnValue != UNZ_OK)
        {
            return ZIP_RETURN_FILE_IO_ERROR;
        }
        //is dir or file
        switch((pFileInfo->external_fa&FILE_ATTRIBUTE_DIR))
        {
        case FILE_ATTRIBUTE_DIR:                    //
        {
            QByteArray m_arry;
            map.insert(szZipFName,m_arry);
            emit emit_process_index(i+1);
        }
            break;
        default:                                        //文件
        {
            //打开文件
            nReturnValue = unzOpenCurrentFile(unzfile);
            if(nReturnValue != UNZ_OK)
            {
                return ZIP_RETURN_FILE_IO_ERROR;
            }

            //read file
            QByteArray m_arry;
            const int BUFFER_SIZE = 4096;
            char szReadBuffer[BUFFER_SIZE];
            while(true)
            {
                memset(szReadBuffer, 0, BUFFER_SIZE);
                int nReadFileSize = unzReadCurrentFile(unzfile, szReadBuffer, BUFFER_SIZE);
                if(nReadFileSize < 0)                //
                {
                    int val =unzCloseCurrentFile(unzfile);
                    unzClose(unzfile);
                    return ZIP_RETURN_FILE_IO_ERROR;
                }
                else if(nReadFileSize == 0)            //
                {
                    int val =unzCloseCurrentFile(unzfile);
                    map.insert(szZipFName,m_arry);
                    emit emit_process_index(i+1);
                    break;
                }
                else                                //
                {
                    m_arry.append(szReadBuffer,nReadFileSize);
                }
            }
        }
            break;
        }
        unzGoToNextFile(unzfile);
    }
    //Close
    if(unzfile)
    {
        int val =unzClose(unzfile);
        qDebug()<<val;
    }
    return ZIP_RETURN_OK;
}
int Zip_process::lib_import()
{
    unzFile unzfile = unzOpen(source.toLocal8Bit().data());
    QMap<QString,QByteArray> data_map;
    if(unzfile == NULL)
    {
        QString error =tr("Import library error, caused by library file  open error, filename :%1").arg(source).append("File:")
                .append(__FILE__).append("Line").append(__LINE__);
        ErrorMessage(error);
        return ZIP_RETURN_FILE_NOT_FOUND;
    }
    unz_global_info* pGlobalInfo = new unz_global_info;
    int nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
    if(nReturnValue != UNZ_OK)
    {
        QString error =tr("Import library error, caused by library file  open error, filename :%1").arg(source).append("File:")
                .append(__FILE__).append("Line").append(__LINE__);
        ErrorMessage(error);
        return ZIP_RETURN_ERROR;
    }
    unz_file_info* pFileInfo = new unz_file_info;
    char szZipFName[MAX_PATH];
    QString key;
    for(int i=0; i<pGlobalInfo->number_entry; i++)
    {
        nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH,
                                             NULL, 0, NULL, 0);
        if(nReturnValue != UNZ_OK)
        {
            return ZIP_RETURN_FILE_IO_ERROR;
        }
        QString tmp =QString::fromLocal8Bit(szZipFName);
        QByteArray arry;
        data_map.insert(tmp,arry);
        unzGoToNextFile(unzfile);
    }
    QStringList keys =data_map.keys();
    //Close
    if(unzfile)
    {
        int val =unzClose(unzfile);
        qDebug()<<val;
    }
    if(!keys.contains(HASH_FILE_NAME,Qt::CaseInsensitive))
        return ZIP_RETURN_NOT_MATCH;
    ui->progressBar->setRange(0,pGlobalInfo->number_entry);

    unzfile = unzOpen(source.toLocal8Bit().data());
    unzGetGlobalInfo(unzfile, pGlobalInfo);
    for(int i=0; i<pGlobalInfo->number_entry; i++)
    {
        nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH,
                                             NULL, 0, NULL, 0);
        if(nReturnValue != UNZ_OK)
        {
            return ZIP_RETURN_FILE_IO_ERROR;
        }
        //is dir or file
        switch((pFileInfo->external_fa&FILE_ATTRIBUTE_DIR))
        {
        case FILE_ATTRIBUTE_DIR:                    //
        {
            QByteArray m_arry;
            data_map.insert(szZipFName,m_arry);
            emit emit_process_index(i+1);
        }
            break;
        default:                                        //文件
        {
            //打开文件
            nReturnValue = unzOpenCurrentFile(unzfile);
            if(nReturnValue != UNZ_OK)
            {
                return ZIP_RETURN_FILE_IO_ERROR;
            }

            //read file
            QByteArray m_arry;
            const int BUFFER_SIZE = 4096;
            char szReadBuffer[BUFFER_SIZE];
            while(true)
            {
                memset(szReadBuffer, 0, BUFFER_SIZE);
                int nReadFileSize = unzReadCurrentFile(unzfile, szReadBuffer, BUFFER_SIZE);
                if(nReadFileSize < 0)                //
                {
                    int val =unzCloseCurrentFile(unzfile);
                    unzClose(unzfile);
                    return ZIP_RETURN_FILE_IO_ERROR;
                }
                else if(nReadFileSize == 0)            //
                {
                    int val =unzCloseCurrentFile(unzfile);
                    data_map.insert(szZipFName,m_arry);
                    emit emit_process_index(i+1);
                    break;
                }
                else                                //
                {
                    m_arry.append(szReadBuffer,nReadFileSize);
                }
            }
        }
            break;
        }
        unzGoToNextFile(unzfile);
    }
    //Close
    if(unzfile)
    {
        int val =unzClose(unzfile);
        qDebug()<<val;
    }
    QByteArray arry =data_map.value(HASH_FILE_NAME);
    pugi::xml_document m_doc;
    pugi::xml_parse_result ret= m_doc.load_string(arry.data());
    if(ret.status!=pugi::status_ok)
    {
        QString error =tr("Import library error, caused by hash file error.").append("File:")
                .append(__FILE__).append("Line").append(__LINE__);
        ErrorMessage(error);
        return ZIP_RETURN_XML_ERROR;
    }
    pugi::xml_node m_node=m_doc.child("SCL");
    if(m_node.empty())
        return ZIP_RETURN_XML_ERROR;
    QString device,version,database;
    device=m_node.attribute("Device").as_string();
    version=m_node.attribute("version").as_string();
    database=m_node.attribute("Database").as_string();
    QMap<QString,QByteArray>hash_map;
    QString xpath =".//item";
    pugi::xpath_node_set xset= m_doc.select_nodes(xpath.toStdString().data());
    for(pugi::xpath_node_set::const_iterator it = xset.begin();it != xset.end();it ++)
    {
        keys.append(it->node().attribute("ID").as_string());
        QString name =it->node().child("name").text().as_string();
        QString md5=it->node().child("MD5").text().as_string();
        hash_map.insert(name,md5.toLocal8Bit());
    }
    for (int i=0;i<hash_map.keys().size();i++)
    {
        QString key =hash_map.keys().at(i);
        QByteArray arry =data_map.value(key);
        QByteArray hash =g_get_MD5(arry);
        if(hash!=hash_map.value(key))
        {
            QString error =tr("Import library error, caused by hash check error, filename :%1").arg(key).append("File:")
                    .append(__FILE__).append("Line").append(__LINE__);
            ErrorMessage(error);
            return ZIP_RETURN_HASH_ERROR;
        }
    }
    QDir mdir;
    QString subpath =QDir::separator()+device+QDir::separator()+version+"_"+database;
    if(mdir.exists(dest+subpath))
    {
        QMessageBox::StandardButton r = QMessageBox::question(this, tr("Question")
                                                              , tr("The folder:%1 is already exist under Template, Do you want to overwrite?").arg(subpath)
                                                              , QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (r == QMessageBox::No)
        {
            return ZIP_RETURN_CANCEL;
        }
        g_delete_dir(dest+subpath);
    }else
    {
        QString ret=compare_version(dest+QDir::separator()+device,version,database);
        if(!ret.isEmpty())
        {
            QMessageBox::StandardButton r = QMessageBox::question(this, tr("Question")
                                                                  , tr("The version is already exist under Template,but folder name isn't %1"
                                                                       ", Do you want to repalce it?").arg(version+database)
                                                                  , QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (r == QMessageBox::No)
            {
                return ZIP_RETURN_CANCEL;
            }
            else
            {
                g_delete_dir(ret);
            }
        }
    }

    mdir.mkpath(dest+subpath);
    for (int i=0;i<data_map.keys().size();i++)
    {
        QString key =data_map.keys().at(i);
        if(data_map.value(key).size()==0)
        {
            mdir.mkpath(dest+subpath+QDir::separator()+key);
            continue;
        }
        QFileInfo info(dest+subpath+QDir::separator()+key);
        QString absoluteFilePath=info.absolutePath();
        if(!mdir.exists(info.absolutePath()))
        {
            mdir.mkpath(info.absolutePath());
        }
        QString path=info.filePath();
        QFile *m_file=new QFile;
        m_file->setFileName(path);
        if(m_file->open(QIODevice::WriteOnly|QIODevice::Unbuffered))
        {
            m_file->write(data_map.value(key));
            m_file->close();
        }else
        {
            QString error =tr("Import library error, caused by write error, filename :%1").arg(info.filePath()).append("File:")
                    .append(__FILE__).append("Line").append(__LINE__);
            ErrorMessage(error);
            g_delete_dir(dest+subpath);
            return ZIP_RETURN_ERROR;
        }

    }
}
int Zip_process::lib_export()
{
    QString filename;
    qDebug()<<"source"<<source;
    qDebug()<<"dest"<<dest;
    pugi::xml_document m_doc;
    QString device,version,database;
    pugi::xml_parse_result ret=m_doc.load_file(QString(source+QDir::separator()+"hash.xml").toLocal8Bit().data());
    if(ret.status!=pugi::status_ok)
    {
        ui->label_Note->setText(tr("Load the hash.xml under path :%1 false,and the offset is %2,check detail in log").arg(source).arg(ret.offset));
        ErrorMessage(ret.description());
        return ZIP_RETURN_XML_ERROR;
    }
    if(m_doc.child("SCL").empty())
    {
        ui->label_Note->setText(tr("The hash.xml is not correct,not contain the SCL node"));
        return ZIP_RETURN_XML_ERROR;
    }
    device =m_doc.child("SCL").attribute("Device").as_string();
    version =m_doc.child("SCL").attribute("version").as_string();
    database =m_doc.child("SCL").attribute("Databse").as_string();
    filename=device+"_"+version+"_"+database;
    output_file=dest+QDir::separator()+filename+".Zip";
    if(QFileInfo::exists(output_file))
    {
        int ret=QMessageBox::information(this,tr("Information"),tr("The file :")+output_file+tr("is exist\nDo you want to overwrite it?")
                                         ,QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        if(ret==QMessageBox::No)
            return ZIP_RETURN_CANCEL;
    }
    QString xpath ="./SCL/File";
    pugi::xpath_node xnode=m_doc.select_node(xpath.toStdString().data());
    pugi::xml_node mnode;
    if(!xnode.node().empty())
    {
        xnode.node().remove_children();
        mnode=xnode.node();
    }else
    {
        mnode=m_doc.child("SCL").append_child("File");
    }
    QStringList m_files =g_getfiles(source);
    ui->progressBar->setRange(0,m_files.size());
    try
    {
        zipFile zf = NULL;
#ifdef WINDOWS_32
        path =path.replace("/","\\");
#endif
        zf = zipOpen(output_file.toLocal8Bit().data(),APPEND_STATUS_CREATE);
        if(zf==NULL)
            return ZIP_RETURN_FILE_NOT_FOUND;
        zip_fileinfo FileInfo;
        memset(&FileInfo,0,sizeof(zip_fileinfo));
        int ret;
        int i=0;
        foreach(QString name, m_files)
        {
            if(name.endsWith("hash.xml",Qt::CaseInsensitive))
                continue;
            QFile m_file(name);
            QFileInfo m_info(name);
            if(!m_file.open(QIODevice::ReadOnly))
                return ZIP_RETURN_ERROR;
            QByteArray arry =m_file.readAll();
            QString temp =name.remove(source+QDir::separator());
            QByteArray ba =g_get_MD5(arry);
            pugi::xml_node m_item=mnode.append_child("item");
            m_item.append_child("name").text().set(temp.toStdString().data());
            m_item.append_child("MD5").text().set(ba.data());
            zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
            uLong readlen =arry.size();
            if(readlen<=0)
                return ZIP_RETURN_FILE_IO_ERROR;
            ret = zipWriteInFileInZip(zf,arry.data(),readlen);
            if(ret != ZIP_OK )
            {

                zipCloseFileInZip(zf);
                zipClose(zf,NULL);
                return ZIP_RETURN_FILE_IO_ERROR;
            }
            i++;
            emit emit_process_index(i);
            zipCloseFileInZip(zf);
        }
        xml_string_writer m_data1;
        m_doc.print(m_data1);
        QByteArray arry;
        arry.append(QString::fromStdString(m_data1.xml).toLocal8Bit());
        QString temp ="hash.xml";
        zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
        uLong readlen =arry.size();
        if(readlen<=0)
            return ZIP_RETURN_FILE_IO_ERROR;
        ret = zipWriteInFileInZip(zf,arry.data(),readlen);
        if(ret != ZIP_OK )
        {

            zipCloseFileInZip(zf);
            zipClose(zf,NULL);
            return ZIP_RETURN_FILE_IO_ERROR;
        }
        i++;
        emit emit_process_index(i);
        zipCloseFileInZip(zf);
        zipClose(zf,NULL);
    }
    catch(...)
    {
        return ZIP_RETURN_ERROR;
    }
    return ZIP_RETURN_OK;

}
int Zip_process::Device2lib_export()
{

}
int Zip_process::pro_import()
{

}
int Zip_process::pro_export()
{
    output_file =dest+QDir::separator()+device+".zip";
    if(QFileInfo::exists(output_file))
    {
        int ret=QMessageBox::information(this,tr("Information"),tr("The file :")+output_file+tr("is exist\nDo you want to overwrite it?")
                                         ,QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        if(ret==QMessageBox::No)
            return ZIP_RETURN_CANCEL;
    }
    QString profile =source+QDir::separator()+device+".KFpro";
    pugi::xml_document m_doc;
    if(m_doc.load_file(profile.toStdString().data()).status!=pugi::status_ok)
    {
        QString error =tr("Export project error, caused by project file error, ").append("filename:").append(profile).append("File:")
                .append(__FILE__).append("Line").append(__LINE__);
        ErrorMessage(error);
        return ZIP_RETURN_XML_ERROR;
    }
    QMap<QString,QString> lib_map;
    QString xpath=".//Deivce";
    pugi::xpath_node_set xset= m_doc.select_nodes(xpath.toStdString().data());
    for(pugi::xpath_node_set::const_iterator it = xset.begin();it != xset.end();it ++)
    {
        xpath=".//item";
        pugi::xpath_node_set xitems= m_doc.select_nodes(xpath.toStdString().data());
        for(pugi::xpath_node_set::const_iterator item = xitems.begin();item != xitems.end();item ++)
        {
            lib_map.insert(item->node().attribute("device").as_string(),item->node().attribute("ver").as_string());
        }
    }
}
int Zip_process::devcie_export()
{
    qDebug()<<"source is"<<source<<" ,dest is"<<dest<<__FILE__<<"  "<<__LINE__;
    pugi::xml_document m_doc;
    pugi::xml_node pre = m_doc.prepend_child(pugi::node_declaration);
    pre.append_attribute("version") = "1.0";
    pre.append_attribute("encoding") = "utf-8";
    pugi::xml_node m_scl =m_doc.append_child("SCL");
    m_scl.append_attribute("Device").set_value(device.toStdString().data());
    m_scl.append_attribute("version").set_value(version.toStdString().data());
    m_scl.append_attribute("Database").set_value(database.toStdString().data());
    pugi::xml_node m_node =m_scl.append_child("Device");

    QStringList m_devfiles =g_getfiles(source);
    g_Template_path=g_lib_map.value(library).value(version).value(database);
    QStringList m_libs =g_getfiles(g_Template_path);
    ui->progressBar->setRange(0,m_devfiles.size()+m_libs.size());
    output_file =dest+QDir::separator()+device+"_"+version+"_"+database+".zip";
    if(QFileInfo::exists(output_file))
    {
        int ret=QMessageBox::information(this,tr("Information"),tr("The file :")+output_file+tr("is exist\nDo you want to overwrite it?")
                                         ,QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        if(ret==QMessageBox::No)
            return ZIP_RETURN_CANCEL;
    }
    try
    {
        zipFile zf = NULL;
#ifdef WINDOWS_32
        path =path.replace("/","\\");
#endif
        zf = zipOpen(output_file.toLocal8Bit().data(),APPEND_STATUS_CREATE);
        if(zf==NULL)
            return ZIP_RETURN_FILE_NOT_FOUND;
        zip_fileinfo FileInfo;
        memset(&FileInfo,0,sizeof(zip_fileinfo));
        int ret;
        int i=0;
        foreach(QString name, m_devfiles)
        {
            if(name.endsWith("hash.xml",Qt::CaseInsensitive))
            {
                i++;
                emit emit_process_index(i);
                continue;
            }
            QFile m_file(name);
            QFileInfo m_info(name);
            if(!m_file.open(QIODevice::ReadOnly))
                return ZIP_RETURN_ERROR;
            QByteArray arry =m_file.readAll();
            QString temp =DEVICE_PATH+name.remove(source);
            QByteArray ba =g_get_MD5(arry);
            pugi::xml_node m_item=m_node.append_child("item");
            m_item.append_child("name").text().set(temp.toStdString().data());
            m_item.append_child("MD5").text().set(ba.data());
            zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
            uLong readlen =arry.size();
            if(readlen<=0)
                return ZIP_RETURN_FILE_IO_ERROR;
            ret = zipWriteInFileInZip(zf,arry.data(),readlen);
            if(ret != ZIP_OK )
            {

                zipCloseFileInZip(zf);
                zipClose(zf,NULL);
                return ZIP_RETURN_FILE_IO_ERROR;
            }
            i++;
            emit emit_process_index(i);
            zipCloseFileInZip(zf);
        }
        m_node =m_scl.append_child("Library");
        m_node.append_attribute("Device").set_value(library.toStdString().data());
        m_node.append_attribute("version").set_value(version.toStdString().data());
        m_node.append_attribute("Database").set_value(database.toStdString().data());
        foreach(QString name, m_libs)
        {
            QFile m_file(name);
            QFileInfo m_info(name);
            if(!m_file.open(QIODevice::ReadOnly))
                return ZIP_RETURN_ERROR;
            QByteArray arry =m_file.readAll();
            QString temp =LIBRARY_PATH+name.remove(g_Template_path);
            QByteArray ba =g_get_MD5(arry);
            pugi::xml_node m_item=m_node.append_child("item");
            m_item.append_child("name").text().set(temp.toStdString().data());
            m_item.append_child("MD5").text().set(ba.data());
            zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
            uLong readlen =arry.size();
            if(readlen<=0)
                return ZIP_RETURN_FILE_IO_ERROR;
            ret = zipWriteInFileInZip(zf,arry.data(),readlen);
            if(ret != ZIP_OK )
            {

                zipCloseFileInZip(zf);
                zipClose(zf,NULL);
                return ZIP_RETURN_FILE_IO_ERROR;
            }
            i++;
            emit emit_process_index(i);
            zipCloseFileInZip(zf);
        }
        xml_string_writer m_data1;
        m_doc.print(m_data1);
        QByteArray arry;
        arry.append(QString::fromStdString(m_data1.xml).toLocal8Bit());
        QString temp =DEV_FILE_NAME;
        zipOpenNewFileInZip(zf,temp.toLocal8Bit().data(),&FileInfo,NULL,0,NULL,0,NULL,Z_DEFLATED,9);
        uLong readlen =arry.size();
        if(readlen<=0)
            return ZIP_RETURN_FILE_IO_ERROR;
        ret = zipWriteInFileInZip(zf,arry.data(),readlen);
        if(ret != ZIP_OK )
        {

            zipCloseFileInZip(zf);
            zipClose(zf,NULL);
            return ZIP_RETURN_FILE_IO_ERROR;
        }
        i++;
        emit emit_process_index(i);
        zipCloseFileInZip(zf);
        zipClose(zf,NULL);
    }
    catch(...)
    {
        return ZIP_RETURN_ERROR;
    }

    return ZIP_RETURN_OK;


}
int Zip_process::devcie_import()
{
    unzFile unzfile = unzOpen(source.toLocal8Bit().data());
    QMap<QString,QByteArray> data_map;
    if(unzfile == NULL)
    {
        QString error =tr("Import device error, caused by library file  open error, filename :%1").arg(source).append("File:")
                .append(__FILE__).append("Line").append(__LINE__);
        ErrorMessage(error);
        return ZIP_RETURN_FILE_NOT_FOUND;
    }
    unz_global_info* pGlobalInfo = new unz_global_info;
    int nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
    if(nReturnValue != UNZ_OK)
    {
        QString error =tr("Import device error, caused by library file  open error, filename :%1").arg(source).append("File:")
                .append(__FILE__).append("Line").append(__LINE__);
        ErrorMessage(error);
        return ZIP_RETURN_ERROR;
    }
    unz_file_info* pFileInfo = new unz_file_info;
    char szZipFName[MAX_PATH];
    QString key;
    for(int i=0; i<pGlobalInfo->number_entry; i++)
    {
        nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH,
                                             NULL, 0, NULL, 0);
        if(nReturnValue != UNZ_OK)
        {
            return ZIP_RETURN_FILE_IO_ERROR;
        }
        QString tmp =QString::fromLocal8Bit(szZipFName);
        QByteArray arry;
        data_map.insert(tmp,arry);
        unzGoToNextFile(unzfile);
    }
    QStringList keys =data_map.keys();
    if(!keys.contains(DEV_FILE_NAME,Qt::CaseInsensitive))
    {
        QString error =tr("Import library error, caused by device.xml file is not exist.").append("File:")
                .append(__FILE__).append("Line").append(__LINE__);
        ErrorMessage(error);
        return ZIP_RETURN_NOT_MATCH;
    }
    ui->progressBar->setRange(0,pGlobalInfo->number_entry);
    unzip2map(source,data_map);
    QByteArray arry =data_map.value(DEV_FILE_NAME);
    pugi::xml_document m_doc;
    pugi::xml_parse_result ret= m_doc.load_string(arry.data());
    if(ret.status!=pugi::status_ok)
    {
        QString error =tr("Import library error, caused by hash file error.").append("File:")
                .append(__FILE__).append("Line").append(__LINE__);
        ErrorMessage(error);
        return ZIP_RETURN_XML_ERROR;
    }
    pugi::xml_node m_node=m_doc.child("SCL");
    if(m_node.empty())
        return ZIP_RETURN_XML_ERROR;
    QString device,ver,db;
    device=m_node.attribute("Device").as_string();
    ver=m_node.attribute("version").as_string();
    db=m_node.attribute("Database").as_string();
    QMap<QString,QByteArray>hash_map;
    QStringList devlist,liblist;
    //get the hash map from xml
    QString xpath =".//item";
    pugi::xpath_node_set xset= m_doc.select_nodes(xpath.toStdString().data());
    for(pugi::xpath_node_set::const_iterator it = xset.begin();it != xset.end();it ++)
    {
        keys.append(it->node().attribute("ID").as_string());
        QString name =it->node().child("name").text().as_string();
        QString md5=it->node().child("MD5").text().as_string();
        hash_map.insert(name,md5.toLocal8Bit());
        if(QString(it->node().parent().name()).compare("Device",Qt::CaseInsensitive)==0)
            devlist.append(name);
        else if(QString(it->node().parent().name()).compare("Library",Qt::CaseInsensitive)==0)
            liblist.append(name);
    }
    //compare the hash from map with the has code of file
    for (int i=0;i<hash_map.keys().size();i++)
    {
        QString key =hash_map.keys().at(i);
        QByteArray arry =data_map.value(key);
        QByteArray hash =g_get_MD5(arry);
        if(hash!=hash_map.value(key))
        {
            QString error =tr("Import library error, caused by hash check error, filename :%1").arg(key).append("File:")
                    .append(__FILE__).append("Line").append(__LINE__);

            ErrorMessage(error);
            return ZIP_RETURN_HASH_ERROR;
        }
    }
    // check the library is exist or not, is not exist, copy library to template folder;
    xpath =".//Library";
    pugi::xpath_node xnode= m_doc.select_node(xpath.toStdString().data());
    if(xnode.node().empty())
    {
        QString error =tr("Import library error, caused by devcie.xml don't contain the library information, filename :%1").arg(key).append("File:")
                .append(__FILE__).append("Line").append(__LINE__);
        ErrorMessage(error);
    }
    //QString libname,libver,libdb;
    library=xnode.node().attribute("Device").as_string();
    version=xnode.node().attribute("version").as_string();
    database=xnode.node().attribute("Database").as_string();
    QDir mdir;
    if(g_lib_map.value(library).value(version).value(database).isEmpty())
    {
        QString subpath=QDir::separator()+library+QDir::separator()+version+"_"+database;
        mdir.mkpath(g_Template_path+subpath);
        for (int i=0;i<liblist.size();i++)
        {
            QString key =liblist.at(i);
            if(data_map.value(key).size()==0)
            {
                mdir.mkpath(g_Template_path+subpath+QDir::separator()+key);
                continue;
            }
            QFileInfo info(g_Template_path+subpath+QDir::separator()+key);
            QString absoluteFilePath=info.absolutePath();
            if(!mdir.exists(info.absolutePath()))
            {
                mdir.mkpath(info.absolutePath());
            }
            QString path=info.filePath();
            QFile *m_file=new QFile;
            m_file->setFileName(path);
            if(m_file->open(QIODevice::WriteOnly|QIODevice::Unbuffered))
            {
                m_file->write(data_map.value(key));
                m_file->close();
            }else
            {
                QString error =tr("Import device error, caused by import library with write error, filename :%1").arg(info.filePath()).append("File:")
                        .append(__FILE__).append("Line").append(__LINE__);
                ErrorMessage(error);
                g_delete_dir(dest+subpath);
                return ZIP_RETURN_ERROR;
            }

        }
    }
    //copy device files to dest path,
    if(!mdir.exists(dest))
        mdir.mkpath(dest);
    for (int i=0;i<devlist.size();i++)
    {

        QString key =devlist.at(i);
        if(data_map.value(key).size()==0)
        {
            mdir.mkpath(dest+QDir::separator()+key);
            continue;
        }
        QString tmp =key;
        tmp.remove(tr(DEVICE_PATH)+QDir::separator());
        QFileInfo info(dest+QDir::separator()+tmp);
        QString absoluteFilePath=info.absolutePath();
        if(!mdir.exists(info.absolutePath()))
        {
            mdir.mkpath(info.absolutePath());
        }
        QString path=info.filePath();
        QFile *m_file=new QFile;
        m_file->setFileName(path);
        if(m_file->open(QIODevice::WriteOnly|QIODevice::Unbuffered))
        {
            m_file->write(data_map.value(key));
            m_file->close();
        }else
        {
            QString error =tr("Import device error, caused by import device with write error, filename :%1").arg(info.filePath()).append("File:")
                    .append(__FILE__).append("Line").append(__LINE__);
            ErrorMessage(error);
            g_delete_dir(dest);
            return ZIP_RETURN_ERROR;
        }

    }
    return ZIP_RETURN_OK;

}
QString Zip_process::compare_version(QString dest,QString version,QString database)
{
    QString path;
    path.clear();
    QStringList mdirs =g_getfiles(dest,LIST_MODE_DIR_TOP);
    for (int i=0;i<mdirs.size();i++)
    {
        QString temp =mdirs.at(i)+QDir::separator()+HASH_FILE_NAME;
        pugi::xml_document m_doc;
        pugi::xml_parse_result ret=m_doc.load_file(temp.toStdString().data());
        if(ret.status==pugi::status_ok)
        {
            pugi::xml_node m_node=m_doc.child("SCL");
            if(!m_node.empty())
            {
                QString subver,subdb;
                subver =m_node.attribute("version").as_string();
                subdb  =m_node.attribute("Database").as_string();
                if(version.compare(subver,Qt::CaseInsensitive)==0&database.compare(subdb,Qt::CaseInsensitive)==0)
                {
                    path =mdirs.at(i);
                    break;
                }
            }
        }
    }
    return path;
}
void Zip_process::on_pushbutton_Ok()
{
    ui->label_Note->clear();
    //    switch (type) {
    //    case ZIP_UPDATE:
    //    case ZIP_RETRIEVE:
    //        retrieve_source2dest(source,dest);
    //        break;
    //    case ZIP_ARCHIVE:
    //        QFile::remove(dest);
    //        archive_source2dest(source,dest,mode);
    //        break;
    //    default:
    //        break;
    //    }
    int ret;
    switch (type) {
    case ZIP_RETRIEVE:
        switch (mode)
        {
        case MODE_LIB:
            ret =lib_import();
            break;
        case MODE_DEV:
            ret =devcie_import();
            break;
        case MODE_PRO:
            ret =pro_import();
            break;
        default:
            break;
        }
        break;
    case ZIP_ARCHIVE:
        switch (mode)
        {
        case MODE_LIB:
            ret =lib_export();

            break;
        case MODE_DEV:
            ret =devcie_export();
            break;
        case MODE_PRO:
            pro_export();
            break;
        case MODE_DEV2LIB:
            Device2lib_export();
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    if(ret==ZIP_RETURN_XML_ERROR)
        ui->label_Note->setText(tr("Operation failed, caused by Xml error"));
    else if(ret==ZIP_RETURN_FILE_IO_ERROR)
        ui->label_Note->setText(tr("Operation failed, caused by File IO"));
    else if(ret==ZIP_RETURN_FILE_NOT_FOUND)
        ui->label_Note->setText(tr("Operation failed, caused by file can't find"));
    else if(ret==ZIP_RETURN_OK)
    {
        if(mode==MODE_DEV)
        {
            if(type==ZIP_ARCHIVE)
                ui->label_Note->setText(tr("Operation success, output file: %1.").arg(output_file));
            else
                accept();
        }else if(mode==MODE_LIB)
        {
            if(type==ZIP_ARCHIVE)
                ui->label_Note->setText(tr("Operation success, output file: %1.").arg(output_file));
            else
                ui->label_Note->setText(tr("Operation success, output folder: %1.").arg(dest));
        }


    }
}
void Zip_process::on_pushbutton_Cancel()
{
    reject();
}
void Zip_process::on_pushbutton_Help()
{
    emit emit_help_message(ui->pushButton_Help->property("name").toString());
}
void Zip_process::on_pushbutton_Browse()
{
    QString tmp =ui->lineEdit->text();

    switch (type)
    {
    case ZIP_UPDATE:
    case ZIP_RETRIEVE:
        if(tmp.isEmpty())
            tmp=dest;
        source = QFileDialog::getOpenFileName(this, tr("Open library file"),
                                              tmp,
                                              tr("Library (*.zip)"));
        if(!source.isEmpty())
        {
            ui->lineEdit->setText(source);
            ui->pushButton_Ok->setEnabled(true);
        }
        break;
    case ZIP_ARCHIVE:
        if(tmp.isEmpty())
            tmp=source+".zip";
        //        dest = QFileDialog::getSaveFileName(this, tr("Save library File"),
        //                                            tmp,
        //                                            tr("Library (*.zip)"));
        dest = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 tmp,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
        if(!dest.isEmpty())
        {
            ui->lineEdit->setText(dest);
            ui->pushButton_Ok->setEnabled(true);
        }
        break;
    default:
        break;
    }

    return;
}
void Zip_process::on_process_index(int val)
{
    ui->progressBar->setValue(val);
}
