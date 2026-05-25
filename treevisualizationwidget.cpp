#include "treevisualizationwidget.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QBrush>
#include <QPen>
#include <cmath>
#include <QRegularExpression>

TreeVisualizationWidget::TreeVisualizationWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setSceneRect(-800, -400, 1600, 1000); // Немного расширил сцену для больших текстов
    setBackgroundBrush(QColor(10, 10, 20));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void TreeVisualizationWidget::buildTree(const QString &text)
{
    m_scene->clear();
    if (text.isEmpty()) {
        drawNode("Нет текста", 0, 0, 0);
        return;
    }

    // Разбиваем на предложения (по . ! ?)
    QStringList sentences = text.split(QRegularExpression("[.!?]+"), Qt::SkipEmptyParts);
    if (sentences.isEmpty()) {
        drawNode("Нет текста", 0, 0, 0);
        return;
    }

    // Константы шагов по вертикали
    const double rootY = -250;
    const double yStepSentence = 120;
    const double yStepWord = 100;
    const double yStepLetter = 80;

    // Настройки геометрии нижнего уровня
    const double letterSpacing = 45; // Расстояние между центрами букв
    const double wordPadding = 25;   // Отступ между соседними словами
    const double sentencePadding = 60; // Отступ между предложениями

    // Глобальный счетчик по оси X для симуляции нижнего ряда
    double currentX = 0;

    // Структуры для предварительного расчета координат
    struct WordInfo {
        QString text;
        double x;
        double y;
        int displayLength;
        bool isLong;
    };

    struct SentenceInfo {
        QString text;
        double x;
        double y;
        QList<WordInfo> words;
    };

    QList<SentenceInfo> treeData;

    // --- ЭТАП 1: Расчет координат снизу вверх ---
    for (int i = 0; i < sentences.size(); ++i) {
        SentenceInfo sInfo;
        sInfo.text = sentences[i].trimmed();
        if (sInfo.text.isEmpty()) continue;

        QStringList words = sInfo.text.split(QRegularExpression("[\\s,;:]+"), Qt::SkipEmptyParts);

        for (int w = 0; w < words.size(); ++w) {
            WordInfo wInfo;
            wInfo.text = words[w];
            wInfo.isLong = (wInfo.text.length() > 8);
            wInfo.displayLength = wInfo.isLong ? 6 : wInfo.text.length(); // 5 букв + 1 для многоточия

            // Считаем границы, которые займут буквы этого слова на нижнем уровне
            double firstLetterX = currentX;
            currentX += (wInfo.displayLength - 1) * letterSpacing;
            double lastLetterX = currentX;

            // Слово встает ровно по центру своих букв
            wInfo.x = firstLetterX + (lastLetterX - firstLetterX) / 2.0;
            wInfo.y = rootY + yStepSentence + yStepWord;

            // Сдвигаем счетчик для следующего слова
            currentX += letterSpacing + wordPadding;

            sInfo.words.append(wInfo);
        }

        // Предложение встает по центру своих слов
        if (!sInfo.words.isEmpty()) {
            double firstWordX = sInfo.words.first().x;
            double lastWordX = sInfo.words.last().x;
            sInfo.x = firstWordX + (lastWordX - firstWordX) / 2.0;
        } else {
            sInfo.x = currentX;
        }
        sInfo.y = rootY + yStepSentence;

        // Сдвигаем счетчик для следующего предложения
        currentX += sentencePadding;
        treeData.append(sInfo);
    }

    // Находим общую ширину получившегося дерева, чтобы отцентрировать его по оси X = 0
    double totalWidth = currentX - sentencePadding - wordPadding;
    double shiftX = -totalWidth / 2.0;

    // --- ЭТАП 2: Отрисовка элементов на сцене ---
    double rootX = 0; // Корень всегда по центру сцены
    drawNode("Текст: " + text.left(30) + (text.length() > 30 ? "..." : ""), rootX, rootY, 0);

    for (const auto &sInfo : treeData) {
        double sX = sInfo.x + shiftX;
        drawNode(sInfo.text.left(25), sX, sInfo.y, 1);
        drawLine(rootX, rootY, sX, sInfo.y);

        for (const auto &wInfo : sInfo.words) {
            double wX = wInfo.x + shiftX;
            drawNode(wInfo.text, wX, wInfo.y, 2);
            drawLine(sX, sInfo.y, wX, wInfo.y);

            // Отрисовка букв
            double lY = wInfo.y + yStepLetter;
            // Вычисляем левую стартовую точку для ряда букв этого конкретного слова
            double lStartX = wX - ((wInfo.displayLength - 1) * letterSpacing) / 2.0;

            if (!wInfo.isLong) {
                for (int l = 0; l < wInfo.text.length(); ++l) {
                    double lX = lStartX + l * letterSpacing;
                    drawNode(QString(wInfo.text[l]), lX, lY, 3);
                    drawLine(wX, wInfo.y, lX, lY);
                }
            } else {
                // Если слово длинное, пишем 5 букв и многоточие
                for (int l = 0; l < 5; ++l) {
                    double lX = lStartX + l * letterSpacing;
                    drawNode(QString(wInfo.text[l]), lX, lY, 3);
                    drawLine(wX, wInfo.y, lX, lY);
                }
                double dotX = lStartX + 5 * letterSpacing;
                drawNode("…", dotX, lY, 3);
                drawLine(wX, wInfo.y, dotX, lY);
            }
        }
    }
}

void TreeVisualizationWidget::drawNode(const QString &label, double x, double y, int level, int /*freq*/)
{
    // Базовый радиус узла
    double radius = 20;

    // Динамически увеличиваем радиус для длинных слов на 2 уровне
    if (level == 2 && label.length() > 3) {
        radius = 12 + (label.length() * 4.5);
    } else if (level == 0) {
        radius = 30; // Корень сделаем чуть солиднее
    }

    // Цветовая палитра
    QColor color;
    if (level == 0) color = QColor(255, 255, 255);      // Корень
    else if (level == 1) color = QColor(100, 200, 255); // Предложения
    else if (level == 2) color = QColor(255, 150, 100); // Слова
    else color = QColor(180, 100, 255);                 // Буквы

    // Отрисовка круга/овала
    QGraphicsEllipseItem *ellipse = m_scene->addEllipse(x - radius, y - radius, radius * 2, radius * 2,
                                                        QPen(Qt::white, 1.5), QBrush(color));
    ellipse->setZValue(-level);

    // Отрисовка текста
    QGraphicsTextItem *textItem = m_scene->addText(label);

    // На белом корне черный текст читается лучше
    textItem->setDefaultTextColor(level == 0 ? Qt::black : Qt::white);

    QFont font("Arial", 9 + (3 - level));
    if (level == 0) font.setBold(true);
    textItem->setFont(font);

    // Выравнивание строго по центру узла
    textItem->setPos(x - textItem->boundingRect().width() / 2.0,
                     y - textItem->boundingRect().height() / 2.0);
}

void TreeVisualizationWidget::drawLine(double x1, double y1, double x2, double y2)
{
    // Полупрозрачные линии, чтобы выглядело аккуратно
    QGraphicsLineItem *line = m_scene->addLine(x1, y1, x2, y2, QPen(QColor(150, 150, 200, 180), 1.5));
    line->setZValue(-10); // Линии всегда на заднем плане под узлами
}