#include "mytableview.h"

MyTableView::MyTableView(QWidget *parent):QTableView(parent)
{
}
void MyTableView::mousePressEvent(QMouseEvent *event)
{

    // 点击空白处时取消所有已选择项
    auto index = this->indexAt(event->pos());
    if (index.row() == -1)
    {
        setCurrentIndex(index);
    }
    else
    {
        QTableView::mousePressEvent(event);
    }


}
