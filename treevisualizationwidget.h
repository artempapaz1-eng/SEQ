#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QGraphicsView>
#include <QMap>

class TreeVisualizationWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TreeVisualizationWidget(QWidget *parent = nullptr);
    void buildTree(const QString &text);   // основная функция

private:
    QGraphicsScene *m_scene;
    void drawNode(const QString &label, double x, double y, int level, int freq = 1);
    void drawLine(double x1, double y1, double x2, double y2);
};

#endif // TREEWIDGET_H