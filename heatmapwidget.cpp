#include "heatmapwidget.h"
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <cmath>
#include <algorithm>

HeatmapWidget::HeatmapWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setSceneRect(0, 0, 800, 600);
}

void HeatmapWidget::buildHeatmap(const QString &text)
{
    m_freq.clear();
    // Подсчёт частот (без пробелов, регистронезависимо)
    for (QChar ch : text) {
        if (ch.isSpace()) continue;
        QChar lower = ch.toLower();
        m_freq[lower] = m_freq.value(lower, 0) + 1;
    }
    drawHistogram(); // теперь drawHistogram рисует радиальную диаграмму
}

void HeatmapWidget::drawHistogram()
{
    m_scene->clear();
    if (m_freq.isEmpty()) return;

    // Находим максимальную частоту
    int maxFreq = 0;
    for (int f : m_freq)
        if (f > maxFreq) maxFreq = f;

    QList<QChar> chars = m_freq.keys();
    std::sort(chars.begin(), chars.end());

    int total = chars.size();
    if (total == 0) return;

    // Центр и базовый радиус
    QPointF center(400, 300);
    double baseRadius = 80.0;
    double maxRadius = 250.0;

    double angleStep = 2 * M_PI / total;

    // Фоновое кольцо (декорация)
    QGraphicsEllipseItem *bgCircle = m_scene->addEllipse(center.x() - maxRadius - 20,
                                                         center.y() - maxRadius - 20,
                                                         (maxRadius+20)*2,
                                                         (maxRadius+20)*2,
                                                         QPen(QColor(80,80,80,100)),
                                                         QBrush(QColor(20,20,40,50)));
    bgCircle->setZValue(-1);

    for (int i = 0; i < total; ++i) {
        QChar ch = chars[i];
        int freq = m_freq[ch];
        double ratio = (double)freq / maxFreq; // 0..1

        // Радиус: от baseRadius до maxRadius в зависимости от частоты
        double r = baseRadius + ratio * (maxRadius - baseRadius);
        double angle = i * angleStep;

        double x = center.x() + r * cos(angle);
        double y = center.y() + r * sin(angle);

        // Цвет от синего (редко) до красного (часто)
        QColor color = QColor::fromRgbF(ratio, 0.3, 1.0 - ratio);
        // Размер шрифта от 10 до 30
        int fontSize = 10 + ratio * 25;

        QGraphicsTextItem *label = m_scene->addText(QString(ch));
        label->setDefaultTextColor(color);
        QFont font("Arial", fontSize);
        font.setBold(true);
        label->setFont(font);
        label->setPos(x - label->boundingRect().width()/2,
                      y - label->boundingRect().height()/2);

        // Добавляем маленькие точки-маркеры для каждой буквы (светящиеся)
        QGraphicsEllipseItem *dot = m_scene->addEllipse(x-3, y-3, 6, 6,
                                                        QPen(QColor(color.red(), color.green(), color.blue(), 200)),
                                                        QBrush(color));
        dot->setZValue(-0.5);
    }

    // Заголовок
    QGraphicsTextItem *title = m_scene->addText("Радиальная тепловая карта частоты букв");
    title->setDefaultTextColor(Qt::white);
    title->setFont(QFont("Arial", 14, QFont::Bold));
    title->setPos(center.x() - title->boundingRect().width()/2, 20);

    // Подпись: редкие буквы внутри, частые снаружи
    QGraphicsTextItem *legend = m_scene->addText("Редкие ← → Частые");
    legend->setDefaultTextColor(QColor(200,200,200));
    legend->setFont(QFont("Arial", 10));
    legend->setPos(center.x() - legend->boundingRect().width()/2, 550);
}