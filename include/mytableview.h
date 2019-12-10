#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QMouseEvent>

class MyTableView : public QTableView
{
    Q_OBJECT
public:
    explicit MyTableView(QWidget *parent = 0);
protected:
    void mousePressEvent(QMouseEvent *event);


};
#endif // MYTABLEVIEW_H
