#ifndef HEATMAPWIDGET_H
#define HEATMAPWIDGET_H

#include <QGraphicsView>
#include <QMap>

class HeatmapWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit HeatmapWidget(QWidget *parent = nullptr);
    void buildHeatmap(const QString &text);  // вызывать при новом тексте

private:
    QGraphicsScene *m_scene;
    QMap<QChar, int> m_freq;
    void drawHistogram();
};

#endif // HEATMAPWIDGET_H