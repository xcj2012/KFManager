#ifndef NEW_PROJECT_H
#define NEW_PROJECT_H

#include <QDialog>
#include "global.h"

namespace Ui {
class New_Project;
}

class New_Project : public QDialog
{
    Q_OBJECT

public:
    explicit New_Project(QWidget *parent = nullptr);
    ~New_Project();
    QStandardItemModel *m_model;
    void setupwidget();
    QString name;
    QString path;
signals:
    void emit_help_message(QString);
private slots:
    void on_pushbutton_Ok();
    void on_pushbutton_Cancel();
    void on_pushbutton_Help();
    void on_pushbutton_Browse();
private:
    Ui::New_Project *ui;
};

#endif // NEW_PROJECT_H
