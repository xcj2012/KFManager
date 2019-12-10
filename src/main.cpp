#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    g_dir.cdUp();
    g_Template_path =g_dir.path()+QDir::separator()+LIBRARY_PATH;
    init_library_map();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
