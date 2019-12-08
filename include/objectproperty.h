#ifndef OBJECTPROPERTY_H
#define OBJECTPROPERTY_H

#include <QDialog>
#include "global.h"
namespace Ui {
class ObjectProperty;
}

class ObjectProperty : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectProperty(QWidget *parent = nullptr);
    QString name;
    QString path;
    QString author;
    QString createtime;
    QString modifytime;
    QString commit;
    QString ipaddress;
    QString address;
    QString device;
    QString version;
    ~ObjectProperty();
    void setupWidget(int type);
signals:
    void emit_help_message(QString);
private slots:
    void on_pushbutton_Ok();
    void on_pushbutton_Cancel();
    void on_pushbutton_Help();
private:
    Ui::ObjectProperty *ui;
};

#endif // OBJECTPROPERTY_H
