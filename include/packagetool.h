#ifndef PACKAGETOOL_H
#define PACKAGETOOL_H

#include <QDialog>
#include "global.h"
using namespace pugi;
namespace Ui {
class PackageTool;
}

class PackageTool : public QDialog
{
    Q_OBJECT

public:
    explicit PackageTool(QWidget *parent = nullptr);
    ~PackageTool();
private:
    void init_devList();
    void disable_function();
private:
    Ui::PackageTool *ui;
};

#endif // PACKAGETOOL_H
