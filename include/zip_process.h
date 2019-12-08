#ifndef ZIP_PROCESS_H
#define ZIP_PROCESS_H

#include <QDialog>
#include "global.h"
#include "zip.h"
#include "unzip.h"
#define MAX_PATH 260
#define FILE_ATTRIBUTE_DIR 0x00000010
using namespace std;
struct xml_string_writer :public pugi::xml_writer
{
 string xml;
 void write(const void* data, size_t size)
 {
  char* pXML = new char[size + 1];
  memset(pXML, 0, size + 1);
  memcpy(pXML, data, size);
  xml += string(pXML);
  delete[] pXML;

 }
 xml_string_writer()
 {
  xml = "";
 }
};
#define ZIP_RETURN_FILE_NOT_FOUND 0x01
#define ZIP_RETURN_ERROR 0x02
#define ZIP_RETURN_FILE_IO_ERROR 0x03
#define ZIP_RETURN_OK 0x00
#define ZIP_RETURN_XML_ERROR 0x04
#define ZIP_RETURN_NOT_MATCH 0x05
#define ZIP_RETURN_CANCEL 0x05

namespace Ui {
class Zip_process;
}

class Zip_process : public QDialog
{
    Q_OBJECT

public:
    explicit Zip_process(QWidget *parent = nullptr);
    ~Zip_process();

    void init_connect();
    void setupwidget();
    void archive_Pro2Pro(QString source,QString dest);
    void archive_Device2Lib(QString source,QString dest);
    void archive_Lib2Lib(QString source,QString dest);
    void archive_Device2Device_PRO(QString source,QString dest);
    void archive_Device2Device_Lib(QString source,QString dest);
    void archive_source2dest(QString source,QString dest,int mode=0);
    void retrieve_source2dest(QString source,QString dest);
    void retrieve_Lib2Lib(QString source,QString dest);
    void retrieve_Device2Device_PRO(QString source,QString dest);
    void retrieve_Device2Device_Lib(QString source,QString dest);
    void retrieve_Pro2Pro(QString source,QString dest);
public:
    void setsource(QString);
    void setdest(QString);
    void setlibrary(QString);
    void setdevice(QString);
    void setversion(QString);
    void setlibpath(QString);
    void setdatabse(QString);
    void settype(int);
    void setmode(int);
    void setmethod(int);
private:
    int type;
    int mode;
    int mehtod;
    QString dest;
    QString source;
    QString library;
    QString device;
    QString version;
    QString database;
    QString libpath;
    int  lib_import();
    int  lib_export();
    void Device2lib_export();
    void pro_import();
    void pro_export();
    int devcie_export();
    void devcie_import();
    QString compare_version(QString desc,QString version,QString database);
signals:
    void emit_help_message(QString);
    void emit_process_index(int val);
private slots:
    void on_pushbutton_Ok();
    void on_pushbutton_Cancel();
    void on_pushbutton_Help();
    void on_pushbutton_Browse();
    void on_process_index(int val);
private:
    Ui::Zip_process *ui;
};

#endif // ZIP_PROCESS_H
